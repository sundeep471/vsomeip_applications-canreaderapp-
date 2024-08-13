#include <condition_variable>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <sstream>
#include <chrono>

#include <vsomeip/vsomeip.hpp>
#include <nlohmann/json.hpp>

#include "mqtt_pub.h"

/*
 * GLOBALS
 */
const uint32_t MESSAGE_ID_FILTER = 0x3fc8002; // The specific message IID that we want to filter for.
const size_t PAYLOAD_MIN_LENGTH = 20; // The minimum length of the payload that we expect in order to properly extract the CAN ID and CAN Data.
const size_t CAN_ID_BYTE_START = 12; // The starting byte index within the payload.
const size_t CAN_ID_LENGTH = 4; // The length (in bytes) of the CAN ID within the payload.
const size_t CAN_DATA_BYTE_START = 16; // The starting byte index within the payload from which the CAN Data begins.
const size_t CAN_DATA_LENGTH = 8; // The length (in bytes) of the CAN Data.
const size_t HEX_WIDTH = 2; // The width used when printing hexadecimal numbers.
mqtt msqt;

// vsomeip application instance
std::shared_ptr<vsomeip::application> app;

// json instance that will contain the service.catalog.json
nlohmann::json json;

template<typename T>
const std::string hex(const T t, ssize_t width=0, const std::string& prefix="0x") {
    std::stringstream stream;
    if (width) {
        stream << prefix << std::hex << std::setw(width) << std::setfill('0') << t;
    } else {
        stream << prefix << std::hex << t;
    }
    return stream.str();
}

/*
 * Output helper methods.
 */

const std::string get_service_name(const vsomeip_v3::service_t service_id) {
    std::string service_key{std::to_string(service_id)};
    std::string service_name{hex(service_id) + "/" + service_key};
    if (json.contains(service_key)) {
        if (json[service_key].contains("name")) {
            service_name = json[service_key]["name"].get<std::string>() + "(" + service_name + ")";
        } else if (json[service_key].contains("shortname_path")) {
            service_name = json[service_key]["shortname_path"].get<std::string>() + "(" + service_name + ")";
        }
    }
    return service_name;
}

const std::string get_method_name(const vsomeip_v3::service_t service_id, const vsomeip_v3::method_t method_id) {
    std::string service_key{std::to_string(service_id)};
    std::string method_key{std::to_string(method_id)};
    std::string method_name{hex(method_id) + "/" + method_key};
    if (json.contains(service_key)) {
        if (json[service_key].contains("methods") && json[service_key]["methods"].contains(method_key)) {
            if (json[service_key]["methods"][method_key].contains("name")) {
                method_name = json[service_key]["methods"][method_key]["name"].get<std::string>() + "(" + method_name + ")";
            } else if (json[service_key]["methods"][method_key].contains("shortname")) {
                method_name = json[service_key]["methods"][method_key]["shortname"].get<std::string>() + "(" + method_name + ")";
            }
        } else if (json[service_key].contains("eventgroups")) {
            for (auto eventgroup = json[service_key]["eventgroups"].begin(); eventgroup != json[service_key]["eventgroups"].end(); eventgroup++) {
                for (auto event = eventgroup->begin(); event != eventgroup->end() ; event++) {
                    if ((*event)["id"] == method_id) {
                        if ((*event).contains("name")) {
                            method_name = (*event)["name"].get<std::string>() + "(" + method_name + ")";
                        } else if ((*event).contains("shortname")) {
                            method_name = (*event)["shortname"].get<std::string>() + "(" + method_name + ")";
                        }
                        goto done;
                    }
                }
            }
        }
    }
    done:
    return method_name;
}

#define RETURN_ENUM_CASE(T, V) case vsomeip_v3::T::V: return #V;
#define ENUM_TO_STRING(T, C) const std::string get_ ## T(const vsomeip_v3::T ## _e& val) { switch(val) C return "UNKNOWN_" #T + hex((int)val); }

ENUM_TO_STRING(message_type, {
    RETURN_ENUM_CASE(message_type_e, MT_REQUEST);
    RETURN_ENUM_CASE(message_type_e, MT_REQUEST_NO_RETURN);
    RETURN_ENUM_CASE(message_type_e, MT_NOTIFICATION);
    RETURN_ENUM_CASE(message_type_e, MT_REQUEST_ACK);
    RETURN_ENUM_CASE(message_type_e, MT_REQUEST_NO_RETURN_ACK);
    RETURN_ENUM_CASE(message_type_e, MT_NOTIFICATION_ACK);
    RETURN_ENUM_CASE(message_type_e, MT_RESPONSE);
    RETURN_ENUM_CASE(message_type_e, MT_ERROR);
    RETURN_ENUM_CASE(message_type_e, MT_RESPONSE_ACK);
    RETURN_ENUM_CASE(message_type_e, MT_ERROR_ACK);
    RETURN_ENUM_CASE(message_type_e, MT_UNKNOWN);
})

ENUM_TO_STRING(return_code, {
    RETURN_ENUM_CASE(return_code_e, E_OK);
    RETURN_ENUM_CASE(return_code_e, E_NOT_OK);
    RETURN_ENUM_CASE(return_code_e, E_UNKNOWN_SERVICE);
    RETURN_ENUM_CASE(return_code_e, E_UNKNOWN_METHOD);
    RETURN_ENUM_CASE(return_code_e, E_NOT_READY);
    RETURN_ENUM_CASE(return_code_e, E_NOT_REACHABLE);
    RETURN_ENUM_CASE(return_code_e, E_TIMEOUT);
    RETURN_ENUM_CASE(return_code_e, E_WRONG_PROTOCOL_VERSION);
    RETURN_ENUM_CASE(return_code_e, E_WRONG_INTERFACE_VERSION);
    RETURN_ENUM_CASE(return_code_e, E_MALFORMED_MESSAGE);
    RETURN_ENUM_CASE(return_code_e, E_WRONG_MESSAGE_TYPE);
    RETURN_ENUM_CASE(return_code_e, E_UNKNOWN);
})

ENUM_TO_STRING(state_type, {
    RETURN_ENUM_CASE(state_type_e, ST_REGISTERED);
    RETURN_ENUM_CASE(state_type_e, ST_DEREGISTERED);
})

ENUM_TO_STRING(routing_state,{
    RETURN_ENUM_CASE(routing_state_e, RS_RUNNING);
    RETURN_ENUM_CASE(routing_state_e, RS_SUSPENDED);
    RETURN_ENUM_CASE(routing_state_e, RS_RESUMED);
    RETURN_ENUM_CASE(routing_state_e, RS_SHUTDOWN);
    RETURN_ENUM_CASE(routing_state_e, RS_DIAGNOSIS);
    RETURN_ENUM_CASE(routing_state_e, RS_UNKNOWN);
})

ENUM_TO_STRING(availability_state, {
    RETURN_ENUM_CASE(availability_state_e, AS_UNAVAILABLE);
    RETURN_ENUM_CASE(availability_state_e, AS_OFFERED);
    RETURN_ENUM_CASE(availability_state_e, AS_AVAILABLE);
    RETURN_ENUM_CASE(availability_state_e, AS_UNKNOWN);
})

/*
 * OUTPUT overloads for vsomeip types,  Unfortunately, we cannot override the integrals types
 * like message type, etc.
 */

std::ostream& operator<<(std::ostream& stream, const vsomeip_sec_uds_client_credentials_t& uds) {
    return stream << "vsomeip_sec_uds_client_credentials{uid=" << uds.user << ", gid=" << uds.group << "}";
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_sec_ip_client_credentials_t& ip) {

    return stream << (int)(((char *)&(ip.ip))[0]) << "."
                  << (int)(((char *)&(ip.ip))[1]) << "."
                  << (int)(((char *)&(ip.ip))[2]) << "."
                  << (int)(((char *)&(ip.ip))[3]) << ":"
                  << ip.port << "}";
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_sec_client_t& vsomeip_sec_client) {
    switch(vsomeip_sec_client.client_type) {
        case vsomeip_sec_client_type_t::VSOMEIP_CLIENT_UDS:
            return stream << "vsomeip_sec_client{" << vsomeip_sec_client.client.uds_client << "}";
        case vsomeip_sec_client_type_t::VSOMEIP_CLIENT_TCP:
            return stream << "vsomeip_sec_client{" << vsomeip_sec_client.client.ip_client << "}";
        case vsomeip_sec_client_type_t::VSOMEIP_CLIENT_INVALID:
            return stream << "vsomeip_sec_client{INVALID}";
        default:
            return stream << "vsomeip_sec_client{UNKNOWN:" << (int)vsomeip_sec_client.client_type << "}";
    }
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_v3::payload& payload) {
    stream << "(" << payload.get_length() << " bytes)[";
    for (int i=0; i<payload.get_length() ;i++) {
        if (i>0) {
            stream << " ";
        }
        stream << std::hex << std::setw(2) << std::setfill('0') << (int)payload.get_data()[i];
    }
    return stream << "]";
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_v3::message& message) {
    std::string service_key = std::to_string(message.get_service());
    auto json_service = json.find(service_key);
    std::string name{service_key};
        uid_t user;
        gid_t group;

    if (json_service != json.end()) {
        name = (*json_service)["name"];
    }

        vsomeip_sec_client_t sec_client = message.get_sec_client();
        if (sec_client.client_type == VSOMEIP_CLIENT_UDS) {
        user = sec_client.client.uds_client.user;
                group = sec_client.client.uds_client.group;
        }

    stream << "vsomeip_v3::message{"
           << "id=" << message.get_message() << ", "
           << "service=" << get_service_name(message.get_service()) << ", "
           << "instance=" << message.get_instance() << "/" << hex(message.get_instance()) << ", "
           << "method=" << get_method_name(message.get_service(), message.get_method()) <<  ", "
           << "length=" << message.get_length() << ", "
           << "client=" << message.get_client() << "/" << hex(message.get_client()) << ", "
           << "session=" << message.get_session() << "/" << hex(message.get_session()) << ", "
           << "protocol_version=" << message.get_protocol_version() << ", "
           << "interface_version=" << message.get_interface_version() << ", "
           << "type=" << get_message_type(message.get_message_type()) << ", "
           << "return_code=" << get_return_code(message.get_return_code()) << ", "
           << "is_reliale=" << message.is_reliable() << ", "
           << "is_initial=" << message.is_initial() << ", "
           << "payload=" << *(message.get_payload()) << ", "
           << "check_result=" << message.get_check_result() << "/" << hex(message.get_check_result()) << ", "
           << "is_valid_crc=" << message.is_valid_crc() << ", "
           << "uid=" << user << ", "
           << "gid=" << group << ", "
           << "env=" << message.get_env() << ", "
           << "sec_client=" << message.get_sec_client() << "}";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_v3::ipv4_address_t& addr) {
    return stream << (int)addr[0] << "." << (int)addr[1] << "." << (int)addr[2] << "." << (int)addr[3];
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_v3::ipv6_address_t& addr) {
    stream << std::hex << (int)addr[0];
    for (int i=1; i<16; i++) {
        stream << "." << (int)addr[i];
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_v3::ip_address_t& addr) {
    if (addr.is_v4_) {
        return stream << addr.address_.v4_;
    }
    return stream << addr.address_.v6_;
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_v3::remote_info_t& remote_info) {
    return stream << "remote_info{ip=" << remote_info.ip_ << ", first=" << remote_info.first_ << ", last=" << remote_info.last_ << ", is_range=" << remote_info.is_range_ << ", is_reliable=" << remote_info.is_reliable_ << "}";
}

std::ostream& operator<<(std::ostream& stream, const vsomeip_v3::message_acceptance_t& message_acceptance) {
    return stream << "message_acceptance{"
                  << "remote_address=" << (unsigned int)(((unsigned char *)&(message_acceptance.remote_address_))[3]) << "."
                                       << (unsigned int)(((unsigned char *)&(message_acceptance.remote_address_))[2]) << "."
                                       << (unsigned int)(((unsigned char *)&(message_acceptance.remote_address_))[1]) << "."
                                       << (unsigned int)(((unsigned char *)&(message_acceptance.remote_address_))[0]) << ", "
                  << "local_port=" << message_acceptance.local_port_ << ", "
                  << "is_local_=" << message_acceptance.is_local_ << ", "
                  << "service=" << get_service_name(message_acceptance.service_) << ", "
                  << "instance=" << message_acceptance.instance_ << "}";
}

void my_state_handler(vsomeip_v3::state_type_e ste) {
    std::cout << "HANDLER:  state_handler(" << get_state_type(ste) << ")" << std::endl;
}

std::string formatPayload(const unsigned char* payload, int start, int length) {
    std::stringstream ss;

    for (int i = start; i < start + length; ++i) {
        ss << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
        if (i < start + length - 1) {
            ss << " ";
        }
    }

    return ss.str();
}

void my_message_handler(const std::shared_ptr<vsomeip_v3::message>& message) {
    std::string canId;
    std::string canData;
        std::ostringstream oss;  // Create a string stream object

    auto payload = message->get_payload()->get_data();

    // If the payload is long enough
    if (message->get_payload()->get_length() >= 20) {
        // Extracting and printing the CAN ID in the correct order
        // CAN ID is located in 4 bytes starting from the 12th byte of the payload

            std::cout << "CAN ID = ";
            std::cout << std::hex << std::uppercase;
            for (int i = 11; i >= 8; --i) {
                std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
            if (i > 8) std::cout << " "; // Leave a space except the last byte
            }

        // Printing CAN Data
            std::cout << "CAN Data = ";
        for (int i = 12; i < 20; ++i) { // CAN Data starts from the 12th byte and is 8 bytes long
                std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                if (i < 19) std::cout << " ";
            }
            std::cout << std::endl;
    }
    //m.publish("test");
    msqt.publish("CANID = 03FC8002", "test/t1", 17);
}

void my_availability_handler(vsomeip_v3::service_t service, vsomeip_v3::instance_t instance, bool available) {
    std::cout << "HANDLER:  availability_handler(" << get_service_name(service) << ", " << instance << ", " << available << ")" << std::endl;
}

void my_subscription_status_handler(const vsomeip_v3::service_t service, const vsomeip_v3::instance_t instance, const vsomeip_v3::eventgroup_t eventgroup, const vsomeip_v3::event_t event, const uint16_t status) {
    std::cout << "HANDLER:  subsription_status_handler(" << get_service_name(service) << ", " << instance << ", " << eventgroup << ", " << event << ", " << status << ")" << std::endl;
}

void my_watchdog_handler() {
    std::cout << "HANDLER:  watchdog_handler()" << std::endl;
}

void my_reboot_notification_handler(const vsomeip_v3::ip_address_t& addr) {
    std::cout << "HANDLER:  reboot_notification_handler(" << addr << ")" << std::endl;
}

void my_routing_ready_handler() {
    std::cout << "HANDLER:  routing_ready_handler()" << std::endl;
}

void my_routing_state_handler(vsomeip_v3::routing_state_e rse) {
    std::cout << "HANDLER:  routing_state_handler(" << get_routing_state(rse) << ")" << std::endl;
}

void my_availability_state_handler(vsomeip_v3::service_t service, vsomeip_v3::instance_t instance, vsomeip_v3::availability_state_e ase) {
    std::cout << "HANDLER: availability_state_handler(" << get_service_name(service) << ", " << instance << ", " << get_availability_state(ase) << ")" << std::endl;
    std::string service_key = std::to_string(service);
    auto json_service = json.find(service_key);
    switch(ase) {
        case vsomeip_v3::availability_state_e::AS_OFFERED:
            if (json_service == json.end()) {
                app->request_service(service, instance);
            } else {
                auto& node = *json_service;
                app->request_service(service, instance, node["major"], node["minor"]);
            }
            break;
        case vsomeip_v3::availability_state_e::AS_AVAILABLE:
            if (json_service == json.end()) {
                std::cout << "  Service " << get_service_name(service) << " not found in JSON." << std::endl;
                std::cout << "  Register event message handler for any method..." << std::endl;
                app->register_message_handler(service, instance, vsomeip_v3::ANY_METHOD, my_message_handler);
                std::cout << "Register subscription status handler for any event..." << std::endl;
                app->register_subscription_status_handler(service, instance, vsomeip_v3::ANY_EVENTGROUP, vsomeip_v3::ANY_EVENT, my_subscription_status_handler);
                std::cout << "Request any event..." << std::endl;
                app->request_event(service, instance, vsomeip_v3::ANY_EVENT, {vsomeip_v3::ANY_EVENTGROUP}, vsomeip_v3::event_type_e::ET_EVENT, vsomeip_v3::reliability_type_e::RT_UNRELIABLE);
                std::cout << "Subscribe to any eventgroup..." << std::endl;
                app->subscribe(service, instance, vsomeip_v3::ANY_EVENTGROUP, vsomeip_v3::ANY_MAJOR);
            } else {
                auto& node = *json_service;
                for (auto i = node["eventgroups"].begin(); i != node["eventgroups"].end(); i++) {
                    for (auto j = i.value().begin(); j != i.value().end(); j++) {
                        std::cout << "  Register event message handler for event " << j.key() << "..." << std::endl;
                        app->register_message_handler(service, instance, (vsomeip_v3::event_t)std::stoul(j.key()), my_message_handler);
                    }
                }
                for (auto i = node["methods"].begin(); i != node["methods"].end(); i++) {
                    std::cout << "  Register event message handler for method " << i.value()["id"] << "..." << std::endl;
                    app->register_message_handler(service, instance, i.value()["id"], my_message_handler);
                }
                for (auto i = node["eventgroups"].begin(); i != node["eventgroups"].end(); i++) {
                    for (auto j = i.value().begin(); j != i.value().end(); j++) {
                        std::cout << "Register subscription status handler for event " << j.key() << "..." << std::endl;
                        app->register_subscription_status_handler(service, instance, (vsomeip_v3::eventgroup_t)std::stoul(i.key()), (vsomeip_v3::event_t)std::stoul(j.key()), my_subscription_status_handler);
                    }
                }
                for (auto i = node["eventgroups"].begin(); i != node["eventgroups"].end(); i++) {
                    for (auto j = i.value().begin(); j != i.value().end(); j++) {
                        std::cout << "Request event " << j.key() << "..." << std::endl;
                        app->request_event(service, instance, (vsomeip_v3::event_t)std::stoul(j.key()), {(vsomeip_v3::eventgroup_t)std::stoul(i.key())}, vsomeip_v3::event_type_e::ET_EVENT, vsomeip_v3::reliability_type_e::RT_UNRELIABLE);
                    }
                }
                for (auto i = node["eventgroups"].begin(); i != node["eventgroups"].end(); i++) {
                    std::cout << "Subscribe to eventgroup " << i.key() << "..." << std::endl;
                    app->subscribe(service, instance, (vsomeip_v3::eventgroup_t)std::stoul(i.key()), node["major"]);
                }
                for (auto i = node["fields"].begin(); i != node["fields"].end(); i++) {
                    if (nullptr != i.value()["get"]) {
                        std::shared_ptr<vsomeip_v3::message> request;
                        request = vsomeip_v3::runtime::get()->create_request();
                        request->set_service(service);
                        request->set_instance(instance);
                        request->set_method(i.value()["get"]);
                        std::cout << "  Calling " << i.value()["name"] << "::" << "get..." << std::endl;
                        app->send(request);
                    }
                }
            }
            break;
    }
}

void my_async_subscription_handler_sec(vsomeip_v3::client_t client, const vsomeip_sec_client_t *vsomeip_sec_client, const std::string& what, bool subscribed, std::function<void(bool)> cb) {
    std::cout << "HANDLER:  async_subscription_handler_sec(" << client << ", " << *vsomeip_sec_client << ", \"" << what << "\", " << subscribed << ", " << cb.target_type().name() << ")" << std::endl;
    cb(false);
}

int main() {
    std::cout << "Creating application..." << std::endl;
    app = vsomeip::runtime::get()->create_application("Client1");

    std::cout << "Initializing application..." << std::endl;
    /*
    2000-01-01 00:23:48.288049 [info] Using configuration file: "./vsomeip.json".
    2000-01-01 00:23:48.288069 [info] Parsed vsomeip configuration in 99ms
    2000-01-01 00:23:48.288069 [info] Configuration module loaded.
    2000-01-01 00:23:48.288079 [info] Initializing vsomeip (3.3.8) application "Client1".
    2000-01-01 00:23:48.288109 [info] Instantiating routing manager [Host].
    2000-01-01 00:23:48.288169 [info] create_routing_root: Routing root @ /var/vsomeip-0
    2000-01-01 00:23:48.288209 [info] Service Discovery enabled. Trying to load module.
    2000-01-01 00:23:48.288229 [info] Service Discovery module loaded.
    2000-01-01 00:23:48.288249 [info] Application(Client1, 0399) is initialized (11, 100).
    */
    msqt.init();
    app->init();

    std::cout << "Reading service config..." << std::endl;

    json = nlohmann::json::parse(std::ifstream("service.catalog.json"));

    app->set_watchdog_handler(my_watchdog_handler, std::chrono::seconds(10));

    app->register_state_handler(my_state_handler);
    app->register_message_handler(vsomeip_v3::ANY_SERVICE, vsomeip_v3::ANY_INSTANCE, vsomeip_v3::ANY_METHOD, my_message_handler); // can be filtered.
    app->register_availability_handler(vsomeip_v3::ANY_SERVICE, vsomeip_v3::ANY_INSTANCE, my_availability_handler, vsomeip_v3::ANY_MAJOR, vsomeip_v3::ANY_MINOR);
    app->register_subscription_status_handler(vsomeip_v3::ANY_SERVICE, vsomeip_v3::ANY_INSTANCE, vsomeip_v3::ANY_EVENTGROUP, vsomeip_v3::ANY_EVENT, my_subscription_status_handler, false);
    app->register_reboot_notification_handler(my_reboot_notification_handler);
    app->register_routing_ready_handler(my_routing_ready_handler);
    app->register_routing_state_handler(my_routing_state_handler);
    app->register_availability_handler(vsomeip_v3::ANY_SERVICE, vsomeip_v3::ANY_INSTANCE, my_availability_state_handler, vsomeip_v3::ANY_MAJOR, vsomeip_v3::ANY_MINOR);
    app->register_async_subscription_handler(vsomeip_v3::ANY_SERVICE, vsomeip_v3::ANY_INSTANCE, vsomeip_v3::ANY_EVENTGROUP, my_async_subscription_handler_sec);

        std::cout << "Starting application..." << std::endl;
        app->start();
        std::cout << "Exiting application." << std::endl;
}
