#include <condition_variable>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <sstream>
#include <chrono>
#include <bitset>

#include <vsomeip/vsomeip.hpp>
#include <nlohmann/json.hpp>

#include "can_message.h"
#include "can_signal.h"
#include "mqtt_pub.h"
//#include "dbc_map.h"
#include "dbc_map_reduced.h"

using namespace std;

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
mqtt msqt_pub;

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

void printMessageInfo(const std::string& messageTypeName, std::uint32_t rawIdentifier) {
    // Print the type name and raw identifier inside the function
    std::cout << "Message Type: " << messageTypeName << std::endl;
    std::cout << "Raw Identifier: " << std::hex << rawIdentifier << std::endl;
}

// Function to handle the publishing process
template <typename T>
void publishSignal(const std::string& topic, const T& rawValue, mqtt msqt_pub) {
    // Print the value in hexadecimal format according to its type
    if constexpr (std::is_same_v<T, std::uint8_t>) {
        std::cout << std::hex << static_cast<int>(rawValue) << std::endl;  // Use int for uint8_t, as casting to uint8_t would lose the proper print format
    } else if constexpr (std::is_same_v<T, std::uint16_t>) {
        std::cout << std::hex << rawValue << std::endl;
    } else if constexpr (std::is_same_v<T, std::uint32_t>) {
        std::cout << std::hex << rawValue << std::endl;
    } else {
        std::cout << std::hex << static_cast<std::uint64_t>(rawValue) << std::endl;  // Default to uint64_t for other types
    }

    // Directly publish the message
    msqt_pub.publish(topic, static_cast<const void*>(&rawValue), sizeof(rawValue));
}

void processCanMessage(std::string &canId, ara::core::Span<const uint8_t> &canData) {
    //std::cout << canId << "\n";
    //std::cout << canData[0] << "\n";
    std::string topic = "Empty: To Be Done"; // Default: To Be Done
    const void* payload; 
    int payloadlen;
    std::uint8_t rawValue8;
    std::uint16_t rawValue16;
    std::uint32_t rawValue32;

    std::cout << "K01" << std::endl;
    //std::cout << "Args: canId: " << std::hex << canId << std::endl;
    std::cout << "Args: (canId | 0x80000000): " << std::hex << "0x" << (std::stoul(canId, nullptr, 16) | 0x80000000) << std::endl;
    std::cout << "Args: canData: " << std::hex << "0x" << canData[0] << canData[1] << canData[2] << canData[3] << canData[4] << canData[5] << canData[6] << std::endl;
    switch (std::stoul(canId, nullptr, 16) | 0x80000000) {
        case 0x98FEE617: {
            constexpr auto message = canID_0x98FEE617{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TimeDate_ICL::Seconds> (SecondsrawValue) = message.Seconds.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SecondsrawValue.value());
            topic = "can/TimeDate_ICL/Seconds";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TimeDate_ICL::Minutes> (MinutesrawValue) = message.Minutes.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(MinutesrawValue.value());
            topic = "can/TimeDate_ICL/Minutes";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TimeDate_ICL::Hours> (HoursrawValue) = message.Hours.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(HoursrawValue.value());
            topic = "can/TimeDate_ICL/Hours";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TimeDate_ICL::Month> (MonthrawValue) = message.Month.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(MonthrawValue.value());
            topic = "can/TimeDate_ICL/Month";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TimeDate_ICL::Day> (DayrawValue) = message.Day.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DayrawValue.value());
            topic = "can/TimeDate_ICL/Day";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TimeDate_ICL::Year> (YearrawValue) = message.Year.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(YearrawValue.value());
            topic = "can/TimeDate_ICL/Year";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF554A: {
            constexpr auto message = canID_0x98FF554A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint16_t> (Metadata_CountryCode_0x06rawValue) = message.Metadata_CountryCode_0x06.GetRawValue(canData);
            rawValue16 = Metadata_CountryCode_0x06rawValue.value();
            topic = "can/ADASP1_RTC/Metadata_CountryCode_0x06";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Offset_0x05> (ProfileLong_Offset_0x05rawValue) = message.ProfileLong_Offset_0x05.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileLong_Offset_0x05rawValue.value());
            topic = "can/ADASP1_RTC/ProfileLong_Offset_0x05";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Metadata_MapProvider_0x06> (Metadata_MapProvider_0x06rawValue) = message.Metadata_MapProvider_0x06.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Metadata_MapProvider_0x06rawValue.value());
            topic = "can/ADASP1_RTC/Metadata_MapProvider_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_Offset_0x01> (Pos_Offset_0x01rawValue) = message.Pos_Offset_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_Offset_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_Offset_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Offset_0x04> (ProfileShort_Offset_0x04rawValue) = message.ProfileShort_Offset_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_Offset_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_Offset_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_Offset_0x03> (Stub_Offset_0x03rawValue) = message.Stub_Offset_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_Offset_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_Offset_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ADAS_MsgType> (ADAS_MsgTyperawValue) = message.ADAS_MsgType.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ADAS_MsgTyperawValue.value());
            topic = "can/ADASP1_RTC/ADAS_MsgType";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint16_t> (Metadata_HardwareVersion_0x06rawValue) = message.Metadata_HardwareVersion_0x06.GetRawValue(canData);
            rawValue16 = Metadata_HardwareVersion_0x06rawValue.value();
            topic = "can/ADASP1_RTC/Metadata_HardwareVersion_0x06";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<std::uint8_t> (Metadata_ProtocolSubVer_0x06rawValue) = message.Metadata_ProtocolSubVer_0x06.GetRawValue(canData);
            rawValue8 = Metadata_ProtocolSubVer_0x06rawValue.value();
            topic = "can/ADASP1_RTC/Metadata_ProtocolSubVer_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ProfileLong_PathIndex_0x05rawValue) = message.ProfileLong_PathIndex_0x05.GetRawValue(canData);
            rawValue8 = ProfileLong_PathIndex_0x05rawValue.value();
            topic = "can/ADASP1_RTC/ProfileLong_PathIndex_0x05";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Metadata_ProtocolVerMajor_0x06rawValue) = message.Metadata_ProtocolVerMajor_0x06.GetRawValue(canData);
            rawValue8 = Metadata_ProtocolVerMajor_0x06rawValue.value();
            topic = "can/ADASP1_RTC/Metadata_ProtocolVerMajor_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_PathIndex_0x01> (Pos_PathIndex_0x01rawValue) = message.Pos_PathIndex_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_PathIndex_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_PathIndex_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ProfileShort_PathIndex_0x04rawValue) = message.ProfileShort_PathIndex_0x04.GetRawValue(canData);
            rawValue8 = ProfileShort_PathIndex_0x04rawValue.value();
            topic = "can/ADASP1_RTC/ProfileShort_PathIndex_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_PathIndex_0x03> (Stub_PathIndex_0x03rawValue) = message.Stub_PathIndex_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_PathIndex_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_PathIndex_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ProfileLong_CyclicCounter_0x05rawValue) = message.ProfileLong_CyclicCounter_0x05.GetRawValue(canData);
            rawValue8 = ProfileLong_CyclicCounter_0x05rawValue.value();
            topic = "can/ADASP1_RTC/ProfileLong_CyclicCounter_0x05";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Metadata_CyclicCounter_0x06rawValue) = message.Metadata_CyclicCounter_0x06.GetRawValue(canData);
            rawValue8 = Metadata_CyclicCounter_0x06rawValue.value();
            topic = "can/ADASP1_RTC/Metadata_CyclicCounter_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Pos_CyclicCounter_0x01rawValue) = message.Pos_CyclicCounter_0x01.GetRawValue(canData);
            rawValue8 = Pos_CyclicCounter_0x01rawValue.value();
            topic = "can/ADASP1_RTC/Pos_CyclicCounter_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ProfileShort_CyclicCounter_0x04rawValue) = message.ProfileShort_CyclicCounter_0x04.GetRawValue(canData);
            rawValue8 = ProfileShort_CyclicCounter_0x04rawValue.value();
            topic = "can/ADASP1_RTC/ProfileShort_CyclicCounter_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Stub_CyclicCounter_0x03rawValue) = message.Stub_CyclicCounter_0x03.GetRawValue(canData);
            rawValue8 = Stub_CyclicCounter_0x03rawValue.value();
            topic = "can/ADASP1_RTC/Stub_CyclicCounter_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Update_0x05> (ProfileLong_Update_0x05rawValue) = message.ProfileLong_Update_0x05.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileLong_Update_0x05rawValue.value());
            topic = "can/ADASP1_RTC/ProfileLong_Update_0x05";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_PositionAge_0x01> (Pos_PositionAge_0x01rawValue) = message.Pos_PositionAge_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_PositionAge_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_PositionAge_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Update_0x04> (ProfileShort_Update_0x04rawValue) = message.ProfileShort_Update_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_Update_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_Update_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Stub_Update_0x03rawValue) = message.Stub_Update_0x03.GetRawValue(canData);
            rawValue8 = Stub_Update_0x03rawValue.value();
            topic = "can/ADASP1_RTC/Stub_Update_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Retransmission_0x05> (ProfileLong_Retransmission_0x05rawValue) = message.ProfileLong_Retransmission_0x05.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileLong_Retransmission_0x05rawValue.value());
            topic = "can/ADASP1_RTC/ProfileLong_Retransmission_0x05";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Retransmission_0x04> (ProfileShort_Retransmission_0x04rawValue) = message.ProfileShort_Retransmission_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_Retransmission_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_Retransmission_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_Retransmission_0x03> (Stub_Retransmission_0x03rawValue) = message.Stub_Retransmission_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_Retransmission_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_Retransmission_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileLong_ControlPoint_0x05> (ProfileLong_ControlPoint_0x05rawValue) = message.ProfileLong_ControlPoint_0x05.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileLong_ControlPoint_0x05rawValue.value());
            topic = "can/ADASP1_RTC/ProfileLong_ControlPoint_0x05";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_ControlPoint_0x04> (ProfileShort_ControlPoint_0x04rawValue) = message.ProfileShort_ControlPoint_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_ControlPoint_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_ControlPoint_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_PositionProbability_0x01> (Pos_PositionProbability_0x01rawValue) = message.Pos_PositionProbability_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_PositionProbability_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_PositionProbability_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileLong_ProfileType_0x05> (ProfileLong_ProfileType_0x05rawValue) = message.ProfileLong_ProfileType_0x05.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileLong_ProfileType_0x05rawValue.value());
            topic = "can/ADASP1_RTC/ProfileLong_ProfileType_0x05";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Pos_PositionIndex_0x01rawValue) = message.Pos_PositionIndex_0x01.GetRawValue(canData);
            rawValue8 = Pos_PositionIndex_0x01rawValue.value();
            topic = "can/ADASP1_RTC/Pos_PositionIndex_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_ProfileType_0x04> (ProfileShort_ProfileType_0x04rawValue) = message.ProfileShort_ProfileType_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_ProfileType_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_ProfileType_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_SubPathIndex_0x03> (Stub_SubPathIndex_0x03rawValue) = message.Stub_SubPathIndex_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_SubPathIndex_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_SubPathIndex_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_FunctionalRoadClass> (Stub_FunctionalRoadClassrawValue) = message.Stub_FunctionalRoadClass.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_FunctionalRoadClassrawValue.value());
            topic = "can/ADASP1_RTC/Stub_FunctionalRoadClass";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Distance1_0x04> (ProfileShort_Distance1_0x04rawValue) = message.ProfileShort_Distance1_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_Distance1_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_Distance1_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Metadata_RegionCode_0x06> (Metadata_RegionCode_0x06rawValue) = message.Metadata_RegionCode_0x06.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Metadata_RegionCode_0x06rawValue.value());
            topic = "can/ADASP1_RTC/Metadata_RegionCode_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint32_t> (ProfileLong_Value_0x05rawValue) = message.ProfileLong_Value_0x05.GetRawValue(canData);
            rawValue32 = ProfileLong_Value_0x05rawValue.value();
            topic = "can/ADASP1_RTC/ProfileLong_Value_0x05";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Metadata_DrivingSide_0x06> (Metadata_DrivingSide_0x06rawValue) = message.Metadata_DrivingSide_0x06.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Metadata_DrivingSide_0x06rawValue.value());
            topic = "can/ADASP1_RTC/Metadata_DrivingSide_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_AccuracyClass_0x04> (ProfileShort_AccuracyClass_0x04rawValue) = message.ProfileShort_AccuracyClass_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_AccuracyClass_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_AccuracyClass_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_RelativeProbability_0x03> (Stub_RelativeProbability_0x03rawValue) = message.Stub_RelativeProbability_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_RelativeProbability_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_RelativeProbability_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_Speed_0x01> (Pos_Speed_0x01rawValue) = message.Pos_Speed_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_Speed_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_Speed_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_CurrentLane_0x01> (Pos_CurrentLane_0x01rawValue) = message.Pos_CurrentLane_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_CurrentLane_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_CurrentLane_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Value0_0x04> (ProfileShort_Value0_0x04rawValue) = message.ProfileShort_Value0_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_Value0_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_Value0_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_FormOfWay_0x03> (Stub_FormOfWay_0x03rawValue) = message.Stub_FormOfWay_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_FormOfWay_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_FormOfWay_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_ComplexIntersection_0x03> (Stub_ComplexIntersection_0x03rawValue) = message.Stub_ComplexIntersection_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_ComplexIntersection_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_ComplexIntersection_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_PositionConfidence_0x01> (Pos_PositionConfidence_0x01rawValue) = message.Pos_PositionConfidence_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_PositionConfidence_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_PositionConfidence_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_PartOfCalculatedRoute_0x03> (Stub_PartOfCalculatedRoute_0x03rawValue) = message.Stub_PartOfCalculatedRoute_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_PartOfCalculatedRoute_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_PartOfCalculatedRoute_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Segment_EffectiveSpeedLimitTyperawValue) = message.Segment_EffectiveSpeedLimitType.GetRawValue(canData);
            rawValue8 = Segment_EffectiveSpeedLimitTyperawValue.value();
            topic = "can/ADASP1_RTC/Segment_EffectiveSpeedLimitType";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Value1_0x04> (ProfileShort_Value1_0x04rawValue) = message.ProfileShort_Value1_0x04.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ProfileShort_Value1_0x04rawValue.value());
            topic = "can/ADASP1_RTC/ProfileShort_Value1_0x04";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Metadata_MapVersionYear_0x06> (Metadata_MapVersionYear_0x06rawValue) = message.Metadata_MapVersionYear_0x06.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Metadata_MapVersionYear_0x06rawValue.value());
            topic = "can/ADASP1_RTC/Metadata_MapVersionYear_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Segment_EffectiveSpeedLimitrawValue) = message.Segment_EffectiveSpeedLimit.GetRawValue(canData);
            rawValue8 = Segment_EffectiveSpeedLimitrawValue.value();
            topic = "can/ADASP1_RTC/Segment_EffectiveSpeedLimit";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Metadata_MapVersionQtr_0x06> (Metadata_MapVersionQtr_0x06rawValue) = message.Metadata_MapVersionQtr_0x06.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Metadata_MapVersionQtr_0x06rawValue.value());
            topic = "can/ADASP1_RTC/Metadata_MapVersionQtr_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_TurnAngle_0x03> (Stub_TurnAngle_0x03rawValue) = message.Stub_TurnAngle_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_TurnAngle_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_TurnAngle_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_NumberOfLanes_0x03> (Stub_NumberOfLanes_0x03rawValue) = message.Stub_NumberOfLanes_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_NumberOfLanes_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_NumberOfLanes_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Metadata_ProtocolVerMinor_0x06rawValue) = message.Metadata_ProtocolVerMinor_0x06.GetRawValue(canData);
            rawValue8 = Metadata_ProtocolVerMinor_0x06rawValue.value();
            topic = "can/ADASP1_RTC/Metadata_ProtocolVerMinor_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Metadata_SpeedUnits_0x06> (Metadata_SpeedUnits_0x06rawValue) = message.Metadata_SpeedUnits_0x06.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Metadata_SpeedUnits_0x06rawValue.value());
            topic = "can/ADASP1_RTC/Metadata_SpeedUnits_0x06";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_NumberOfOppositeLanes_0x03> (Stub_NumberOfOppositeLanes_0x03rawValue) = message.Stub_NumberOfOppositeLanes_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_NumberOfOppositeLanes_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_NumberOfOppositeLanes_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_RightOfWay_0x03> (Stub_RightOfWay_0x03rawValue) = message.Stub_RightOfWay_0x03.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_RightOfWay_0x03rawValue.value());
            topic = "can/ADASP1_RTC/Stub_RightOfWay_0x03";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Pos_RelativeHeading_0x01> (Pos_RelativeHeading_0x01rawValue) = message.Pos_RelativeHeading_0x01.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Pos_RelativeHeading_0x01rawValue.value());
            topic = "can/ADASP1_RTC/Pos_RelativeHeading_0x01";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ADASP1_RTC::Stub_LastStubAtOffset> (Stub_LastStubAtOffsetrawValue) = message.Stub_LastStubAtOffset.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Stub_LastStubAtOffsetrawValue.value());
            topic = "can/ADASP1_RTC/Stub_LastStubAtOffset";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF6027: {
            constexpr auto message = canID_0x98FF6027{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum GPM1_K::CalculatedGTWStatus> (CalculatedGTWStatusrawValue) = message.CalculatedGTWStatus.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(CalculatedGTWStatusrawValue.value());
            topic = "can/GPM1_K/CalculatedGTWStatus";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum GPM1_K::CalculatedGTW> (CalculatedGTWrawValue) = message.CalculatedGTW.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(CalculatedGTWrawValue.value());
            topic = "can/GPM1_K/CalculatedGTW";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x8CFF6327: {
            constexpr auto message = canID_0x8CFF6327{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum GPM2_K::Charge61> (Charge61rawValue) = message.Charge61.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Charge61rawValue.value());
            topic = "can/GPM2_K/Charge61";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum GPM2_K::TrailerDetectionAndABSStatus> (TrailerDetectionAndABSStatusrawValue) = message.TrailerDetectionAndABSStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TrailerDetectionAndABSStatusrawValue.value());
            topic = "can/GPM2_K/TrailerDetectionAndABSStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum GPM2_K::CoastingActiveSignal> (CoastingActiveSignalrawValue) = message.CoastingActiveSignal.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(CoastingActiveSignalrawValue.value());
            topic = "can/GPM2_K/CoastingActiveSignal";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum GPM2_K::EngineStartInhibitByOTAStatus> (EngineStartInhibitByOTAStatusrawValue) = message.EngineStartInhibitByOTAStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EngineStartInhibitByOTAStatusrawValue.value());
            topic = "can/GPM2_K/EngineStartInhibitByOTAStatus";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF4327: {
            constexpr auto message = canID_0x98FF4327{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum BrakeLiningWearInformation2_K::AxleNumber> (AxleNumberrawValue) = message.AxleNumber.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(AxleNumberrawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/AxleNumber";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_1> (BrakeLiningRemainingLeftWheel_1rawValue) = message.BrakeLiningRemainingLeftWheel_1.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_1rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingLeftWheel_1";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_2> (BrakeLiningRemainingLeftWheel_2rawValue) = message.BrakeLiningRemainingLeftWheel_2.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_2rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingLeftWheel_2";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_3> (BrakeLiningRemainingLeftWheel_3rawValue) = message.BrakeLiningRemainingLeftWheel_3.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_3rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingLeftWheel_3";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_4> (BrakeLiningRemainingLeftWheel_4rawValue) = message.BrakeLiningRemainingLeftWheel_4.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_4rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingLeftWheel_4";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_5> (BrakeLiningRemainingLeftWheel_5rawValue) = message.BrakeLiningRemainingLeftWheel_5.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_5rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingLeftWheel_5";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_1> (BrakeLiningRemainingRightWheel_1rawValue) = message.BrakeLiningRemainingRightWheel_1.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_1rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingRightWheel_1";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_2> (BrakeLiningRemainingRightWheel_2rawValue) = message.BrakeLiningRemainingRightWheel_2.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_2rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingRightWheel_2";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_3> (BrakeLiningRemainingRightWheel_3rawValue) = message.BrakeLiningRemainingRightWheel_3.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_3rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingRightWheel_3";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_4> (BrakeLiningRemainingRightWheel_4rawValue) = message.BrakeLiningRemainingRightWheel_4.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_4rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingRightWheel_4";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_5> (BrakeLiningRemainingRightWheel_5rawValue) = message.BrakeLiningRemainingRightWheel_5.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_5rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningRemainingRightWheel_5";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_1> (BrakeLiningDistanceRemainingLeftWheel_1rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_1.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_1rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingLeftWheel_1";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_2> (BrakeLiningDistanceRemainingLeftWheel_2rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_2.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_2rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingLeftWheel_2";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_3> (BrakeLiningDistanceRemainingLeftWheel_3rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_3.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_3rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingLeftWheel_3";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_4> (BrakeLiningDistanceRemainingLeftWheel_4rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_4.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_4rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingLeftWheel_4";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_5> (BrakeLiningDistanceRemainingLeftWheel_5rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_5.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_5rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingLeftWheel_5";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_1> (BrakeLiningDistanceRemainingRightWheel_1rawValue) = message.BrakeLiningDistanceRemainingRightWheel_1.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_1rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingRightWheel_1";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_2> (BrakeLiningDistanceRemainingRightWheel_2rawValue) = message.BrakeLiningDistanceRemainingRightWheel_2.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_2rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingRightWheel_2";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_3> (BrakeLiningDistanceRemainingRightWheel_3rawValue) = message.BrakeLiningDistanceRemainingRightWheel_3.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_3rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingRightWheel_3";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_4> (BrakeLiningDistanceRemainingRightWheel_4rawValue) = message.BrakeLiningDistanceRemainingRightWheel_4.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_4rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingRightWheel_4";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_5> (BrakeLiningDistanceRemainingRightWheel_5rawValue) = message.BrakeLiningDistanceRemainingRightWheel_5.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_5rawValue.value());
            topic = "can/BrakeLiningWearInformation2_K/BrakeLiningDistanceRemainingRightWheel_5";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FE5EC8: {
            constexpr auto message = canID_0x98FE5EC8{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            rawValue8 = TyreOrWheelIdentificationrawValue.value();
            topic = "can/RGE23_TT_C8/TyreOrWheelIdentification";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            rawValue16 = TyreTemperaturerawValue.value();
            topic = "can/RGE23_TT_C8/TyreTemperature";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            rawValue16 = AirLeakageDetectionrawValue.value();
            topic = "can/RGE23_TT_C8/AirLeakageDetection";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            rawValue8 = TyrePressureThresholdDetectionrawValue.value();
            topic = "can/RGE23_TT_C8/TyrePressureThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            rawValue8 = TyreModulePowerSupplyrawValue.value();
            topic = "can/RGE23_TT_C8/TyreModulePowerSupply";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum RGE23_TT_C8::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value());
            topic = "can/RGE23_TT_C8/IdentificationDataIndex";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FE5EC0: {
            constexpr auto message = canID_0x98FE5EC0{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            rawValue8 = TyreOrWheelIdentificationrawValue.value();
            topic = "can/RGE23_TT_C0/TyreOrWheelIdentification";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            rawValue16 = TyreTemperaturerawValue.value();
            topic = "can/RGE23_TT_C0/TyreTemperature";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            rawValue16 = AirLeakageDetectionrawValue.value();
            topic = "can/RGE23_TT_C0/AirLeakageDetection";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            rawValue8 = TyrePressureThresholdDetectionrawValue.value();
            topic = "can/RGE23_TT_C0/TyrePressureThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            rawValue8 = TyreModulePowerSupplyrawValue.value();
            topic = "can/RGE23_TT_C0/TyreModulePowerSupply";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum RGE23_TT_C0::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value());
            topic = "can/RGE23_TT_C0/IdentificationDataIndex";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FE5EB8: {
            constexpr auto message = canID_0x98FE5EB8{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            rawValue8 = TyreOrWheelIdentificationrawValue.value();
            topic = "can/RGE23_TT_B8/TyreOrWheelIdentification";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            rawValue16 = TyreTemperaturerawValue.value();
            topic = "can/RGE23_TT_B8/TyreTemperature";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            rawValue16 = AirLeakageDetectionrawValue.value();
            topic = "can/RGE23_TT_B8/AirLeakageDetection";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            rawValue8 = TyrePressureThresholdDetectionrawValue.value();
            topic = "can/RGE23_TT_B8/TyrePressureThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            rawValue8 = TyreModulePowerSupplyrawValue.value();
            topic = "can/RGE23_TT_B8/TyreModulePowerSupply";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum RGE23_TT_B8::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value());
            topic = "can/RGE23_TT_B8/IdentificationDataIndex";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FE5EB0: {
            constexpr auto message = canID_0x98FE5EB0{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            rawValue8 = TyreOrWheelIdentificationrawValue.value();
            topic = "can/RGE23_TT_B0/TyreOrWheelIdentification";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            rawValue16 = TyreTemperaturerawValue.value();
            topic = "can/RGE23_TT_B0/TyreTemperature";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            rawValue16 = AirLeakageDetectionrawValue.value();
            topic = "can/RGE23_TT_B0/AirLeakageDetection";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            rawValue8 = TyrePressureThresholdDetectionrawValue.value();
            topic = "can/RGE23_TT_B0/TyrePressureThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            rawValue8 = TyreModulePowerSupplyrawValue.value();
            topic = "can/RGE23_TT_B0/TyreModulePowerSupply";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum RGE23_TT_B0::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value());
            topic = "can/RGE23_TT_B0/IdentificationDataIndex";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FE5EA8: {
            constexpr auto message = canID_0x98FE5EA8{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            rawValue8 = TyreOrWheelIdentificationrawValue.value();
            topic = "can/RGE23_TT_A8/TyreOrWheelIdentification";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            rawValue16 = TyreTemperaturerawValue.value();
            topic = "can/RGE23_TT_A8/TyreTemperature";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            rawValue16 = AirLeakageDetectionrawValue.value();
            topic = "can/RGE23_TT_A8/AirLeakageDetection";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            rawValue8 = TyrePressureThresholdDetectionrawValue.value();
            topic = "can/RGE23_TT_A8/TyrePressureThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            rawValue8 = TyreModulePowerSupplyrawValue.value();
            topic = "can/RGE23_TT_A8/TyreModulePowerSupply";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum RGE23_TT_A8::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value());
            topic = "can/RGE23_TT_A8/IdentificationDataIndex";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEEA17: {
            constexpr auto message = canID_0x98FEEA17{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum VehicleWeight_ICL::AxleLocation> (AxleLocationrawValue) = message.AxleLocation.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(AxleLocationrawValue.value());
            topic = "can/VehicleWeight_ICL/AxleLocation";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum VehicleWeight_ICL::AxleWeight> (AxleWeightrawValue) = message.AxleWeight.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(AxleWeightrawValue.value());
            topic = "can/VehicleWeight_ICL/AxleWeight";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum VehicleWeight_ICL::TrailerWeight> (TrailerWeightrawValue) = message.TrailerWeight.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(TrailerWeightrawValue.value());
            topic = "can/VehicleWeight_ICL/TrailerWeight";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum VehicleWeight_ICL::CargoWeight> (CargoWeightrawValue) = message.CargoWeight.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(CargoWeightrawValue.value());
            topic = "can/VehicleWeight_ICL/CargoWeight";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x99FF4F4A: {
            constexpr auto message = canID_0x99FF4F4A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::motorwayMapSpeedLim> (motorwayMapSpeedLimrawValue) = message.motorwayMapSpeedLim.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(motorwayMapSpeedLimrawValue.value());
            topic = "can/ImplicitSpeedLimit_RTC/motorwayMapSpeedLim";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ImplicitSpeedLimit_RTC::DCMapSpeedLim> (DCMapSpeedLimrawValue) = message.DCMapSpeedLim.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DCMapSpeedLimrawValue.value());
            topic = "can/ImplicitSpeedLimit_RTC/DCMapSpeedLim";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ImplicitSpeedLimit_RTC::SCMapSpeedLim> (SCMapSpeedLimrawValue) = message.SCMapSpeedLim.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SCMapSpeedLimrawValue.value());
            topic = "can/ImplicitSpeedLimit_RTC/SCMapSpeedLim";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ImplicitSpeedLimit_RTC::ruralMapSpeedLim> (ruralMapSpeedLimrawValue) = message.ruralMapSpeedLim.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ruralMapSpeedLimrawValue.value());
            topic = "can/ImplicitSpeedLimit_RTC/ruralMapSpeedLim";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ImplicitSpeedLimit_RTC::urbanMapSpeedLim> (urbanMapSpeedLimrawValue) = message.urbanMapSpeedLim.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(urbanMapSpeedLimrawValue.value());
            topic = "can/ImplicitSpeedLimit_RTC/urbanMapSpeedLim";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ImplicitSpeedLimit_RTC::playstreetMapSpeedLim> (playstreetMapSpeedLimrawValue) = message.playstreetMapSpeedLim.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(playstreetMapSpeedLimrawValue.value());
            topic = "can/ImplicitSpeedLimit_RTC/playstreetMapSpeedLim";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF474A: {
            constexpr auto message = canID_0x99FF474A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum RTCInformationProprietary2_RTC::UpdatePrecondition> (UpdatePreconditionrawValue) = message.UpdatePrecondition.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(UpdatePreconditionrawValue.value());
            topic = "can/RTCInformationProprietary2_RTC/UpdatePrecondition";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF7347: {
            constexpr auto message = canID_0x99FF7347{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum CI3_CMS::Battery24VCurrent> (Battery24VCurrentrawValue) = message.Battery24VCurrent.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(Battery24VCurrentrawValue.value());
            topic = "can/CI3_CMS/Battery24VCurrent";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FEF127: {
            constexpr auto message = canID_0x98FEF127{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum CruiseControlVehSpeed_K::ParkingBrakeSwitch> (ParkingBrakeSwitchrawValue) = message.ParkingBrakeSwitch.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ParkingBrakeSwitchrawValue.value());
            topic = "can/CruiseControlVehSpeed_K/ParkingBrakeSwitch";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum CruiseControlVehSpeed_K::WheelBasedVehicleSpeed> (WheelBasedVehicleSpeedrawValue) = message.WheelBasedVehicleSpeed.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(WheelBasedVehicleSpeedrawValue.value());
            topic = "can/CruiseControlVehSpeed_K/WheelBasedVehicleSpeed";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum CruiseControlVehSpeed_K::CruiseCtrlActive> (CruiseCtrlActiverawValue) = message.CruiseCtrlActive.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(CruiseCtrlActiverawValue.value());
            topic = "can/CruiseControlVehSpeed_K/CruiseCtrlActive";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum CruiseControlVehSpeed_K::BrakeSwitch> (BrakeSwitchrawValue) = message.BrakeSwitch.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(BrakeSwitchrawValue.value());
            topic = "can/CruiseControlVehSpeed_K/BrakeSwitch";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum CruiseControlVehSpeed_K::ClutchSwitch> (ClutchSwitchrawValue) = message.ClutchSwitch.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ClutchSwitchrawValue.value());
            topic = "can/CruiseControlVehSpeed_K/ClutchSwitch";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum CruiseControlVehSpeed_K::PTOState> (PTOStaterawValue) = message.PTOState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PTOStaterawValue.value());
            topic = "can/CruiseControlVehSpeed_K/PTOState";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF4150: {
            constexpr auto message = canID_0x99FF4150{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum PBS1_EPB::ParkingBrakeReleaseInhibitStatus> (ParkingBrakeReleaseInhibitStatusrawValue) = message.ParkingBrakeReleaseInhibitStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitStatusrawValue.value());
            topic = "can/PBS1_EPB/ParkingBrakeReleaseInhibitStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum PBS1_EPB::ParkingBrakeStatus> (ParkingBrakeStatusrawValue) = message.ParkingBrakeStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ParkingBrakeStatusrawValue.value());
            topic = "can/PBS1_EPB/ParkingBrakeStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum PBS1_EPB::ParkingBrakeReleaseInhibitReason> (ParkingBrakeReleaseInhibitReasonrawValue) = message.ParkingBrakeReleaseInhibitReason.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitReasonrawValue.value());
            topic = "can/PBS1_EPB/ParkingBrakeReleaseInhibitReason";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8DFF404A: {
            constexpr auto message = canID_0x8DFF404A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum PBC1_RTC::ParkingBrakeReleaseInhibit> (ParkingBrakeReleaseInhibitrawValue) = message.ParkingBrakeReleaseInhibit.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitrawValue.value());
            topic = "can/PBC1_RTC/ParkingBrakeReleaseInhibit";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum PBC1_RTC::ParkingBrakeReleaseInhibitRequestSource> (ParkingBrakeReleaseInhibitRequestSourcerawValue) = message.ParkingBrakeReleaseInhibitRequestSource.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitRequestSourcerawValue.value());
            topic = "can/PBC1_RTC/ParkingBrakeReleaseInhibitRequestSource";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (XBRMessageCounterrawValue) = message.XBRMessageCounter.GetRawValue(canData);
            rawValue8 = XBRMessageCounterrawValue.value();
            topic = "can/PBC1_RTC/XBRMessageCounter";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (XBRMessageChecksumrawValue) = message.XBRMessageChecksum.GetRawValue(canData);
            rawValue8 = XBRMessageChecksumrawValue.value();
            topic = "can/PBC1_RTC/XBRMessageChecksum";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CFFFC4A: {
            constexpr auto message = canID_0x8CFFFC4A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum ExternalControlMessage_RTC::EngineStartAllowed> (EngineStartAllowedrawValue) = message.EngineStartAllowed.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EngineStartAllowedrawValue.value());
            topic = "can/ExternalControlMessage_RTC/EngineStartAllowed";

            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF8227: {
            constexpr auto message = canID_0x98FF8227{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum DLN9_K::PropulsionState> (PropulsionStaterawValue) = message.PropulsionState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PropulsionStaterawValue.value());
            topic = "can/DLN9_K/PropulsionState";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DLN9_K::ElectricDriveModeRequest> (ElectricDriveModeRequestrawValue) = message.ElectricDriveModeRequest.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ElectricDriveModeRequestrawValue.value());
            topic = "can/DLN9_K/ElectricDriveModeRequest";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98F00503: {
            constexpr auto message = canID_0x98F00503{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum ETC2_T::SelectedGear> (SelectedGearrawValue) = message.SelectedGear.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SelectedGearrawValue.value());
            topic = "can/ETC2_T/SelectedGear";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ETC2_T::CurrentGear> (CurrentGearrawValue) = message.CurrentGear.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(CurrentGearrawValue.value());
            topic = "can/ETC2_T/CurrentGear";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEF34A: {
            constexpr auto message = canID_0x98FEF34A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum VehiclePosition_1000_RTC::Latitude> (LatituderawValue) = message.Latitude.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(LatituderawValue.value());
            topic = "can/VehiclePosition_1000_RTC/Latitude";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum VehiclePosition_1000_RTC::Longitude> (LongituderawValue) = message.Longitude.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(LongituderawValue.value());
            topic = "can/VehiclePosition_1000_RTC/Longitude";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FEE84A: {
            constexpr auto message = canID_0x98FEE84A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::CompassBearing> (CompassBearingrawValue) = message.CompassBearing.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(CompassBearingrawValue.value());
            topic = "can/VehiDirSpeed_1000_RTC/CompassBearing";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum VehiDirSpeed_1000_RTC::NavigationBasedVehicleSpeed> (NavigationBasedVehicleSpeedrawValue) = message.NavigationBasedVehicleSpeed.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(NavigationBasedVehicleSpeedrawValue.value());
            topic = "can/VehiDirSpeed_1000_RTC/NavigationBasedVehicleSpeed";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum VehiDirSpeed_1000_RTC::Pitch> (PitchrawValue) = message.Pitch.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(PitchrawValue.value());
            topic = "can/VehiDirSpeed_1000_RTC/Pitch";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum VehiDirSpeed_1000_RTC::Altitude> (AltituderawValue) = message.Altitude.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(AltituderawValue.value());
            topic = "can/VehiDirSpeed_1000_RTC/Altitude";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FF6E03: {
            constexpr auto message = canID_0x98FF6E03{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum TI_T::EcoRollActive> (EcoRollActiverawValue) = message.EcoRollActive.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EcoRollActiverawValue.value());
            topic = "can/TI_T/EcoRollActive";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TI_T::GearboxInReverse> (GearboxInReverserawValue) = message.GearboxInReverse.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(GearboxInReverserawValue.value());
            topic = "can/TI_T/GearboxInReverse";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CFF04EF: {
            constexpr auto message = canID_0x8CFF04EF{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum SecondaryWakeUp_HMS::WakeUp_RTC> (WakeUp_RTCrawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(WakeUp_RTCrawValue.value());
            topic = "can/SecondaryWakeUp_HMS/WakeUp_RTC";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FD7D17: {
            constexpr auto message = canID_0x98FD7D17{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleBlockID> (TelltaleBlockIDrawValue) = message.TelltaleBlockID.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleBlockIDrawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleBlockID";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus1> (TelltaleStatus1rawValue) = message.TelltaleStatus1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus1rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus2> (TelltaleStatus2rawValue) = message.TelltaleStatus2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus2rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus3> (TelltaleStatus3rawValue) = message.TelltaleStatus3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus3rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus4> (TelltaleStatus4rawValue) = message.TelltaleStatus4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus4rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus5> (TelltaleStatus5rawValue) = message.TelltaleStatus5.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus5rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus5";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus6> (TelltaleStatus6rawValue) = message.TelltaleStatus6.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus6rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus6";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus7> (TelltaleStatus7rawValue) = message.TelltaleStatus7.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus7rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus7";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus8> (TelltaleStatus8rawValue) = message.TelltaleStatus8.GetEnumValue(canData);
            topic = "can/TelltaleStatus_ICL/TelltaleStatus8";
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus8rawValue.value());
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus9> (TelltaleStatus9rawValue) = message.TelltaleStatus9.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus9rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus9";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus10> (TelltaleStatus10rawValue) = message.TelltaleStatus10.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus10rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus10";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus11> (TelltaleStatus11rawValue) = message.TelltaleStatus11.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus11rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus11";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus12> (TelltaleStatus12rawValue) = message.TelltaleStatus12.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus12rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus12";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus13> (TelltaleStatus13rawValue) = message.TelltaleStatus13.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus13rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus13";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus14> (TelltaleStatus14rawValue) = message.TelltaleStatus14.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus14rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus14";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus15> (TelltaleStatus15rawValue) = message.TelltaleStatus15.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TelltaleStatus15rawValue.value());
            topic = "can/TelltaleStatus_ICL/TelltaleStatus15";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF6B27: {
            constexpr auto message = canID_0x98FF6B27{};
            
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum GPM3_K::LowEngineCoolantWaterLevel> (LowEngineCoolantWaterLevelrawValue) = message.LowEngineCoolantWaterLevel.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(LowEngineCoolantWaterLevelrawValue.value());
            topic = "can/GPM3_K/LowEngineCoolantWaterLevel";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum GPM3_K::VehicleSpeedImplausible> (VehicleSpeedImplausiblerawValue) = message.VehicleSpeedImplausible.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(VehicleSpeedImplausiblerawValue.value());
            topic = "can/GPM3_K/VehicleSpeedImplausible";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum GPM3_K::SeatBeltReminder> (SeatBeltReminderrawValue) = message.SeatBeltReminder.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(SeatBeltReminderrawValue.value());
            topic = "can/GPM3_K/SeatBeltReminder";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum GPM3_K::TotalPropulsionStateReadyHours> (TotalPropulsionStateReadyHoursrawValue) = message.TotalPropulsionStateReadyHours.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TotalPropulsionStateReadyHoursrawValue.value());
            topic = "can/GPM3_K/TotalPropulsionStateReadyHours";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FFAE27: {
            constexpr auto message = canID_0x98FFAE27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum ServiceInformationProp_K::ServiceDistanceProp> (ServiceDistanceProprawValue) = message.ServiceDistanceProp.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ServiceDistanceProprawValue.value());
            topic = "can/ServiceInformationProp_K/ServiceDistanceProp";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ServiceInformationProp_K::ServiceTime> (ServiceTimerawValue) = message.ServiceTime.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ServiceTimerawValue.value());
            topic = "can/ServiceInformationProp_K/ServiceTime";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ServiceInformationProp_K::ServiceDistanceWarning> (ServiceDistanceWarningrawValue) = message.ServiceDistanceWarning.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ServiceDistanceWarningrawValue.value());
            topic = "can/ServiceInformationProp_K/ServiceDistanceWarning";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ServiceInformationProp_K::OperationalTimeWarning> (OperationalTimeWarningrawValue) = message.OperationalTimeWarning.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OperationalTimeWarningrawValue.value());
            topic = "can/ServiceInformationProp_K/OperationalTimeWarning";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FFBE1D: {
            constexpr auto message = canID_0x98FFBE1D{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmOnOffStatus> (AlarmOnOffStatusrawValue) = message.AlarmOnOffStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(AlarmOnOffStatusrawValue.value());
            topic = "can/AlarmStatusProp_ALM/AlarmOnOffStatus";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmModeStatus> (AlarmModeStatusrawValue) = message.AlarmModeStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(AlarmModeStatusrawValue.value());
            topic = "can/AlarmStatusProp_ALM/AlarmModeStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmSystemActivationReason> (AlarmSystemActivationReasonrawValue) = message.AlarmSystemActivationReason.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(AlarmSystemActivationReasonrawValue.value());
            topic = "can/AlarmStatusProp_ALM/AlarmSystemActivationReason";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorIgnition> (SensorIgnitionrawValue) = message.SensorIgnition.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorIgnitionrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorIgnition";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::PanicAlarmStatus> (PanicAlarmStatusrawValue) = message.PanicAlarmStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PanicAlarmStatusrawValue.value());
            topic = "can/AlarmStatusProp_ALM/PanicAlarmStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDriverDoor> (SensorDriverDoorrawValue) = message.SensorDriverDoor.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorDriverDoorrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorDriverDoor";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorPassengerDoor> (SensorPassengerDoorrawValue) = message.SensorPassengerDoor.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorPassengerDoorrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorPassengerDoor";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDoorBehindDriver> (SensorDoorBehindDriverrawValue) = message.SensorDoorBehindDriver.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorDoorBehindDriverrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorDoorBehindDriver";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDoorBehindPassenger> (SensorDoorBehindPassengerrawValue) = message.SensorDoorBehindPassenger.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorDoorBehindPassengerrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorDoorBehindPassenger";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorCargoDoor> (SensorCargoDoorrawValue) = message.SensorCargoDoor.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorCargoDoorrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorCargoDoor";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxDriverSideUpper> (SensorStorageBoxDriverSideUpperrawValue) = message.SensorStorageBoxDriverSideUpper.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorStorageBoxDriverSideUpperrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorStorageBoxDriverSideUpper";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxDriverSideLower> (SensorStorageBoxDriverSideLowerrawValue) = message.SensorStorageBoxDriverSideLower.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorStorageBoxDriverSideLowerrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorStorageBoxDriverSideLower";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxPassengerSideUpper> (SensorStorageBoxPassengerSideUpperrawValue) = message.SensorStorageBoxPassengerSideUpper.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorStorageBoxPassengerSideUpperrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorStorageBoxPassengerSideUpper";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxPassengerSideLower> (SensorStorageBoxPassengerSideLowerrawValue) = message.SensorStorageBoxPassengerSideLower.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorStorageBoxPassengerSideLowerrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorStorageBoxPassengerSideLower";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorResAnalogue> (SensorResAnaloguerawValue) = message.SensorResAnalogue.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorResAnaloguerawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorResAnalogue";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFrontGrille> (SensorFrontGrillerawValue) = message.SensorFrontGrille.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorFrontGrillerawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorFrontGrille";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorRoofHatch> (SensorRoofHatchrawValue) = message.SensorRoofHatch.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorRoofHatchrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorRoofHatch";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra1> (SensorExtra1rawValue) = message.SensorExtra1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorExtra1rawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorExtra1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra2> (SensorExtra2rawValue) = message.SensorExtra2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorExtra2rawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorExtra2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra3> (SensorExtra3rawValue) = message.SensorExtra3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorExtra3rawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorExtra3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorSiren> (SensorSirenrawValue) = message.SensorSiren.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorSirenrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorSiren";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFTAStatus> (SensorFTAStatusrawValue) = message.SensorFTAStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorFTAStatusrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorFTAStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorMotion> (SensorMotionrawValue) = message.SensorMotion.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorMotionrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorMotion";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorInclination> (SensorInclinationrawValue) = message.SensorInclination.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorInclinationrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorInclination";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFTATheft> (SensorFTATheftrawValue) = message.SensorFTATheft.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SensorFTATheftrawValue.value());
            topic = "can/AlarmStatusProp_ALM/SensorFTATheft";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CFF7427: {
            constexpr auto message = canID_0x8CFF7427{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum PowertrainControl2_K::AcceleratorPedalPosition> (AcceleratorPedalPositionrawValue) = message.AcceleratorPedalPosition.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(AcceleratorPedalPositionrawValue.value());
            topic = "can/PowertrainControl2_K/AcceleratorPedalPosition";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF2327: {
            constexpr auto message = canID_0x99FF2327{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum AccumulatedTripDataParams_K::AccumulatedTripDataParams> (AccumulatedTripDataParamsrawValue) = message.AccumulatedTripDataParams.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(AccumulatedTripDataParamsrawValue.value());
            topic = "can/AccumulatedTripDataParams_K/AccumulatedTripDataParams";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_ForcedChargingMode_0x16> (Distance_ForcedChargingMode_0x16rawValue) = message.Distance_ForcedChargingMode_0x16.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Distance_ForcedChargingMode_0x16rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Distance_ForcedChargingMode_0x16";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_ForcedElectricMode_0x15> (Distance_ForcedElectricMode_0x15rawValue) = message.Distance_ForcedElectricMode_0x15.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Distance_ForcedElectricMode_0x15rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Distance_ForcedElectricMode_0x15";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_IdlStopStartDeactivated_0x18> (Distance_IdlStopStartDeactivated_0x18rawValue) = message.Distance_IdlStopStartDeactivated_0x18.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Distance_IdlStopStartDeactivated_0x18rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Distance_IdlStopStartDeactivated_0x18";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_PTO_In_Drive_0x13> (Distance_PTO_In_Drive_0x13rawValue) = message.Distance_PTO_In_Drive_0x13.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Distance_PTO_In_Drive_0x13rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Distance_PTO_In_Drive_0x13";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_TotalAccumulated_0x10> (Distance_TotalAccumulated_0x10rawValue) = message.Distance_TotalAccumulated_0x10.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Distance_TotalAccumulated_0x10rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Distance_TotalAccumulated_0x10";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::DistanceInPause_PTO_In_Drive_0x63> (DistanceInPause_PTO_In_Drive_0x63rawValue) = message.DistanceInPause_PTO_In_Drive_0x63.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(DistanceInPause_PTO_In_Drive_0x63rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/DistanceInPause_PTO_In_Drive_0x63";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::DistanceInPause_TotalAccumulated_0x60> (DistanceInPause_TotalAccumulated_0x60rawValue) = message.DistanceInPause_TotalAccumulated_0x60.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(DistanceInPause_TotalAccumulated_0x60rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/DistanceInPause_TotalAccumulated_0x60";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_Idle_0x41> (EnergyUsed_Idle_0x41rawValue) = message.EnergyUsed_Idle_0x41.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(EnergyUsed_Idle_0x41rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/EnergyUsed_Idle_0x41";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_PTO_In_Drive_0x43> (EnergyUsed_PTO_In_Drive_0x43rawValue) = message.EnergyUsed_PTO_In_Drive_0x43.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(EnergyUsed_PTO_In_Drive_0x43rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/EnergyUsed_PTO_In_Drive_0x43";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_PTO_In_Idle_0x42> (EnergyUsed_PTO_In_Idle_0x42rawValue) = message.EnergyUsed_PTO_In_Idle_0x42.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(EnergyUsed_PTO_In_Idle_0x42rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/EnergyUsed_PTO_In_Idle_0x42";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_TotalAccumulated_0x40> (EnergyUsed_TotalAccumulated_0x40rawValue) = message.EnergyUsed_TotalAccumulated_0x40.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(EnergyUsed_TotalAccumulated_0x40rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/EnergyUsed_TotalAccumulated_0x40";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_Idle_0x31> (FuelGaseous_Idle_0x31rawValue) = message.FuelGaseous_Idle_0x31.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelGaseous_Idle_0x31rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelGaseous_Idle_0x31";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_PTO_In_Drive_0x33> (FuelGaseous_PTO_In_Drive_0x33rawValue) = message.FuelGaseous_PTO_In_Drive_0x33.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelGaseous_PTO_In_Drive_0x33rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelGaseous_PTO_In_Drive_0x33";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_PTO_In_Idle_0x32> (FuelGaseous_PTO_In_Idle_0x32rawValue) = message.FuelGaseous_PTO_In_Idle_0x32.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelGaseous_PTO_In_Idle_0x32rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelGaseous_PTO_In_Idle_0x32";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_TotalAccumulated_0x30> (FuelGaseous_TotalAccumulated_0x30rawValue) = message.FuelGaseous_TotalAccumulated_0x30.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelGaseous_TotalAccumulated_0x30rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelGaseous_TotalAccumulated_0x30";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_Idle_0x21> (FuelLiquid_Idle_0x21rawValue) = message.FuelLiquid_Idle_0x21.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelLiquid_Idle_0x21rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelLiquid_Idle_0x21";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_PTO_In_Drive_0x23> (FuelLiquid_PTO_In_Drive_0x23rawValue) = message.FuelLiquid_PTO_In_Drive_0x23.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelLiquid_PTO_In_Drive_0x23rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelLiquid_PTO_In_Drive_0x23";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_PTO_In_Idle_0x22> (FuelLiquid_PTO_In_Idle_0x22rawValue) = message.FuelLiquid_PTO_In_Idle_0x22.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelLiquid_PTO_In_Idle_0x22rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelLiquid_PTO_In_Idle_0x22";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_TotalAccumulated_0x20> (FuelLiquid_TotalAccumulated_0x20rawValue) = message.FuelLiquid_TotalAccumulated_0x20.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(FuelLiquid_TotalAccumulated_0x20rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/FuelLiquid_TotalAccumulated_0x20";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_ForcedChargingMode_0x06> (Time_ForcedChargingMode_0x06rawValue) = message.Time_ForcedChargingMode_0x06.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Time_ForcedChargingMode_0x06rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Time_ForcedChargingMode_0x06";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_ForcedElectricMode_0x05> (Time_ForcedElectricMode_0x05rawValue) = message.Time_ForcedElectricMode_0x05.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Time_ForcedElectricMode_0x05rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Time_ForcedElectricMode_0x05";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_Idle_0x01> (Time_Idle_0x01rawValue) = message.Time_Idle_0x01.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Time_Idle_0x01rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Time_Idle_0x01";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_IdlStopStartDeactivated_0x08> (Time_IdlStopStartDeactivated_0x08rawValue) = message.Time_IdlStopStartDeactivated_0x08.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Time_IdlStopStartDeactivated_0x08rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Time_IdlStopStartDeactivated_0x08";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_PTO_In_Drive_0x03> (Time_PTO_In_Drive_0x03rawValue) = message.Time_PTO_In_Drive_0x03.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Time_PTO_In_Drive_0x03rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Time_PTO_In_Drive_0x03";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_PTO_In_Idle_0x02> (Time_PTO_In_Idle_0x02rawValue) = message.Time_PTO_In_Idle_0x02.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Time_PTO_In_Idle_0x02rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Time_PTO_In_Idle_0x02";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_TotalAccumulated_0x00> (Time_TotalAccumulated_0x00rawValue) = message.Time_TotalAccumulated_0x00.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(Time_TotalAccumulated_0x00rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/Time_TotalAccumulated_0x00";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_Idle_0x51> (TimeInPause_Idle_0x51rawValue) = message.TimeInPause_Idle_0x51.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TimeInPause_Idle_0x51rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/TimeInPause_Idle_0x51";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_PTO_In_Drive_0x53> (TimeInPause_PTO_In_Drive_0x53rawValue) = message.TimeInPause_PTO_In_Drive_0x53.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TimeInPause_PTO_In_Drive_0x53rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/TimeInPause_PTO_In_Drive_0x53";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_PTO_In_Idle_0x52> (TimeInPause_PTO_In_Idle_0x52rawValue) = message.TimeInPause_PTO_In_Idle_0x52.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TimeInPause_PTO_In_Idle_0x52rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/TimeInPause_PTO_In_Idle_0x52";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_TotalAccumulated_0x50> (TimeInPause_TotalAccumulated_0x50rawValue) = message.TimeInPause_TotalAccumulated_0x50.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TimeInPause_TotalAccumulated_0x50rawValue.value());
            topic = "can/AccumulatedTripDataParams_K/TimeInPause_TotalAccumulated_0x50";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FEF527: {
            constexpr auto message = canID_0x98FEF527{};
            
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            ara::core::Optional<enum AmbientConditions_K::AmbientAirTemperature> (AmbientAirTemperaturerawValue) = message.AmbientAirTemperature.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(AmbientAirTemperaturerawValue.value());
            topic = "can/AmbientConditions_K/AmbientAirTemperature";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x99FF3C27: {
            constexpr auto message = canID_0x99FF3C27{};
            
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            ara::core::Optional<enum GPM10_K::AuxiliaryAmbientAirTemperature> (AuxiliaryAmbientAirTemperaturerawValue) = message.AuxiliaryAmbientAirTemperature.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(AuxiliaryAmbientAirTemperaturerawValue.value());
            topic = "can/GPM10_K/AuxiliaryAmbientAirTemperature";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FE6B27: {
            constexpr auto message = canID_0x98FE6B27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::int8_t> (Driver1IdentifierrawValue) = message.Driver1Identifier.GetRawValue(canData);
            rawValue8 = Driver1IdentifierrawValue.value();
            topic = "can/DI_OnChange_K_FF/Driver1Identifier";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::int8_t> (Delimiter1rawValue) = message.Delimiter1.GetRawValue(canData);
            rawValue8 = Delimiter1rawValue.value();
            topic = "can/DI_OnChange_K_FF/Delimiter1";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::int8_t> (Driver2IdentifierrawValue) = message.Driver2Identifier.GetRawValue(canData);
            rawValue8 = Driver2IdentifierrawValue.value();
            topic = "can/DI_OnChange_K_FF/Driver2Identifier";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::int8_t> (Delimiter2rawValue) = message.Delimiter2.GetRawValue(canData);
            rawValue8 = Delimiter2rawValue.value();
            topic = "can/DI_OnChange_K_FF/Delimiter2";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98ECFF27: {
            constexpr auto message = canID_0x98ECFF27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TP_CM_K_FF::ControlByteTP_CM> (ControlByteTP_CMrawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ControlByteTP_CMrawValue.value());
            topic = "can/TP_CM_K_FF/ControlByteTP_CM";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint16_t> (TPCM_BAM_TotalMessageSizerawValue) = message.TPCM_BAM_TotalMessageSize.GetRawValue(canData);
            rawValue16 = TPCM_BAM_TotalMessageSizerawValue.value();
            topic = "can/TP_CM_K_FF/TPCM_BAM_TotalMessageSize";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<std::uint8_t> (TPCM_BAM_TotalNumberOfPacketsrawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            rawValue8 = TPCM_BAM_TotalNumberOfPacketsrawValue.value();
            topic = "can/TP_CM_K_FF/TPCM_BAM_TotalNumberOfPackets";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint32_t> (TPCM_BAM_PGNumberrawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            rawValue32 = TPCM_BAM_PGNumberrawValue.value();
            topic = "can/TP_CM_K_FF/TPCM_BAM_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98EBFF27: {
            constexpr auto message = canID_0x98EBFF27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint8_t> (SequenceNumberrawValue) = message.SequenceNumber.GetRawValue(canData);
            rawValue8 = SequenceNumberrawValue.value();
            topic = "can/TP_DT_K_FF/SequenceNumber";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (Byte1rawValue) = message.Byte1.GetRawValue(canData);
            rawValue8 = Byte1rawValue.value();
            topic = "can/TP_DT_K_FF/Byte1";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (Byte2rawValue) = message.Byte2.GetRawValue(canData);
            rawValue8 = Byte2rawValue.value();
            topic = "can/TP_DT_K_FF/Byte2";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (Byte3rawValue) = message.Byte3.GetRawValue(canData);
            rawValue8 = Byte3rawValue.value();
            topic = "can/TP_DT_K_FF/Byte3";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (Byte4rawValue) = message.Byte4.GetRawValue(canData);
            rawValue8 = Byte4rawValue.value();
            topic = "can/TP_DT_K_FF/Byte4";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (Byte5rawValue) = message.Byte5.GetRawValue(canData);
            rawValue8 = Byte5rawValue.value();
            topic = "can/TP_DT_K_FF/Byte5";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (Byte6rawValue) = message.Byte6.GetRawValue(canData);
            rawValue8 = Byte6rawValue.value();
            topic = "can/TP_DT_K_FF/Byte6";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (Byte7rawValue) = message.Byte7.GetRawValue(canData);
            rawValue8 = Byte7rawValue.value();
            topic = "can/TP_DT_K_FF/Byte7";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF9027: {
            constexpr auto message = canID_0x98FF9027{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum PTOInformationProp_K::PTO_AL1> (PTO_AL1rawValue) = message.PTO_AL1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PTO_AL1rawValue.value());
            topic = "can/PTOInformationProp_K/PTO_AL1";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum PTOInformationProp_K::PTO_AL2> (PTO_AL2rawValue) = message.PTO_AL2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PTO_AL2rawValue.value());
            topic = "can/PTOInformationProp_K/PTO_AL2";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum PTOInformationProp_K::PTO_EG3> (PTO_EG3rawValue) = message.PTO_EG3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PTO_EG3rawValue.value());
            topic = "can/PTOInformationProp_K/PTO_EG3";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum PTOInformationProp_K::PTO_Engaged> (PTO_EngagedrawValue) = message.PTO_Engaged.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PTO_EngagedrawValue.value());
            topic = "can/PTOInformationProp_K/PTO_Engaged";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x9CFF0627: {
            constexpr auto message = canID_0x9CFF0627{};
            
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory1> (DriverEvaluationCategory1rawValue) = message.DriverEvaluationCategory1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverEvaluationCategory1rawValue.value());
            topic = "can/DriverEvaluationData_K/DriverEvaluationCategory1";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory2> (DriverEvaluationCategory2rawValue) = message.DriverEvaluationCategory2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverEvaluationCategory2rawValue.value());
            topic = "can/DriverEvaluationData_K/DriverEvaluationCategory2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory3> (DriverEvaluationCategory3rawValue) = message.DriverEvaluationCategory3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverEvaluationCategory3rawValue.value());
            topic = "can/DriverEvaluationData_K/DriverEvaluationCategory3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory4> (DriverEvaluationCategory4rawValue) = message.DriverEvaluationCategory4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverEvaluationCategory4rawValue.value());
            topic = "can/DriverEvaluationData_K/DriverEvaluationCategory4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory1> (EvaluationActiveCategory1rawValue) = message.EvaluationActiveCategory1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EvaluationActiveCategory1rawValue.value());
            topic = "can/DriverEvaluationData_K/EvaluationActiveCategory1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory2> (EvaluationActiveCategory2rawValue) = message.EvaluationActiveCategory2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EvaluationActiveCategory2rawValue.value());
            topic = "can/DriverEvaluationData_K/EvaluationActiveCategory2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory3> (EvaluationActiveCategory3rawValue) = message.EvaluationActiveCategory3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EvaluationActiveCategory3rawValue.value());
            topic = "can/DriverEvaluationData_K/EvaluationActiveCategory3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory4> (EvaluationActiveCategory4rawValue) = message.EvaluationActiveCategory4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EvaluationActiveCategory4rawValue.value());
            topic = "can/DriverEvaluationData_K/EvaluationActiveCategory4";
            publishSignal(topic, rawValue8, msqt_pub);

            }
            break;
        case 0x9CFF1327: {
            constexpr auto message = canID_0x9CFF1327{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat1> (InstEventWeight_Cat1rawValue) = message.InstEventWeight_Cat1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventWeight_Cat1rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventWeight_Cat1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat2> (InstEventWeight_Cat2rawValue) = message.InstEventWeight_Cat2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventWeight_Cat2rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventWeight_Cat2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat3> (InstEventWeight_Cat3rawValue) = message.InstEventWeight_Cat3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventWeight_Cat3rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventWeight_Cat3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat4> (InstEventWeight_Cat4rawValue) = message.InstEventWeight_Cat4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventWeight_Cat4rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventWeight_Cat4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat1> (InstEventGrade_Cat1rawValue) = message.InstEventGrade_Cat1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventGrade_Cat1rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventGrade_Cat1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat2> (InstEventGrade_Cat2rawValue) = message.InstEventGrade_Cat2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventGrade_Cat2rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventGrade_Cat2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat3> (InstEventGrade_Cat3rawValue) = message.InstEventGrade_Cat3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventGrade_Cat3rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventGrade_Cat3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat4> (InstEventGrade_Cat4rawValue) = message.InstEventGrade_Cat4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(InstEventGrade_Cat4rawValue.value());
            topic = "can/DriverEvaluationEvents_K/InstEventGrade_Cat4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DriverEvaluationEvents_K::DriverEvaluationVersionNumber> (DriverEvaluationVersionNumberrawValue) = message.DriverEvaluationVersionNumber.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverEvaluationVersionNumberrawValue.value());
            topic = "can/DriverEvaluationEvents_K/DriverEvaluationVersionNumber";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98EC4A27: {
            constexpr auto message = canID_0x98EC4A27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TP_CM_K_RTC::ControlByteTP_CM> (ControlByteTP_CMrawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ControlByteTP_CMrawValue.value());
            topic = "can/TP_CM_K_RTC/ControlByteTP_CM";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint16_t> (TPCM_RTS_TotalMessageSizerawValue) = message.TPCM_RTS_TotalMessageSize.GetRawValue(canData);
            rawValue16 = TPCM_RTS_TotalMessageSizerawValue.value();
            topic = "can/TP_CM_K_RTC/TPCM_RTS_TotalMessageSize";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<std::uint8_t> (TPCM_BAM_TotalNumberOfPacketsrawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            rawValue8 = TPCM_BAM_TotalNumberOfPacketsrawValue.value();
            topic = "can/TP_CM_K_RTC/TPCM_BAM_TotalNumberOfPackets";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint32_t> (TPCM_BAM_PGNumberrawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            rawValue32 = TPCM_BAM_PGNumberrawValue.value();
            topic = "can/TP_CM_K_RTC/TPCM_BAM_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98EB4A27: {
            constexpr auto message = canID_0x98EB4A27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint8_t> (SequenceNumberrawValue) = message.SequenceNumber.GetRawValue(canData);
            rawValue8 = SequenceNumberrawValue.value();
            topic = "can/TP_DT_K_RTC/SequenceNumber";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte1rawValue) = message.Byte1.GetRawValue(canData);
            rawValue8 = Byte1rawValue.value();
            topic = "can/TP_DT_K_RTC/Byte1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte2rawValue) = message.Byte2.GetRawValue(canData);
            rawValue8 = Byte2rawValue.value();
            topic = "can/TP_DT_K_RTC/Byte2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte3rawValue) = message.Byte3.GetRawValue(canData);
            rawValue8 = Byte3rawValue.value();
            topic = "can/TP_DT_K_RTC/Byte3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte4rawValue) = message.Byte4.GetRawValue(canData);
            rawValue8 = Byte4rawValue.value();
            topic = "can/TP_DT_K_RTC/Byte4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte5rawValue) = message.Byte5.GetRawValue(canData);
            rawValue8 = Byte5rawValue.value();
            topic = "can/TP_DT_K_RTC/Byte5";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte6rawValue) = message.Byte6.GetRawValue(canData);
            rawValue8 = Byte6rawValue.value();
            topic = "can/TP_DT_K_RTC/Byte6";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte7rawValue) = message.Byte7.GetRawValue(canData);
            rawValue8 = Byte7rawValue.value();
            topic = "can/TP_DT_K_RTC/Byte7";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEEC27: {
            constexpr auto message = canID_0x98FEEC27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::int8_t> (VehicleIdentificationrawValue) = message.VehicleIdentification.GetRawValue(canData);
            rawValue8 = VehicleIdentificationrawValue.value();
            topic = "can/VehicleId_K_RTC/VehicleIdentification";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::int8_t> (DelimiterrawValue) = message.Delimiter.GetRawValue(canData);
            rawValue8 = DelimiterrawValue.value();
            topic = "can/VehicleId_K_RTC/Delimiter";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FFB11E: {
            constexpr auto message = canID_0x98FFB11E{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum CUVInformation_V::HeadLampFailure> (HeadLampFailurerawValue) = message.HeadLampFailure.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(HeadLampFailurerawValue.value());
            topic = "can/CUVInformation_V/HeadLampFailure";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum CUVInformation_V::HazardWarningSignalStatus> (HazardWarningSignalStatusrawValue) = message.HazardWarningSignalStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(HazardWarningSignalStatusrawValue.value());
            topic = "can/CUVInformation_V/HazardWarningSignalStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum CUVInformation_V::TrailerDetection> (TrailerDetectionrawValue) = message.TrailerDetection.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TrailerDetectionrawValue.value());
            topic = "can/CUVInformation_V/TrailerDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FFD517: {
            constexpr auto message = canID_0x98FFD517{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum ICLInformationProprietary_ICL::PerformanceMode> (PerformanceModerawValue) = message.PerformanceMode.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PerformanceModerawValue.value());
            topic = "can/ICLInformationProprietary_ICL/PerformanceMode";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF8400: {
            constexpr auto message = canID_0x99FF8400{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum ReductantDTEInformation_E::TotalReductantUsed> (TotalReductantUsedrawValue) = message.TotalReductantUsed.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TotalReductantUsedrawValue.value());
            topic = "can/ReductantDTEInformation_E/TotalReductantUsed";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x8CFE6CEE: {
            constexpr auto message = canID_0x8CFE6CEE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum TCO1_TCO::Driver1WorkingState> (Driver1WorkingStaterawValue) = message.Driver1WorkingState.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(Driver1WorkingStaterawValue.value());
            topic = "can/TCO1_TCO/Driver1WorkingState";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::Driver2WorkingState> (Driver2WorkingStaterawValue) = message.Driver2WorkingState.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(Driver2WorkingStaterawValue.value());
            topic = "can/TCO1_TCO/Driver2WorkingState";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::VehicleMotion> (VehicleMotionrawValue) = message.VehicleMotion.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VehicleMotionrawValue.value());
            topic = "can/TCO1_TCO/VehicleMotion";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::Driver1TimeRelatedStates> (Driver1TimeRelatedStatesrawValue) = message.Driver1TimeRelatedStates.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(Driver1TimeRelatedStatesrawValue.value());
            topic = "can/TCO1_TCO/Driver1TimeRelatedStates";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::DriverCardDriver1> (DriverCardDriver1rawValue) = message.DriverCardDriver1.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(DriverCardDriver1rawValue.value());
            topic = "can/TCO1_TCO/DriverCardDriver1";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::Overspeed> (OverspeedrawValue) = message.Overspeed.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(OverspeedrawValue.value());
            topic = "can/TCO1_TCO/Overspeed";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::Driver2TimeRelatedStates> (Driver2TimeRelatedStatesrawValue) = message.Driver2TimeRelatedStates.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(Driver2TimeRelatedStatesrawValue.value());
            topic = "can/TCO1_TCO/Driver2TimeRelatedStates";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::DriverCardDriver2> (DriverCardDriver2rawValue) = message.DriverCardDriver2.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(DriverCardDriver2rawValue.value());
            topic = "can/TCO1_TCO/DriverCardDriver2";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::SystemEvent> (SystemEventrawValue) = message.SystemEvent.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(SystemEventrawValue.value());
            topic = "can/TCO1_TCO/SystemEvent";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::HandlingInformation> (HandlingInformationrawValue) = message.HandlingInformation.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(HandlingInformationrawValue.value());
            topic = "can/TCO1_TCO/HandlingInformation";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::SystemPerformance> (SystemPerformancerawValue) = message.SystemPerformance.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(SystemPerformancerawValue.value());
            topic = "can/TCO1_TCO/SystemPerformance";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::DirectionIndicator> (DirectionIndicatorrawValue) = message.DirectionIndicator.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(DirectionIndicatorrawValue.value());
            topic = "can/TCO1_TCO/DirectionIndicator";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum TCO1_TCO::TCOVehSpeed> (TCOVehSpeedrawValue) = message.TCOVehSpeed.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(TCOVehSpeedrawValue.value());
            topic = "can/TCO1_TCO/TCOVehSpeed";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FEC1EE: {
            constexpr auto message = canID_0x98FEC1EE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum HiResVehicleDist_TCO::HighResolTotalVehicleDistance> (HighResolTotalVehicleDistancerawValue) = message.HighResolTotalVehicleDistance.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(HighResolTotalVehicleDistancerawValue.value());
            topic = "can/HiResVehicleDist_TCO/HighResolTotalVehicleDistance";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FF400B: {
            constexpr auto message = canID_0x98FF400B{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum BSS2_A::BrakeLiningWearWarning> (BrakeLiningWearWarningrawValue) = message.BrakeLiningWearWarning.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(BrakeLiningWearWarningrawValue.value());
            topic = "can/BSS2_A/BrakeLiningWearWarning";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF5F00: {
            constexpr auto message = canID_0x98FF5F00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum EngineInfoProprietary2_E::UreaLevel> (UreaLevelrawValue) = message.UreaLevel.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(UreaLevelrawValue.value());
            topic = "can/EngineInfoProprietary2_E/UreaLevel";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum EngineInfoProprietary2_E::TorqueLimit> (TorqueLimitrawValue) = message.TorqueLimit.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TorqueLimitrawValue.value());
            topic = "can/EngineInfoProprietary2_E/TorqueLimit";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum EngineInfoProprietary2_E::EngineAirFilterStatus> (EngineAirFilterStatusrawValue) = message.EngineAirFilterStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EngineAirFilterStatusrawValue.value());
            topic = "can/EngineInfoProprietary2_E/EngineAirFilterStatus";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CF00300: {
            constexpr auto message = canID_0x8CF00300{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum EEC2_E::AcceleratorPedalPosition> (AcceleratorPedalPositionrawValue) = message.AcceleratorPedalPosition.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(AcceleratorPedalPositionrawValue.value());
            topic = "can/EEC2_E/AcceleratorPedalPosition";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum EEC2_E::PercentLoadAtCurrentSpeed> (PercentLoadAtCurrentSpeedrawValue) = message.PercentLoadAtCurrentSpeed.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PercentLoadAtCurrentSpeedrawValue.value());
            topic = "can/EEC2_E/PercentLoadAtCurrentSpeed";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FD0900: {
            constexpr auto message = canID_0x98FD0900{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum HRLFC_E::HighResEngineTotalFuelUsed> (HighResEngineTotalFuelUsedrawValue) = message.HighResEngineTotalFuelUsed.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(HighResEngineTotalFuelUsedrawValue.value());
            topic = "can/HRLFC_E/HighResEngineTotalFuelUsed";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x99FF4600: {
            constexpr auto message = canID_0x99FF4600{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum OBDInformation2_E::DegradationInformation> (DegradationInformationrawValue) = message.DegradationInformation.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(DegradationInformationrawValue.value());
            topic = "can/OBDInformation2_E/DegradationInformation";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FF7C00: {
            constexpr auto message = canID_0x98FF7C00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum PowertrainInformation3_E::LowEngineOilPressure> (LowEngineOilPressurerawValue) = message.LowEngineOilPressure.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LowEngineOilPressurerawValue.value());
            topic = "can/PowertrainInformation3_E/LowEngineOilPressure";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum PowertrainInformation3_E::HighEngineCoolantTemp> (HighEngineCoolantTemprawValue) = message.HighEngineCoolantTemp.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(HighEngineCoolantTemprawValue.value());
            topic = "can/PowertrainInformation3_E/HighEngineCoolantTemp";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum PowertrainInformation3_E::DPFRegenerationState> (DPFRegenerationStaterawValue) = message.DPFRegenerationState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DPFRegenerationStaterawValue.value());
            topic = "can/PowertrainInformation3_E/DPFRegenerationState";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CF00400: {
            constexpr auto message = canID_0x8CF00400{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum EEC1_E::ActualEngine_PercTorque> (ActualEngine_PercTorquerawValue) = message.ActualEngine_PercTorque.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(ActualEngine_PercTorquerawValue.value());
            topic = "can/EEC1_E/ActualEngine_PercTorque";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<enum EEC1_E::EngineSpeed> (EngineSpeedrawValue) = message.EngineSpeed.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(EngineSpeedrawValue.value());
            topic = "can/EEC1_E/EngineSpeed";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FEE900: {
            constexpr auto message = canID_0x98FEE900{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum FuelConsumption_E::TotalFuelUsed> (TotalFuelUsedrawValue) = message.TotalFuelUsed.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TotalFuelUsedrawValue.value());
            topic = "can/FuelConsumption_E/TotalFuelUsed";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FEF200: {
            constexpr auto message = canID_0x98FEF200{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum FuelEconomy_E::FuelRate> (FuelRaterawValue) = message.FuelRate.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(FuelRaterawValue.value());
            topic = "can/FuelEconomy_E/FuelRate";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<enum FuelEconomy_E::InstantaneousFuelEconomy> (InstantaneousFuelEconomyrawValue) = message.InstantaneousFuelEconomy.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(InstantaneousFuelEconomyrawValue.value());
            topic = "can/FuelEconomy_E/InstantaneousFuelEconomy";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FECA00: {
            constexpr auto message = canID_0x98FECA00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum DM1_E::RedStopLampStatus> (RedStopLampStatusrawValue) = message.RedStopLampStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(RedStopLampStatusrawValue.value());
            topic = "can/DM1_E/RedStopLampStatus";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF6000: {
            constexpr auto message = canID_0x98FF6000{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum GPM1_E::CalculatedGTW> (CalculatedGTWrawValue) = message.CalculatedGTW.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(CalculatedGTWrawValue.value());
            topic = "can/GPM1_E/CalculatedGTW";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x98FD7C00: {
            constexpr auto message = canID_0x98FD7C00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum DPFC1_E::DpfStatus> (DpfStatusrawValue) = message.DpfStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DpfStatusrawValue.value());
            topic = "can/DPFC1_E/DpfStatus";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF7A00: {
            constexpr auto message = canID_0x98FF7A00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum PowertrainInformation1_E::ManualRegenState> (ManualRegenStaterawValue) = message.ManualRegenState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ManualRegenStaterawValue.value());
            topic = "can/PowertrainInformation1_E/ManualRegenState";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum PowertrainInformation1_E::RegenNeedLevel> (RegenNeedLevelrawValue) = message.RegenNeedLevel.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(RegenNeedLevelrawValue.value());
            topic = "can/PowertrainInformation1_E/RegenNeedLevel";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEBF0B: {
            constexpr auto message = canID_0x98FEBF0B{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum EBC2_A::FrontAxleSpeed> (FrontAxleSpeedrawValue) = message.FrontAxleSpeed.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(FrontAxleSpeedrawValue.value());
            topic = "can/EBC2_A/FrontAxleSpeed";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (RelSpeedRearAxle_1LeftWheelrawValue) = message.RelSpeedRearAxle_1LeftWheel.GetRawValue(canData);
            rawValue8 = RelSpeedRearAxle_1LeftWheelrawValue.value();
            topic = "can/EBC2_A/RelSpeedRearAxle_1LeftWheel";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint8_t> (RelSpeedRearAxle_1RightWheelrawValue) = message.RelSpeedRearAxle_1RightWheel.GetRawValue(canData);
            rawValue8 = RelSpeedRearAxle_1RightWheelrawValue.value();
            topic = "can/EBC2_A/RelSpeedRearAxle_1RightWheel";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FFA303: {
            constexpr auto message = canID_0x98FFA303{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum TransmissionProprietary3_T::LowClutchFluidLevel> (LowClutchFluidLevelrawValue) = message.LowClutchFluidLevel.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LowClutchFluidLevelrawValue.value());
            topic = "can/TransmissionProprietary3_T/LowClutchFluidLevel";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<enum TransmissionProprietary3_T::ClutchOverloadInformation> (ClutchOverloadInformationrawValue) = message.ClutchOverloadInformation.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ClutchOverloadInformationrawValue.value());
            topic = "can/TransmissionProprietary3_T/ClutchOverloadInformation";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FDC40B: {
            constexpr auto message = canID_0x98FDC40B{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum EBC5_A::BrakeTemperatureWarning> (BrakeTemperatureWarningrawValue) = message.BrakeTemperatureWarning.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(BrakeTemperatureWarningrawValue.value());
            topic = "can/EBC5_A/BrakeTemperatureWarning";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEFC47: {
            constexpr auto message = canID_0x98FEFC47{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum DashDisplay_CMS::FuelLevel> (FuelLevelrawValue) = message.FuelLevel.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(FuelLevelrawValue.value());
            topic = "can/DashDisplay_CMS/FuelLevel";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF7B00: {
            constexpr auto message = canID_0x98FF7B00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum OBDInformation_E::TorqueLimTimeHoursOrMinutes> (TorqueLimTimeHoursOrMinutesrawValue) = message.TorqueLimTimeHoursOrMinutes.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TorqueLimTimeHoursOrMinutesrawValue.value());
            topic = "can/OBDInformation_E/TorqueLimTimeHoursOrMinutes";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum OBDInformation_E::SpeedLimTimeHoursOrMinutes> (SpeedLimTimeHoursOrMinutesrawValue) = message.SpeedLimTimeHoursOrMinutes.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SpeedLimTimeHoursOrMinutesrawValue.value());
            topic = "can/OBDInformation_E/SpeedLimTimeHoursOrMinutes";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum OBDInformation_E::TimeToTorqueLimit> (TimeToTorqueLimitrawValue) = message.TimeToTorqueLimit.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TimeToTorqueLimitrawValue.value());
            topic = "can/OBDInformation_E/TimeToTorqueLimit";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum OBDInformation_E::TimeToSpeedLimit> (TimeToSpeedLimitrawValue) = message.TimeToSpeedLimit.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TimeToSpeedLimitrawValue.value());
            topic = "can/OBDInformation_E/TimeToSpeedLimit";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FE4EE6: {
            constexpr auto message = canID_0x98FE4EE6{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum DoorControl1_BCI::PositionOfDoors> (PositionOfDoorsrawValue) = message.PositionOfDoors.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PositionOfDoorsrawValue.value());
            topic = "can/DoorControl1_BCI/PositionOfDoors";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FF6017: {
            constexpr auto message = canID_0x98FF6017{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum GPM1_ICL::ChassiNo> (ChassiNorawValue) = message.ChassiNo.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(ChassiNorawValue.value());
            topic = "can/GPM1_ICL/ChassiNo";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98E8FFEE: {
            constexpr auto message = canID_0x98E8FFEE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum Acknowledgement_TCO_FF::ControlByte_ACK> (ControlByte_ACKrawValue) = message.ControlByte_ACK.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ControlByte_ACKrawValue.value());
            topic = "can/Acknowledgement_TCO_FF/ControlByte_ACK";
            publishSignal(topic, rawValue8, msqt_pub);
            
            ara::core::Optional<std::uint32_t> (ACKM_ACK_PGNumberrawValue) = message.ACKM_ACK_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_ACK_PGNumberrawValue.value();
            topic = "can/Acknowledgement_TCO_FF/ACKM_ACK_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            
            ara::core::Optional<std::uint32_t> (ACKM_NACK_AD_PGNumberrawValue) = message.ACKM_NACK_AD_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_NACK_AD_PGNumberrawValue.value();
            topic = "can/Acknowledgement_TCO_FF/ACKM_NACK_AD_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            
            ara::core::Optional<std::uint32_t> (ACKM_NACK_Busy_PGNumberrawValue) = message.ACKM_NACK_Busy_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_NACK_Busy_PGNumberrawValue.value();
            topic = "can/Acknowledgement_TCO_FF/ACKM_NACK_Busy_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            
            ara::core::Optional<std::uint32_t> (ACKM_NACK_PGNumberrawValue) = message.ACKM_NACK_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_NACK_PGNumberrawValue.value();
            topic = "can/Acknowledgement_TCO_FF/ACKM_NACK_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98EAEE4A: {
            constexpr auto message = canID_0x98EAEE4A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<std::uint32_t> (PGNrawValue) = message.PGN.GetRawValue(canData);
            rawValue32 = PGNrawValue.value();
            topic = "can/RequestPGN_RTC_TCO/PGN";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98EA274A: {
            constexpr auto message = canID_0x98EA274A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<std::uint32_t> (PGNrawValue) = message.PGN.GetRawValue(canData);
            rawValue32 = PGNrawValue.value();
            topic = "can/RequestPGN_RTC_K/PGN";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98E8FF27: {
            constexpr auto message = canID_0x98E8FF27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum Acknowledgement_K_FF::ControlByte_ACK> (ControlByte_ACKrawValue) = message.ControlByte_ACK.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ControlByte_ACKrawValue.value());
            topic = "can/Acknowledgement_K_FF/ControlByte_ACK";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_ACK_GroupFunctionValue> (ACKM_ACK_GroupFunctionValuerawValue) = message.ACKM_ACK_GroupFunctionValue.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ACKM_ACK_GroupFunctionValuerawValue.value());
            topic = "can/Acknowledgement_K_FF/ACKM_ACK_GroupFunctionValue";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_AD_GroupFunctionValue> (ACKM_NACK_AD_GroupFunctionValuerawValue) = message.ACKM_NACK_AD_GroupFunctionValue.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ACKM_NACK_AD_GroupFunctionValuerawValue.value());
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_AD_GroupFunctionValue";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_Bsy_GroupFunctionValue> (ACKM_NACK_Bsy_GroupFunctionValuerawValue) = message.ACKM_NACK_Bsy_GroupFunctionValue.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ACKM_NACK_Bsy_GroupFunctionValuerawValue.value());
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_Bsy_GroupFunctionValue";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_GroupFunctionValue> (ACKM_NACK_GroupFunctionValuerawValue) = message.ACKM_NACK_GroupFunctionValue.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ACKM_NACK_GroupFunctionValuerawValue.value());
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_GroupFunctionValue";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ACKM_ACK_AddressrawValue) = message.ACKM_ACK_Address.GetRawValue(canData);
            rawValue8 = ACKM_ACK_AddressrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_ACK_Address";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ACKM_NACK_AD_AddressrawValue) = message.ACKM_NACK_AD_Address.GetRawValue(canData);
            rawValue8 = ACKM_NACK_AD_AddressrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_AD_Address";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ACKM_NACK_AddressrawValue) = message.ACKM_NACK_Address.GetRawValue(canData);
            rawValue8 = ACKM_NACK_AddressrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_Address";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (ACKM_NACK_Busy_AddressrawValue) = message.ACKM_NACK_Busy_Address.GetRawValue(canData);
            rawValue8 = ACKM_NACK_Busy_AddressrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_Busy_Address";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint32_t> (ACKM_ACK_PGNumberrawValue) = message.ACKM_ACK_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_ACK_PGNumberrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_ACK_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<std::uint32_t> (ACKM_NACK_AD_PGNumberrawValue) = message.ACKM_NACK_AD_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_NACK_AD_PGNumberrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_AD_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<std::uint32_t> (ACKM_NACK_Busy_PGNumberrawValue) = message.ACKM_NACK_Busy_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_NACK_Busy_PGNumberrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_Busy_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);

            ara::core::Optional<std::uint32_t> (ACKM_NACK_PGNumberrawValue) = message.ACKM_NACK_PGNumber.GetRawValue(canData);
            rawValue32 = ACKM_NACK_PGNumberrawValue.value();
            topic = "can/Acknowledgement_K_FF/ACKM_NACK_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FF52EF: {
            constexpr auto message = canID_0x98FF52EF{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryChargeMux> (VCBBatteryChargeMuxrawValue) = message.VCBBatteryChargeMux.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VCBBatteryChargeMuxrawValue.value());
            topic = "can/HybridInfoProp_HMS/VCBBatteryChargeMux";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_KM_resolution> (VCBBatteryDTE_KM_resolutionrawValue) = message.VCBBatteryDTE_KM_resolution.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VCBBatteryDTE_KM_resolutionrawValue.value());
            topic = "can/HybridInfoProp_HMS/VCBBatteryDTE_KM_resolution";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_MI_resolution> (VCBBatteryDTE_MI_resolutionrawValue) = message.VCBBatteryDTE_MI_resolution.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VCBBatteryDTE_MI_resolutionrawValue.value());
            topic = "can/HybridInfoProp_HMS/VCBBatteryDTE_MI_resolution";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryTTE_resolution> (VCBBatteryTTE_resolutionrawValue) = message.VCBBatteryTTE_resolution.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VCBBatteryTTE_resolutionrawValue.value());
            topic = "can/HybridInfoProp_HMS/VCBBatteryTTE_resolution";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_KM> (VCBBatteryDTE_KMrawValue) = message.VCBBatteryDTE_KM.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VCBBatteryDTE_KMrawValue.value());
            topic = "can/HybridInfoProp_HMS/VCBBatteryDTE_KM";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_MI> (VCBBatteryDTE_MIrawValue) = message.VCBBatteryDTE_MI.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VCBBatteryDTE_MIrawValue.value());
            topic = "can/HybridInfoProp_HMS/VCBBatteryDTE_MI";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryTTE> (VCBBatteryTTErawValue) = message.VCBBatteryTTE.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(VCBBatteryTTErawValue.value());
            topic = "can/HybridInfoProp_HMS/VCBBatteryTTE";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x9CFE9200: {
            constexpr auto message = canID_0x9CFE9200{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum EI_E::EngMassFlow> (EngMassFlowrawValue) = message.EngMassFlow.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(EngMassFlowrawValue.value());
            topic = "can/EI_E/EngMassFlow";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x9CFEAF00: {
            constexpr auto message = canID_0x9CFEAF00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());
            
            ara::core::Optional<enum GFC_E::TotalFuelUsed> (TotalFuelUsedrawValue) = message.TotalFuelUsed.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TotalFuelUsedrawValue.value());
            topic = "can/GFC_E/TotalFuelUsed";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98FE6BEE: {
            constexpr auto message = canID_0x98FE6BEE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<std::int8_t> (Driver1IdentifierrawValue) = message.Driver1Identifier.GetRawValue(canData);
            rawValue8 = Driver1IdentifierrawValue.value();
            topic = "can/DI_TCO_RTC/Driver1Identifier";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::int8_t> (Delimiter1rawValue) = message.Delimiter1.GetRawValue(canData);
            rawValue8 = Delimiter1rawValue.value();
            topic = "can/DI_TCO_RTC/Delimiter1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::int8_t> (Delimiter2rawValue) = message.Delimiter2.GetRawValue(canData);
            rawValue8 = Delimiter2rawValue.value();
            topic = "can/DI_TCO_RTC/Delimiter2";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98EC4AEE: {
            constexpr auto message = canID_0x98EC4AEE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TP_CM_TCO_RTC::ControlByteTP_CM> (ControlByteTP_CMrawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ControlByteTP_CMrawValue.value());
            topic = "can/TP_CM_TCO_RTC/ControlByteTP_CM";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint16_t> (TPCM_BAM_TotalMessageSizerawValue) = message.TPCM_BAM_TotalMessageSize.GetRawValue(canData);
            rawValue16 = TPCM_BAM_TotalMessageSizerawValue.value();
            topic = "can/TP_CM_TCO_RTC/TPCM_BAM_TotalMessageSize";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<std::uint8_t> (TPCM_BAM_TotalNumberOfPacketsrawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            rawValue8 = TPCM_BAM_TotalNumberOfPacketsrawValue.value();
            topic = "can/TP_CM_TCO_RTC/TPCM_BAM_TotalNumberOfPackets";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint32_t> (TPCM_BAM_PGNumberrawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            rawValue32 = TPCM_BAM_PGNumberrawValue.value();
            topic = "can/TP_CM_TCO_RTC/TPCM_BAM_PGNumber";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98EB4AEE: {
            //TODO
            constexpr auto message = canID_0x98EB4AEE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<std::uint8_t> (SequenceNumberrawValue) = message.SequenceNumber.GetRawValue(canData);
            rawValue8 = SequenceNumberrawValue.value();
            topic = "can/TP_DT_TCO_RTC/SequenceNumber";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte1rawValue) = message.Byte1.GetRawValue(canData);
            rawValue8 = Byte1rawValue.value();
            topic = "can/TP_DT_TCO_RTC/Byte1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte2rawValue) = message.Byte2.GetRawValue(canData);
            rawValue8 = Byte2rawValue.value();
            topic = "can/TP_DT_TCO_RTC/Byte2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte3rawValue) = message.Byte3.GetRawValue(canData);
            rawValue8 = Byte3rawValue.value();
            topic = "can/TP_DT_TCO_RTC/Byte3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte4rawValue) = message.Byte4.GetRawValue(canData);
            rawValue8 = Byte4rawValue.value();
            topic = "can/TP_DT_TCO_RTC/Byte4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte5rawValue) = message.Byte5.GetRawValue(canData);
            rawValue8 = Byte5rawValue.value();
            topic = "can/TP_DT_TCO_RTC/Byte5";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte6rawValue) = message.Byte6.GetRawValue(canData);
            rawValue8 = Byte6rawValue.value();
            topic = "can/TP_DT_TCO_RTC/Byte6";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<std::uint8_t> (Byte7rawValue) = message.Byte7.GetRawValue(canData);
            topic = "can/TP_DT_TCO_RTC/Byte7";
            rawValue8 = Byte7rawValue.value();
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF8347: {
            constexpr auto message = canID_0x99FF8347{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum DistanceToEmpty_CMS::DTEUnit> (DTEUnitrawValue) = message.DTEUnit.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(DTEUnitrawValue.value());
            topic = "can/DistanceToEmpty_CMS/DTEUnit";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum DistanceToEmpty_CMS::FuelDTE_KM> (FuelDTE_KMrawValue) = message.FuelDTE_KM.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(FuelDTE_KMrawValue.value());
            topic = "can/DistanceToEmpty_CMS/FuelDTE_KM";
            publishSignal(topic, rawValue16, msqt_pub);
            
            ara::core::Optional<enum DistanceToEmpty_CMS::FuelDTE_MI> (FuelDTE_MIrawValue) = message.FuelDTE_MI.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(FuelDTE_MIrawValue.value());
            topic = "can/DistanceToEmpty_CMS/FuelDTE_MI";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum DistanceToEmpty_CMS::FuelTTE> (FuelTTErawValue) = message.FuelTTE.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(FuelTTErawValue.value());
            topic = "can/DistanceToEmpty_CMS/FuelTTE";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantTTE> (ReductantTTErawValue) = message.ReductantTTE.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(ReductantTTErawValue.value());
            topic = "can/DistanceToEmpty_CMS/ReductantTTE";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantDTE_KM> (ReductantDTE_KMrawValue) = message.ReductantDTE_KM.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(ReductantDTE_KMrawValue.value());
            topic = "can/DistanceToEmpty_CMS/ReductantDTE_KM";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantDTE_MI> (ReductantDTE_MIrawValue) = message.ReductantDTE_MI.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(ReductantDTE_MIrawValue.value());
            topic = "can/DistanceToEmpty_CMS/ReductantDTE_MI";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x99FFA633: {
            constexpr auto message = canID_0x99FFA633{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TireConditionProprietary2_TM::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value());
            topic = "can/TireConditionProprietary2_TM/TirePressThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FFA6AF: {
            constexpr auto message = canID_0x99FFA6AF{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TireConditionProprietary2_TPM_AF::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value());
            topic = "can/TireConditionProprietary2_TPM_AF/TirePressThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FFA6B7: {
            constexpr auto message = canID_0x99FFA6B7{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TireConditionProprietary2_TPM_B7::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value());
            topic = "can/TireConditionProprietary2_TPM_B7/TirePressThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FFA6BF: {
            constexpr auto message = canID_0x99FFA6BF{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TireConditionProprietary2_TPM_BF::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value());
            topic = "can/TireConditionProprietary2_TPM_BF/TirePressThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FFA6C7: {
            constexpr auto message = canID_0x99FFA6C7{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TireConditionProprietary2_TPM_C7::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value());
            topic = "can/TireConditionProprietary2_TPM_C7/TirePressThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FFA6CF: {
            constexpr auto message = canID_0x99FFA6CF{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum TireConditionProprietary2_TPM_CF::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value());
            topic = "can/TireConditionProprietary2_TPM_CF/TirePressThresholdDetection";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF2E27: {
            constexpr auto message = canID_0x99FF2E27{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_PolicyID> (ZM_PolicyIDrawValue) = message.ZM_PolicyID.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ZM_PolicyIDrawValue.value());
            topic = "can/ZM_PolicyResponse_K/ZM_PolicyID";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_FullfillStatus> (ZM_FullfillStatusrawValue) = message.ZM_FullfillStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ZM_FullfillStatusrawValue.value());
            topic = "can/ZM_PolicyResponse_K/ZM_FullfillStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_AcceptStatus> (ZM_AcceptStatusrawValue) = message.ZM_AcceptStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ZM_AcceptStatusrawValue.value());
            topic = "can/ZM_PolicyResponse_K/ZM_AcceptStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_ActiveState> (ZM_ActiveStaterawValue) = message.ZM_ActiveState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ZM_ActiveStaterawValue.value());
            topic = "can/ZM_PolicyResponse_K/ZM_ActiveState";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_PolicyType> (ZM_PolicyTyperawValue) = message.ZM_PolicyType.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ZM_PolicyTyperawValue.value());
            topic = "can/ZM_PolicyResponse_K/ZM_PolicyType";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF3F4A: {
            constexpr auto message = canID_0x99FF3F4A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum ZM_Status_RTC::ZM_ServiceStatus> (ZM_ServiceStatusrawValue) = message.ZM_ServiceStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ZM_ServiceStatusrawValue.value());
            topic = "can/ZM_Status_RTC/ZM_ServiceStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_Status_RTC::ZM_DisplayStatus> (ZM_DisplayStatusrawValue) = message.ZM_DisplayStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ZM_DisplayStatusrawValue.value());
            topic = "can/ZM_Status_RTC/ZM_DisplayStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_Status_RTC::DisplayInformation> (DisplayInformationrawValue) = message.DisplayInformation.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DisplayInformationrawValue.value());
            topic = "can/ZM_Status_RTC/DisplayInformation";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_Status_RTC::SpeedPolicyState> (SpeedPolicyStaterawValue) = message.SpeedPolicyState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(SpeedPolicyStaterawValue.value());
            topic = "can/ZM_Status_RTC/SpeedPolicyState";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_Status_RTC::EmissionPolicyState> (EmissionPolicyStaterawValue) = message.EmissionPolicyState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EmissionPolicyStaterawValue.value());
            topic = "can/ZM_Status_RTC/EmissionPolicyState";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_Status_RTC::NoisePolicyState> (NoisePolicyStaterawValue) = message.NoisePolicyState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(NoisePolicyStaterawValue.value());
            topic = "can/ZM_Status_RTC/NoisePolicyState";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ZM_Status_RTC::BodyBuilderPolicyState> (BodyBuilderPolicyStaterawValue) = message.BodyBuilderPolicyState.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(BodyBuilderPolicyStaterawValue.value());
            topic = "can/ZM_Status_RTC/BodyBuilderPolicyState";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CFF041D: {
            constexpr auto message = canID_0x8CFF041D{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum SecondaryWakeUp_ALM::WakeUp_RTC> (WakeUp_RTCrawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(WakeUp_RTCrawValue.value());
            topic = "can/SecondaryWakeUp_ALM/WakeUp_RTC";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CFF04E6: {
            constexpr auto message = canID_0x8CFF04E6{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum SecondaryWakeUp_BCI::WakeUp_RTC> (WakeUp_RTCrawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(WakeUp_RTCrawValue.value());
            topic = "can/SecondaryWakeUp_BCI/WakeUp_RTC";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x99FF7D4A: {
            constexpr auto message = canID_0x99FF7D4A{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum StayAlive_RTC::StayAliveRequest> (StayAliveRequestrawValue) = message.StayAliveRequest.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(StayAliveRequestrawValue.value());
            topic = "can/StayAlive_RTC/StayAliveRequest";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum StayAlive_RTC::TargetSystem_Byte1> (TargetSystem_Byte1rawValue) = message.TargetSystem_Byte1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(TargetSystem_Byte1rawValue.value());
            topic = "can/StayAlive_RTC/TargetSystem_Byte1";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FFBDEC: {
            constexpr auto message = canID_0x98FFBDEC{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverDoorOpenStatus> (DriverDoorOpenStatusrawValue) = message.DriverDoorOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverDoorOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/DriverDoorOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerDoorOpenStatus> (PassengerDoorOpenStatusrawValue) = message.PassengerDoorOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PassengerDoorOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/PassengerDoorOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBDOpenStatus> (DoorBDOpenStatusrawValue) = message.DoorBDOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DoorBDOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/DoorBDOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBPOpenStatus> (DoorBPOpenStatusrawValue) = message.DoorBPOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DoorBPOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/DoorBPOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverDoorLockStatus> (DriverDoorLockStatusrawValue) = message.DriverDoorLockStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverDoorLockStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/DriverDoorLockStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerDoorLockStatus> (PassengerDoorLockStatusrawValue) = message.PassengerDoorLockStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PassengerDoorLockStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/PassengerDoorLockStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBDLockStatus> (DoorBDLockStatusrawValue) = message.DoorBDLockStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DoorBDLockStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/DoorBDLockStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBPLockStatus> (DoorBPLockStatusrawValue) = message.DoorBPLockStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DoorBPLockStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/DoorBPLockStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverWindowOpenStatus> (DriverWindowOpenStatusrawValue) = message.DriverWindowOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(DriverWindowOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/DriverWindowOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerWindowOpenStatus> (PassengerWindowOpenStatusrawValue) = message.PassengerWindowOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PassengerWindowOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/PassengerWindowOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::WindowBDOpenStatus> (WindowBDOpenStatusrawValue) = message.WindowBDOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(WindowBDOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/WindowBDOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorStatusProprietary_DCS::WindowBPOpenStatus> (WindowBPOpenStatusrawValue) = message.WindowBPOpenStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(WindowBPOpenStatusrawValue.value());
            topic = "can/DoorStatusProprietary_DCS/WindowBPOpenStatus";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x8CF00203: {
            constexpr auto message = canID_0x8CF00203{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum ETC1_T::InputShaftSpeed> (InputShaftSpeedrawValue) = message.InputShaftSpeed.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(InputShaftSpeedrawValue.value());
            topic = "can/ETC1_T/InputShaftSpeed";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        case 0x99FF82E6: {
            constexpr auto message = canID_0x99FF82E6{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum BSM2_BCI::StopSignalVisual> (StopSignalVisualrawValue) = message.StopSignalVisual.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(StopSignalVisualrawValue.value());
            topic = "can/BSM2_BCI/StopSignalVisual";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum BSM2_BCI::PramsignalVisual> (PramsignalVisualrawValue) = message.PramsignalVisual.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PramsignalVisualrawValue.value());
            topic = "can/BSM2_BCI/PramsignalVisual";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FFB647: {
            constexpr auto message = canID_0x98FFB647{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum SuspensionInformationProprietary_CMS::KneelingStatus> (KneelingStatusrawValue) = message.KneelingStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(KneelingStatusrawValue.value());
            topic = "can/SuspensionInformationProprietary_CMS/KneelingStatus";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FE4EFE: {
            constexpr auto message = canID_0x98FE4EFE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum DoorControl1_Unknown::PositionOfDoors> (PositionOfDoorsrawValue) = message.PositionOfDoors.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(PositionOfDoorsrawValue.value());
            topic = "can/DoorControl1_Unknown/PositionOfDoors";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl1_Unknown::Ramp_WheelchairLiftStatus> (Ramp_WheelchairLiftStatusrawValue) = message.Ramp_WheelchairLiftStatus.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Ramp_WheelchairLiftStatusrawValue.value());
            topic = "can/DoorControl1_Unknown/Ramp_WheelchairLiftStatus";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl1_Unknown::Status2OfDoors> (Status2OfDoorsrawValue) = message.Status2OfDoors.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(Status2OfDoorsrawValue.value());
            topic = "can/DoorControl1_Unknown/Status2OfDoors";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FDA5FE: {
            constexpr auto message = canID_0x98FDA5FE{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor1> (LockStatusOfDoor1rawValue) = message.LockStatusOfDoor1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor1rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor1> (OpenStatusOfDoor1rawValue) = message.OpenStatusOfDoor1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor1rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor1> (EnableStatusOfDoor1rawValue) = message.EnableStatusOfDoor1.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor1rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor2> (LockStatusOfDoor2rawValue) = message.LockStatusOfDoor2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor2rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor2> (OpenStatusOfDoor2rawValue) = message.OpenStatusOfDoor2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor2rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor2> (EnableStatusOfDoor2rawValue) = message.EnableStatusOfDoor2.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor2rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor2";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor3> (LockStatusOfDoor3rawValue) = message.LockStatusOfDoor3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor3rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor3> (OpenStatusOfDoor3rawValue) = message.OpenStatusOfDoor3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor3rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor3> (EnableStatusOfDoor3rawValue) = message.EnableStatusOfDoor3.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor3rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor3";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor4> (LockStatusOfDoor4rawValue) = message.LockStatusOfDoor4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor4rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor4> (OpenStatusOfDoor4rawValue) = message.OpenStatusOfDoor4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor4rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor4> (EnableStatusOfDoor4rawValue) = message.EnableStatusOfDoor4.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor4rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor4";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor5> (LockStatusOfDoor5rawValue) = message.LockStatusOfDoor5.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor5rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor5";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor5> (OpenStatusOfDoor5rawValue) = message.OpenStatusOfDoor5.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor5rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor5";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor5> (EnableStatusOfDoor5rawValue) = message.EnableStatusOfDoor5.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor5rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor5";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor6> (LockStatusOfDoor6rawValue) = message.LockStatusOfDoor6.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor6rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor6";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor6> (OpenStatusOfDoor6rawValue) = message.OpenStatusOfDoor6.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor6rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor6";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor6> (EnableStatusOfDoor6rawValue) = message.EnableStatusOfDoor6.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor6rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor6";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor7> (LockStatusOfDoor7rawValue) = message.LockStatusOfDoor7.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor7rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor7";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor7> (OpenStatusOfDoor7rawValue) = message.OpenStatusOfDoor7.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor7rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor7";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor7> (EnableStatusOfDoor7rawValue) = message.EnableStatusOfDoor7.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor7rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor7";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor8> (LockStatusOfDoor8rawValue) = message.LockStatusOfDoor8.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor8rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor8";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor8> (OpenStatusOfDoor8rawValue) = message.OpenStatusOfDoor8.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor8rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor8";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor8> (EnableStatusOfDoor8rawValue) = message.EnableStatusOfDoor8.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor8rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor8";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor9> (LockStatusOfDoor9rawValue) = message.LockStatusOfDoor9.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor9rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor9";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor9> (OpenStatusOfDoor9rawValue) = message.OpenStatusOfDoor9.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor9rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor9";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor9> (EnableStatusOfDoor9rawValue) = message.EnableStatusOfDoor9.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor9rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor9";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor10> (LockStatusOfDoor10rawValue) = message.LockStatusOfDoor10.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(LockStatusOfDoor10rawValue.value());
            topic = "can/DoorControl2_Unknown/LockStatusOfDoor10";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor10> (OpenStatusOfDoor10rawValue) = message.OpenStatusOfDoor10.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(OpenStatusOfDoor10rawValue.value());
            topic = "can/DoorControl2_Unknown/OpenStatusOfDoor10";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor10> (EnableStatusOfDoor10rawValue) = message.EnableStatusOfDoor10.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EnableStatusOfDoor10rawValue.value());
            topic = "can/DoorControl2_Unknown/EnableStatusOfDoor10";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEEE00: {
            constexpr auto message = canID_0x98FEEE00{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum EngineTemp_E::EngineCoolantTemperature> (EngineCoolantTemperaturerawValue) = message.EngineCoolantTemperature.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EngineCoolantTemperaturerawValue.value());
            topic = "can/EngineTemp_E/EngineCoolantTemperature";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEE500: {
            constexpr auto message = canID_0x98FEE500{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum EngineHoursRevolutions_E::TotalEngineHours> (TotalEngineHoursrawValue) = message.TotalEngineHours.GetEnumValue(canData);
            rawValue32 = static_cast <std::uint32_t>(TotalEngineHoursrawValue.value());
            topic = "can/EngineHoursRevolutions_E/TotalEngineHours";
            publishSignal(topic, rawValue32, msqt_pub);
            }
            break;
        case 0x98F00010: {
            constexpr auto message = canID_0x98F00010{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum ERC1_RD::EngineRetarderTorqueMode> (EngineRetarderTorqueModerawValue) = message.EngineRetarderTorqueMode.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(EngineRetarderTorqueModerawValue.value());
            topic = "can/ERC1_RD/EngineRetarderTorqueMode";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum ERC1_RD::ActualRetarderPercentTorque> (ActualRetarderPercentTorquerawValue) = message.ActualRetarderPercentTorque.GetEnumValue(canData);
            rawValue8 = static_cast <std::uint8_t>(ActualRetarderPercentTorquerawValue.value());
            topic = "can/ERC1_RD/ActualRetarderPercentTorque";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FEAE30: {
            constexpr auto message = canID_0x98FEAE30{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum AIR1_APS::ServiceBrakeAirPressure1> (ServiceBrakeAirPressure1rawValue) = message.ServiceBrakeAirPressure1.GetEnumValue(canData);
            rawValue8 = static_cast<std::uint8_t>(ServiceBrakeAirPressure1rawValue.value());
            topic = "can/AIR1_APS/ServiceBrakeAirPressure1";
            publishSignal(topic, rawValue8, msqt_pub);

            ara::core::Optional<enum AIR1_APS::ServiceBrakeAirPressure2> (ServiceBrakeAirPressure2rawValue) = message.ServiceBrakeAirPressure2.GetEnumValue(canData);
            rawValue8 = static_cast<std::uint8_t>(ServiceBrakeAirPressure2rawValue.value());
            topic = "can/AIR1_APS/ServiceBrakeAirPressure2";
            publishSignal(topic, rawValue8, msqt_pub);
            }
            break;
        case 0x98FE582F: {
            constexpr auto message = canID_0x98FE582F{};
            printMessageInfo(typeid(message).name(), message.GetRawIdentifier());

            ara::core::Optional<enum ASC4_F::BellowPressFrontAxleLeft> (BellowPressFrontAxleLeftrawValue) = message.BellowPressFrontAxleLeft.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BellowPressFrontAxleLeftrawValue.value());
            topic = "can/ASC4_F/BellowPressFrontAxleLeft";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum ASC4_F::BellowPressFrontAxleRight> (BellowPressFrontAxleRightrawValue) = message.BellowPressFrontAxleRight.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BellowPressFrontAxleRightrawValue.value());
            topic = "can/ASC4_F/BellowPressFrontAxleRight";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum ASC4_F::BellowPressRearAxleLeft> (BellowPressRearAxleLeftrawValue) = message.BellowPressRearAxleLeft.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BellowPressRearAxleLeftrawValue.value());
            topic = "can/ASC4_F/BellowPressRearAxleLeft";
            publishSignal(topic, rawValue16, msqt_pub);

            ara::core::Optional<enum ASC4_F::BellowPressRearAxleRight> (BellowPressRearAxleRightrawValue) = message.BellowPressRearAxleRight.GetEnumValue(canData);
            rawValue16 = static_cast <std::uint16_t>(BellowPressRearAxleRightrawValue.value());
            topic = "can/ASC4_F/BellowPressRearAxleRight";
            publishSignal(topic, rawValue16, msqt_pub);
            }
            break;
        default:
            std::cout << "Unknown CAN ID: " << std::hex << "0x" << canId << std::endl;
            break;
    }
}

#if 0
// Extract and publish individual signals as per dbc definition.
void extract_signals(std::string &canId, ara::core::Span<const uint8_t> &canData)
{
   //std::cout << canId << "\n";
   //std::cout << canData[0] << "\n";
   processCanMessage(canId, canData);
   return;
}
#endif

void my_message_handler(const std::shared_ptr<vsomeip_v3::message>& message) {
    std::stringstream canId;
    //std::stringstream canData;
    //ara::core::Span<uint8_t> canData;
    ara::core::Vector<uint8_t> canData;
    std::string msg;
    std::ostringstream oss;  // Create a string stream object

    //auto payload = message->get_payload()->get_data();
    auto payload = message->get_payload() ? message->get_payload()->get_data() : nullptr;

    if (!payload) {
        return;
    }

    try {
        if (message->get_payload()->get_length() >= PAYLOAD_MIN_LENGTH) { // If the payload is long enough
            // Extracting and printing the CAN ID in the correct order
            // CAN ID is located in 4 bytes starting from the 12th byte of the payload
            //std::cout << "CAN ID = 0x";
            std::cout << std::hex << std::uppercase;
            canId << std::hex << std::uppercase << std::setw(2) << std::setfill('0');
            for (int i = 11; i >= 8; --i) {
                canId << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                //std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                if (i > 8) std::cout << " "; // Leave a space except for the last byte
            }
            //std::cout << std::endl;
            std::string canId_msg = canId.str();
            //msqt_pub.publish("test/t1", static_cast<const void*>(canId_msg.c_str()), canId_msg.size());

            //std::cout << "CAN Data = ";
            //canData << std::hex << std::uppercase << std::setw(2) << std::setfill('0');
            for (int i = 12; i < 20; ++i) { // CAN Data starts from the 12th byte and is 8 bytes long
                //canData << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                canData.emplace_back(payload[i]);
                //std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                //if (i < 19) std::cout << " ";
            }
            ara::core::Span<const uint8_t> canDataSpan{canData.data(), canData.size()};

            //std::cout << std::endl;
            //std::string canData_msg = canData.str();
            //msqt_pub.publish(static_cast<const void*>(canData_msg.c_str()), canData_msg.size());
            processCanMessage(canId_msg, canDataSpan);
        } else {
            std::cout << "Not " << std::dec << PAYLOAD_MIN_LENGTH << " Bytes: len(" << message->get_payload()->get_length() << ") payload(" << payload << ")" << std::endl << std::hex << std::uppercase;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
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
    std::cout << "Creating application ..." << std::endl;
    /*
    std::string idstring = "0x8CF00400";
    ara::core::Vector<uint8_t> vvar {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Example for EEC4_00
    ara::core::Span<const uint8_t> payload1 {vvar.data(), vvar.size()};
    extract_signals(idstring, payload1);
    exit(1);
    */
    app = vsomeip::runtime::get()->create_application("Client1");

    std::cout << "Initializing application..." << std::endl;
    /*
    [info] Using configuration file: "./vsomeip.json".
    [info] Parsed vsomeip configuration in 99ms
    [info] Configuration module loaded.
    [info] Initializing vsomeip (3.3.8) application "Client1".
    [info] Instantiating routing manager [Host].
    [info] create_routing_root: Routing root @ /var/vsomeip-0
    [info] Service Discovery enabled. Trying to load module.
    [info] Service Discovery module loaded.
    [info] Application(Client1, 0399) is initialized (11, 100).
    */
    msqt_pub.init();
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
