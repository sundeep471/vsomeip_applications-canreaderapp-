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

//void processCanMessage(std::string &canId, const ara::core::Span<uint8_t>& canData) {
//void processCanMessage(std::string &canId,  std::string &canData) {
void processCanMessage(std::string &canId,  ara::core::Span<const uint8_t> &canData) {
    //std::cout << canId << "\n";
    //std::cout << canData[0] << "\n";

    std::cout << "K01" << std::endl;
    std::cout << "Args: canId: " << std::hex << canId << std::endl;
    std::cout << "Args: canData: " << std::hex << canData[0] << canData[1] << canData[2] << canData[3] << canData[4] << canData[5] << canData[6] << std::endl;
    switch (std::stoul(canId, nullptr, 16)) {
        #if 0
        case 0x98FEE617: {
            constexpr auto message = canID_0x98FEE617{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Seconds>(rawValue) = message.Seconds.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Minutes>(rawValue) = message.Minutes.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Hours>(rawValue) = message.Hours.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Month>(rawValue) = message.Month.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Day>(rawValue) = message.Day.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Year>(rawValue) = message.Year.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF554A: {
            constexpr auto message = canID_0x98FF554A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.Metadata_CountryCode_0x06.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Offset_0x05>(rawValue) = message.ProfileLong_Offset_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_MapProvider_0x06>(rawValue) = message.Metadata_MapProvider_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_Offset_0x01>(rawValue) = message.Pos_Offset_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Offset_0x04>(rawValue) = message.ProfileShort_Offset_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_Offset_0x03>(rawValue) = message.Stub_Offset_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ADAS_MsgType>(rawValue) = message.ADAS_MsgType.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.Metadata_HardwareVersion_0x06.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Metadata_ProtocolSubVer_0x06.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ProfileLong_PathIndex_0x05.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Metadata_ProtocolVerMajor_0x06.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PathIndex_0x01>(rawValue) = message.Pos_PathIndex_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ProfileShort_PathIndex_0x04.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_PathIndex_0x03>(rawValue) = message.Stub_PathIndex_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ProfileLong_CyclicCounter_0x05.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Metadata_CyclicCounter_0x06.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Pos_CyclicCounter_0x01.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ProfileShort_CyclicCounter_0x04.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Stub_CyclicCounter_0x03.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Update_0x05>(rawValue) = message.ProfileLong_Update_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PositionAge_0x01>(rawValue) = message.Pos_PositionAge_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Update_0x04>(rawValue) = message.ProfileShort_Update_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Stub_Update_0x03.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Retransmission_0x05>(rawValue) = message.ProfileLong_Retransmission_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Retransmission_0x04>(rawValue) = message.ProfileShort_Retransmission_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_Retransmission_0x03>(rawValue) = message.Stub_Retransmission_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_ControlPoint_0x05>(rawValue) = message.ProfileLong_ControlPoint_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_ControlPoint_0x04>(rawValue) = message.ProfileShort_ControlPoint_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PositionProbability_0x01>(rawValue) = message.Pos_PositionProbability_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_ProfileType_0x05>(rawValue) = message.ProfileLong_ProfileType_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Pos_PositionIndex_0x01.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_ProfileType_0x04>(rawValue) = message.ProfileShort_ProfileType_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_SubPathIndex_0x03>(rawValue) = message.Stub_SubPathIndex_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_FunctionalRoadClass>(rawValue) = message.Stub_FunctionalRoadClass.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Distance1_0x04>(rawValue) = message.ProfileShort_Distance1_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_RegionCode_0x06>(rawValue) = message.Metadata_RegionCode_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ProfileLong_Value_0x05.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_DrivingSide_0x06>(rawValue) = message.Metadata_DrivingSide_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_AccuracyClass_0x04>(rawValue) = message.ProfileShort_AccuracyClass_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_RelativeProbability_0x03>(rawValue) = message.Stub_RelativeProbability_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_Speed_0x01>(rawValue) = message.Pos_Speed_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_CurrentLane_0x01>(rawValue) = message.Pos_CurrentLane_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Value0_0x04>(rawValue) = message.ProfileShort_Value0_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_FormOfWay_0x03>(rawValue) = message.Stub_FormOfWay_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_ComplexIntersection_0x03>(rawValue) = message.Stub_ComplexIntersection_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PositionConfidence_0x01>(rawValue) = message.Pos_PositionConfidence_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_PartOfCalculatedRoute_0x03>(rawValue) = message.Stub_PartOfCalculatedRoute_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Value1_0x04>(rawValue) = message.ProfileShort_Value1_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Segment_EffectiveSpeedLimitType.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_MapVersionYear_0x06>(rawValue) = message.Metadata_MapVersionYear_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Segment_EffectiveSpeedLimit.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_MapVersionQtr_0x06>(rawValue) = message.Metadata_MapVersionQtr_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_TurnAngle_0x03>(rawValue) = message.Stub_TurnAngle_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_NumberOfLanes_0x03>(rawValue) = message.Stub_NumberOfLanes_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Metadata_ProtocolVerMinor_0x06.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_SpeedUnits_0x06>(rawValue) = message.Metadata_SpeedUnits_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_NumberOfOppositeLanes_0x03>(rawValue) = message.Stub_NumberOfOppositeLanes_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_RightOfWay_0x03>(rawValue) = message.Stub_RightOfWay_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_RelativeHeading_0x01>(rawValue) = message.Pos_RelativeHeading_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_LastStubAtOffset>(rawValue) = message.Stub_LastStubAtOffset.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6027: {
            constexpr auto message = canID_0x98FF6027{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM1_K::CalculatedGTWStatus>(rawValue) = message.CalculatedGTWStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM1_K::CalculatedGTW>(rawValue) = message.CalculatedGTW.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF6327: {
            constexpr auto message = canID_0x8CFF6327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM2_K::Charge61>(rawValue) = message.Charge61.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM2_K::TrailerDetectionAndABSStatus>(rawValue) = message.TrailerDetectionAndABSStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM2_K::CoastingActiveSignal>(rawValue) = message.CoastingActiveSignal.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM2_K::EngineStartInhibitByOTAStatus>(rawValue) = message.EngineStartInhibitByOTAStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF4327: {
            constexpr auto message = canID_0x98FF4327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::AxleNumber>(rawValue) = message.AxleNumber.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_1>(rawValue) = message.BrakeLiningRemainingLeftWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_2>(rawValue) = message.BrakeLiningRemainingLeftWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_3>(rawValue) = message.BrakeLiningRemainingLeftWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_4>(rawValue) = message.BrakeLiningRemainingLeftWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_5>(rawValue) = message.BrakeLiningRemainingLeftWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_1>(rawValue) = message.BrakeLiningRemainingRightWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_2>(rawValue) = message.BrakeLiningRemainingRightWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_3>(rawValue) = message.BrakeLiningRemainingRightWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_4>(rawValue) = message.BrakeLiningRemainingRightWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_5>(rawValue) = message.BrakeLiningRemainingRightWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_1>(rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_2>(rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_3>(rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_4>(rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_5>(rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_1>(rawValue) = message.BrakeLiningDistanceRemainingRightWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_2>(rawValue) = message.BrakeLiningDistanceRemainingRightWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_3>(rawValue) = message.BrakeLiningDistanceRemainingRightWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_4>(rawValue) = message.BrakeLiningDistanceRemainingRightWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_5>(rawValue) = message.BrakeLiningDistanceRemainingRightWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EC8: {
            constexpr auto message = canID_0x98FE5EC8{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_C8::TyreTemperature>(rawValue) = message.TyreTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_C8::IdentificationDataIndex>(rawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EC0: {
            constexpr auto message = canID_0x98FE5EC0{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_C0::TyreTemperature>(rawValue) = message.TyreTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_C0::IdentificationDataIndex>(rawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EB8: {
            constexpr auto message = canID_0x98FE5EB8{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_B8::TyreTemperature>(rawValue) = message.TyreTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_B8::IdentificationDataIndex>(rawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EB0: {
            constexpr auto message = canID_0x98FE5EB0{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_B0::TyreTemperature>(rawValue) = message.TyreTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_B0::IdentificationDataIndex>(rawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EA8: {
            constexpr auto message = canID_0x98FE5EA8{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_A8::TyreTemperature>(rawValue) = message.TyreTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<enum RGE23_TT_A8::IdentificationDataIndex>(rawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEEA17: {
            constexpr auto message = canID_0x98FEEA17{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::AxleLocation>(rawValue) = message.AxleLocation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::AxleWeight>(rawValue) = message.AxleWeight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::TrailerWeight>(rawValue) = message.TrailerWeight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::CargoWeight>(rawValue) = message.CargoWeight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF4F4A: {
            constexpr auto message = canID_0x99FF4F4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::motorwayMapSpeedLim>(rawValue) = message.motorwayMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::DCMapSpeedLim>(rawValue) = message.DCMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::SCMapSpeedLim>(rawValue) = message.SCMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::ruralMapSpeedLim>(rawValue) = message.ruralMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::urbanMapSpeedLim>(rawValue) = message.urbanMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::playstreetMapSpeedLim>(rawValue) = message.playstreetMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF474A: {
            constexpr auto message = canID_0x99FF474A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum RTCInformationProprietary2_RTC::UpdatePrecondition>(rawValue) = message.UpdatePrecondition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF7347: {
            constexpr auto message = canID_0x99FF7347{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum CI3_CMS::Battery24VCurrent>(rawValue) = message.Battery24VCurrent.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF127: {
            constexpr auto message = canID_0x98FEF127{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::ParkingBrakeSwitch>(rawValue) = message.ParkingBrakeSwitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::WheelBasedVehicleSpeed>(rawValue) = message.WheelBasedVehicleSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::CruiseCtrlActive>(rawValue) = message.CruiseCtrlActive.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::BrakeSwitch>(rawValue) = message.BrakeSwitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::ClutchSwitch>(rawValue) = message.ClutchSwitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::PTOState>(rawValue) = message.PTOState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF4150: {
            constexpr auto message = canID_0x99FF4150{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PBS1_EPB::ParkingBrakeReleaseInhibitStatus>(rawValue) = message.ParkingBrakeReleaseInhibitStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PBS1_EPB::ParkingBrakeStatus>(rawValue) = message.ParkingBrakeStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PBS1_EPB::ParkingBrakeReleaseInhibitReason>(rawValue) = message.ParkingBrakeReleaseInhibitReason.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8DFF404A: {
            constexpr auto message = canID_0x8DFF404A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PBC1_RTC::ParkingBrakeReleaseInhibit>(rawValue) = message.ParkingBrakeReleaseInhibit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PBC1_RTC::ParkingBrakeReleaseInhibitRequestSource>(rawValue) = message.ParkingBrakeReleaseInhibitRequestSource.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.XBRMessageCounter.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.XBRMessageChecksum.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x8CFFFC4A: {
            constexpr auto message = canID_0x8CFFFC4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ExternalControlMessage_RTC::EngineStartAllowed>(rawValue) = message.EngineStartAllowed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF8227: {
            constexpr auto message = canID_0x98FF8227{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DLN9_K::PropulsionState>(rawValue) = message.PropulsionState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DLN9_K::ElectricDriveModeRequest>(rawValue) = message.ElectricDriveModeRequest.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98F00503: {
            constexpr auto message = canID_0x98F00503{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ETC2_T::SelectedGear>(rawValue) = message.SelectedGear.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ETC2_T::CurrentGear>(rawValue) = message.CurrentGear.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF34A: {
            constexpr auto message = canID_0x98FEF34A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum VehiclePosition_1000_RTC::Latitude>(rawValue) = message.Latitude.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehiclePosition_1000_RTC::Longitude>(rawValue) = message.Longitude.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEE84A: {
            constexpr auto message = canID_0x98FEE84A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::CompassBearing>(rawValue) = message.CompassBearing.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::NavigationBasedVehicleSpeed>(rawValue) = message.NavigationBasedVehicleSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::Pitch>(rawValue) = message.Pitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::Altitude>(rawValue) = message.Altitude.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6E03: {
            constexpr auto message = canID_0x98FF6E03{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TI_T::EcoRollActive>(rawValue) = message.EcoRollActive.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TI_T::GearboxInReverse>(rawValue) = message.GearboxInReverse.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF04EF: {
            constexpr auto message = canID_0x8CFF04EF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SecondaryWakeUp_HMS::WakeUp_RTC>(rawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FD7D17: {
            constexpr auto message = canID_0x98FD7D17{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleBlockID>(rawValue) = message.TelltaleBlockID.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus1>(rawValue) = message.TelltaleStatus1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus2>(rawValue) = message.TelltaleStatus2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus3>(rawValue) = message.TelltaleStatus3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus4>(rawValue) = message.TelltaleStatus4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus5>(rawValue) = message.TelltaleStatus5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus6>(rawValue) = message.TelltaleStatus6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus7>(rawValue) = message.TelltaleStatus7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus8>(rawValue) = message.TelltaleStatus8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus9>(rawValue) = message.TelltaleStatus9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus10>(rawValue) = message.TelltaleStatus10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus11>(rawValue) = message.TelltaleStatus11.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus12>(rawValue) = message.TelltaleStatus12.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus13>(rawValue) = message.TelltaleStatus13.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus14>(rawValue) = message.TelltaleStatus14.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus15>(rawValue) = message.TelltaleStatus15.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6B27: {
            constexpr auto message = canID_0x98FF6B27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM3_K::LowEngineCoolantWaterLevel>(rawValue) = message.LowEngineCoolantWaterLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM3_K::VehicleSpeedImplausible>(rawValue) = message.VehicleSpeedImplausible.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM3_K::SeatBeltReminder>(rawValue) = message.SeatBeltReminder.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM3_K::TotalPropulsionStateReadyHours>(rawValue) = message.TotalPropulsionStateReadyHours.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FFAE27: {
            constexpr auto message = canID_0x98FFAE27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::ServiceDistanceProp>(rawValue) = message.ServiceDistanceProp.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::ServiceTime>(rawValue) = message.ServiceTime.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::ServiceDistanceWarning>(rawValue) = message.ServiceDistanceWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::OperationalTimeWarning>(rawValue) = message.OperationalTimeWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FFBE1D: {
            constexpr auto message = canID_0x98FFBE1D{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmOnOffStatus>(rawValue) = message.AlarmOnOffStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmModeStatus>(rawValue) = message.AlarmModeStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmSystemActivationReason>(rawValue) = message.AlarmSystemActivationReason.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorIgnition>(rawValue) = message.SensorIgnition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::PanicAlarmStatus>(rawValue) = message.PanicAlarmStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDriverDoor>(rawValue) = message.SensorDriverDoor.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorPassengerDoor>(rawValue) = message.SensorPassengerDoor.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDoorBehindDriver>(rawValue) = message.SensorDoorBehindDriver.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDoorBehindPassenger>(rawValue) = message.SensorDoorBehindPassenger.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorCargoDoor>(rawValue) = message.SensorCargoDoor.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxDriverSideUpper>(rawValue) = message.SensorStorageBoxDriverSideUpper.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxDriverSideLower>(rawValue) = message.SensorStorageBoxDriverSideLower.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxPassengerSideUpper>(rawValue) = message.SensorStorageBoxPassengerSideUpper.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxPassengerSideLower>(rawValue) = message.SensorStorageBoxPassengerSideLower.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorResAnalogue>(rawValue) = message.SensorResAnalogue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFrontGrille>(rawValue) = message.SensorFrontGrille.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorRoofHatch>(rawValue) = message.SensorRoofHatch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra1>(rawValue) = message.SensorExtra1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra2>(rawValue) = message.SensorExtra2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra3>(rawValue) = message.SensorExtra3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorSiren>(rawValue) = message.SensorSiren.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFTAStatus>(rawValue) = message.SensorFTAStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorMotion>(rawValue) = message.SensorMotion.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorInclination>(rawValue) = message.SensorInclination.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFTATheft>(rawValue) = message.SensorFTATheft.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF7427: {
            constexpr auto message = canID_0x8CFF7427{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PowertrainControl2_K::AcceleratorPedalPosition>(rawValue) = message.AcceleratorPedalPosition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF2327: {
            constexpr auto message = canID_0x99FF2327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::AccumulatedTripDataParams>(rawValue) = message.AccumulatedTripDataParams.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_ForcedChargingMode_0x16>(rawValue) = message.Distance_ForcedChargingMode_0x16.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_ForcedElectricMode_0x15>(rawValue) = message.Distance_ForcedElectricMode_0x15.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_IdlStopStartDeactivated_0x18>(rawValue) = message.Distance_IdlStopStartDeactivated_0x18.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_PTO_In_Drive_0x13>(rawValue) = message.Distance_PTO_In_Drive_0x13.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_TotalAccumulated_0x10>(rawValue) = message.Distance_TotalAccumulated_0x10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::DistanceInPause_PTO_In_Drive_0x63>(rawValue) = message.DistanceInPause_PTO_In_Drive_0x63.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::DistanceInPause_TotalAccumulated_0x60>(rawValue) = message.DistanceInPause_TotalAccumulated_0x60.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_Idle_0x41>(rawValue) = message.EnergyUsed_Idle_0x41.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_PTO_In_Drive_0x43>(rawValue) = message.EnergyUsed_PTO_In_Drive_0x43.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_PTO_In_Idle_0x42>(rawValue) = message.EnergyUsed_PTO_In_Idle_0x42.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_TotalAccumulated_0x40>(rawValue) = message.EnergyUsed_TotalAccumulated_0x40.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_Idle_0x31>(rawValue) = message.FuelGaseous_Idle_0x31.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_PTO_In_Drive_0x33>(rawValue) = message.FuelGaseous_PTO_In_Drive_0x33.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_PTO_In_Idle_0x32>(rawValue) = message.FuelGaseous_PTO_In_Idle_0x32.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_TotalAccumulated_0x30>(rawValue) = message.FuelGaseous_TotalAccumulated_0x30.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_Idle_0x21>(rawValue) = message.FuelLiquid_Idle_0x21.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_PTO_In_Drive_0x23>(rawValue) = message.FuelLiquid_PTO_In_Drive_0x23.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_PTO_In_Idle_0x22>(rawValue) = message.FuelLiquid_PTO_In_Idle_0x22.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_TotalAccumulated_0x20>(rawValue) = message.FuelLiquid_TotalAccumulated_0x20.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_ForcedChargingMode_0x06>(rawValue) = message.Time_ForcedChargingMode_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_ForcedElectricMode_0x05>(rawValue) = message.Time_ForcedElectricMode_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_Idle_0x01>(rawValue) = message.Time_Idle_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_IdlStopStartDeactivated_0x08>(rawValue) = message.Time_IdlStopStartDeactivated_0x08.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_PTO_In_Drive_0x03>(rawValue) = message.Time_PTO_In_Drive_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_PTO_In_Idle_0x02>(rawValue) = message.Time_PTO_In_Idle_0x02.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_TotalAccumulated_0x00>(rawValue) = message.Time_TotalAccumulated_0x00.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_Idle_0x51>(rawValue) = message.TimeInPause_Idle_0x51.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_PTO_In_Drive_0x53>(rawValue) = message.TimeInPause_PTO_In_Drive_0x53.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_PTO_In_Idle_0x52>(rawValue) = message.TimeInPause_PTO_In_Idle_0x52.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_TotalAccumulated_0x50>(rawValue) = message.TimeInPause_TotalAccumulated_0x50.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF527: {
            constexpr auto message = canID_0x98FEF527{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AmbientConditions_K::AmbientAirTemperature>(rawValue) = message.AmbientAirTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF3C27: {
            constexpr auto message = canID_0x99FF3C27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM10_K::AuxiliaryAmbientAirTemperature>(rawValue) = message.AuxiliaryAmbientAirTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE6B27: {
            constexpr auto message = canID_0x98FE6B27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DI_OnChange_K_FF::Driver1Identifier>(rawValue) = message.Driver1Identifier.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DI_OnChange_K_FF::Delimiter1>(rawValue) = message.Delimiter1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DI_OnChange_K_FF::Driver2Identifier>(rawValue) = message.Driver2Identifier.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DI_OnChange_K_FF::Delimiter2>(rawValue) = message.Delimiter2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98ECFF27: {
            constexpr auto message = canID_0x98ECFF27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TP_CM_K_FF::ControlByteTP_CM>(rawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.TPCM_BAM_TotalMessageSize.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98EBFF27: {
            constexpr auto message = canID_0x98EBFF27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.SequenceNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte1.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte2.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte3.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte4.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte5.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte6.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte7.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98FF9027: {
            constexpr auto message = canID_0x98FF9027{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_AL1>(rawValue) = message.PTO_AL1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_AL2>(rawValue) = message.PTO_AL2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_EG3>(rawValue) = message.PTO_EG3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_Engaged>(rawValue) = message.PTO_Engaged.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x9CFF0627: {
            constexpr auto message = canID_0x9CFF0627{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory1>(rawValue) = message.DriverEvaluationCategory1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory2>(rawValue) = message.DriverEvaluationCategory2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory3>(rawValue) = message.DriverEvaluationCategory3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory4>(rawValue) = message.DriverEvaluationCategory4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory1>(rawValue) = message.EvaluationActiveCategory1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory2>(rawValue) = message.EvaluationActiveCategory2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory3>(rawValue) = message.EvaluationActiveCategory3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory4>(rawValue) = message.EvaluationActiveCategory4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x9CFF1327: {
            constexpr auto message = canID_0x9CFF1327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat1>(rawValue) = message.InstEventWeight_Cat1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat2>(rawValue) = message.InstEventWeight_Cat2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat3>(rawValue) = message.InstEventWeight_Cat3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat4>(rawValue) = message.InstEventWeight_Cat4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat1>(rawValue) = message.InstEventGrade_Cat1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat2>(rawValue) = message.InstEventGrade_Cat2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat3>(rawValue) = message.InstEventGrade_Cat3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat4>(rawValue) = message.InstEventGrade_Cat4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::DriverEvaluationVersionNumber>(rawValue) = message.DriverEvaluationVersionNumber.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98EC4A27: {
            constexpr auto message = canID_0x98EC4A27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TP_CM_K_RTC::ControlByteTP_CM>(rawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.TPCM_RTS_TotalMessageSize.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98EB4A27: {
            constexpr auto message = canID_0x98EB4A27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.SequenceNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte1.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte2.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte3.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte4.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte5.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte6.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte7.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98FEEC27: {
            constexpr auto message = canID_0x98FEEC27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum VehicleId_K_RTC::VehicleIdentification>(rawValue) = message.VehicleIdentification.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            ara::core::Optional<enum VehicleId_K_RTC::Delimiter>(rawValue) = message.Delimiter.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FFB11E: {
            constexpr auto message = canID_0x98FFB11E{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum CUVInformation_V::HeadLampFailure>(rawValue) = message.HeadLampFailure.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum CUVInformation_V::HazardWarningSignalStatus>(rawValue) = message.HazardWarningSignalStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum CUVInformation_V::TrailerDetection>(rawValue) = message.TrailerDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FFD517: {
            constexpr auto message = canID_0x98FFD517{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ICLInformationProprietary_ICL::PerformanceMode>(rawValue) = message.PerformanceMode.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF8400: {
            constexpr auto message = canID_0x99FF8400{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ReductantDTEInformation_E::TotalReductantUsed>(rawValue) = message.TotalReductantUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CFE6CEE: {
            constexpr auto message = canID_0x8CFE6CEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver1WorkingState>(rawValue) = message.Driver1WorkingState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver2WorkingState>(rawValue) = message.Driver2WorkingState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::VehicleMotion>(rawValue) = message.VehicleMotion.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver1TimeRelatedStates>(rawValue) = message.Driver1TimeRelatedStates.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::DriverCardDriver1>(rawValue) = message.DriverCardDriver1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Overspeed>(rawValue) = message.Overspeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver2TimeRelatedStates>(rawValue) = message.Driver2TimeRelatedStates.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::DriverCardDriver2>(rawValue) = message.DriverCardDriver2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::SystemEvent>(rawValue) = message.SystemEvent.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::HandlingInformation>(rawValue) = message.HandlingInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::SystemPerformance>(rawValue) = message.SystemPerformance.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::DirectionIndicator>(rawValue) = message.DirectionIndicator.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::TCOVehSpeed>(rawValue) = message.TCOVehSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEC1EE: {
            constexpr auto message = canID_0x98FEC1EE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum HiResVehicleDist_TCO::HighResolTotalVehicleDistance>(rawValue) = message.HighResolTotalVehicleDistance.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF400B: {
            constexpr auto message = canID_0x98FF400B{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum BSS2_A::BrakeLiningWearWarning>(rawValue) = message.BrakeLiningWearWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF5F00: {
            constexpr auto message = canID_0x98FF5F00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EngineInfoProprietary2_E::UreaLevel>(rawValue) = message.UreaLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum EngineInfoProprietary2_E::TorqueLimit>(rawValue) = message.TorqueLimit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum EngineInfoProprietary2_E::EngineAirFilterStatus>(rawValue) = message.EngineAirFilterStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CF00300: {
            constexpr auto message = canID_0x8CF00300{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EEC2_E::AcceleratorPedalPosition>(rawValue) = message.AcceleratorPedalPosition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum EEC2_E::PercentLoadAtCurrentSpeed>(rawValue) = message.PercentLoadAtCurrentSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FD0900: {
            constexpr auto message = canID_0x98FD0900{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum HRLFC_E::HighResEngineTotalFuelUsed>(rawValue) = message.HighResEngineTotalFuelUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF4600: {
            constexpr auto message = canID_0x99FF4600{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum OBDInformation2_E::DegradationInformation>(rawValue) = message.DegradationInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF7C00: {
            constexpr auto message = canID_0x98FF7C00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PowertrainInformation3_E::LowEngineOilPressure>(rawValue) = message.LowEngineOilPressure.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PowertrainInformation3_E::HighEngineCoolantTemp>(rawValue) = message.HighEngineCoolantTemp.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PowertrainInformation3_E::DPFRegenerationState>(rawValue) = message.DPFRegenerationState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CF00400: {
            constexpr auto message = canID_0x8CF00400{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EEC1_E::ActualEngine_PercTorque>(rawValue) = message.ActualEngine_PercTorque.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum EEC1_E::EngineSpeed>(rawValue) = message.EngineSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEE900: {
            constexpr auto message = canID_0x98FEE900{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum FuelConsumption_E::TotalFuelUsed>(rawValue) = message.TotalFuelUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF200: {
            constexpr auto message = canID_0x98FEF200{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum FuelEconomy_E::FuelRate>(rawValue) = message.FuelRate.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum FuelEconomy_E::InstantaneousFuelEconomy>(rawValue) = message.InstantaneousFuelEconomy.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FECA00: {
            constexpr auto message = canID_0x98FECA00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DM1_E::RedStopLampStatus>(rawValue) = message.RedStopLampStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6000: {
            constexpr auto message = canID_0x98FF6000{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM1_E::CalculatedGTW>(rawValue) = message.CalculatedGTW.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FD7C00: {
            constexpr auto message = canID_0x98FD7C00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DPFC1_E::DpfStatus>(rawValue) = message.DpfStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF7A00: {
            constexpr auto message = canID_0x98FF7A00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PowertrainInformation1_E::ManualRegenState>(rawValue) = message.ManualRegenState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum PowertrainInformation1_E::RegenNeedLevel>(rawValue) = message.RegenNeedLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEBF0B: {
            constexpr auto message = canID_0x98FEBF0B{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EBC2_A::FrontAxleSpeed>(rawValue) = message.FrontAxleSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.RelSpeedRearAxle_1LeftWheel.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.RelSpeedRearAxle_1RightWheel.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98FFA303: {
            constexpr auto message = canID_0x98FFA303{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TransmissionProprietary3_T::LowClutchFluidLevel>(rawValue) = message.LowClutchFluidLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum TransmissionProprietary3_T::ClutchOverloadInformation>(rawValue) = message.ClutchOverloadInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FDC40B: {
            constexpr auto message = canID_0x98FDC40B{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EBC5_A::BrakeTemperatureWarning>(rawValue) = message.BrakeTemperatureWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEFC47: {
            constexpr auto message = canID_0x98FEFC47{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DashDisplay_CMS::FuelLevel>(rawValue) = message.FuelLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF7B00: {
            constexpr auto message = canID_0x98FF7B00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum OBDInformation_E::TorqueLimTimeHoursOrMinutes>(rawValue) = message.TorqueLimTimeHoursOrMinutes.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum OBDInformation_E::SpeedLimTimeHoursOrMinutes>(rawValue) = message.SpeedLimTimeHoursOrMinutes.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum OBDInformation_E::TimeToTorqueLimit>(rawValue) = message.TimeToTorqueLimit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum OBDInformation_E::TimeToSpeedLimit>(rawValue) = message.TimeToSpeedLimit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE4EE6: {
            constexpr auto message = canID_0x98FE4EE6{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorControl1_BCI::PositionOfDoors>(rawValue) = message.PositionOfDoors.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6017: {
            constexpr auto message = canID_0x98FF6017{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM1_ICL::ChassiNo>(rawValue) = message.ChassiNo.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98E8FFEE: {
            constexpr auto message = canID_0x98E8FFEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum Acknowledgement_TCO_FF::ControlByte_ACK>(rawValue) = message.ControlByte_ACK.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_ACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_NACK_AD_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_NACK_Busy_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_NACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98EAEE4A: {
            constexpr auto message = canID_0x98EAEE4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.PGN.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98EA274A: {
            constexpr auto message = canID_0x98EA274A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.PGN.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98E8FF27: {
            constexpr auto message = canID_0x98E8FF27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ControlByte_ACK>(rawValue) = message.ControlByte_ACK.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_ACK_GroupFunctionValue>(rawValue) = message.ACKM_ACK_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_AD_GroupFunctionValue>(rawValue) = message.ACKM_NACK_AD_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_Bsy_GroupFunctionValue>(rawValue) = message.ACKM_NACK_Bsy_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_GroupFunctionValue>(rawValue) = message.ACKM_NACK_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ACKM_ACK_Address.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ACKM_NACK_AD_Address.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ACKM_NACK_Address.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.ACKM_NACK_Busy_Address.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_ACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_NACK_AD_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_NACK_Busy_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.ACKM_NACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98FF52EF: {
            constexpr auto message = canID_0x98FF52EF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryChargeMux>(rawValue) = message.VCBBatteryChargeMux.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_KM_resolution>(rawValue) = message.VCBBatteryDTE_KM_resolution.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_MI_resolution>(rawValue) = message.VCBBatteryDTE_MI_resolution.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryTTE_resolution>(rawValue) = message.VCBBatteryTTE_resolution.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_KM>(rawValue) = message.VCBBatteryDTE_KM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_MI>(rawValue) = message.VCBBatteryDTE_MI.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryTTE>(rawValue) = message.VCBBatteryTTE.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x9CFE9200: {
            constexpr auto message = canID_0x9CFE9200{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EI_E::EngMassFlow>(rawValue) = message.EngMassFlow.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x9CFEAF00: {
            constexpr auto message = canID_0x9CFEAF00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GFC_E::TotalFuelUsed>(rawValue) = message.TotalFuelUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE6BEE: {
            constexpr auto message = canID_0x98FE6BEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DI_TCO_RTC::Driver1Identifier>(rawValue) = message.Driver1Identifier.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DI_TCO_RTC::Delimiter1>(rawValue) = message.Delimiter1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DI_TCO_RTC::Delimiter2>(rawValue) = message.Delimiter2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98EC4AEE: {
            constexpr auto message = canID_0x98EC4AEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TP_CM_TCO_RTC::ControlByteTP_CM>(rawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t>(rawValue) = message.TPCM_BAM_TotalMessageSize.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint32_t>(rawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x98EB4AEE: {
            constexpr auto message = canID_0x98EB4AEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.SequenceNumber.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte1.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte2.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte3.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte4.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte5.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte6.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            ara::core::Optional<std::uint8_t>(rawValue) = message.Byte7.GetRawValue(canData);
            std::cout << std::hex << (rawValue) << std::endl;
            }
            break;
        case 0x99FF8347: {
            constexpr auto message = canID_0x99FF8347{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::DTEUnit>(rawValue) = message.DTEUnit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::FuelDTE_KM>(rawValue) = message.FuelDTE_KM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::FuelDTE_MI>(rawValue) = message.FuelDTE_MI.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::FuelTTE>(rawValue) = message.FuelTTE.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantTTE>(rawValue) = message.ReductantTTE.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantDTE_KM>(rawValue) = message.ReductantDTE_KM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantDTE_MI>(rawValue) = message.ReductantDTE_MI.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA633: {
            constexpr auto message = canID_0x99FFA633{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TM::TirePressThresholdDetection>(rawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6AF: {
            constexpr auto message = canID_0x99FFA6AF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_AF::TirePressThresholdDetection>(rawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6B7: {
            constexpr auto message = canID_0x99FFA6B7{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_B7::TirePressThresholdDetection>(rawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6BF: {
            constexpr auto message = canID_0x99FFA6BF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_BF::TirePressThresholdDetection>(rawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6C7: {
            constexpr auto message = canID_0x99FFA6C7{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_C7::TirePressThresholdDetection>(rawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6CF: {
            constexpr auto message = canID_0x99FFA6CF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_CF::TirePressThresholdDetection>(rawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF2E27: {
            constexpr auto message = canID_0x99FF2E27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_PolicyID>(rawValue) = message.ZM_PolicyID.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_FullfillStatus>(rawValue) = message.ZM_FullfillStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_AcceptStatus>(rawValue) = message.ZM_AcceptStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_ActiveState>(rawValue) = message.ZM_ActiveState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_PolicyType>(rawValue) = message.ZM_PolicyType.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF3F4A: {
            constexpr auto message = canID_0x99FF3F4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::ZM_ServiceStatus>(rawValue) = message.ZM_ServiceStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::ZM_DisplayStatus>(rawValue) = message.ZM_DisplayStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::DisplayInformation>(rawValue) = message.DisplayInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::SpeedPolicyState>(rawValue) = message.SpeedPolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::EmissionPolicyState>(rawValue) = message.EmissionPolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::NoisePolicyState>(rawValue) = message.NoisePolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::BodyBuilderPolicyState>(rawValue) = message.BodyBuilderPolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF041D: {
            constexpr auto message = canID_0x8CFF041D{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SecondaryWakeUp_ALM::WakeUp_RTC>(rawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF04E6: {
            constexpr auto message = canID_0x8CFF04E6{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SecondaryWakeUp_BCI::WakeUp_RTC>(rawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF7D4A: {
            constexpr auto message = canID_0x99FF7D4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum StayAlive_RTC::StayAliveRequest>(rawValue) = message.StayAliveRequest.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum StayAlive_RTC::TargetSystem_Byte1>(rawValue) = message.TargetSystem_Byte1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FFBDEC: {
            constexpr auto message = canID_0x98FFBDEC{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverDoorOpenStatus>(rawValue) = message.DriverDoorOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerDoorOpenStatus>(rawValue) = message.PassengerDoorOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBDOpenStatus>(rawValue) = message.DoorBDOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBPOpenStatus>(rawValue) = message.DoorBPOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverDoorLockStatus>(rawValue) = message.DriverDoorLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerDoorLockStatus>(rawValue) = message.PassengerDoorLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBDLockStatus>(rawValue) = message.DoorBDLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBPLockStatus>(rawValue) = message.DoorBPLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverWindowOpenStatus>(rawValue) = message.DriverWindowOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerWindowOpenStatus>(rawValue) = message.PassengerWindowOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::WindowBDOpenStatus>(rawValue) = message.WindowBDOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::WindowBPOpenStatus>(rawValue) = message.WindowBPOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x8CF00203: {
            constexpr auto message = canID_0x8CF00203{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ETC1_T::InputShaftSpeed>(rawValue) = message.InputShaftSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x99FF82E6: {
            constexpr auto message = canID_0x99FF82E6{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum BSM2_BCI::StopSignalVisual>(rawValue) = message.StopSignalVisual.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum BSM2_BCI::PramsignalVisual>(rawValue) = message.PramsignalVisual.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FFB647: {
            constexpr auto message = canID_0x98FFB647{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SuspensionInformationProprietary_CMS::KneelingStatus>(rawValue) = message.KneelingStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE4EFE: {
            constexpr auto message = canID_0x98FE4EFE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorControl1_Unknown::PositionOfDoors>(rawValue) = message.PositionOfDoors.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl1_Unknown::Ramp_WheelchairLiftStatus>(rawValue) = message.Ramp_WheelchairLiftStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl1_Unknown::Status2OfDoors>(rawValue) = message.Status2OfDoors.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FDA5FE: {
            constexpr auto message = canID_0x98FDA5FE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor1>(rawValue) = message.LockStatusOfDoor1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor1>(rawValue) = message.OpenStatusOfDoor1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor1>(rawValue) = message.EnableStatusOfDoor1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor2>(rawValue) = message.LockStatusOfDoor2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor2>(rawValue) = message.OpenStatusOfDoor2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor2>(rawValue) = message.EnableStatusOfDoor2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor3>(rawValue) = message.LockStatusOfDoor3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor3>(rawValue) = message.OpenStatusOfDoor3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor3>(rawValue) = message.EnableStatusOfDoor3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor4>(rawValue) = message.LockStatusOfDoor4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor4>(rawValue) = message.OpenStatusOfDoor4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor4>(rawValue) = message.EnableStatusOfDoor4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor5>(rawValue) = message.LockStatusOfDoor5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor5>(rawValue) = message.OpenStatusOfDoor5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor5>(rawValue) = message.EnableStatusOfDoor5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor6>(rawValue) = message.LockStatusOfDoor6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor6>(rawValue) = message.OpenStatusOfDoor6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor6>(rawValue) = message.EnableStatusOfDoor6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor7>(rawValue) = message.LockStatusOfDoor7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor7>(rawValue) = message.OpenStatusOfDoor7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor7>(rawValue) = message.EnableStatusOfDoor7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor8>(rawValue) = message.LockStatusOfDoor8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor8>(rawValue) = message.OpenStatusOfDoor8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor8>(rawValue) = message.EnableStatusOfDoor8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor9>(rawValue) = message.LockStatusOfDoor9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor9>(rawValue) = message.OpenStatusOfDoor9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor9>(rawValue) = message.EnableStatusOfDoor9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor10>(rawValue) = message.LockStatusOfDoor10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor10>(rawValue) = message.OpenStatusOfDoor10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor10>(rawValue) = message.EnableStatusOfDoor10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEEE00: {
            constexpr auto message = canID_0x98FEEE00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EngineTemp_E::EngineCoolantTemperature>(rawValue) = message.EngineCoolantTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEE500: {
            constexpr auto message = canID_0x98FEE500{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EngineHoursRevolutions_E::TotalEngineHours>(rawValue) = message.TotalEngineHours.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98F00010: {
            constexpr auto message = canID_0x98F00010{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ERC1_RD::EngineRetarderTorqueMode>(rawValue) = message.EngineRetarderTorqueMode.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ERC1_RD::ActualRetarderPercentTorque>(rawValue) = message.ActualRetarderPercentTorque.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEAE30: {
            constexpr auto message = canID_0x98FEAE30{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AIR1_APS::ServiceBrakeAirPressure1>(rawValue) = message.ServiceBrakeAirPressure1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum AIR1_APS::ServiceBrakeAirPressure2>(rawValue) = message.ServiceBrakeAirPressure2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(rawValue.value()) << std::endl;
            }
            break;
        #endif
        case 0x98FE582F: {
            constexpr auto message = canID_0x98FE582F{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            /*
            ara::core::Optional<enum ASC4_F::BellowPressFrontAxleLeft>(rawValue) = message.BellowPressFrontAxleLeft.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ASC4_F::BellowPressFrontAxleRight>(rawValue) = message.BellowPressFrontAxleRight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            ara::core::Optional<enum ASC4_F::BellowPressRearAxleLeft>(rawValue) = message.BellowPressRearAxleLeft.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            */
            ara::core::Optional<enum ASC4_F::BellowPressRearAxleRight>(rawValue) = message.BellowPressRearAxleRight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(rawValue.value()) << std::endl;
            }
            break;

        default:
            std::cout << "Unknown CAN ID: " << std::hex << canId << std::endl;
            break;
    }
}

// Extract and publish individual signals as per dbc definition.
void extract_signals(std::string &canId, ara::core::Span<const uint8_t> &canData)
{
   //std::cout << canId << "\n";
   //std::cout << canData[0] << "\n";
   processCanMessage(canId, canData);
   return;
}

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
        if (message->get_payload()->get_length() >= 20) { // If the payload is long enough
            // Extracting and printing the CAN ID in the correct order
            // CAN ID is located in 4 bytes starting from the 12th byte of the payload
            std::cout << "CAN ID = ";
            std::cout << std::hex << std::uppercase;
            canId << std::hex << std::uppercase << std::setw(2) << std::setfill('0');
            for (int i = 11; i >= 8; --i) {
                canId << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                if (i > 8) std::cout << " "; // Leave a space except for the last byte
            }
            std::cout << std::endl;
            std::string canId_msg = canId.str();
            msqt_pub.publish(static_cast<const void*>(canId_msg.c_str()), canId_msg.size());

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
            extract_signals(canId_msg, canDataSpan);
        } else {
            std::cout << "Not 20 Bytes: " << message->get_payload()->get_length() << payload << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception caught : " << e.what() << std::endl;
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
