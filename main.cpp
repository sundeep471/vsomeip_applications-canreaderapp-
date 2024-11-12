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
    //std::cout << "K01" << std::endl;
    std::cout << "Args: canId: " << std::hex << canId << std::endl;
    std::cout << "Args: (canId & 0x7FFFFFFF): " << std::hex << (std::stoul(canId, nullptr, 16) & 0x7FFFFFFF) << std::endl;
    std::cout << "Args: canData start byte: \"" << std::hex << canData[0] << canData[1] << canData[2] << canData[3] << canData[4] << canData[5] << canData[6] << "\" End byte" << std::endl;
    #if 1
    switch (std::stoul(canId, nullptr, 16) | 0x80000000) {
        case 0x98FEE617: {
            constexpr auto message = canID_0x98FEE617{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Seconds> (SecondsrawValue) = message.Seconds.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SecondsrawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Minutes> (MinutesrawValue) = message.Minutes.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(MinutesrawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Hours> (HoursrawValue) = message.Hours.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(HoursrawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Month> (MonthrawValue) = message.Month.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(MonthrawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Day> (DayrawValue) = message.Day.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DayrawValue.value()) << std::endl;
            ara::core::Optional<enum TimeDate_ICL::Year> (YearrawValue) = message.Year.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(YearrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF554A: {
            constexpr auto message = canID_0x98FF554A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> (Metadata_CountryCode_0x06rawValue) = message.Metadata_CountryCode_0x06.GetRawValue(canData);
            std::cout << std::hex << Metadata_CountryCode_0x06rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Offset_0x05> (ProfileLong_Offset_0x05rawValue) = message.ProfileLong_Offset_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileLong_Offset_0x05rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_MapProvider_0x06> (Metadata_MapProvider_0x06rawValue) = message.Metadata_MapProvider_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Metadata_MapProvider_0x06rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_Offset_0x01> (Pos_Offset_0x01rawValue) = message.Pos_Offset_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_Offset_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Offset_0x04> (ProfileShort_Offset_0x04rawValue) = message.ProfileShort_Offset_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_Offset_0x04rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_Offset_0x03> (Stub_Offset_0x03rawValue) = message.Stub_Offset_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_Offset_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ADAS_MsgType> (ADAS_MsgTyperawValue) = message.ADAS_MsgType.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ADAS_MsgTyperawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t> (Metadata_HardwareVersion_0x06rawValue) = message.Metadata_HardwareVersion_0x06.GetRawValue(canData);
            std::cout << std::hex << Metadata_HardwareVersion_0x06rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Metadata_ProtocolSubVer_0x06rawValue) = message.Metadata_ProtocolSubVer_0x06.GetRawValue(canData);
            std::cout << std::hex << Metadata_ProtocolSubVer_0x06rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (ProfileLong_PathIndex_0x05rawValue) = message.ProfileLong_PathIndex_0x05.GetRawValue(canData);
            std::cout << std::hex << ProfileLong_PathIndex_0x05rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Metadata_ProtocolVerMajor_0x06rawValue) = message.Metadata_ProtocolVerMajor_0x06.GetRawValue(canData);
            std::cout << std::hex << Metadata_ProtocolVerMajor_0x06rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PathIndex_0x01> (Pos_PathIndex_0x01rawValue) = message.Pos_PathIndex_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_PathIndex_0x01rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (ProfileShort_PathIndex_0x04rawValue) = message.ProfileShort_PathIndex_0x04.GetRawValue(canData);
            std::cout << std::hex << ProfileShort_PathIndex_0x04rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_PathIndex_0x03> (Stub_PathIndex_0x03rawValue) = message.Stub_PathIndex_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_PathIndex_0x03rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (ProfileLong_CyclicCounter_0x05rawValue) = message.ProfileLong_CyclicCounter_0x05.GetRawValue(canData);
            std::cout << std::hex << ProfileLong_CyclicCounter_0x05rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Metadata_CyclicCounter_0x06rawValue) = message.Metadata_CyclicCounter_0x06.GetRawValue(canData);
            std::cout << std::hex << Metadata_CyclicCounter_0x06rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Pos_CyclicCounter_0x01rawValue) = message.Pos_CyclicCounter_0x01.GetRawValue(canData);
            std::cout << std::hex << Pos_CyclicCounter_0x01rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (ProfileShort_CyclicCounter_0x04rawValue) = message.ProfileShort_CyclicCounter_0x04.GetRawValue(canData);
            std::cout << std::hex << ProfileShort_CyclicCounter_0x04rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Stub_CyclicCounter_0x03rawValue) = message.Stub_CyclicCounter_0x03.GetRawValue(canData);
            std::cout << std::hex << Stub_CyclicCounter_0x03rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Update_0x05> (ProfileLong_Update_0x05rawValue) = message.ProfileLong_Update_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileLong_Update_0x05rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PositionAge_0x01> (Pos_PositionAge_0x01rawValue) = message.Pos_PositionAge_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_PositionAge_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Update_0x04> (ProfileShort_Update_0x04rawValue) = message.ProfileShort_Update_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_Update_0x04rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (Stub_Update_0x03rawValue) = message.Stub_Update_0x03.GetRawValue(canData);
            std::cout << std::hex << Stub_Update_0x03rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_Retransmission_0x05> (ProfileLong_Retransmission_0x05rawValue) = message.ProfileLong_Retransmission_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileLong_Retransmission_0x05rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Retransmission_0x04> (ProfileShort_Retransmission_0x04rawValue) = message.ProfileShort_Retransmission_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_Retransmission_0x04rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_Retransmission_0x03> (Stub_Retransmission_0x03rawValue) = message.Stub_Retransmission_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_Retransmission_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_ControlPoint_0x05> (ProfileLong_ControlPoint_0x05rawValue) = message.ProfileLong_ControlPoint_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileLong_ControlPoint_0x05rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_ControlPoint_0x04> (ProfileShort_ControlPoint_0x04rawValue) = message.ProfileShort_ControlPoint_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_ControlPoint_0x04rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PositionProbability_0x01> (Pos_PositionProbability_0x01rawValue) = message.Pos_PositionProbability_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_PositionProbability_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileLong_ProfileType_0x05> (ProfileLong_ProfileType_0x05rawValue) = message.ProfileLong_ProfileType_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileLong_ProfileType_0x05rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (Pos_PositionIndex_0x01rawValue) = message.Pos_PositionIndex_0x01.GetRawValue(canData);
            std::cout << std::hex << Pos_PositionIndex_0x01rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_ProfileType_0x04> (ProfileShort_ProfileType_0x04rawValue) = message.ProfileShort_ProfileType_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_ProfileType_0x04rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_SubPathIndex_0x03> (Stub_SubPathIndex_0x03rawValue) = message.Stub_SubPathIndex_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_SubPathIndex_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_FunctionalRoadClass> (Stub_FunctionalRoadClassrawValue) = message.Stub_FunctionalRoadClass.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_FunctionalRoadClassrawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Distance1_0x04> (ProfileShort_Distance1_0x04rawValue) = message.ProfileShort_Distance1_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_Distance1_0x04rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_RegionCode_0x06> (Metadata_RegionCode_0x06rawValue) = message.Metadata_RegionCode_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Metadata_RegionCode_0x06rawValue.value()) << std::endl;
            ara::core::Optional<std::uint32_t> (ProfileLong_Value_0x05rawValue) = message.ProfileLong_Value_0x05.GetRawValue(canData);
            std::cout << std::hex << ProfileLong_Value_0x05rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_DrivingSide_0x06> (Metadata_DrivingSide_0x06rawValue) = message.Metadata_DrivingSide_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Metadata_DrivingSide_0x06rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_AccuracyClass_0x04> (ProfileShort_AccuracyClass_0x04rawValue) = message.ProfileShort_AccuracyClass_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_AccuracyClass_0x04rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_RelativeProbability_0x03> (Stub_RelativeProbability_0x03rawValue) = message.Stub_RelativeProbability_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_RelativeProbability_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_Speed_0x01> (Pos_Speed_0x01rawValue) = message.Pos_Speed_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_Speed_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_CurrentLane_0x01> (Pos_CurrentLane_0x01rawValue) = message.Pos_CurrentLane_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_CurrentLane_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Value0_0x04> (ProfileShort_Value0_0x04rawValue) = message.ProfileShort_Value0_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_Value0_0x04rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_FormOfWay_0x03> (Stub_FormOfWay_0x03rawValue) = message.Stub_FormOfWay_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_FormOfWay_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_ComplexIntersection_0x03> (Stub_ComplexIntersection_0x03rawValue) = message.Stub_ComplexIntersection_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_ComplexIntersection_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_PositionConfidence_0x01> (Pos_PositionConfidence_0x01rawValue) = message.Pos_PositionConfidence_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_PositionConfidence_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_PartOfCalculatedRoute_0x03> (Stub_PartOfCalculatedRoute_0x03rawValue) = message.Stub_PartOfCalculatedRoute_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_PartOfCalculatedRoute_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::ProfileShort_Value1_0x04> (ProfileShort_Value1_0x04rawValue) = message.ProfileShort_Value1_0x04.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ProfileShort_Value1_0x04rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (Segment_EffectiveSpeedLimitTyperawValue) = message.Segment_EffectiveSpeedLimitType.GetRawValue(canData);
            std::cout << std::hex << Segment_EffectiveSpeedLimitTyperawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_MapVersionYear_0x06> (Metadata_MapVersionYear_0x06rawValue) = message.Metadata_MapVersionYear_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Metadata_MapVersionYear_0x06rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (Segment_EffectiveSpeedLimitrawValue) = message.Segment_EffectiveSpeedLimit.GetRawValue(canData);
            std::cout << std::hex << Segment_EffectiveSpeedLimitrawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_MapVersionQtr_0x06> (Metadata_MapVersionQtr_0x06rawValue) = message.Metadata_MapVersionQtr_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Metadata_MapVersionQtr_0x06rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_TurnAngle_0x03> (Stub_TurnAngle_0x03rawValue) = message.Stub_TurnAngle_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_TurnAngle_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_NumberOfLanes_0x03> (Stub_NumberOfLanes_0x03rawValue) = message.Stub_NumberOfLanes_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_NumberOfLanes_0x03rawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (Metadata_ProtocolVerMinor_0x06rawValue) = message.Metadata_ProtocolVerMinor_0x06.GetRawValue(canData);
            std::cout << std::hex << Metadata_ProtocolVerMinor_0x06rawValue.value() << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Metadata_SpeedUnits_0x06> (Metadata_SpeedUnits_0x06rawValue) = message.Metadata_SpeedUnits_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Metadata_SpeedUnits_0x06rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_NumberOfOppositeLanes_0x03> (Stub_NumberOfOppositeLanes_0x03rawValue) = message.Stub_NumberOfOppositeLanes_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_NumberOfOppositeLanes_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_RightOfWay_0x03> (Stub_RightOfWay_0x03rawValue) = message.Stub_RightOfWay_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_RightOfWay_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Pos_RelativeHeading_0x01> (Pos_RelativeHeading_0x01rawValue) = message.Pos_RelativeHeading_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Pos_RelativeHeading_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum ADASP1_RTC::Stub_LastStubAtOffset> (Stub_LastStubAtOffsetrawValue) = message.Stub_LastStubAtOffset.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Stub_LastStubAtOffsetrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6027: {
            constexpr auto message = canID_0x98FF6027{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM1_K::CalculatedGTWStatus> (CalculatedGTWStatusrawValue) = message.CalculatedGTWStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(CalculatedGTWStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum GPM1_K::CalculatedGTW> (CalculatedGTWrawValue) = message.CalculatedGTW.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(CalculatedGTWrawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF6327: {
            constexpr auto message = canID_0x8CFF6327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM2_K::Charge61> (Charge61rawValue) = message.Charge61.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Charge61rawValue.value()) << std::endl;
            ara::core::Optional<enum GPM2_K::TrailerDetectionAndABSStatus> (TrailerDetectionAndABSStatusrawValue) = message.TrailerDetectionAndABSStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TrailerDetectionAndABSStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum GPM2_K::CoastingActiveSignal> (CoastingActiveSignalrawValue) = message.CoastingActiveSignal.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(CoastingActiveSignalrawValue.value()) << std::endl;
            ara::core::Optional<enum GPM2_K::EngineStartInhibitByOTAStatus> (EngineStartInhibitByOTAStatusrawValue) = message.EngineStartInhibitByOTAStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EngineStartInhibitByOTAStatusrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF4327: {
            constexpr auto message = canID_0x98FF4327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::AxleNumber> (AxleNumberrawValue) = message.AxleNumber.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(AxleNumberrawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_1> (BrakeLiningRemainingLeftWheel_1rawValue) = message.BrakeLiningRemainingLeftWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_1rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_2> (BrakeLiningRemainingLeftWheel_2rawValue) = message.BrakeLiningRemainingLeftWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_2rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_3> (BrakeLiningRemainingLeftWheel_3rawValue) = message.BrakeLiningRemainingLeftWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_3rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_4> (BrakeLiningRemainingLeftWheel_4rawValue) = message.BrakeLiningRemainingLeftWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_4rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingLeftWheel_5> (BrakeLiningRemainingLeftWheel_5rawValue) = message.BrakeLiningRemainingLeftWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingLeftWheel_5rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_1> (BrakeLiningRemainingRightWheel_1rawValue) = message.BrakeLiningRemainingRightWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_1rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_2> (BrakeLiningRemainingRightWheel_2rawValue) = message.BrakeLiningRemainingRightWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_2rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_3> (BrakeLiningRemainingRightWheel_3rawValue) = message.BrakeLiningRemainingRightWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_3rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_4> (BrakeLiningRemainingRightWheel_4rawValue) = message.BrakeLiningRemainingRightWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_4rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningRemainingRightWheel_5> (BrakeLiningRemainingRightWheel_5rawValue) = message.BrakeLiningRemainingRightWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningRemainingRightWheel_5rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_1> (BrakeLiningDistanceRemainingLeftWheel_1rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_1rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_2> (BrakeLiningDistanceRemainingLeftWheel_2rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_2rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_3> (BrakeLiningDistanceRemainingLeftWheel_3rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_3rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_4> (BrakeLiningDistanceRemainingLeftWheel_4rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_4rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingLeftWheel_5> (BrakeLiningDistanceRemainingLeftWheel_5rawValue) = message.BrakeLiningDistanceRemainingLeftWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingLeftWheel_5rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_1> (BrakeLiningDistanceRemainingRightWheel_1rawValue) = message.BrakeLiningDistanceRemainingRightWheel_1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_1rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_2> (BrakeLiningDistanceRemainingRightWheel_2rawValue) = message.BrakeLiningDistanceRemainingRightWheel_2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_2rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_3> (BrakeLiningDistanceRemainingRightWheel_3rawValue) = message.BrakeLiningDistanceRemainingRightWheel_3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_3rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_4> (BrakeLiningDistanceRemainingRightWheel_4rawValue) = message.BrakeLiningDistanceRemainingRightWheel_4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_4rawValue.value()) << std::endl;
            ara::core::Optional<enum BrakeLiningWearInformation2_K::BrakeLiningDistanceRemainingRightWheel_5> (BrakeLiningDistanceRemainingRightWheel_5rawValue) = message.BrakeLiningDistanceRemainingRightWheel_5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BrakeLiningDistanceRemainingRightWheel_5rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EC8: {
            constexpr auto message = canID_0x98FE5EC8{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << TyreOrWheelIdentificationrawValue.value() << std::endl;
            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            std::cout << std::hex << TyreTemperaturerawValue.value() << std::endl;
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << AirLeakageDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << TyrePressureThresholdDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << TyreModulePowerSupplyrawValue.value() << std::endl;
            ara::core::Optional<enum RGE23_TT_C8::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EC0: {
            constexpr auto message = canID_0x98FE5EC0{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << TyreOrWheelIdentificationrawValue.value() << std::endl;
            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            std::cout << std::hex << TyreTemperaturerawValue.value() << std::endl;
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << AirLeakageDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << TyrePressureThresholdDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << TyreModulePowerSupplyrawValue.value() << std::endl;
            ara::core::Optional<enum RGE23_TT_C0::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EB8: {
            constexpr auto message = canID_0x98FE5EB8{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << TyreOrWheelIdentificationrawValue.value() << std::endl;
            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            std::cout << std::hex << TyreTemperaturerawValue.value() << std::endl;
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << AirLeakageDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << TyrePressureThresholdDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << TyreModulePowerSupplyrawValue.value() << std::endl;
            ara::core::Optional<enum RGE23_TT_B8::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EB0: {
            constexpr auto message = canID_0x98FE5EB0{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << TyreOrWheelIdentificationrawValue.value() << std::endl;
            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            std::cout << std::hex << TyreTemperaturerawValue.value() << std::endl;
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << AirLeakageDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << TyrePressureThresholdDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << TyreModulePowerSupplyrawValue.value() << std::endl;
            ara::core::Optional<enum RGE23_TT_B0::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value()) << std::endl;
            }
            break;
        case 0x98FE5EA8: {
            constexpr auto message = canID_0x98FE5EA8{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreOrWheelIdentificationrawValue) = message.TyreOrWheelIdentification.GetRawValue(canData);
            std::cout << std::hex << TyreOrWheelIdentificationrawValue.value() << std::endl;
            ara::core::Optional<std::int16_t> (TyreTemperaturerawValue) = message.TyreTemperature.GetRawValue(canData);
            std::cout << std::hex << TyreTemperaturerawValue.value() << std::endl;
            ara::core::Optional<std::uint16_t> (AirLeakageDetectionrawValue) = message.AirLeakageDetection.GetRawValue(canData);
            std::cout << std::hex << AirLeakageDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyrePressureThresholdDetectionrawValue) = message.TyrePressureThresholdDetection.GetRawValue(canData);
            std::cout << std::hex << TyrePressureThresholdDetectionrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TyreModulePowerSupplyrawValue) = message.TyreModulePowerSupply.GetRawValue(canData);
            std::cout << std::hex << TyreModulePowerSupplyrawValue.value() << std::endl;
            ara::core::Optional<enum RGE23_TT_A8::IdentificationDataIndex> (IdentificationDataIndexrawValue) = message.IdentificationDataIndex.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(IdentificationDataIndexrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEEA17: {
            constexpr auto message = canID_0x98FEEA17{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::AxleLocation> (AxleLocationrawValue) = message.AxleLocation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(AxleLocationrawValue.value()) << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::AxleWeight> (AxleWeightrawValue) = message.AxleWeight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(AxleWeightrawValue.value()) << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::TrailerWeight> (TrailerWeightrawValue) = message.TrailerWeight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(TrailerWeightrawValue.value()) << std::endl;
            ara::core::Optional<enum VehicleWeight_ICL::CargoWeight> (CargoWeightrawValue) = message.CargoWeight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(CargoWeightrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF4F4A: {
            constexpr auto message = canID_0x99FF4F4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::motorwayMapSpeedLim> (motorwayMapSpeedLimrawValue) = message.motorwayMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(motorwayMapSpeedLimrawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::DCMapSpeedLim> (DCMapSpeedLimrawValue) = message.DCMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DCMapSpeedLimrawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::SCMapSpeedLim> (SCMapSpeedLimrawValue) = message.SCMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SCMapSpeedLimrawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::ruralMapSpeedLim> (ruralMapSpeedLimrawValue) = message.ruralMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ruralMapSpeedLimrawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::urbanMapSpeedLim> (urbanMapSpeedLimrawValue) = message.urbanMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(urbanMapSpeedLimrawValue.value()) << std::endl;
            ara::core::Optional<enum ImplicitSpeedLimit_RTC::playstreetMapSpeedLim> (playstreetMapSpeedLimrawValue) = message.playstreetMapSpeedLim.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(playstreetMapSpeedLimrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF474A: {
            constexpr auto message = canID_0x99FF474A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum RTCInformationProprietary2_RTC::UpdatePrecondition> (UpdatePreconditionrawValue) = message.UpdatePrecondition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(UpdatePreconditionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF7347: {
            constexpr auto message = canID_0x99FF7347{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum CI3_CMS::Battery24VCurrent> (Battery24VCurrentrawValue) = message.Battery24VCurrent.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(Battery24VCurrentrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF127: {
            constexpr auto message = canID_0x98FEF127{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::ParkingBrakeSwitch> (ParkingBrakeSwitchrawValue) = message.ParkingBrakeSwitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ParkingBrakeSwitchrawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::WheelBasedVehicleSpeed> (WheelBasedVehicleSpeedrawValue) = message.WheelBasedVehicleSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(WheelBasedVehicleSpeedrawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::CruiseCtrlActive> (CruiseCtrlActiverawValue) = message.CruiseCtrlActive.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(CruiseCtrlActiverawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::BrakeSwitch> (BrakeSwitchrawValue) = message.BrakeSwitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(BrakeSwitchrawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::ClutchSwitch> (ClutchSwitchrawValue) = message.ClutchSwitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ClutchSwitchrawValue.value()) << std::endl;
            ara::core::Optional<enum CruiseControlVehSpeed_K::PTOState> (PTOStaterawValue) = message.PTOState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PTOStaterawValue.value()) << std::endl;
            }
            break;
        case 0x99FF4150: {
            constexpr auto message = canID_0x99FF4150{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PBS1_EPB::ParkingBrakeReleaseInhibitStatus> (ParkingBrakeReleaseInhibitStatusrawValue) = message.ParkingBrakeReleaseInhibitStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum PBS1_EPB::ParkingBrakeStatus> (ParkingBrakeStatusrawValue) = message.ParkingBrakeStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ParkingBrakeStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum PBS1_EPB::ParkingBrakeReleaseInhibitReason> (ParkingBrakeReleaseInhibitReasonrawValue) = message.ParkingBrakeReleaseInhibitReason.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitReasonrawValue.value()) << std::endl;
            }
            break;
        case 0x8DFF404A: {
            constexpr auto message = canID_0x8DFF404A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PBC1_RTC::ParkingBrakeReleaseInhibit> (ParkingBrakeReleaseInhibitrawValue) = message.ParkingBrakeReleaseInhibit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitrawValue.value()) << std::endl;
            ara::core::Optional<enum PBC1_RTC::ParkingBrakeReleaseInhibitRequestSource> (ParkingBrakeReleaseInhibitRequestSourcerawValue) = message.ParkingBrakeReleaseInhibitRequestSource.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ParkingBrakeReleaseInhibitRequestSourcerawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (XBRMessageCounterrawValue) = message.XBRMessageCounter.GetRawValue(canData);
            std::cout << std::hex << XBRMessageCounterrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (XBRMessageChecksumrawValue) = message.XBRMessageChecksum.GetRawValue(canData);
            std::cout << std::hex << XBRMessageChecksumrawValue.value() << std::endl;
            }
            break;
        case 0x8CFFFC4A: {
            constexpr auto message = canID_0x8CFFFC4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ExternalControlMessage_RTC::EngineStartAllowed> (EngineStartAllowedrawValue) = message.EngineStartAllowed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EngineStartAllowedrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF8227: {
            constexpr auto message = canID_0x98FF8227{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DLN9_K::PropulsionState> (PropulsionStaterawValue) = message.PropulsionState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PropulsionStaterawValue.value()) << std::endl;
            ara::core::Optional<enum DLN9_K::ElectricDriveModeRequest> (ElectricDriveModeRequestrawValue) = message.ElectricDriveModeRequest.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ElectricDriveModeRequestrawValue.value()) << std::endl;
            }
            break;
        case 0x98F00503: {
            constexpr auto message = canID_0x98F00503{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ETC2_T::SelectedGear> (SelectedGearrawValue) = message.SelectedGear.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SelectedGearrawValue.value()) << std::endl;
            ara::core::Optional<enum ETC2_T::CurrentGear> (CurrentGearrawValue) = message.CurrentGear.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(CurrentGearrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF34A: {
            constexpr auto message = canID_0x98FEF34A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum VehiclePosition_1000_RTC::Latitude> (LatituderawValue) = message.Latitude.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(LatituderawValue.value()) << std::endl;
            ara::core::Optional<enum VehiclePosition_1000_RTC::Longitude> (LongituderawValue) = message.Longitude.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(LongituderawValue.value()) << std::endl;
            }
            break;
        case 0x98FEE84A: {
            constexpr auto message = canID_0x98FEE84A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::CompassBearing> (CompassBearingrawValue) = message.CompassBearing.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(CompassBearingrawValue.value()) << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::NavigationBasedVehicleSpeed> (NavigationBasedVehicleSpeedrawValue) = message.NavigationBasedVehicleSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(NavigationBasedVehicleSpeedrawValue.value()) << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::Pitch> (PitchrawValue) = message.Pitch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(PitchrawValue.value()) << std::endl;
            ara::core::Optional<enum VehiDirSpeed_1000_RTC::Altitude> (AltituderawValue) = message.Altitude.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(AltituderawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6E03: {
            constexpr auto message = canID_0x98FF6E03{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TI_T::EcoRollActive> (EcoRollActiverawValue) = message.EcoRollActive.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EcoRollActiverawValue.value()) << std::endl;
            ara::core::Optional<enum TI_T::GearboxInReverse> (GearboxInReverserawValue) = message.GearboxInReverse.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(GearboxInReverserawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF04EF: {
            constexpr auto message = canID_0x8CFF04EF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SecondaryWakeUp_HMS::WakeUp_RTC> (WakeUp_RTCrawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(WakeUp_RTCrawValue.value()) << std::endl;
            }
            break;
        case 0x98FD7D17: {
            constexpr auto message = canID_0x98FD7D17{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleBlockID> (TelltaleBlockIDrawValue) = message.TelltaleBlockID.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleBlockIDrawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus1> (TelltaleStatus1rawValue) = message.TelltaleStatus1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus1rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus2> (TelltaleStatus2rawValue) = message.TelltaleStatus2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus2rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus3> (TelltaleStatus3rawValue) = message.TelltaleStatus3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus3rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus4> (TelltaleStatus4rawValue) = message.TelltaleStatus4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus4rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus5> (TelltaleStatus5rawValue) = message.TelltaleStatus5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus5rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus6> (TelltaleStatus6rawValue) = message.TelltaleStatus6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus6rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus7> (TelltaleStatus7rawValue) = message.TelltaleStatus7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus7rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus8> (TelltaleStatus8rawValue) = message.TelltaleStatus8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus8rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus9> (TelltaleStatus9rawValue) = message.TelltaleStatus9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus9rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus10> (TelltaleStatus10rawValue) = message.TelltaleStatus10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus10rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus11> (TelltaleStatus11rawValue) = message.TelltaleStatus11.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus11rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus12> (TelltaleStatus12rawValue) = message.TelltaleStatus12.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus12rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus13> (TelltaleStatus13rawValue) = message.TelltaleStatus13.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus13rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus14> (TelltaleStatus14rawValue) = message.TelltaleStatus14.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus14rawValue.value()) << std::endl;
            ara::core::Optional<enum TelltaleStatus_ICL::TelltaleStatus15> (TelltaleStatus15rawValue) = message.TelltaleStatus15.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TelltaleStatus15rawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6B27: {
            constexpr auto message = canID_0x98FF6B27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM3_K::LowEngineCoolantWaterLevel> (LowEngineCoolantWaterLevelrawValue) = message.LowEngineCoolantWaterLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(LowEngineCoolantWaterLevelrawValue.value()) << std::endl;
            ara::core::Optional<enum GPM3_K::VehicleSpeedImplausible> (VehicleSpeedImplausiblerawValue) = message.VehicleSpeedImplausible.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(VehicleSpeedImplausiblerawValue.value()) << std::endl;
            ara::core::Optional<enum GPM3_K::SeatBeltReminder> (SeatBeltReminderrawValue) = message.SeatBeltReminder.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(SeatBeltReminderrawValue.value()) << std::endl;
            ara::core::Optional<enum GPM3_K::TotalPropulsionStateReadyHours> (TotalPropulsionStateReadyHoursrawValue) = message.TotalPropulsionStateReadyHours.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TotalPropulsionStateReadyHoursrawValue.value()) << std::endl;
            }
            break;
        case 0x98FFAE27: {
            constexpr auto message = canID_0x98FFAE27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::ServiceDistanceProp> (ServiceDistanceProprawValue) = message.ServiceDistanceProp.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ServiceDistanceProprawValue.value()) << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::ServiceTime> (ServiceTimerawValue) = message.ServiceTime.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ServiceTimerawValue.value()) << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::ServiceDistanceWarning> (ServiceDistanceWarningrawValue) = message.ServiceDistanceWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ServiceDistanceWarningrawValue.value()) << std::endl;
            ara::core::Optional<enum ServiceInformationProp_K::OperationalTimeWarning> (OperationalTimeWarningrawValue) = message.OperationalTimeWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OperationalTimeWarningrawValue.value()) << std::endl;
            }
            break;
        case 0x98FFBE1D: {
            constexpr auto message = canID_0x98FFBE1D{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmOnOffStatus> (AlarmOnOffStatusrawValue) = message.AlarmOnOffStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(AlarmOnOffStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmModeStatus> (AlarmModeStatusrawValue) = message.AlarmModeStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(AlarmModeStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::AlarmSystemActivationReason> (AlarmSystemActivationReasonrawValue) = message.AlarmSystemActivationReason.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(AlarmSystemActivationReasonrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorIgnition> (SensorIgnitionrawValue) = message.SensorIgnition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorIgnitionrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::PanicAlarmStatus> (PanicAlarmStatusrawValue) = message.PanicAlarmStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PanicAlarmStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDriverDoor> (SensorDriverDoorrawValue) = message.SensorDriverDoor.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorDriverDoorrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorPassengerDoor> (SensorPassengerDoorrawValue) = message.SensorPassengerDoor.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorPassengerDoorrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDoorBehindDriver> (SensorDoorBehindDriverrawValue) = message.SensorDoorBehindDriver.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorDoorBehindDriverrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorDoorBehindPassenger> (SensorDoorBehindPassengerrawValue) = message.SensorDoorBehindPassenger.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorDoorBehindPassengerrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorCargoDoor> (SensorCargoDoorrawValue) = message.SensorCargoDoor.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorCargoDoorrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxDriverSideUpper> (SensorStorageBoxDriverSideUpperrawValue) = message.SensorStorageBoxDriverSideUpper.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorStorageBoxDriverSideUpperrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxDriverSideLower> (SensorStorageBoxDriverSideLowerrawValue) = message.SensorStorageBoxDriverSideLower.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorStorageBoxDriverSideLowerrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxPassengerSideUpper> (SensorStorageBoxPassengerSideUpperrawValue) = message.SensorStorageBoxPassengerSideUpper.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorStorageBoxPassengerSideUpperrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorStorageBoxPassengerSideLower> (SensorStorageBoxPassengerSideLowerrawValue) = message.SensorStorageBoxPassengerSideLower.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorStorageBoxPassengerSideLowerrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorResAnalogue> (SensorResAnaloguerawValue) = message.SensorResAnalogue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorResAnaloguerawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFrontGrille> (SensorFrontGrillerawValue) = message.SensorFrontGrille.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorFrontGrillerawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorRoofHatch> (SensorRoofHatchrawValue) = message.SensorRoofHatch.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorRoofHatchrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra1> (SensorExtra1rawValue) = message.SensorExtra1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorExtra1rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra2> (SensorExtra2rawValue) = message.SensorExtra2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorExtra2rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorExtra3> (SensorExtra3rawValue) = message.SensorExtra3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorExtra3rawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorSiren> (SensorSirenrawValue) = message.SensorSiren.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorSirenrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFTAStatus> (SensorFTAStatusrawValue) = message.SensorFTAStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorFTAStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorMotion> (SensorMotionrawValue) = message.SensorMotion.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorMotionrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorInclination> (SensorInclinationrawValue) = message.SensorInclination.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorInclinationrawValue.value()) << std::endl;
            ara::core::Optional<enum AlarmStatusProp_ALM::SensorFTATheft> (SensorFTATheftrawValue) = message.SensorFTATheft.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SensorFTATheftrawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF7427: {
            constexpr auto message = canID_0x8CFF7427{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PowertrainControl2_K::AcceleratorPedalPosition> (AcceleratorPedalPositionrawValue) = message.AcceleratorPedalPosition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(AcceleratorPedalPositionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF2327: {
            constexpr auto message = canID_0x99FF2327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::AccumulatedTripDataParams> (AccumulatedTripDataParamsrawValue) = message.AccumulatedTripDataParams.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(AccumulatedTripDataParamsrawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_ForcedChargingMode_0x16> (Distance_ForcedChargingMode_0x16rawValue) = message.Distance_ForcedChargingMode_0x16.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Distance_ForcedChargingMode_0x16rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_ForcedElectricMode_0x15> (Distance_ForcedElectricMode_0x15rawValue) = message.Distance_ForcedElectricMode_0x15.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Distance_ForcedElectricMode_0x15rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_IdlStopStartDeactivated_0x18> (Distance_IdlStopStartDeactivated_0x18rawValue) = message.Distance_IdlStopStartDeactivated_0x18.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Distance_IdlStopStartDeactivated_0x18rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_PTO_In_Drive_0x13> (Distance_PTO_In_Drive_0x13rawValue) = message.Distance_PTO_In_Drive_0x13.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Distance_PTO_In_Drive_0x13rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Distance_TotalAccumulated_0x10> (Distance_TotalAccumulated_0x10rawValue) = message.Distance_TotalAccumulated_0x10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Distance_TotalAccumulated_0x10rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::DistanceInPause_PTO_In_Drive_0x63> (DistanceInPause_PTO_In_Drive_0x63rawValue) = message.DistanceInPause_PTO_In_Drive_0x63.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(DistanceInPause_PTO_In_Drive_0x63rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::DistanceInPause_TotalAccumulated_0x60> (DistanceInPause_TotalAccumulated_0x60rawValue) = message.DistanceInPause_TotalAccumulated_0x60.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(DistanceInPause_TotalAccumulated_0x60rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_Idle_0x41> (EnergyUsed_Idle_0x41rawValue) = message.EnergyUsed_Idle_0x41.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(EnergyUsed_Idle_0x41rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_PTO_In_Drive_0x43> (EnergyUsed_PTO_In_Drive_0x43rawValue) = message.EnergyUsed_PTO_In_Drive_0x43.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(EnergyUsed_PTO_In_Drive_0x43rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_PTO_In_Idle_0x42> (EnergyUsed_PTO_In_Idle_0x42rawValue) = message.EnergyUsed_PTO_In_Idle_0x42.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(EnergyUsed_PTO_In_Idle_0x42rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::EnergyUsed_TotalAccumulated_0x40> (EnergyUsed_TotalAccumulated_0x40rawValue) = message.EnergyUsed_TotalAccumulated_0x40.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(EnergyUsed_TotalAccumulated_0x40rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_Idle_0x31> (FuelGaseous_Idle_0x31rawValue) = message.FuelGaseous_Idle_0x31.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelGaseous_Idle_0x31rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_PTO_In_Drive_0x33> (FuelGaseous_PTO_In_Drive_0x33rawValue) = message.FuelGaseous_PTO_In_Drive_0x33.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelGaseous_PTO_In_Drive_0x33rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_PTO_In_Idle_0x32> (FuelGaseous_PTO_In_Idle_0x32rawValue) = message.FuelGaseous_PTO_In_Idle_0x32.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelGaseous_PTO_In_Idle_0x32rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelGaseous_TotalAccumulated_0x30> (FuelGaseous_TotalAccumulated_0x30rawValue) = message.FuelGaseous_TotalAccumulated_0x30.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelGaseous_TotalAccumulated_0x30rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_Idle_0x21> (FuelLiquid_Idle_0x21rawValue) = message.FuelLiquid_Idle_0x21.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelLiquid_Idle_0x21rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_PTO_In_Drive_0x23> (FuelLiquid_PTO_In_Drive_0x23rawValue) = message.FuelLiquid_PTO_In_Drive_0x23.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelLiquid_PTO_In_Drive_0x23rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_PTO_In_Idle_0x22> (FuelLiquid_PTO_In_Idle_0x22rawValue) = message.FuelLiquid_PTO_In_Idle_0x22.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelLiquid_PTO_In_Idle_0x22rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::FuelLiquid_TotalAccumulated_0x20> (FuelLiquid_TotalAccumulated_0x20rawValue) = message.FuelLiquid_TotalAccumulated_0x20.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(FuelLiquid_TotalAccumulated_0x20rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_ForcedChargingMode_0x06> (Time_ForcedChargingMode_0x06rawValue) = message.Time_ForcedChargingMode_0x06.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Time_ForcedChargingMode_0x06rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_ForcedElectricMode_0x05> (Time_ForcedElectricMode_0x05rawValue) = message.Time_ForcedElectricMode_0x05.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Time_ForcedElectricMode_0x05rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_Idle_0x01> (Time_Idle_0x01rawValue) = message.Time_Idle_0x01.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Time_Idle_0x01rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_IdlStopStartDeactivated_0x08> (Time_IdlStopStartDeactivated_0x08rawValue) = message.Time_IdlStopStartDeactivated_0x08.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Time_IdlStopStartDeactivated_0x08rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_PTO_In_Drive_0x03> (Time_PTO_In_Drive_0x03rawValue) = message.Time_PTO_In_Drive_0x03.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Time_PTO_In_Drive_0x03rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_PTO_In_Idle_0x02> (Time_PTO_In_Idle_0x02rawValue) = message.Time_PTO_In_Idle_0x02.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Time_PTO_In_Idle_0x02rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::Time_TotalAccumulated_0x00> (Time_TotalAccumulated_0x00rawValue) = message.Time_TotalAccumulated_0x00.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(Time_TotalAccumulated_0x00rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_Idle_0x51> (TimeInPause_Idle_0x51rawValue) = message.TimeInPause_Idle_0x51.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TimeInPause_Idle_0x51rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_PTO_In_Drive_0x53> (TimeInPause_PTO_In_Drive_0x53rawValue) = message.TimeInPause_PTO_In_Drive_0x53.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TimeInPause_PTO_In_Drive_0x53rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_PTO_In_Idle_0x52> (TimeInPause_PTO_In_Idle_0x52rawValue) = message.TimeInPause_PTO_In_Idle_0x52.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TimeInPause_PTO_In_Idle_0x52rawValue.value()) << std::endl;
            ara::core::Optional<enum AccumulatedTripDataParams_K::TimeInPause_TotalAccumulated_0x50> (TimeInPause_TotalAccumulated_0x50rawValue) = message.TimeInPause_TotalAccumulated_0x50.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TimeInPause_TotalAccumulated_0x50rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF527: {
            constexpr auto message = canID_0x98FEF527{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AmbientConditions_K::AmbientAirTemperature> (AmbientAirTemperaturerawValue) = message.AmbientAirTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(AmbientAirTemperaturerawValue.value()) << std::endl;
            }
            break;
        case 0x99FF3C27: {
            constexpr auto message = canID_0x99FF3C27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM10_K::AuxiliaryAmbientAirTemperature> (AuxiliaryAmbientAirTemperaturerawValue) = message.AuxiliaryAmbientAirTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(AuxiliaryAmbientAirTemperaturerawValue.value()) << std::endl;
            }
            break;
        case 0x98FE6B27: {
            constexpr auto message = canID_0x98FE6B27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::int8_t> (Driver1IdentifierrawValue) = message.Driver1Identifier.GetRawValue(canData);
            std::cout << std::hex << Driver1IdentifierrawValue.value() << std::endl;
            ara::core::Optional<std::int8_t> (Delimiter1rawValue) = message.Delimiter1.GetRawValue(canData);
            std::cout << std::hex << Delimiter1rawValue.value() << std::endl;
            ara::core::Optional<std::int8_t> (Driver2IdentifierrawValue) = message.Driver2Identifier.GetRawValue(canData);
            std::cout << std::hex << Driver2IdentifierrawValue.value() << std::endl;
            ara::core::Optional<std::int8_t> (Delimiter2rawValue) = message.Delimiter2.GetRawValue(canData);
            std::cout << std::hex << Delimiter2rawValue.value() << std::endl;
            }
            break;
        case 0x98ECFF27: {
            constexpr auto message = canID_0x98ECFF27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TP_CM_K_FF::ControlByteTP_CM> (ControlByteTP_CMrawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ControlByteTP_CMrawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t> (TPCM_BAM_TotalMessageSizerawValue) = message.TPCM_BAM_TotalMessageSize.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_TotalMessageSizerawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TPCM_BAM_TotalNumberOfPacketsrawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_TotalNumberOfPacketsrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (TPCM_BAM_PGNumberrawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_PGNumberrawValue.value() << std::endl;
            }
            break;
        case 0x98EBFF27: {
            constexpr auto message = canID_0x98EBFF27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (SequenceNumberrawValue) = message.SequenceNumber.GetRawValue(canData);
            std::cout << std::hex << SequenceNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte1rawValue) = message.Byte1.GetRawValue(canData);
            std::cout << std::hex << Byte1rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte2rawValue) = message.Byte2.GetRawValue(canData);
            std::cout << std::hex << Byte2rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte3rawValue) = message.Byte3.GetRawValue(canData);
            std::cout << std::hex << Byte3rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte4rawValue) = message.Byte4.GetRawValue(canData);
            std::cout << std::hex << Byte4rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte5rawValue) = message.Byte5.GetRawValue(canData);
            std::cout << std::hex << Byte5rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte6rawValue) = message.Byte6.GetRawValue(canData);
            std::cout << std::hex << Byte6rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte7rawValue) = message.Byte7.GetRawValue(canData);
            std::cout << std::hex << Byte7rawValue.value() << std::endl;
            }
            break;
        case 0x98FF9027: {
            constexpr auto message = canID_0x98FF9027{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_AL1> (PTO_AL1rawValue) = message.PTO_AL1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PTO_AL1rawValue.value()) << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_AL2> (PTO_AL2rawValue) = message.PTO_AL2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PTO_AL2rawValue.value()) << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_EG3> (PTO_EG3rawValue) = message.PTO_EG3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PTO_EG3rawValue.value()) << std::endl;
            ara::core::Optional<enum PTOInformationProp_K::PTO_Engaged> (PTO_EngagedrawValue) = message.PTO_Engaged.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PTO_EngagedrawValue.value()) << std::endl;
            }
            break;
        case 0x9CFF0627: {
            constexpr auto message = canID_0x9CFF0627{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory1> (DriverEvaluationCategory1rawValue) = message.DriverEvaluationCategory1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverEvaluationCategory1rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory2> (DriverEvaluationCategory2rawValue) = message.DriverEvaluationCategory2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverEvaluationCategory2rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory3> (DriverEvaluationCategory3rawValue) = message.DriverEvaluationCategory3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverEvaluationCategory3rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::DriverEvaluationCategory4> (DriverEvaluationCategory4rawValue) = message.DriverEvaluationCategory4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverEvaluationCategory4rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory1> (EvaluationActiveCategory1rawValue) = message.EvaluationActiveCategory1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EvaluationActiveCategory1rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory2> (EvaluationActiveCategory2rawValue) = message.EvaluationActiveCategory2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EvaluationActiveCategory2rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory3> (EvaluationActiveCategory3rawValue) = message.EvaluationActiveCategory3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EvaluationActiveCategory3rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationData_K::EvaluationActiveCategory4> (EvaluationActiveCategory4rawValue) = message.EvaluationActiveCategory4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EvaluationActiveCategory4rawValue.value()) << std::endl;
            }
            break;
        case 0x9CFF1327: {
            constexpr auto message = canID_0x9CFF1327{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat1> (InstEventWeight_Cat1rawValue) = message.InstEventWeight_Cat1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventWeight_Cat1rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat2> (InstEventWeight_Cat2rawValue) = message.InstEventWeight_Cat2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventWeight_Cat2rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat3> (InstEventWeight_Cat3rawValue) = message.InstEventWeight_Cat3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventWeight_Cat3rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventWeight_Cat4> (InstEventWeight_Cat4rawValue) = message.InstEventWeight_Cat4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventWeight_Cat4rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat1> (InstEventGrade_Cat1rawValue) = message.InstEventGrade_Cat1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventGrade_Cat1rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat2> (InstEventGrade_Cat2rawValue) = message.InstEventGrade_Cat2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventGrade_Cat2rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat3> (InstEventGrade_Cat3rawValue) = message.InstEventGrade_Cat3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventGrade_Cat3rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::InstEventGrade_Cat4> (InstEventGrade_Cat4rawValue) = message.InstEventGrade_Cat4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(InstEventGrade_Cat4rawValue.value()) << std::endl;
            ara::core::Optional<enum DriverEvaluationEvents_K::DriverEvaluationVersionNumber> (DriverEvaluationVersionNumberrawValue) = message.DriverEvaluationVersionNumber.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverEvaluationVersionNumberrawValue.value()) << std::endl;
            }
            break;
        case 0x98EC4A27: {
            constexpr auto message = canID_0x98EC4A27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TP_CM_K_RTC::ControlByteTP_CM> (ControlByteTP_CMrawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ControlByteTP_CMrawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t> (TPCM_RTS_TotalMessageSizerawValue) = message.TPCM_RTS_TotalMessageSize.GetRawValue(canData);
            std::cout << std::hex << TPCM_RTS_TotalMessageSizerawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TPCM_BAM_TotalNumberOfPacketsrawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_TotalNumberOfPacketsrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (TPCM_BAM_PGNumberrawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_PGNumberrawValue.value() << std::endl;
            }
            break;
        case 0x98EB4A27: {
            constexpr auto message = canID_0x98EB4A27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (SequenceNumberrawValue) = message.SequenceNumber.GetRawValue(canData);
            std::cout << std::hex << SequenceNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte1rawValue) = message.Byte1.GetRawValue(canData);
            std::cout << std::hex << Byte1rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte2rawValue) = message.Byte2.GetRawValue(canData);
            std::cout << std::hex << Byte2rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte3rawValue) = message.Byte3.GetRawValue(canData);
            std::cout << std::hex << Byte3rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte4rawValue) = message.Byte4.GetRawValue(canData);
            std::cout << std::hex << Byte4rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte5rawValue) = message.Byte5.GetRawValue(canData);
            std::cout << std::hex << Byte5rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte6rawValue) = message.Byte6.GetRawValue(canData);
            std::cout << std::hex << Byte6rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte7rawValue) = message.Byte7.GetRawValue(canData);
            std::cout << std::hex << Byte7rawValue.value() << std::endl;
            }
            break;
        case 0x98FEEC27: {
            constexpr auto message = canID_0x98FEEC27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::int8_t> (VehicleIdentificationrawValue) = message.VehicleIdentification.GetRawValue(canData);
            std::cout << std::hex << VehicleIdentificationrawValue.value() << std::endl;
            ara::core::Optional<std::int8_t> (DelimiterrawValue) = message.Delimiter.GetRawValue(canData);
            std::cout << std::hex << DelimiterrawValue.value() << std::endl;
            }
            break;
        case 0x98FFB11E: {
            constexpr auto message = canID_0x98FFB11E{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum CUVInformation_V::HeadLampFailure> (HeadLampFailurerawValue) = message.HeadLampFailure.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(HeadLampFailurerawValue.value()) << std::endl;
            ara::core::Optional<enum CUVInformation_V::HazardWarningSignalStatus> (HazardWarningSignalStatusrawValue) = message.HazardWarningSignalStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(HazardWarningSignalStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum CUVInformation_V::TrailerDetection> (TrailerDetectionrawValue) = message.TrailerDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TrailerDetectionrawValue.value()) << std::endl;
            }
            break;
        case 0x98FFD517: {
            constexpr auto message = canID_0x98FFD517{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ICLInformationProprietary_ICL::PerformanceMode> (PerformanceModerawValue) = message.PerformanceMode.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PerformanceModerawValue.value()) << std::endl;
            }
            break;
        case 0x99FF8400: {
            constexpr auto message = canID_0x99FF8400{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ReductantDTEInformation_E::TotalReductantUsed> (TotalReductantUsedrawValue) = message.TotalReductantUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TotalReductantUsedrawValue.value()) << std::endl;
            }
            break;
        case 0x8CFE6CEE: {
            constexpr auto message = canID_0x8CFE6CEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver1WorkingState> (Driver1WorkingStaterawValue) = message.Driver1WorkingState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(Driver1WorkingStaterawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver2WorkingState> (Driver2WorkingStaterawValue) = message.Driver2WorkingState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(Driver2WorkingStaterawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::VehicleMotion> (VehicleMotionrawValue) = message.VehicleMotion.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VehicleMotionrawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver1TimeRelatedStates> (Driver1TimeRelatedStatesrawValue) = message.Driver1TimeRelatedStates.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(Driver1TimeRelatedStatesrawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::DriverCardDriver1> (DriverCardDriver1rawValue) = message.DriverCardDriver1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(DriverCardDriver1rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Overspeed> (OverspeedrawValue) = message.Overspeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(OverspeedrawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::Driver2TimeRelatedStates> (Driver2TimeRelatedStatesrawValue) = message.Driver2TimeRelatedStates.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(Driver2TimeRelatedStatesrawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::DriverCardDriver2> (DriverCardDriver2rawValue) = message.DriverCardDriver2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(DriverCardDriver2rawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::SystemEvent> (SystemEventrawValue) = message.SystemEvent.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(SystemEventrawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::HandlingInformation> (HandlingInformationrawValue) = message.HandlingInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(HandlingInformationrawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::SystemPerformance> (SystemPerformancerawValue) = message.SystemPerformance.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(SystemPerformancerawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::DirectionIndicator> (DirectionIndicatorrawValue) = message.DirectionIndicator.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(DirectionIndicatorrawValue.value()) << std::endl;
            ara::core::Optional<enum TCO1_TCO::TCOVehSpeed> (TCOVehSpeedrawValue) = message.TCOVehSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(TCOVehSpeedrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEC1EE: {
            constexpr auto message = canID_0x98FEC1EE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum HiResVehicleDist_TCO::HighResolTotalVehicleDistance> (HighResolTotalVehicleDistancerawValue) = message.HighResolTotalVehicleDistance.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(HighResolTotalVehicleDistancerawValue.value()) << std::endl;
            }
            break;
        case 0x98FF400B: {
            constexpr auto message = canID_0x98FF400B{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum BSS2_A::BrakeLiningWearWarning> (BrakeLiningWearWarningrawValue) = message.BrakeLiningWearWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(BrakeLiningWearWarningrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF5F00: {
            constexpr auto message = canID_0x98FF5F00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EngineInfoProprietary2_E::UreaLevel> (UreaLevelrawValue) = message.UreaLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(UreaLevelrawValue.value()) << std::endl;
            ara::core::Optional<enum EngineInfoProprietary2_E::TorqueLimit> (TorqueLimitrawValue) = message.TorqueLimit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TorqueLimitrawValue.value()) << std::endl;
            ara::core::Optional<enum EngineInfoProprietary2_E::EngineAirFilterStatus> (EngineAirFilterStatusrawValue) = message.EngineAirFilterStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EngineAirFilterStatusrawValue.value()) << std::endl;
            }
            break;
        case 0x8CF00300: {
            constexpr auto message = canID_0x8CF00300{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EEC2_E::AcceleratorPedalPosition> (AcceleratorPedalPositionrawValue) = message.AcceleratorPedalPosition.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(AcceleratorPedalPositionrawValue.value()) << std::endl;
            ara::core::Optional<enum EEC2_E::PercentLoadAtCurrentSpeed> (PercentLoadAtCurrentSpeedrawValue) = message.PercentLoadAtCurrentSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PercentLoadAtCurrentSpeedrawValue.value()) << std::endl;
            }
            break;
        case 0x98FD0900: {
            constexpr auto message = canID_0x98FD0900{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum HRLFC_E::HighResEngineTotalFuelUsed> (HighResEngineTotalFuelUsedrawValue) = message.HighResEngineTotalFuelUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(HighResEngineTotalFuelUsedrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF4600: {
            constexpr auto message = canID_0x99FF4600{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum OBDInformation2_E::DegradationInformation> (DegradationInformationrawValue) = message.DegradationInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(DegradationInformationrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF7C00: {
            constexpr auto message = canID_0x98FF7C00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PowertrainInformation3_E::LowEngineOilPressure> (LowEngineOilPressurerawValue) = message.LowEngineOilPressure.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LowEngineOilPressurerawValue.value()) << std::endl;
            ara::core::Optional<enum PowertrainInformation3_E::HighEngineCoolantTemp> (HighEngineCoolantTemprawValue) = message.HighEngineCoolantTemp.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(HighEngineCoolantTemprawValue.value()) << std::endl;
            ara::core::Optional<enum PowertrainInformation3_E::DPFRegenerationState> (DPFRegenerationStaterawValue) = message.DPFRegenerationState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DPFRegenerationStaterawValue.value()) << std::endl;
            }
            break;
        case 0x8CF00400: {
            constexpr auto message = canID_0x8CF00400{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EEC1_E::ActualEngine_PercTorque> (ActualEngine_PercTorquerawValue) = message.ActualEngine_PercTorque.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(ActualEngine_PercTorquerawValue.value()) << std::endl;
            ara::core::Optional<enum EEC1_E::EngineSpeed> (EngineSpeedrawValue) = message.EngineSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(EngineSpeedrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEE900: {
            constexpr auto message = canID_0x98FEE900{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum FuelConsumption_E::TotalFuelUsed> (TotalFuelUsedrawValue) = message.TotalFuelUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TotalFuelUsedrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEF200: {
            constexpr auto message = canID_0x98FEF200{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum FuelEconomy_E::FuelRate> (FuelRaterawValue) = message.FuelRate.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(FuelRaterawValue.value()) << std::endl;
            ara::core::Optional<enum FuelEconomy_E::InstantaneousFuelEconomy> (InstantaneousFuelEconomyrawValue) = message.InstantaneousFuelEconomy.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(InstantaneousFuelEconomyrawValue.value()) << std::endl;
            }
            break;
        case 0x98FECA00: {
            constexpr auto message = canID_0x98FECA00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DM1_E::RedStopLampStatus> (RedStopLampStatusrawValue) = message.RedStopLampStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(RedStopLampStatusrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6000: {
            constexpr auto message = canID_0x98FF6000{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM1_E::CalculatedGTW> (CalculatedGTWrawValue) = message.CalculatedGTW.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(CalculatedGTWrawValue.value()) << std::endl;
            }
            break;
        case 0x98FD7C00: {
            constexpr auto message = canID_0x98FD7C00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DPFC1_E::DpfStatus> (DpfStatusrawValue) = message.DpfStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DpfStatusrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF7A00: {
            constexpr auto message = canID_0x98FF7A00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum PowertrainInformation1_E::ManualRegenState> (ManualRegenStaterawValue) = message.ManualRegenState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ManualRegenStaterawValue.value()) << std::endl;
            ara::core::Optional<enum PowertrainInformation1_E::RegenNeedLevel> (RegenNeedLevelrawValue) = message.RegenNeedLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(RegenNeedLevelrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEBF0B: {
            constexpr auto message = canID_0x98FEBF0B{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EBC2_A::FrontAxleSpeed> (FrontAxleSpeedrawValue) = message.FrontAxleSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(FrontAxleSpeedrawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (RelSpeedRearAxle_1LeftWheelrawValue) = message.RelSpeedRearAxle_1LeftWheel.GetRawValue(canData);
            std::cout << std::hex << RelSpeedRearAxle_1LeftWheelrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (RelSpeedRearAxle_1RightWheelrawValue) = message.RelSpeedRearAxle_1RightWheel.GetRawValue(canData);
            std::cout << std::hex << RelSpeedRearAxle_1RightWheelrawValue.value() << std::endl;
            }
            break;
        case 0x98FFA303: {
            constexpr auto message = canID_0x98FFA303{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TransmissionProprietary3_T::LowClutchFluidLevel> (LowClutchFluidLevelrawValue) = message.LowClutchFluidLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LowClutchFluidLevelrawValue.value()) << std::endl;
            ara::core::Optional<enum TransmissionProprietary3_T::ClutchOverloadInformation> (ClutchOverloadInformationrawValue) = message.ClutchOverloadInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ClutchOverloadInformationrawValue.value()) << std::endl;
            }
            break;
        case 0x98FDC40B: {
            constexpr auto message = canID_0x98FDC40B{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EBC5_A::BrakeTemperatureWarning> (BrakeTemperatureWarningrawValue) = message.BrakeTemperatureWarning.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(BrakeTemperatureWarningrawValue.value()) << std::endl;
            }
            break;
        case 0x98FEFC47: {
            constexpr auto message = canID_0x98FEFC47{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DashDisplay_CMS::FuelLevel> (FuelLevelrawValue) = message.FuelLevel.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(FuelLevelrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF7B00: {
            constexpr auto message = canID_0x98FF7B00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum OBDInformation_E::TorqueLimTimeHoursOrMinutes> (TorqueLimTimeHoursOrMinutesrawValue) = message.TorqueLimTimeHoursOrMinutes.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TorqueLimTimeHoursOrMinutesrawValue.value()) << std::endl;
            ara::core::Optional<enum OBDInformation_E::SpeedLimTimeHoursOrMinutes> (SpeedLimTimeHoursOrMinutesrawValue) = message.SpeedLimTimeHoursOrMinutes.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SpeedLimTimeHoursOrMinutesrawValue.value()) << std::endl;
            ara::core::Optional<enum OBDInformation_E::TimeToTorqueLimit> (TimeToTorqueLimitrawValue) = message.TimeToTorqueLimit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TimeToTorqueLimitrawValue.value()) << std::endl;
            ara::core::Optional<enum OBDInformation_E::TimeToSpeedLimit> (TimeToSpeedLimitrawValue) = message.TimeToSpeedLimit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TimeToSpeedLimitrawValue.value()) << std::endl;
            }
            break;
        case 0x98FE4EE6: {
            constexpr auto message = canID_0x98FE4EE6{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorControl1_BCI::PositionOfDoors> (PositionOfDoorsrawValue) = message.PositionOfDoors.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PositionOfDoorsrawValue.value()) << std::endl;
            }
            break;
        case 0x98FF6017: {
            constexpr auto message = canID_0x98FF6017{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GPM1_ICL::ChassiNo> (ChassiNorawValue) = message.ChassiNo.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(ChassiNorawValue.value()) << std::endl;
            }
            break;
        case 0x98E8FFEE: {
            constexpr auto message = canID_0x98E8FFEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum Acknowledgement_TCO_FF::ControlByte_ACK> (ControlByte_ACKrawValue) = message.ControlByte_ACK.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ControlByte_ACKrawValue.value()) << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_ACK_PGNumberrawValue) = message.ACKM_ACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_ACK_PGNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_NACK_AD_PGNumberrawValue) = message.ACKM_NACK_AD_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_AD_PGNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_NACK_Busy_PGNumberrawValue) = message.ACKM_NACK_Busy_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_Busy_PGNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_NACK_PGNumberrawValue) = message.ACKM_NACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_PGNumberrawValue.value() << std::endl;
            }
            break;
        case 0x98EAEE4A: {
            constexpr auto message = canID_0x98EAEE4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> (PGNrawValue) = message.PGN.GetRawValue(canData);
            std::cout << std::hex << PGNrawValue.value() << std::endl;
            }
            break;
        case 0x98EA274A: {
            constexpr auto message = canID_0x98EA274A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> (PGNrawValue) = message.PGN.GetRawValue(canData);
            std::cout << std::hex << PGNrawValue.value() << std::endl;
            }
            break;
        case 0x98E8FF27: {
            constexpr auto message = canID_0x98E8FF27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ControlByte_ACK> (ControlByte_ACKrawValue) = message.ControlByte_ACK.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ControlByte_ACKrawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_ACK_GroupFunctionValue> (ACKM_ACK_GroupFunctionValuerawValue) = message.ACKM_ACK_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ACKM_ACK_GroupFunctionValuerawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_AD_GroupFunctionValue> (ACKM_NACK_AD_GroupFunctionValuerawValue) = message.ACKM_NACK_AD_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ACKM_NACK_AD_GroupFunctionValuerawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_Bsy_GroupFunctionValue> (ACKM_NACK_Bsy_GroupFunctionValuerawValue) = message.ACKM_NACK_Bsy_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ACKM_NACK_Bsy_GroupFunctionValuerawValue.value()) << std::endl;
            ara::core::Optional<enum Acknowledgement_K_FF::ACKM_NACK_GroupFunctionValue> (ACKM_NACK_GroupFunctionValuerawValue) = message.ACKM_NACK_GroupFunctionValue.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ACKM_NACK_GroupFunctionValuerawValue.value()) << std::endl;
            ara::core::Optional<std::uint8_t> (ACKM_ACK_AddressrawValue) = message.ACKM_ACK_Address.GetRawValue(canData);
            std::cout << std::hex << ACKM_ACK_AddressrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (ACKM_NACK_AD_AddressrawValue) = message.ACKM_NACK_AD_Address.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_AD_AddressrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (ACKM_NACK_AddressrawValue) = message.ACKM_NACK_Address.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_AddressrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (ACKM_NACK_Busy_AddressrawValue) = message.ACKM_NACK_Busy_Address.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_Busy_AddressrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_ACK_PGNumberrawValue) = message.ACKM_ACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_ACK_PGNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_NACK_AD_PGNumberrawValue) = message.ACKM_NACK_AD_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_AD_PGNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_NACK_Busy_PGNumberrawValue) = message.ACKM_NACK_Busy_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_Busy_PGNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (ACKM_NACK_PGNumberrawValue) = message.ACKM_NACK_PGNumber.GetRawValue(canData);
            std::cout << std::hex << ACKM_NACK_PGNumberrawValue.value() << std::endl;
            }
            break;
        case 0x98FF52EF: {
            constexpr auto message = canID_0x98FF52EF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryChargeMux> (VCBBatteryChargeMuxrawValue) = message.VCBBatteryChargeMux.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VCBBatteryChargeMuxrawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_KM_resolution> (VCBBatteryDTE_KM_resolutionrawValue) = message.VCBBatteryDTE_KM_resolution.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VCBBatteryDTE_KM_resolutionrawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_MI_resolution> (VCBBatteryDTE_MI_resolutionrawValue) = message.VCBBatteryDTE_MI_resolution.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VCBBatteryDTE_MI_resolutionrawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryTTE_resolution> (VCBBatteryTTE_resolutionrawValue) = message.VCBBatteryTTE_resolution.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VCBBatteryTTE_resolutionrawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_KM> (VCBBatteryDTE_KMrawValue) = message.VCBBatteryDTE_KM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VCBBatteryDTE_KMrawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryDTE_MI> (VCBBatteryDTE_MIrawValue) = message.VCBBatteryDTE_MI.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VCBBatteryDTE_MIrawValue.value()) << std::endl;
            ara::core::Optional<enum HybridInfoProp_HMS::VCBBatteryTTE> (VCBBatteryTTErawValue) = message.VCBBatteryTTE.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(VCBBatteryTTErawValue.value()) << std::endl;
            }
            break;
        case 0x9CFE9200: {
            constexpr auto message = canID_0x9CFE9200{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EI_E::EngMassFlow> (EngMassFlowrawValue) = message.EngMassFlow.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(EngMassFlowrawValue.value()) << std::endl;
            }
            break;
        case 0x9CFEAF00: {
            constexpr auto message = canID_0x9CFEAF00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum GFC_E::TotalFuelUsed> (TotalFuelUsedrawValue) = message.TotalFuelUsed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TotalFuelUsedrawValue.value()) << std::endl;
            }
            break;
        case 0x98FE6BEE: {
            constexpr auto message = canID_0x98FE6BEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::int8_t> (Driver1IdentifierrawValue) = message.Driver1Identifier.GetRawValue(canData);
            std::cout << std::hex << Driver1IdentifierrawValue.value() << std::endl;
            ara::core::Optional<std::int8_t> (Delimiter1rawValue) = message.Delimiter1.GetRawValue(canData);
            std::cout << std::hex << Delimiter1rawValue.value() << std::endl;
            ara::core::Optional<std::int8_t> (Delimiter2rawValue) = message.Delimiter2.GetRawValue(canData);
            std::cout << std::hex << Delimiter2rawValue.value() << std::endl;
            }
            break;
        case 0x98EC4AEE: {
            constexpr auto message = canID_0x98EC4AEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TP_CM_TCO_RTC::ControlByteTP_CM> (ControlByteTP_CMrawValue) = message.ControlByteTP_CM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ControlByteTP_CMrawValue.value()) << std::endl;
            ara::core::Optional<std::uint16_t> (TPCM_BAM_TotalMessageSizerawValue) = message.TPCM_BAM_TotalMessageSize.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_TotalMessageSizerawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (TPCM_BAM_TotalNumberOfPacketsrawValue) = message.TPCM_BAM_TotalNumberOfPackets.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_TotalNumberOfPacketsrawValue.value() << std::endl;
            ara::core::Optional<std::uint32_t> (TPCM_BAM_PGNumberrawValue) = message.TPCM_BAM_PGNumber.GetRawValue(canData);
            std::cout << std::hex << TPCM_BAM_PGNumberrawValue.value() << std::endl;
            }
            break;
        case 0x98EB4AEE: {
            constexpr auto message = canID_0x98EB4AEE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> (SequenceNumberrawValue) = message.SequenceNumber.GetRawValue(canData);
            std::cout << std::hex << SequenceNumberrawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte1rawValue) = message.Byte1.GetRawValue(canData);
            std::cout << std::hex << Byte1rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte2rawValue) = message.Byte2.GetRawValue(canData);
            std::cout << std::hex << Byte2rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte3rawValue) = message.Byte3.GetRawValue(canData);
            std::cout << std::hex << Byte3rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte4rawValue) = message.Byte4.GetRawValue(canData);
            std::cout << std::hex << Byte4rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte5rawValue) = message.Byte5.GetRawValue(canData);
            std::cout << std::hex << Byte5rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte6rawValue) = message.Byte6.GetRawValue(canData);
            std::cout << std::hex << Byte6rawValue.value() << std::endl;
            ara::core::Optional<std::uint8_t> (Byte7rawValue) = message.Byte7.GetRawValue(canData);
            std::cout << std::hex << Byte7rawValue.value() << std::endl;
            }
            break;
        case 0x99FF8347: {
            constexpr auto message = canID_0x99FF8347{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::DTEUnit> (DTEUnitrawValue) = message.DTEUnit.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(DTEUnitrawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::FuelDTE_KM> (FuelDTE_KMrawValue) = message.FuelDTE_KM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(FuelDTE_KMrawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::FuelDTE_MI> (FuelDTE_MIrawValue) = message.FuelDTE_MI.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(FuelDTE_MIrawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::FuelTTE> (FuelTTErawValue) = message.FuelTTE.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(FuelTTErawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantTTE> (ReductantTTErawValue) = message.ReductantTTE.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(ReductantTTErawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantDTE_KM> (ReductantDTE_KMrawValue) = message.ReductantDTE_KM.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(ReductantDTE_KMrawValue.value()) << std::endl;
            ara::core::Optional<enum DistanceToEmpty_CMS::ReductantDTE_MI> (ReductantDTE_MIrawValue) = message.ReductantDTE_MI.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(ReductantDTE_MIrawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA633: {
            constexpr auto message = canID_0x99FFA633{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TM::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6AF: {
            constexpr auto message = canID_0x99FFA6AF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_AF::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6B7: {
            constexpr auto message = canID_0x99FFA6B7{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_B7::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6BF: {
            constexpr auto message = canID_0x99FFA6BF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_BF::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6C7: {
            constexpr auto message = canID_0x99FFA6C7{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_C7::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FFA6CF: {
            constexpr auto message = canID_0x99FFA6CF{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum TireConditionProprietary2_TPM_CF::TirePressThresholdDetection> (TirePressThresholdDetectionrawValue) = message.TirePressThresholdDetection.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TirePressThresholdDetectionrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF2E27: {
            constexpr auto message = canID_0x99FF2E27{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_PolicyID> (ZM_PolicyIDrawValue) = message.ZM_PolicyID.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ZM_PolicyIDrawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_FullfillStatus> (ZM_FullfillStatusrawValue) = message.ZM_FullfillStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ZM_FullfillStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_AcceptStatus> (ZM_AcceptStatusrawValue) = message.ZM_AcceptStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ZM_AcceptStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_ActiveState> (ZM_ActiveStaterawValue) = message.ZM_ActiveState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ZM_ActiveStaterawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_PolicyResponse_K::ZM_PolicyType> (ZM_PolicyTyperawValue) = message.ZM_PolicyType.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ZM_PolicyTyperawValue.value()) << std::endl;
            }
            break;
        case 0x99FF3F4A: {
            constexpr auto message = canID_0x99FF3F4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::ZM_ServiceStatus> (ZM_ServiceStatusrawValue) = message.ZM_ServiceStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ZM_ServiceStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::ZM_DisplayStatus> (ZM_DisplayStatusrawValue) = message.ZM_DisplayStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ZM_DisplayStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::DisplayInformation> (DisplayInformationrawValue) = message.DisplayInformation.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DisplayInformationrawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::SpeedPolicyState> (SpeedPolicyStaterawValue) = message.SpeedPolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(SpeedPolicyStaterawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::EmissionPolicyState> (EmissionPolicyStaterawValue) = message.EmissionPolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EmissionPolicyStaterawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::NoisePolicyState> (NoisePolicyStaterawValue) = message.NoisePolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(NoisePolicyStaterawValue.value()) << std::endl;
            ara::core::Optional<enum ZM_Status_RTC::BodyBuilderPolicyState> (BodyBuilderPolicyStaterawValue) = message.BodyBuilderPolicyState.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(BodyBuilderPolicyStaterawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF041D: {
            constexpr auto message = canID_0x8CFF041D{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SecondaryWakeUp_ALM::WakeUp_RTC> (WakeUp_RTCrawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(WakeUp_RTCrawValue.value()) << std::endl;
            }
            break;
        case 0x8CFF04E6: {
            constexpr auto message = canID_0x8CFF04E6{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SecondaryWakeUp_BCI::WakeUp_RTC> (WakeUp_RTCrawValue) = message.WakeUp_RTC.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(WakeUp_RTCrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF7D4A: {
            constexpr auto message = canID_0x99FF7D4A{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum StayAlive_RTC::StayAliveRequest> (StayAliveRequestrawValue) = message.StayAliveRequest.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(StayAliveRequestrawValue.value()) << std::endl;
            ara::core::Optional<enum StayAlive_RTC::TargetSystem_Byte1> (TargetSystem_Byte1rawValue) = message.TargetSystem_Byte1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(TargetSystem_Byte1rawValue.value()) << std::endl;
            }
            break;
        case 0x98FFBDEC: {
            constexpr auto message = canID_0x98FFBDEC{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverDoorOpenStatus> (DriverDoorOpenStatusrawValue) = message.DriverDoorOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverDoorOpenStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerDoorOpenStatus> (PassengerDoorOpenStatusrawValue) = message.PassengerDoorOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PassengerDoorOpenStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBDOpenStatus> (DoorBDOpenStatusrawValue) = message.DoorBDOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DoorBDOpenStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBPOpenStatus> (DoorBPOpenStatusrawValue) = message.DoorBPOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DoorBPOpenStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverDoorLockStatus> (DriverDoorLockStatusrawValue) = message.DriverDoorLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverDoorLockStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerDoorLockStatus> (PassengerDoorLockStatusrawValue) = message.PassengerDoorLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PassengerDoorLockStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBDLockStatus> (DoorBDLockStatusrawValue) = message.DoorBDLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DoorBDLockStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DoorBPLockStatus> (DoorBPLockStatusrawValue) = message.DoorBPLockStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DoorBPLockStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::DriverWindowOpenStatus> (DriverWindowOpenStatusrawValue) = message.DriverWindowOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(DriverWindowOpenStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::PassengerWindowOpenStatus> (PassengerWindowOpenStatusrawValue) = message.PassengerWindowOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PassengerWindowOpenStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::WindowBDOpenStatus> (WindowBDOpenStatusrawValue) = message.WindowBDOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(WindowBDOpenStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorStatusProprietary_DCS::WindowBPOpenStatus> (WindowBPOpenStatusrawValue) = message.WindowBPOpenStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(WindowBPOpenStatusrawValue.value()) << std::endl;
            }
            break;
        case 0x8CF00203: {
            constexpr auto message = canID_0x8CF00203{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ETC1_T::InputShaftSpeed> (InputShaftSpeedrawValue) = message.InputShaftSpeed.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(InputShaftSpeedrawValue.value()) << std::endl;
            }
            break;
        case 0x99FF82E6: {
            constexpr auto message = canID_0x99FF82E6{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum BSM2_BCI::StopSignalVisual> (StopSignalVisualrawValue) = message.StopSignalVisual.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(StopSignalVisualrawValue.value()) << std::endl;
            ara::core::Optional<enum BSM2_BCI::PramsignalVisual> (PramsignalVisualrawValue) = message.PramsignalVisual.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PramsignalVisualrawValue.value()) << std::endl;
            }
            break;
        #if 0
        #endif
        case 0x98FFB647: {
            constexpr auto message = canID_0x98FFB647{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum SuspensionInformationProprietary_CMS::KneelingStatus> (KneelingStatusrawValue) = message.KneelingStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(KneelingStatusrawValue.value()) << std::endl;
            }
            break;
        case 0x98FE4EFE: {
            constexpr auto message = canID_0x98FE4EFE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorControl1_Unknown::PositionOfDoors> (PositionOfDoorsrawValue) = message.PositionOfDoors.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(PositionOfDoorsrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl1_Unknown::Ramp_WheelchairLiftStatus> (Ramp_WheelchairLiftStatusrawValue) = message.Ramp_WheelchairLiftStatus.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Ramp_WheelchairLiftStatusrawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl1_Unknown::Status2OfDoors> (Status2OfDoorsrawValue) = message.Status2OfDoors.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(Status2OfDoorsrawValue.value()) << std::endl;
            }
            break;
        case 0x98FDA5FE: {
            constexpr auto message = canID_0x98FDA5FE{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor1> (LockStatusOfDoor1rawValue) = message.LockStatusOfDoor1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor1rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor1> (OpenStatusOfDoor1rawValue) = message.OpenStatusOfDoor1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor1rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor1> (EnableStatusOfDoor1rawValue) = message.EnableStatusOfDoor1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor1rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor2> (LockStatusOfDoor2rawValue) = message.LockStatusOfDoor2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor2rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor2> (OpenStatusOfDoor2rawValue) = message.OpenStatusOfDoor2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor2rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor2> (EnableStatusOfDoor2rawValue) = message.EnableStatusOfDoor2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor2rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor3> (LockStatusOfDoor3rawValue) = message.LockStatusOfDoor3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor3rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor3> (OpenStatusOfDoor3rawValue) = message.OpenStatusOfDoor3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor3rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor3> (EnableStatusOfDoor3rawValue) = message.EnableStatusOfDoor3.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor3rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor4> (LockStatusOfDoor4rawValue) = message.LockStatusOfDoor4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor4rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor4> (OpenStatusOfDoor4rawValue) = message.OpenStatusOfDoor4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor4rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor4> (EnableStatusOfDoor4rawValue) = message.EnableStatusOfDoor4.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor4rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor5> (LockStatusOfDoor5rawValue) = message.LockStatusOfDoor5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor5rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor5> (OpenStatusOfDoor5rawValue) = message.OpenStatusOfDoor5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor5rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor5> (EnableStatusOfDoor5rawValue) = message.EnableStatusOfDoor5.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor5rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor6> (LockStatusOfDoor6rawValue) = message.LockStatusOfDoor6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor6rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor6> (OpenStatusOfDoor6rawValue) = message.OpenStatusOfDoor6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor6rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor6> (EnableStatusOfDoor6rawValue) = message.EnableStatusOfDoor6.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor6rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor7> (LockStatusOfDoor7rawValue) = message.LockStatusOfDoor7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor7rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor7> (OpenStatusOfDoor7rawValue) = message.OpenStatusOfDoor7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor7rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor7> (EnableStatusOfDoor7rawValue) = message.EnableStatusOfDoor7.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor7rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor8> (LockStatusOfDoor8rawValue) = message.LockStatusOfDoor8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor8rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor8> (OpenStatusOfDoor8rawValue) = message.OpenStatusOfDoor8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor8rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor8> (EnableStatusOfDoor8rawValue) = message.EnableStatusOfDoor8.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor8rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor9> (LockStatusOfDoor9rawValue) = message.LockStatusOfDoor9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor9rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor9> (OpenStatusOfDoor9rawValue) = message.OpenStatusOfDoor9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor9rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor9> (EnableStatusOfDoor9rawValue) = message.EnableStatusOfDoor9.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor9rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::LockStatusOfDoor10> (LockStatusOfDoor10rawValue) = message.LockStatusOfDoor10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(LockStatusOfDoor10rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::OpenStatusOfDoor10> (OpenStatusOfDoor10rawValue) = message.OpenStatusOfDoor10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(OpenStatusOfDoor10rawValue.value()) << std::endl;
            ara::core::Optional<enum DoorControl2_Unknown::EnableStatusOfDoor10> (EnableStatusOfDoor10rawValue) = message.EnableStatusOfDoor10.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EnableStatusOfDoor10rawValue.value()) << std::endl;
            }
            break;
        case 0x98FEEE00: {
            constexpr auto message = canID_0x98FEEE00{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EngineTemp_E::EngineCoolantTemperature> (EngineCoolantTemperaturerawValue) = message.EngineCoolantTemperature.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EngineCoolantTemperaturerawValue.value()) << std::endl;
            }
            break;
        case 0x98FEE500: {
            constexpr auto message = canID_0x98FEE500{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum EngineHoursRevolutions_E::TotalEngineHours> (TotalEngineHoursrawValue) = message.TotalEngineHours.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint32_t>(TotalEngineHoursrawValue.value()) << std::endl;
            }
            break;
        case 0x98F00010: {
            constexpr auto message = canID_0x98F00010{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ERC1_RD::EngineRetarderTorqueMode> (EngineRetarderTorqueModerawValue) = message.EngineRetarderTorqueMode.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(EngineRetarderTorqueModerawValue.value()) << std::endl;
            ara::core::Optional<enum ERC1_RD::ActualRetarderPercentTorque> (ActualRetarderPercentTorquerawValue) = message.ActualRetarderPercentTorque.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ActualRetarderPercentTorquerawValue.value()) << std::endl;
            }
            break;
        case 0x98FEAE30: {
            constexpr auto message = canID_0x98FEAE30{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum AIR1_APS::ServiceBrakeAirPressure1> (ServiceBrakeAirPressure1rawValue) = message.ServiceBrakeAirPressure1.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ServiceBrakeAirPressure1rawValue.value()) << std::endl;
            ara::core::Optional<enum AIR1_APS::ServiceBrakeAirPressure2> (ServiceBrakeAirPressure2rawValue) = message.ServiceBrakeAirPressure2.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint8_t>(ServiceBrakeAirPressure2rawValue.value()) << std::endl;
            }
            break;
        case 0x98FE582F: {
            constexpr auto message = canID_0x98FE582F{};
            std::cout << std::hex << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<enum ASC4_F::BellowPressFrontAxleLeft> (BellowPressFrontAxleLeftrawValue) = message.BellowPressFrontAxleLeft.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BellowPressFrontAxleLeftrawValue.value()) << std::endl;
            ara::core::Optional<enum ASC4_F::BellowPressFrontAxleRight> (BellowPressFrontAxleRightrawValue) = message.BellowPressFrontAxleRight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BellowPressFrontAxleRightrawValue.value()) << std::endl;
            ara::core::Optional<enum ASC4_F::BellowPressRearAxleLeft> (BellowPressRearAxleLeftrawValue) = message.BellowPressRearAxleLeft.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BellowPressRearAxleLeftrawValue.value()) << std::endl;
            ara::core::Optional<enum ASC4_F::BellowPressRearAxleRight> (BellowPressRearAxleRightrawValue) = message.BellowPressRearAxleRight.GetEnumValue(canData);
            std::cout << std::hex << static_cast <std::uint16_t>(BellowPressRearAxleRightrawValue.value()) << std::endl;
            }
            break;
        default:
            std::cout << "Unknown CAN ID: " << std::hex << canId << std::endl;
            break;
    }
    #endif
}

// Extract and publish individual signals as per dbc definition.
void extract_signals(std::string &canId, ara::core::Span<const uint8_t> &canData)
{
   //std::cout << "CAN ID: " << std::hex << canId << "\n";
   //std::cout << "CAN DATA: " << canData[0] << "\n";
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
            //std::cout << "CAN ID = ";
            //std::cout << std::hex << std::uppercase;
            canId << std::hex << std::uppercase << std::setw(2) << std::setfill('0');
            for (int i = 11; i >= 8; --i) {
                canId << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                //std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(payload[i]);
                if (i > 8) std::cout << " "; // Leave a space except for the last byte
            }
            //std::cout << std::endl;
            std::string canId_msg = canId.str();
            //msqt_pub.publish(static_cast<const void*>(canId_msg.c_str()), canId_msg.size());

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
            //extract_signals(canId_msg, canDataSpan);

            // Known CM4 generated signals
            unsigned long CAN_int_value = std::stoul(canId_msg, nullptr, 16);
            switch (CAN_int_value) {
                //Old QNX Version
                case 0xC3AB8000:
                case 0xC3AC0000:
                case 0xC3AC8000:
                case 0xC3AD0000:
                case 0xC3AD8000:
                case 0xC3AE0000:
                case 0xC3AE8000:
                case 0xC3AF0000:
                case 0xC3AF8000:
                case 0xC3B00000:
                case 0xC3B08000:
                case 0xC3B10000:
                case 0xC3B18000:
                case 0xC3B20000:
                case 0xC3B28000:
                case 0xC3B30000:
                case 0xC3B38000:
                case 0xC3B40000:
                case 0xC3B48000:
                case 0xC3B60000:
                case 0xC3B78000:
                case 0xC3B90000:

                //New QNX version
                case 0xC2B40000:
                case 0xC2B60000:
                case 0xC2B80000:
                case 0xC2BA0000:
                case 0xC2BC0000:
                case 0xC2BE0000:
                case 0xC2C00000:
                case 0xC2C20000:
                case 0xC2C40000:
                case 0xC2C60000:
                case 0xC2C80000:
                case 0xC2CA0000:
                case 0xC2CC0000:
                case 0xC2CE0000:
                case 0xC2D00000:
                case 0xC2D20000:
                case 0xC2D40000:
                case 0xC2D60000:
                case 0xC2D80000:
                std::cout << "(" << std::hex << CAN_int_value << ")" << std::endl;
                break;
                default:
                    extract_signals(canId_msg, canDataSpan);
            }
        } else {
            ;
            //std::cout << "Not 20 Bytes: len(" << message->get_payload()->get_length() << ") payload(" << payload << ")" << std::endl;
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

