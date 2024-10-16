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
void processCanMessage(std::string &canId,  ara::core::Span<const uint8_t>& canData) {
    //std::cout << canId << "\n";
    //std::cout << canData[0] << "\n";

    std::cout << "K01" << std::endl;
    switch (std::stoul(canId, nullptr, 16)) {
        case 0x9CFEBE00: {
            constexpr auto message = canID_0x9CFEBE00{};
            std::cout << "A00" << std::endl;
            std::cout << typeid(message).name() << std::endl;
            //std::cout << "CarbRawData: " << message.GetCanIdentifier() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> rawValue = message.CarbRawData.GetRawValue(canData);
            std::cout << typeid(rawValue).name() << std::endl;
            std::cout << "A01" << std::endl;
            /*
            if (rawValue.has_value()) {
                std::cout << "CarbRawData: " << rawValue.value() << std::endl;
            }
            */

            //std::cout << "CarbRawData: " << message.GetRawValue(canData) << std::endl;
            break;
        }
        case 0x98FF20D9: {
            constexpr auto message = canID_0x98FF20D9{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            //std::cout << "tmpract: " << message.tmpract.GetValue(canData) << " °C" << std::endl;
            ara::core::Optional<std::uint16_t> tmpractrawValue = message.tmpract.GetRawValue(canData);
            //std::cout << "tmprset: " << message.tmprset.GetValue(canData) << " °C" << std::endl;
            ara::core::Optional<std::uint16_t> tmprsetrawValue = message.tmprset.GetRawValue(canData);
            break;
        }
/* 
        case 0xC0000000: {
            constexpr auto message = canID_0xC0000000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> TripServiceBrakeApplicationsrawValue = message.TripServiceBrakeApplications.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripServiceBrakeDistancerawValue = message.TripServiceBrakeDistance.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripCompressionBrakeDistancerawValue = message.TripCompressionBrakeDistance.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TotalCompressionBrakeDistancerawValue = message.TotalCompressionBrakeDistance.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TripDriveFuelEconomyGaseousrawValue = message.TripDriveFuelEconomyGaseous.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripCruiseFuelUsedGaseousrawValue = message.TripCruiseFuelUsedGaseous.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripVehicleIdleFuelUsedGaseousrawValue = message.TripVehicleIdleFuelUsedGaseous.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripPTOGovNonmovingFuelUsedrawValue = message.TripPTOGovNonmovingFuelUsed.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripPTOGovernorMovingFuelUsedGasrawValue = message.TripPTOGovernorMovingFuelUsedGas.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripDriveFuelUsedGaseousrawValue = message.TripDriveFuelUsedGaseous.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> LongituderawValue = message.Longitude.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> LatituderawValue = message.Latitude.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SupportVarTranRepRateforAcceSenrawValue = message.SupportVarTranRepRateforAcceSen.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> VerticalAccFigureofMeritExtRangerawValue = message.VerticalAccFigureofMeritExtRange.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LongitudinalAccFigMeritExtRangerawValue = message.LongitudinalAccFigMeritExtRange.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LateralAccFigureofMeritExtRangerawValue = message.LateralAccFigureofMeritExtRange.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> VerticalAccelerationExtRangerawValue = message.VerticalAccelerationExtRange.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> LongitudinalAccelerationExtRangerawValue = message.LongitudinalAccelerationExtRange.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> LateralAccelerationExtendedRangerawValue = message.LateralAccelerationExtendedRange.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EnginePrelubeOilLowPressureThrerawValue = message.EnginePrelubeOilLowPressureThre.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CrankAttemptCntonPresentStartAttrawValue = message.CrankAttemptCntonPresentStartAtt.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineIntakeManifoldPresCntrModerawValue = message.EngineIntakeManifoldPresCntrMode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineRotationDirectionrawValue = message.EngineRotationDirection.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineRatedSpeedrawValue = message.EngineRatedSpeed.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineRatedPowerrawValue = message.EngineRatedPower.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripFanOnTimeDuetoaManualSwitchrawValue = message.TripFanOnTimeDuetoaManualSwitch.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripFanOnTimeDuetotheEngineSysrawValue = message.TripFanOnTimeDuetotheEngineSys.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AxleLocationrawValue = message.AxleLocation.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ADRCompliantHeaterInstallationrawValue = message.ADRCompliantHeaterInstallation.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CabHeatingZonerawValue = message.CabHeatingZone.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineHeatingZonerawValue = message.EngineHeatingZone.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CabVentilationrawValue = message.CabVentilation.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AuxiliaryHeaterWaterPumpStatusrawValue = message.AuxiliaryHeaterWaterPumpStatus.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AuxiliaryHeaterOutputPowerPercerawValue = message.AuxiliaryHeaterOutputPowerPerce.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AuxiliaryHeaterInputAirTemprawValue = message.AuxiliaryHeaterInputAirTemp.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MemorylevelrawValue = message.Memorylevel.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuspensionControlRefusalInforawValue = message.SuspensionControlRefusalInfo.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SpeedDependantLevelControlStatusrawValue = message.SpeedDependantLevelControlStatus.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AllowLevelControlDuringBrakingStrawValue = message.AllowLevelControlDuringBrakingSt.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuspensionRemotecontrol2rawValue = message.SuspensionRemotecontrol2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuspensionRemoteControl1rawValue = message.SuspensionRemoteControl1.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LiftAxle2PositionrawValue = message.LiftAxle2Position.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> RearAxleinBumperRangerawValue = message.RearAxleinBumperRange.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> FrontAxleinBumperRangerawValue = message.FrontAxleinBumperRange.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DoorReleaserawValue = message.DoorRelease.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> VehicleMotionInhibitrawValue = message.VehicleMotionInhibit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SecurityDevicerawValue = message.SecurityDevice.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LevelControlModerawValue = message.LevelControlMode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> KneelingInformationrawValue = message.KneelingInformation.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LiftingControlModeRearAxlerawValue = message.LiftingControlModeRearAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LiftingControlModeFrontAxlerawValue = message.LiftingControlModeFrontAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LoweringControlModeRearAxlerawValue = message.LoweringControlModeRearAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> LoweringControlModeFrontAxlerawValue = message.LoweringControlModeFrontAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AboveNominalLevelRearAxlerawValue = message.AboveNominalLevelRearAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AboveNominalLevelFrontAxlerawValue = message.AboveNominalLevelFrontAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BelowNominalLevelFrontAxlerawValue = message.BelowNominalLevelFrontAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BelowNominalLevelRearAxlerawValue = message.BelowNominalLevelRearAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> NominalLevelRearAxlerawValue = message.NominalLevelRearAxle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> RollAngleFigureofMeritrawValue = message.RollAngleFigureofMerit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PitchAngleFigureofMeritrawValue = message.PitchAngleFigureofMerit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PitchRateFigureofMeritrawValue = message.PitchRateFigureofMerit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PitchandRollCompensatedrawValue = message.PitchandRollCompensated.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> RollandPitchMeasurementLatencyrawValue = message.RollandPitchMeasurementLatency.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> actual_eng_percent_torque_fracrawValue = message.actual_eng_percent_torque_frac.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Demand_Percent_Torque_ArawValue = message.Engine_Demand_Percent_Torque_A.GetRawValue(canData);
            ara::core::Optional<Engine_Starter_Mode> Engine_Starter_ModerawValue = message.Engine_Starter_Mode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Src_Add_Cont_Dev_Eng_Control_ArawValue = message.Src_Add_Cont_Dev_Eng_Control_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedrawValue = message.EngineSpeed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TorqueEngineActualPercentrawValue = message.TorqueEngineActualPercent.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TorqueDriversEngineDemandrawValue = message.TorqueDriversEngineDemand.GetRawValue(canData);
            ara::core::Optional<OtaTimerStatus_17> OtaTimerStatus_17rawValue = message.OtaTimerStatus_17.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PowerAvailable_5ArawValue = message.PowerAvailable_5A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EnergyAvailable_5ArawValue = message.EnergyAvailable_5A.GetRawValue(canData);
            ara::core::Optional<V2gStatusMode_5A> V2gStatusMode_5ArawValue = message.V2gStatusMode_5A.GetRawValue(canData);
            ara::core::Optional<V2gAllowedPermissions_5A> V2gAllowedPermissions_5ArawValue = message.V2gAllowedPermissions_5A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ChecksumValue_5A_4BrawValue = message.ChecksumValue_5A_4B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CounterValue_5A_4BrawValue = message.CounterValue_5A_4B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Power_Request_5A_4BrawValue = message.Power_Request_5A_4B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Energy_Request_5A_4BrawValue = message.Energy_Request_5A_4B.GetRawValue(canData);
            ara::core::Optional<Grid_V2X_Request_5A_4B> Grid_V2X_Request_5A_4BrawValue = message.Grid_V2X_Request_5A_4B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Comm_Header_Byte_5A_4BrawValue = message.Comm_Header_Byte_5A_4B.GetRawValue(canData);
            ara::core::Optional<Grid_V2X_Confirmation_5A_4B> Grid_V2X_Confirmation_5A_4BrawValue = message.Grid_V2X_Confirmation_5A_4B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ChecksumValue_5A_4ArawValue = message.ChecksumValue_5A_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CounterValue_5A_4ArawValue = message.CounterValue_5A_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PowerRequest_5A_4ArawValue = message.PowerRequest_5A_4A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EnergyRequest_5A_4ArawValue = message.EnergyRequest_5A_4A.GetRawValue(canData);
            ara::core::Optional<GridV2xConfirmation_5A_4A> GridV2xConfirmation_5A_4ArawValue = message.GridV2xConfirmation_5A_4A.GetRawValue(canData);
            ara::core::Optional<GridV2xRequest_5A_4A> GridV2xRequest_5A_4ArawValue = message.GridV2xRequest_5A_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CommHeaderByte_5A_4ArawValue = message.CommHeaderByte_5A_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineDemandPercentTorq_4A_03rawValue = message.EngineDemandPercentTorq_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineStarterMode_4A_03rawValue = message.EngineStarterMode_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SourAddofContDevforEngCont_4A_03rawValue = message.SourAddofContDevforEngCont_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeed_4A_03rawValue = message.EngineSpeed_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ActualEnginePercentTorq_4A_03rawValue = message.ActualEnginePercentTorq_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DriversDemandEngPerTorq_4A_03rawValue = message.DriversDemandEngPerTorq_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ActualEngPercTorqueFrac_4A_03rawValue = message.ActualEngPercTorqueFrac_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineTorqueMode_4A_03rawValue = message.EngineTorqueMode_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripDistance_4A_03rawValue = message.TripDistance_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TotalVehicleDistance_4A_03rawValue = message.TotalVehicleDistance_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> CargoAmbientTemperature_4A_03rawValue = message.CargoAmbientTemperature_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> FuelLevel2_4A_03rawValue = message.FuelLevel2_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFuelFilterDiffPress_4A_03rawValue = message.EngineFuelFilterDiffPress_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineOilFilterDiffPress_4A_03rawValue = message.EngineOilFilterDiffPress_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> WasherFluidLevel_4A_03rawValue = message.WasherFluidLevel_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngOilFilterDiffPressExt_4A_03rawValue = message.EngOilFilterDiffPressExt_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> FuelLevel1_4A_03rawValue = message.FuelLevel1_4A_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECUTyprawValue = message.ECUTyp.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECUParNumrawValue = message.ECUParNum.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECUSerNumrawValue = message.ECUSerNum.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECULocrawValue = message.ECULoc.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECUManNamrawValue = message.ECUManNam.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECUHarIDrawValue = message.ECUHarID.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            ara::core::Optional<std::uint64_t> BCMCommData_4A_21rawValue = message.BCMCommData_4A_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CompressorConfiguration_21rawValue = message.CompressorConfiguration_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AlternatorConfiguration_21rawValue = message.AlternatorConfiguration_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> BatteryPackCapacity_5BrawValue = message.BatteryPackCapacity_5B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> BatteryPackStateOfCharge_5BrawValue = message.BatteryPackStateOfCharge_5B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AirCompressorStatus_30rawValue = message.AirCompressorStatus_30.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AlternatorSetpointVoltageCmd_11rawValue = message.AlternatorSetpointVoltageCmd_11.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AverageCompressorDutyCycle_30rawValue = message.AverageCompressorDutyCycle_30.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AverageVehicleSpeed_30rawValue = message.AverageVehicleSpeed_30.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaximumAirTankPrLim_30rawValue = message.MaximumAirTankPrLim_30.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinimumAirTankPrLim_30rawValue = message.MinimumAirTankPrLim_30.GetRawValue(canData);
            ara::core::Optional<std::int8_t> ExternalChargeInhibitSt_30rawValue = message.ExternalChargeInhibitSt_30.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ServiceBrake1AirPrRequest_11rawValue = message.ServiceBrake1AirPrRequest_11.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ServiceBrake2AirPrRequest_11rawValue = message.ServiceBrake2AirPrRequest_11.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ServiceBrakeCircuit1AirPr_21rawValue = message.ServiceBrakeCircuit1AirPr_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ServiceBrakeCircuit2AirPr_21rawValue = message.ServiceBrakeCircuit2AirPr_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Alternator1Status_1ArawValue = message.Alternator1Status_1A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AlternatorSpeed_1ArawValue = message.AlternatorSpeed_1A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AlternatorTrqRampMaxSpeedCmd_1ArawValue = message.AlternatorTrqRampMaxSpeedCmd_1A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AlternatorTorqueRampTimeCmd_1ArawValue = message.AlternatorTorqueRampTimeCmd_1A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AlternatorExcitMaxCurentLimit_1ArawValue = message.AlternatorExcitMaxCurentLimit_1A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AlternatorSetpointVoltageCmd_1ArawValue = message.AlternatorSetpointVoltageCmd_1A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> StatusofSteeringAxle_5ArawValue = message.StatusofSteeringAxle_5A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MtrGrtr1Temp1_5ArawValue = message.MtrGrtr1Temp1_5A.GetRawValue(canData);
            ara::core::Optional<OvrdCtrlModePriority_7> OvrdCtrlModePriority_7rawValue = message.OvrdCtrlModePriority_7.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngRqstdTrqLimit_7rawValue = message.EngRqstdTrqLimit_7.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngRqstdSpdLimit_7rawValue = message.EngRqstdSpdLimit_7.GetRawValue(canData);
            ara::core::Optional<EngRqstdSpdCtrlCondns_7> EngRqstdSpdCtrlCondns_7rawValue = message.EngRqstdSpdCtrlCondns_7.GetRawValue(canData);
            ara::core::Optional<EngOvrdCtrlMode_7> EngOvrdCtrlMode_7rawValue = message.EngOvrdCtrlMode_7.GetRawValue(canData);
            ara::core::Optional<AtleastOnePtoEngaged_3> AtleastOnePtoEngaged_3rawValue = message.AtleastOnePtoEngaged_3.GetRawValue(canData);
            ara::core::Optional<EngageMntcnstPtoEngFlyWheel_3> EngageMntcnstPtoEngFlyWheel_3rawValue = message.EngageMntcnstPtoEngFlyWheel_3.GetRawValue(canData);
            ara::core::Optional<EngagementStatusPtoEngFlyWheel_3> EngagementStatusPtoEngFlyWheel_3rawValue = message.EngagementStatusPtoEngFlyWheel_3.GetRawValue(canData);
            ara::core::Optional<EnggMntTrnsFrcsOtputShaftPto_3> EnggMntTrnsFrcsOtputShaftPto_3rawValue = message.EnggMntTrnsFrcsOtputShaftPto_3.GetRawValue(canData);
            ara::core::Optional<EnggMntTrnsFrcsOtptShaftPto_3> EnggMntTrnsFrcsOtptShaftPto_3rawValue = message.EnggMntTrnsFrcsOtptShaftPto_3.GetRawValue(canData);
            ara::core::Optional<EnggMntTransOutputShaftPto_3> EnggMntTransOutputShaftPto_3rawValue = message.EnggMntTransOutputShaftPto_3.GetRawValue(canData);
            ara::core::Optional<EnggMntTransInputShaftPto2_3> EnggMntTransInputShaftPto2_3rawValue = message.EnggMntTransInputShaftPto2_3.GetRawValue(canData);
            ara::core::Optional<EnggMntTransInputShaftPto1_3> EnggMntTransInputShaftPto1_3rawValue = message.EnggMntTransInputShaftPto1_3.GetRawValue(canData);
            ara::core::Optional<EnggMntSttsPtoAccessoryDrive2_3> EnggMntSttsPtoAccessoryDrive2_3rawValue = message.EnggMntSttsPtoAccessoryDrive2_3.GetRawValue(canData);
            ara::core::Optional<EnggMntSttsPtoAccessoryDrive1_3> EnggMntSttsPtoAccessoryDrive1_3rawValue = message.EnggMntSttsPtoAccessoryDrive1_3.GetRawValue(canData);
            ara::core::Optional<EnggMntStsTransOutputShaftPto_3> EnggMntStsTransOutputShaftPto_3rawValue = message.EnggMntStsTransOutputShaftPto_3.GetRawValue(canData);
            ara::core::Optional<EnggMntStsTransInputShaftPto2_3> EnggMntStsTransInputShaftPto2_3rawValue = message.EnggMntStsTransInputShaftPto2_3.GetRawValue(canData);
            ara::core::Optional<EnggMntStsTransInputShaftPto1_3> EnggMntStsTransInputShaftPto1_3rawValue = message.EnggMntStsTransInputShaftPto1_3.GetRawValue(canData);
            ara::core::Optional<EnggMntcnsntPtoAccssoryDrive2_3> EnggMntcnsntPtoAccssoryDrive2_3rawValue = message.EnggMntcnsntPtoAccssoryDrive2_3.GetRawValue(canData);
            ara::core::Optional<EnggMntcnsntPtoAccssoryDrive1_3> EnggMntcnsntPtoAccssoryDrive1_3rawValue = message.EnggMntcnsntPtoAccssoryDrive1_3.GetRawValue(canData);
            ara::core::Optional<EnblSwtchTrnscseOutputShftPto_3> EnblSwtchTrnscseOutputShftPto_3rawValue = message.EnblSwtchTrnscseOutputShftPto_3.GetRawValue(canData);
            ara::core::Optional<EnbleSwitchTransOutputShftPto_3> EnbleSwitchTransOutputShftPto_3rawValue = message.EnbleSwitchTransOutputShftPto_3.GetRawValue(canData);
            ara::core::Optional<EnbleSwitchTransInputShftPto1_3> EnbleSwitchTransInputShftPto1_3rawValue = message.EnbleSwitchTransInputShftPto1_3.GetRawValue(canData);
            ara::core::Optional<EnbleSwitchPtoAccessoryDrive2_3> EnbleSwitchPtoAccessoryDrive2_3rawValue = message.EnbleSwitchPtoAccessoryDrive2_3.GetRawValue(canData);
            ara::core::Optional<EnbleSwitchPtoAccessoryDrive1_3> EnbleSwitchPtoAccessoryDrive1_3rawValue = message.EnbleSwitchPtoAccessoryDrive1_3.GetRawValue(canData);
            ara::core::Optional<EnableSwtchTransInputShftPto2_3> EnableSwtchTransInputShftPto2_3rawValue = message.EnableSwtchTransInputShftPto2_3.GetRawValue(canData);
            ara::core::Optional<EnableSwitchPtoEngFlyWheel_3> EnableSwitchPtoEngFlyWheel_3rawValue = message.EnableSwitchPtoEngFlyWheel_3.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock10Status_53rawValue = message.BeltLock10Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock9Status_53rawValue = message.BeltLock9Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock8Status_53rawValue = message.BeltLock8Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock7Status_53rawValue = message.BeltLock7Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PsngrAirbagDeactiveSwitchSts_53rawValue = message.PsngrAirbagDeactiveSwitchSts_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock6Status_53rawValue = message.BeltLock6Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock5Status_53rawValue = message.BeltLock5Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock4Status_53rawValue = message.BeltLock4Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BeltLock3Status_53rawValue = message.BeltLock3Status_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PassengerBeltLockStatus_53rawValue = message.PassengerBeltLockStatus_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DriverBeltlockStatus_53rawValue = message.DriverBeltlockStatus_53.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineTotalAverageFuelEconomy_0rawValue = message.EngineTotalAverageFuelEconomy_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineTotalAverageFuelRate_0rawValue = message.EngineTotalAverageFuelRate_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> numberofreversegearratios_03rawValue = message.numberofreversegearratios_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> numberofforwardgearratios_03rawValue = message.numberofforwardgearratios_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> transgearratio_03rawValue = message.transgearratio_03.GetRawValue(canData);
            ara::core::Optional<enblswtchtrnscseoutputshftpto_0> enblswtchtrnscseoutputshftpto_0rawValue = message.enblswtchtrnscseoutputshftpto_0.GetRawValue(canData);
            ara::core::Optional<enbleswitchtransoutputshftpto_0> enbleswitchtransoutputshftpto_0rawValue = message.enbleswitchtransoutputshftpto_0.GetRawValue(canData);
            ara::core::Optional<enableswtchtransinputshftpto2_0> enableswtchtransinputshftpto2_0rawValue = message.enableswtchtransinputshftpto2_0.GetRawValue(canData);
            ara::core::Optional<enbleswitchtransinputshftpto1_0> enbleswitchtransinputshftpto1_0rawValue = message.enbleswitchtransinputshftpto1_0.GetRawValue(canData);
            ara::core::Optional<enableswitchptoengflywheel_0> enableswitchptoengflywheel_0rawValue = message.enableswitchptoengflywheel_0.GetRawValue(canData);
            ara::core::Optional<enbleswitchptoaccessorydrive1_0> enbleswitchptoaccessorydrive1_0rawValue = message.enbleswitchptoaccessorydrive1_0.GetRawValue(canData);
            ara::core::Optional<enbleswitchptoaccessorydrive2_0> enbleswitchptoaccessorydrive2_0rawValue = message.enbleswitchptoaccessorydrive2_0.GetRawValue(canData);
            ara::core::Optional<enggmnttrnsfrcsotptshaftpto_0> enggmnttrnsfrcsotptshaftpto_0rawValue = message.enggmnttrnsfrcsotptshaftpto_0.GetRawValue(canData);
            ara::core::Optional<enggmnttransoutputshaftpto_0> enggmnttransoutputshaftpto_0rawValue = message.enggmnttransoutputshaftpto_0.GetRawValue(canData);
            ara::core::Optional<enggmnttransinputshaftpto2_0> enggmnttransinputshaftpto2_0rawValue = message.enggmnttransinputshaftpto2_0.GetRawValue(canData);
            ara::core::Optional<enggmnttransinputshaftpto1_0> enggmnttransinputshaftpto1_0rawValue = message.enggmnttransinputshaftpto1_0.GetRawValue(canData);
            ara::core::Optional<engagemntcnstptoengflywheel_0> engagemntcnstptoengflywheel_0rawValue = message.engagemntcnstptoengflywheel_0.GetRawValue(canData);
            ara::core::Optional<enggmntcnsntptoaccssorydrive1_0> enggmntcnsntptoaccssorydrive1_0rawValue = message.enggmntcnsntptoaccssorydrive1_0.GetRawValue(canData);
            ara::core::Optional<enggmntcnsntptoaccssorydrive2_0> enggmntcnsntptoaccssorydrive2_0rawValue = message.enggmntcnsntptoaccssorydrive2_0.GetRawValue(canData);
            ara::core::Optional<enggmnttrnsfrcsotputshaftpto_0> enggmnttrnsfrcsotputshaftpto_0rawValue = message.enggmnttrnsfrcsotputshaftpto_0.GetRawValue(canData);
            ara::core::Optional<enggmntststransoutputshaftpto_0> enggmntststransoutputshaftpto_0rawValue = message.enggmntststransoutputshaftpto_0.GetRawValue(canData);
            ara::core::Optional<enggmntststransinputshaftpto2_0> enggmntststransinputshaftpto2_0rawValue = message.enggmntststransinputshaftpto2_0.GetRawValue(canData);
            ara::core::Optional<enggmntststransinputshaftpto1_0> enggmntststransinputshaftpto1_0rawValue = message.enggmntststransinputshaftpto1_0.GetRawValue(canData);
            ara::core::Optional<engagementstatusptoengflywheel_0> engagementstatusptoengflywheel_0rawValue = message.engagementstatusptoengflywheel_0.GetRawValue(canData);
            ara::core::Optional<enggmntsttsptoaccessorydrive1_0> enggmntsttsptoaccessorydrive1_0rawValue = message.enggmntsttsptoaccessorydrive1_0.GetRawValue(canData);
            ara::core::Optional<enggmntsttsptoaccessorydrive2_0> enggmntsttsptoaccessorydrive2_0rawValue = message.enggmntsttsptoaccessorydrive2_0.GetRawValue(canData);
            ara::core::Optional<atleastoneptoengaged_0> atleastoneptoengaged_0rawValue = message.atleastoneptoengaged_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engrqstdtrqlimit_0rawValue = message.engrqstdtrqlimit_0.GetRawValue(canData);
            ara::core::Optional<engovrdctrlmode_0> engovrdctrlmode_0rawValue = message.engovrdctrlmode_0.GetRawValue(canData);
            ara::core::Optional<RunningLightCmd> RunningLightCmdrawValue = message.RunningLightCmd.GetRawValue(canData);
            ara::core::Optional<AltBeamHeadLightCmd> AltBeamHeadLightCmdrawValue = message.AltBeamHeadLightCmd.GetRawValue(canData);
            ara::core::Optional<LowBeamHeadLightCmd> LowBeamHeadLightCmdrawValue = message.LowBeamHeadLightCmd.GetRawValue(canData);
            ara::core::Optional<HighBeamHeadLightCmd> HighBeamHeadLightCmdrawValue = message.HighBeamHeadLightCmd.GetRawValue(canData);
            ara::core::Optional<TractorFrontFogLightsCmd> TractorFrontFogLightsCmdrawValue = message.TractorFrontFogLightsCmd.GetRawValue(canData);
            ara::core::Optional<RotatingBeaconLightCmd> RotatingBeaconLightCmdrawValue = message.RotatingBeaconLightCmd.GetRawValue(canData);
            ara::core::Optional<BackUpLightAndAlarmHornCmd> BackUpLightAndAlarmHornCmdrawValue = message.BackUpLightAndAlarmHornCmd.GetRawValue(canData);
            ara::core::Optional<CenterStopLightCmd> CenterStopLightCmdrawValue = message.CenterStopLightCmd.GetRawValue(canData);
            ara::core::Optional<Right_Stop_Light_Command> Right_Stop_Light_CommandrawValue = message.Right_Stop_Light_Command.GetRawValue(canData);
            ara::core::Optional<Left_Stop_Light_Command> Left_Stop_Light_CommandrawValue = message.Left_Stop_Light_Command.GetRawValue(canData);
            ara::core::Optional<ImplementClearanceLightCmd> ImplementClearanceLightCmdrawValue = message.ImplementClearanceLightCmd.GetRawValue(canData);
            ara::core::Optional<TractorClearanceLightCmd> TractorClearanceLightCmdrawValue = message.TractorClearanceLightCmd.GetRawValue(canData);
            ara::core::Optional<ImplementMarkerLightCmd> ImplementMarkerLightCmdrawValue = message.ImplementMarkerLightCmd.GetRawValue(canData);
            ara::core::Optional<TractorMarkerLightCmd> TractorMarkerLightCmdrawValue = message.TractorMarkerLightCmd.GetRawValue(canData);
            ara::core::Optional<RearFogLightCmd> RearFogLightCmdrawValue = message.RearFogLightCmd.GetRawValue(canData);
            ara::core::Optional<TrctrUndrsdeMountedWorkLightsCmd> TrctrUndrsdeMountedWorkLightsCmdrawValue = message.TrctrUndrsdeMountedWorkLightsCmd.GetRawValue(canData);
            ara::core::Optional<TrctrRearLowMountedWorkLightsCmd> TrctrRearLowMountedWorkLightsCmdrawValue = message.TrctrRearLowMountedWorkLightsCmd.GetRawValue(canData);
            ara::core::Optional<TrctrRarHighMountedWorkLightsCmd> TrctrRarHighMountedWorkLightsCmdrawValue = message.TrctrRarHighMountedWorkLightsCmd.GetRawValue(canData);
            ara::core::Optional<TrctrSideLowMountedWorkLightsCmd> TrctrSideLowMountedWorkLightsCmdrawValue = message.TrctrSideLowMountedWorkLightsCmd.GetRawValue(canData);
            ara::core::Optional<TrctrSdeHighMountedWorkLightsCmd> TrctrSdeHighMountedWorkLightsCmdrawValue = message.TrctrSdeHighMountedWorkLightsCmd.GetRawValue(canData);
            ara::core::Optional<TrctrFrntLowMountedWorkLightsCmd> TrctrFrntLowMountedWorkLightsCmdrawValue = message.TrctrFrntLowMountedWorkLightsCmd.GetRawValue(canData);
            ara::core::Optional<TrctrFrntHghMountedWorkLightsCmd> TrctrFrntHghMountedWorkLightsCmdrawValue = message.TrctrFrntHghMountedWorkLightsCmd.GetRawValue(canData);
            ara::core::Optional<ImplementOEMOption2LightCmd> ImplementOEMOption2LightCmdrawValue = message.ImplementOEMOption2LightCmd.GetRawValue(canData);
            ara::core::Optional<ImplementOEMOption1LightCmd> ImplementOEMOption1LightCmdrawValue = message.ImplementOEMOption1LightCmd.GetRawValue(canData);
            ara::core::Optional<ImplementRightFacingWorkLightCmd> ImplementRightFacingWorkLightCmdrawValue = message.ImplementRightFacingWorkLightCmd.GetRawValue(canData);
            ara::core::Optional<ImplementLeftFacingWorkLightCmd> ImplementLeftFacingWorkLightCmdrawValue = message.ImplementLeftFacingWorkLightCmd.GetRawValue(canData);
            ara::core::Optional<LightingDataRqCmd> LightingDataRqCmdrawValue = message.LightingDataRqCmd.GetRawValue(canData);
            ara::core::Optional<ImplmentRightForwardWorkLightCmd> ImplmentRightForwardWorkLightCmdrawValue = message.ImplmentRightForwardWorkLightCmd.GetRawValue(canData);
            ara::core::Optional<ImplementLeftForwardWorkLightCmd> ImplementLeftForwardWorkLightCmdrawValue = message.ImplementLeftForwardWorkLightCmd.GetRawValue(canData);
            ara::core::Optional<ImplementRearWorkLightCmd> ImplementRearWorkLightCmdrawValue = message.ImplementRearWorkLightCmd.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> frontaxlespeed_brawValue = message.frontaxlespeed_b.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> relativespdfrontaxleleftwheel_brawValue = message.relativespdfrontaxleleftwheel_b.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> relativespdfrontaxlerightwheel_brawValue = message.relativespdfrontaxlerightwheel_b.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> relativespdrearaxle1leftwheel_brawValue = message.relativespdrearaxle1leftwheel_b.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> relativespdrearaxle1rightwheel_brawValue = message.relativespdrearaxle1rightwheel_b.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> relativespdrearaxle2leftwheel_brawValue = message.relativespdrearaxle2leftwheel_b.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> relativespdrearaxle2rightwheel_brawValue = message.relativespdrearaxle2rightwheel_b.GetRawValue(canData);
            ara::core::Optional<transdrivelineengaged_03> transdrivelineengaged_03rawValue = message.transdrivelineengaged_03.GetRawValue(canData);
            ara::core::Optional<trnstrqconverterlockupengaged_03> trnstrqconverterlockupengaged_03rawValue = message.trnstrqconverterlockupengaged_03.GetRawValue(canData);
            ara::core::Optional<transshiftinprocess_03> transshiftinprocess_03rawValue = message.transshiftinprocess_03.GetRawValue(canData);
            ara::core::Optional<transtorquelockupprocess_03> transtorquelockupprocess_03rawValue = message.transtorquelockupprocess_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> transoutputshaftspeed_03rawValue = message.transoutputshaftspeed_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> percentclutchslip_03rawValue = message.percentclutchslip_03.GetRawValue(canData);
            ara::core::Optional<engmomentaryoverspeedenable_03> engmomentaryoverspeedenable_03rawValue = message.engmomentaryoverspeedenable_03.GetRawValue(canData);
            ara::core::Optional<progressiveshiftdisable_03> progressiveshiftdisable_03rawValue = message.progressiveshiftdisable_03.GetRawValue(canData);
            ara::core::Optional<momentaryengmaxpowerenable_03> momentaryengmaxpowerenable_03rawValue = message.momentaryengmaxpowerenable_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> transinputshaftspeed_03rawValue = message.transinputshaftspeed_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> srcaddrssofcntrldvcfrtrnsctrl_03rawValue = message.srcaddrssofcntrldvcfrtrnsctrl_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> IlluminationBrightnessPercent_21rawValue = message.IlluminationBrightnessPercent_21.GetRawValue(canData);
            ara::core::Optional<OvrdCtrlModePriority_0_24> OvrdCtrlModePriority_0_24rawValue = message.OvrdCtrlModePriority_0_24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngRqstdTrqLimit_0_24rawValue = message.EngRqstdTrqLimit_0_24.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngRqstdSpdLimit_0_24rawValue = message.EngRqstdSpdLimit_0_24.GetRawValue(canData);
            ara::core::Optional<EngRqstdSpdCtrlCondns_0_24> EngRqstdSpdCtrlCondns_0_24rawValue = message.EngRqstdSpdCtrlCondns_0_24.GetRawValue(canData);
            ara::core::Optional<EngOvrdCtrlMode_0_24> EngOvrdCtrlMode_0_24rawValue = message.EngOvrdCtrlMode_0_24.GetRawValue(canData);
            ara::core::Optional<TSC1_CtrlPurpose_F_B> TSC1_CtrlPurpose_F_BrawValue = message.TSC1_CtrlPurpose_F_B.GetRawValue(canData);
            ara::core::Optional<OvrdCtrlModePriority_F_B> OvrdCtrlModePriority_F_BrawValue = message.OvrdCtrlModePriority_F_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngRqstdTrqLimit_F_BrawValue = message.EngRqstdTrqLimit_F_B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngRqstdSpdLimit_F_BrawValue = message.EngRqstdSpdLimit_F_B.GetRawValue(canData);
            ara::core::Optional<EngOvrdCtrlMode_F_B> EngOvrdCtrlMode_F_BrawValue = message.EngOvrdCtrlMode_F_B.GetRawValue(canData);
            ara::core::Optional<TSC1_CtrlPurpose_0_B> TSC1_CtrlPurpose_0_BrawValue = message.TSC1_CtrlPurpose_0_B.GetRawValue(canData);
            ara::core::Optional<OvrdCtrlModePriority_0_B> OvrdCtrlModePriority_0_BrawValue = message.OvrdCtrlModePriority_0_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngRqstdTrqLimit_0_BrawValue = message.EngRqstdTrqLimit_0_B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngRqstdSpdLimit_0_BrawValue = message.EngRqstdSpdLimit_0_B.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> trip_time_in_topgearrawValue = message.trip_time_in_topgear.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> trip_time_in_gear_downrawValue = message.trip_time_in_gear_down.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> trip_time_in_derate_by_enginerawValue = message.trip_time_in_derate_by_engine.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> trip_time_in_vslrawValue = message.trip_time_in_vsl.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> xbr_message_checksumrawValue = message.xbr_message_checksum.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> xbr_message_counterrawValue = message.xbr_message_counter.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> xbr_urgencyrawValue = message.xbr_urgency.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> xbr_control_moderawValue = message.xbr_control_mode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> xbr_priorityrawValue = message.xbr_priority.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> xbr_ebi_moderawValue = message.xbr_ebi_mode.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> external_acceleration_demandrawValue = message.external_acceleration_demand.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> road_departure_aebs_state_2ArawValue = message.road_departure_aebs_state_2A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> timetocollisionrelevantobj_2ArawValue = message.timetocollisionrelevantobj_2A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> bendoff_probab_relvnt_object_2ArawValue = message.bendoff_probab_relvnt_object_2A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> relevant_object_detect_aebs_2ArawValue = message.relevant_object_detect_aebs_2A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> collision_warning_level_2ArawValue = message.collision_warning_level_2A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> advan_emer_braking_sys_state_2ArawValue = message.advan_emer_braking_sys_state_2A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EcoRolPerSta_ERPSrawValue = message.EcoRolPerSta_ERPS.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BraPedSwi2_B2rawValue = message.BraPedSwi2_B2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi9_EngRCHiResrawValue = message.MaxStaFlyTorPoi9_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi8_EngRCHiResrawValue = message.MaxStaFlyTorPoi8_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi7_EngRCHiResrawValue = message.MaxStaFlyTorPoi7_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi6_EngRCHiResrawValue = message.MaxStaFlyTorPoi6_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi5_EngRCHiResrawValue = message.MaxStaFlyTorPoi5_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi4_EngRCHiResrawValue = message.MaxStaFlyTorPoi4_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi3_EngRCHiResrawValue = message.MaxStaFlyTorPoi3_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi2_EngRCHiResrawValue = message.MaxStaFlyTorPoi2_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxStaFlyTorPoi1_EngRCHiResrawValue = message.MaxStaFlyTorPoi1_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi9_EngRCHiResrawValue = message.MinStaFlyTorPoi9_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi8_EngRCHiResrawValue = message.MinStaFlyTorPoi8_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi7_EngRCHiResrawValue = message.MinStaFlyTorPoi7_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi6_EngRCHiResrawValue = message.MinStaFlyTorPoi6_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi5_EngRCHiResrawValue = message.MinStaFlyTorPoi5_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi4_EngRCHiResrawValue = message.MinStaFlyTorPoi4_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi3_EngRCHiResrawValue = message.MinStaFlyTorPoi3_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi2_EngRCHiResrawValue = message.MinStaFlyTorPoi2_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinStaFlyTorPoi1_EngRCHiResrawValue = message.MinStaFlyTorPoi1_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi9_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi9_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi8_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi8_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi7_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi7_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi6_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi6_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi5_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi5_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi4_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi4_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi3_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi3_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi2_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi2_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MaxEngBraFlyTorPoi1_EngRCHiResrawValue = message.MaxEngBraFlyTorPoi1_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi9_EngRCHiResrawValue = message.EngSpePoi9_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi8_EngRCHiResrawValue = message.EngSpePoi8_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi7_EngRCHiResrawValue = message.EngSpePoi7_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi6_EngRCHiResrawValue = message.EngSpePoi6_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi5_EngRCHiResrawValue = message.EngSpePoi5_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi4_EngRCHiResrawValue = message.EngSpePoi4_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi3_EngRCHiResrawValue = message.EngSpePoi3_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi2_EngRCHiResrawValue = message.EngSpePoi2_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngSpePoi1_EngRCHiResrawValue = message.EngSpePoi1_EngRCHiRes.GetRawValue(canData);
            ara::core::Optional<PCC_Drvln_Diseng_Inhbt_Req> PCC_Drvln_Diseng_Inhbt_ReqrawValue = message.PCC_Drvln_Diseng_Inhbt_Req.GetRawValue(canData);
            ara::core::Optional<Tra_Idle_Gov_Fuel_Inhib_Support> Tra_Idle_Gov_Fuel_Inhib_SupportrawValue = message.Tra_Idle_Gov_Fuel_Inhib_Support.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Outlet_Corrected_NOxrawValue = message.Outlet_Corrected_NOx.GetRawValue(canData);
            ara::core::Optional<TC1_Trans_Mode4_BobTail> TC1_Trans_Mode4_BobTailrawValue = message.TC1_Trans_Mode4_BobTail.GetRawValue(canData);
            ara::core::Optional<TC1_ECoast_Ctrl> TC1_ECoast_CtrlrawValue = message.TC1_ECoast_Ctrl.GetRawValue(canData);
            ara::core::Optional<Clstr_Dead_Pedal_Txt_Alert> Clstr_Dead_Pedal_Txt_AlertrawValue = message.Clstr_Dead_Pedal_Txt_Alert.GetRawValue(canData);
            ara::core::Optional<Clstr_CAP_Txt_Alert> Clstr_CAP_Txt_AlertrawValue = message.Clstr_CAP_Txt_Alert.GetRawValue(canData);
            ara::core::Optional<Clstr_HC_Desorb_Txt_Alert> Clstr_HC_Desorb_Txt_AlertrawValue = message.Clstr_HC_Desorb_Txt_Alert.GetRawValue(canData);
            ara::core::Optional<Clstr_Parked_Regen_Txt_Alert> Clstr_Parked_Regen_Txt_AlertrawValue = message.Clstr_Parked_Regen_Txt_Alert.GetRawValue(canData);
            ara::core::Optional<Clstr_Regen_Fail_Txt_Alert> Clstr_Regen_Fail_Txt_AlertrawValue = message.Clstr_Regen_Fail_Txt_Alert.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Flame_Start_Cntrl_Sol_StatusrawValue = message.Flame_Start_Cntrl_Sol_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Glow_Plug_Relay_Enable_StatusrawValue = message.Glow_Plug_Relay_Enable_Status.GetRawValue(canData);
            ara::core::Optional<Status_Actuator1> Status_Actuator1rawValue = message.Status_Actuator1.GetRawValue(canData);
            ara::core::Optional<Status_Actuator0> Status_Actuator0rawValue = message.Status_Actuator0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Fan_Control_Duty_CyclerawValue = message.Fan_Control_Duty_Cycle.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Des_Exh_Man_Gauge_PressrawValue = message.Des_Exh_Man_Gauge_Press.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> VGT_Span_Perc_EOLrawValue = message.VGT_Span_Perc_EOL.GetRawValue(canData);
            ara::core::Optional<PM_Regen_Status> PM_Regen_StatusrawValue = message.PM_Regen_Status.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> PM_Probe_TemperaturerawValue = message.PM_Probe_Temperature.GetRawValue(canData);
            ara::core::Optional<PM_Measurement_Status> PM_Measurement_StatusrawValue = message.PM_Measurement_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EGR_Temp1_Pres_VoltsrawValue = message.EGR_Temp1_Pres_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CAVD_Cyl6rawValue = message.CAVD_Cyl6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CAVD_Cyl5rawValue = message.CAVD_Cyl5.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CAVD_Cyl4rawValue = message.CAVD_Cyl4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CAVD_Cyl3rawValue = message.CAVD_Cyl3.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CAVD_Cyl2rawValue = message.CAVD_Cyl2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CAVD_Cyl1rawValue = message.CAVD_Cyl1.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DEF_Pressure_Sensor_VoltagerawValue = message.DEF_Pressure_Sensor_Voltage.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Exh_Man_T_Raw_VrawValue = message.Exh_Man_T_Raw_V.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TCOT_Raw_VrawValue = message.TCOT_Raw_V.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Intake_Air_Temp_VoltsrawValue = message.Intake_Air_Temp_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Oil_Temp_VoltsrawValue = message.Oil_Temp_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Intake_Manifold_Temp_VoltsrawValue = message.Intake_Manifold_Temp_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Coolant_Temperature_VoltsrawValue = message.Coolant_Temperature_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Boost_Air_Temp_VoltsrawValue = message.Boost_Air_Temp_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DPFOP_Sensor_VoltagerawValue = message.DPFOP_Sensor_Voltage.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DPFDP_Sensor_VoltagerawValue = message.DPFDP_Sensor_Voltage.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Exh_Man_P_Raw_VrawValue = message.Exh_Man_P_Raw_V.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Supply_Fuel_Pres_VoltsrawValue = message.Supply_Fuel_Pres_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Oil_Pres_VoltsrawValue = message.Oil_Pres_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Fuel_Pressure_VoltsrawValue = message.Fuel_Pressure_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Boost_Pressure_VoltsrawValue = message.Boost_Pressure_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Atmospheric_Pressure_VoltsrawValue = message.Atmospheric_Pressure_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Exh_Man_TemprawValue = message.Exh_Man_Temp.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_Fan_On_TimerawValue = message.Trip_Fan_On_Time.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> PM_Sensor_CurrentrawValue = message.PM_Sensor_Current.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Accel_Pedal_1_Chan_2rawValue = message.Accel_Pedal_1_Chan_2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Intake_Corrected_NOxrawValue = message.Intake_Corrected_NOx.GetRawValue(canData);
            ara::core::Optional<Cruise_Control_Resume_Command> Cruise_Control_Resume_CommandrawValue = message.Cruise_Control_Resume_Command.GetRawValue(canData);
            ara::core::Optional<Cruise_Control_Pause_Command> Cruise_Control_Pause_CommandrawValue = message.Cruise_Control_Pause_Command.GetRawValue(canData);
            ara::core::Optional<Cruise_Control_Disable_Command> Cruise_Control_Disable_CommandrawValue = message.Cruise_Control_Disable_Command.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Heater_ResistancerawValue = message.Heater_Resistance.GetRawValue(canData);
            ara::core::Optional<Htr_OFF_Pwr_Out_Rng_Hi_Di_Status> Htr_OFF_Pwr_Out_Rng_Hi_Di_StatusrawValue = message.Htr_OFF_Pwr_Out_Rng_Hi_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_ON_Pwr_Out_Rng_Lo_Di_Status> Htr_ON_Pwr_Out_Rng_Lo_Di_StatusrawValue = message.Htr_ON_Pwr_Out_Rng_Lo_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_ON_Pwr_Out_Rng_Hi_Di_Status> Htr_ON_Pwr_Out_Rng_Hi_Di_StatusrawValue = message.Htr_ON_Pwr_Out_Rng_Hi_Di_Status.GetRawValue(canData);
            ara::core::Optional<CAN_MSG_Time_Out_Di_Status> CAN_MSG_Time_Out_Di_StatusrawValue = message.CAN_MSG_Time_Out_Di_Status.GetRawValue(canData);
            ara::core::Optional<Boost_Volt_Out_Of_Rng_Di_Status> Boost_Volt_Out_Of_Rng_Di_StatusrawValue = message.Boost_Volt_Out_Of_Rng_Di_Status.GetRawValue(canData);
            ara::core::Optional<Boost_Vol_Ip_Ovr_Volt_Di_Status> Boost_Vol_Ip_Ovr_Volt_Di_StatusrawValue = message.Boost_Vol_Ip_Ovr_Volt_Di_Status.GetRawValue(canData);
            ara::core::Optional<Boost_Volt_Ip_Shrt_GND_Di_Status> Boost_Volt_Ip_Shrt_GND_Di_StatusrawValue = message.Boost_Volt_Ip_Shrt_GND_Di_Status.GetRawValue(canData);
            ara::core::Optional<Boost_Volt_Ip_Shrt_Bat_Di_Status> Boost_Volt_Ip_Shrt_Bat_Di_StatusrawValue = message.Boost_Volt_Ip_Shrt_Bat_Di_Status.GetRawValue(canData);
            ara::core::Optional<Hi_Sensor_Ip_Short_GND_Di_Status> Hi_Sensor_Ip_Short_GND_Di_StatusrawValue = message.Hi_Sensor_Ip_Short_GND_Di_Status.GetRawValue(canData);
            ara::core::Optional<Hi_Sensor_Ip_Short_Bat_Di_Status> Hi_Sensor_Ip_Short_Bat_Di_StatusrawValue = message.Hi_Sensor_Ip_Short_Bat_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Lo_Drive_Short_GND_Di_Status> Htr_Lo_Drive_Short_GND_Di_StatusrawValue = message.Htr_Lo_Drive_Short_GND_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Lo_Drive_Over_Crnt_Di_Status> Htr_Lo_Drive_Over_Crnt_Di_StatusrawValue = message.Htr_Lo_Drive_Over_Crnt_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Lo_Drive_Opn_Cir_Di_Status> Htr_Lo_Drive_Opn_Cir_Di_StatusrawValue = message.Htr_Lo_Drive_Opn_Cir_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Lo_Drive_Short_Bat_Di_Status> Htr_Lo_Drive_Short_Bat_Di_StatusrawValue = message.Htr_Lo_Drive_Short_Bat_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Hi_Drive_Short_Bat_Di_Status> Htr_Hi_Drive_Short_Bat_Di_StatusrawValue = message.Htr_Hi_Drive_Short_Bat_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Hi_Drive_Over_Crnt_Di_Status> Htr_Hi_Drive_Over_Crnt_Di_StatusrawValue = message.Htr_Hi_Drive_Over_Crnt_Di_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Hi_Drive_Opn_Diag_Status> Htr_Hi_Drive_Opn_Diag_StatusrawValue = message.Htr_Hi_Drive_Opn_Diag_Status.GetRawValue(canData);
            ara::core::Optional<Htr_Hi_Drive_Short_GND_Di_Status> Htr_Hi_Drive_Short_GND_Di_StatusrawValue = message.Htr_Hi_Drive_Short_GND_Di_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Part_Sen_Max_ResistancerawValue = message.Part_Sen_Max_Resistance.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Part_Sensor_TemperaturerawValue = message.Part_Sensor_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Part_Sen_Regen_Failed_CountrawValue = message.Part_Sen_Regen_Failed_Count.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Part_Sensor_Power_SupplyrawValue = message.Part_Sensor_Power_Supply.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Part_Sensor_Detection_StatusrawValue = message.Part_Sensor_Detection_Status.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Part_Sen_Time_Between_RegensrawValue = message.Part_Sen_Time_Between_Regens.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Part_Sen_Active_Accum_TimerawValue = message.Part_Sen_Active_Accum_Time.GetRawValue(canData);
            ara::core::Optional<Particulate_Sensor_Regen_State> Particulate_Sensor_Regen_StaterawValue = message.Particulate_Sensor_Regen_State.GetRawValue(canData);
            ara::core::Optional<Particulate_Sensor_Regen_Status> Particulate_Sensor_Regen_StatusrawValue = message.Particulate_Sensor_Regen_Status.GetRawValue(canData);
            ara::core::Optional<Particulate_Sensor_Heater_State> Particulate_Sensor_Heater_StaterawValue = message.Particulate_Sensor_Heater_State.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Particulate_Sensor_ResistancerawValue = message.Particulate_Sensor_Resistance.GetRawValue(canData);
            ara::core::Optional<Fire_App_Pump_Engagement> Fire_App_Pump_EngagementrawValue = message.Fire_App_Pump_Engagement.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Vehicle_Speed_FrawValue = message.Vehicle_Speed_F.GetRawValue(canData);
            ara::core::Optional<Cruise_Control_Resume_Switch_G> Cruise_Control_Resume_Switch_GrawValue = message.Cruise_Control_Resume_Switch_G.GetRawValue(canData);
            ara::core::Optional<Cruise_Control_Pause_Switch> Cruise_Control_Pause_SwitchrawValue = message.Cruise_Control_Pause_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Req_ACC_Distance_ModerawValue = message.Req_ACC_Distance_Mode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SeatBeltSwitch_21rawValue = message.SeatBeltSwitch_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Veh_Limit_Speed_Gov_EnablerawValue = message.Veh_Limit_Speed_Gov_Enable.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Auto_Start_Enable_SwitchrawValue = message.Eng_Auto_Start_Enable_Switch.GetRawValue(canData);
            ara::core::Optional<DPF_Regen_Force_Switch_C> DPF_Regen_Force_Switch_CrawValue = message.DPF_Regen_Force_Switch_C.GetRawValue(canData);
            ara::core::Optional<DPF_Regen_Inhibit_Switch_C> DPF_Regen_Inhibit_Switch_CrawValue = message.DPF_Regen_Inhibit_Switch_C.GetRawValue(canData);
            ara::core::Optional<Veh_Lim_Spd_Gov_Inc_Switch_A> Veh_Lim_Spd_Gov_Inc_Switch_ArawValue = message.Veh_Lim_Spd_Gov_Inc_Switch_A.GetRawValue(canData);
            ara::core::Optional<Veh_Lim_Spd_Gov_Dec_Switch> Veh_Lim_Spd_Gov_Dec_SwitchrawValue = message.Veh_Lim_Spd_Gov_Dec_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Req_Percent_Fan_Speed_ArawValue = message.Req_Percent_Fan_Speed_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Remote_Accel_Pedal_Position_DrawValue = message.Remote_Accel_Pedal_Position_D.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EGRV_Position_ErrrawValue = message.EGRV_Position_Err.GetRawValue(canData);
            ara::core::Optional<Fuel_Type_A> Fuel_Type_ArawValue = message.Fuel_Type_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TC2_Compressor_Out_TemperaturerawValue = message.TC2_Compressor_Out_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TC1_Compressor_Out_TemperaturerawValue = message.TC1_Compressor_Out_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Engine_State_ArawValue = message.Engine_State_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Exhaust_Back_Press_CTLrawValue = message.Exhaust_Back_Press_CTL.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TC1_Turbine_Outlet_PressurerawValue = message.TC1_Turbine_Outlet_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TC1_Turbine_Outlet_Press_DesiredrawValue = message.TC1_Turbine_Outlet_Press_Desired.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Actual_Engine_TorquerawValue = message.Actual_Engine_Torque.GetRawValue(canData);
            ara::core::Optional<Adaptive_Cruise_Control_Mode> Adaptive_Cruise_Control_ModerawValue = message.Adaptive_Cruise_Control_Mode.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AFT_O2_Inlet_Percent_DesiredrawValue = message.AFT_O2_Inlet_Percent_Desired.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Fuel_TemperaturerawValue = message.Fuel_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Fuel_Pressure_2rawValue = message.Fuel_Pressure_2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Cylinder_6_Ignition_TimingrawValue = message.Cylinder_6_Ignition_Timing.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Cylinder_5_Ignition_TimingrawValue = message.Cylinder_5_Ignition_Timing.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Cylinder_4_Ignition_TimingrawValue = message.Cylinder_4_Ignition_Timing.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Cylinder_3_Ignition_TimingrawValue = message.Cylinder_3_Ignition_Timing.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Cylinder_2_Ignition_TimingrawValue = message.Cylinder_2_Ignition_Timing.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Cylinder_1_Ignition_TimingrawValue = message.Cylinder_1_Ignition_Timing.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Intake_Manifold_PressurerawValue = message.Eng_Intake_Manifold_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> enginecoolantpumpoutlettemp_0rawValue = message.enginecoolantpumpoutlettemp_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Coolant_Temperature_2rawValue = message.Engine_Coolant_Temperature_2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DEF_Pump_Motor_SpeedrawValue = message.DEF_Pump_Motor_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Desired_Fan_SpeedrawValue = message.Desired_Fan_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Desired_Boost_PressurerawValue = message.Desired_Boost_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Main_Fuel_Injection_Pulse_WidthrawValue = message.Main_Fuel_Injection_Pulse_Width.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Lambda_Measured_ValuerawValue = message.Lambda_Measured_Value.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Lambda_Temperature_ValuerawValue = message.Lambda_Temperature_Value.GetRawValue(canData);
            ara::core::Optional<SCR_Warm_Up_Status> SCR_Warm_Up_StatusrawValue = message.SCR_Warm_Up_Status.GetRawValue(canData);
            ara::core::Optional<Retarder_Inh_EBS_Switch2_Status> Retarder_Inh_EBS_Switch2_StatusrawValue = message.Retarder_Inh_EBS_Switch2_Status.GetRawValue(canData);
            ara::core::Optional<Transfer_Case_Status_B> Transfer_Case_Status_BrawValue = message.Transfer_Case_Status_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Accel_Ped_Pos_2_VoltsrawValue = message.Accel_Ped_Pos_2_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Intake_Air_Temp_Volts_ArawValue = message.Intake_Air_Temp_Volts_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> RPSAD_CountsrawValue = message.RPSAD_Counts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> APSAD_CountsrawValue = message.APSAD_Counts.GetRawValue(canData);
            ara::core::Optional<Retarder_Inh_EBS_Switch_Status> Retarder_Inh_EBS_Switch_StatusrawValue = message.Retarder_Inh_EBS_Switch_Status.GetRawValue(canData);
            ara::core::Optional<Change_Oil_Lamp_Status> Change_Oil_Lamp_StatusrawValue = message.Change_Oil_Lamp_Status.GetRawValue(canData);
            ara::core::Optional<Electronic_Fan_Control_Status> Electronic_Fan_Control_StatusrawValue = message.Electronic_Fan_Control_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Fuel_Temp_Volts_ArawValue = message.Fuel_Temp_Volts_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Fuel_Press_2_Volts_ArawValue = message.Fuel_Press_2_Volts_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DPF_Outlet_Temp_VoltsrawValue = message.DPF_Outlet_Temp_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DPF_Delta_Pressure_VoltsrawValue = message.DPF_Delta_Pressure_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Doser_Fuel_Pressure_VoltsrawValue = message.Doser_Fuel_Pressure_Volts.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DOC_Outlet_Temp_Volts_ArawValue = message.DOC_Outlet_Temp_Volts_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DOC_Inlet_Temp_Volts_ArawValue = message.DOC_Inlet_Temp_Volts_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Fuel_Pressure_Control_CmdrawValue = message.Fuel_Pressure_Control_Cmd.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Eng_Throttle_Act_1_Control_Cmd_ArawValue = message.Eng_Throttle_Act_1_Control_Cmd_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Fuel_Volume_Control_CmdrawValue = message.Fuel_Volume_Control_Cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TC2_Wastegate_Control_ArawValue = message.TC2_Wastegate_Control_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TC1_Wastegate_ControlrawValue = message.TC1_Wastegate_Control.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engintakevalveactuationsystemrawValue = message.engintakevalveactuationsystem.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engcoolanttemphighresolutionrawValue = message.engcoolanttemphighresolution.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> CAC_Outlet_Temperature_ArawValue = message.CAC_Outlet_Temperature_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Eng_Intake_Mainfold_TemperaturerawValue = message.Eng_Intake_Mainfold_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> ReservedforSAEassignment_DM29rawValue = message.ReservedforSAEassignment_DM29.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PermanentDTCs_DM29rawValue = message.PermanentDTCs_DM29.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PreviouslyMILOnDTCs_DM29rawValue = message.PreviouslyMILOnDTCs_DM29.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MILOnDTCs_DM29rawValue = message.MILOnDTCs_DM29.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AllPendingDTCs_DM29rawValue = message.AllPendingDTCs_DM29.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PendingDTCs_DM29rawValue = message.PendingDTCs_DM29.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN10_DM28rawValue = message.CM_SPN10_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN10_DM28rawValue = message.OC_SPN10_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN10High_DM28rawValue = message.SPN10High_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN10_DM28> FMI_SPN10_DM28rawValue = message.FMI_SPN10_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN10_DM28rawValue = message.SPN10_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN9_DM28rawValue = message.CM_SPN9_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN9_DM28rawValue = message.OC_SPN9_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN9High_DM28rawValue = message.SPN9High_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN9_DM28> FMI_SPN9_DM28rawValue = message.FMI_SPN9_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN9_DM28rawValue = message.SPN9_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN8_DM28rawValue = message.CM_SPN8_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN8_DM28rawValue = message.OC_SPN8_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN8High_DM28rawValue = message.SPN8High_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN8_DM28> FMI_SPN8_DM28rawValue = message.FMI_SPN8_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN8_DM28rawValue = message.SPN8_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN7_DM28rawValue = message.CM_SPN7_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN7_DM28rawValue = message.OC_SPN7_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN7High_DM28rawValue = message.SPN7High_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN7_DM28> FMI_SPN7_DM28rawValue = message.FMI_SPN7_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN7_DM28rawValue = message.SPN7_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN6_DM28rawValue = message.CM_SPN6_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN6_DM28rawValue = message.OC_SPN6_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN6High_DM28rawValue = message.SPN6High_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN6_DM28> FMI_SPN6_DM28rawValue = message.FMI_SPN6_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN6_DM28rawValue = message.SPN6_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN5_DM28rawValue = message.OC_SPN5_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN4_DM28rawValue = message.OC_SPN4_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN3_DM28rawValue = message.OC_SPN3_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN2_DM28rawValue = message.OC_SPN2_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN5_DM28rawValue = message.CM_SPN5_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN4_DM28rawValue = message.CM_SPN4_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN3_DM28rawValue = message.CM_SPN3_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN2_DM28rawValue = message.CM_SPN2_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN5High_DM28rawValue = message.SPN5High_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN5_DM28rawValue = message.SPN5_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN4High_DM28rawValue = message.SPN4High_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN4_DM28rawValue = message.SPN4_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN3High_DM28rawValue = message.SPN3High_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN3_DM28rawValue = message.SPN3_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN2High_DM28rawValue = message.SPN2High_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN2_DM28rawValue = message.SPN2_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM28rawValue = message.SPN1High_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM28rawValue = message.SPN1_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN5_DM28> FMI_SPN5_DM28rawValue = message.FMI_SPN5_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN4_DM28> FMI_SPN4_DM28rawValue = message.FMI_SPN4_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN3_DM28> FMI_SPN3_DM28rawValue = message.FMI_SPN3_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN2_DM28> FMI_SPN2_DM28rawValue = message.FMI_SPN2_DM28.GetRawValue(canData);
            ara::core::Optional<FMI_SPN1_DM28> FMI_SPN1_DM28rawValue = message.FMI_SPN1_DM28.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp_DM28> FlashRedStopLamp_DM28rawValue = message.FlashRedStopLamp_DM28.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp_DM28> FlashProtectLamp_DM28rawValue = message.FlashProtectLamp_DM28.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp_DM28> FlashMalfuncIndicatorLamp_DM28rawValue = message.FlashMalfuncIndicatorLamp_DM28.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp_DM28> FlashAmberWarningLamp_DM28rawValue = message.FlashAmberWarningLamp_DM28.GetRawValue(canData);
            ara::core::Optional<RedStopLampStatus_DM28> RedStopLampStatus_DM28rawValue = message.RedStopLampStatus_DM28.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatusDM28> ProtectLampStatusDM28rawValue = message.ProtectLampStatusDM28.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus_DM28> AmberWarningLampStatus_DM28rawValue = message.AmberWarningLampStatus_DM28.GetRawValue(canData);
            ara::core::Optional<MalfuncIndicatorLampStatus_DM28> MalfuncIndicatorLampStatus_DM28rawValue = message.MalfuncIndicatorLampStatus_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN1_DM28rawValue = message.OC_SPN1_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN1_DM28rawValue = message.CM_SPN1_DM28.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> SPNData_DM25rawValue = message.SPNData_DM25.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ExpandedFreezeFrameLength_DM25rawValue = message.ExpandedFreezeFrameLength_DM25.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN1_DM25rawValue = message.CM_SPN1_DM25.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN1_DM25rawValue = message.OC_SPN1_DM25.GetRawValue(canData);
            ara::core::Optional<FMI_SPN1_DM25> FMI_SPN1_DM25rawValue = message.FMI_SPN1_DM25.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM25rawValue = message.SPN1_DM25.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM25rawValue = message.SPN1High_DM25.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppScaledTestResults_SPN5_DM24rawValue = message.SuppScaledTestResults_SPN5_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppScaledTestResults_SPN4_DM24rawValue = message.SuppScaledTestResults_SPN4_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppScaledTestResults_SPN3_DM24rawValue = message.SuppScaledTestResults_SPN3_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppScaledTestResults_SPN2_DM24rawValue = message.SuppScaledTestResults_SPN2_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppScaledTestResults_SPN1_DM24rawValue = message.SuppScaledTestResults_SPN1_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppDataStream_SPN5_DM24rawValue = message.SuppDataStream_SPN5_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppDataStream_SPN4_DM24rawValue = message.SuppDataStream_SPN4_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppDataStream_SPN3_DM24rawValue = message.SuppDataStream_SPN3_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppDataStream_SPN2_DM24rawValue = message.SuppDataStream_SPN2_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppDataStream_SPN1_DM24rawValue = message.SuppDataStream_SPN1_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppExpFreezeFrame_SPN5_DM24rawValue = message.SuppExpFreezeFrame_SPN5_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppExpFreezeFrame_SPN4_DM24rawValue = message.SuppExpFreezeFrame_SPN4_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppExpFreezeFrame_SPN3_DM24rawValue = message.SuppExpFreezeFrame_SPN3_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppExpFreezeFrame_SPN2_DM24rawValue = message.SuppExpFreezeFrame_SPN2_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SuppExpFreezeFrame_SPN1_DM24rawValue = message.SuppExpFreezeFrame_SPN1_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN5DataLength_DM24rawValue = message.SPN5DataLength_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN4DataLength_DM24rawValue = message.SPN4DataLength_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN3DataLength_DM24rawValue = message.SPN3DataLength_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN2DataLength_DM24rawValue = message.SPN2DataLength_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1DataLength_DM24rawValue = message.SPN1DataLength_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN5_DM24rawValue = message.SPN5_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN4_DM24rawValue = message.SPN4_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN3_DM24rawValue = message.SPN3_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN2_DM24rawValue = message.SPN2_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM24rawValue = message.SPN1_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN5High_DM24rawValue = message.SPN5High_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN4High_DM24rawValue = message.SPN4High_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN3High_DM24rawValue = message.SPN3High_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN2High_DM24rawValue = message.SPN2High_DM24.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM24rawValue = message.SPN1High_DM24.GetRawValue(canData);
            ara::core::Optional<NOx_Quality_Indicator> NOx_Quality_IndicatorrawValue = message.NOx_Quality_Indicator.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Turbine_Out_Exhaust_O2rawValue = message.Turbine_Out_Exhaust_O2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Turbine_Out_Exhaust_NOxrawValue = message.Turbine_Out_Exhaust_NOx.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Turbine_Out_Exhaust_NO2rawValue = message.Turbine_Out_Exhaust_NO2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Turbine_Out_Exhaust_PMrawValue = message.Turbine_Out_Exhaust_PM.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Operator_Mode_PermitrawValue = message.Operator_Mode_Permit.GetRawValue(canData);
            ara::core::Optional<Engine_Operating_Mode> Engine_Operating_ModerawValue = message.Engine_Operating_Mode.GetRawValue(canData);
            ara::core::Optional<Engine_Operating_State_A> Engine_Operating_State_ArawValue = message.Engine_Operating_State_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Mass_Flow_Rate_ProprawValue = message.Mass_Flow_Rate_Prop.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Drive_Cycle_Free_RunningrawValue = message.Drive_Cycle_Free_Running.GetRawValue(canData);
            ara::core::Optional<ACM_Test_Reset> ACM_Test_ResetrawValue = message.ACM_Test_Reset.GetRawValue(canData);
            ara::core::Optional<DPF_Reg_Release_Indicator> DPF_Reg_Release_IndicatorrawValue = message.DPF_Reg_Release_Indicator.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Requested_ILT_PosistionrawValue = message.Requested_ILT_Posistion.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Actual_ILT_PositionrawValue = message.Actual_ILT_Position.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Actual_EGR_PositionrawValue = message.Actual_EGR_Position.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Requested_EGR_PosistionrawValue = message.Requested_EGR_Posistion.GetRawValue(canData);
            ara::core::Optional<FMI_SPN5_DM32> FMI_SPN5_DM32rawValue = message.FMI_SPN5_DM32.GetRawValue(canData);
            ara::core::Optional<FMI_SPN4_DM32> FMI_SPN4_DM32rawValue = message.FMI_SPN4_DM32.GetRawValue(canData);
            ara::core::Optional<FMI_SPN3_DM32> FMI_SPN3_DM32rawValue = message.FMI_SPN3_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_PrevActive_Time_5_DM32rawValue = message.DTC_Total_PrevActive_Time_5_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_Active_Time_5_DM32rawValue = message.DTC_Total_Active_Time_5_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DTC_Time_Until_Derate_5_DM32rawValue = message.DTC_Time_Until_Derate_5_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_PrevActive_Time_4_DM32rawValue = message.DTC_Total_PrevActive_Time_4_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_Active_Time_4_DM32rawValue = message.DTC_Total_Active_Time_4_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DTC_Time_Until_Derate_4_DM32rawValue = message.DTC_Time_Until_Derate_4_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_PrevActive_Time_3_DM32rawValue = message.DTC_Total_PrevActive_Time_3_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_Active_Time_3_DM32rawValue = message.DTC_Total_Active_Time_3_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DTC_Time_Until_Derate_3_DM32rawValue = message.DTC_Time_Until_Derate_3_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_PrevActive_Time_2_DM32rawValue = message.DTC_Total_PrevActive_Time_2_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_Active_Time_2_DM32rawValue = message.DTC_Total_Active_Time_2_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DTC_Time_Until_Derate_2_DM32rawValue = message.DTC_Time_Until_Derate_2_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_PrevActive_Time_1_DM32rawValue = message.DTC_Total_PrevActive_Time_1_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DTC_Total_Active_Time_1_DM32rawValue = message.DTC_Total_Active_Time_1_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DTC_Time_Until_Derate_1_DM32rawValue = message.DTC_Time_Until_Derate_1_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN5_DM32rawValue = message.SPN5_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN4_DM32rawValue = message.SPN4_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN3_DM32rawValue = message.SPN3_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN2_DM32rawValue = message.SPN2_DM32.GetRawValue(canData);
            ara::core::Optional<FMI_SPN1_DM32> FMI_SPN1_DM32rawValue = message.FMI_SPN1_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM32rawValue = message.SPN1_DM32.GetRawValue(canData);
            ara::core::Optional<FMI_SPN2_DM32> FMI_SPN2_DM32rawValue = message.FMI_SPN2_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN5High_DM32rawValue = message.SPN5High_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN4High_DM32rawValue = message.SPN4High_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN3High_DM32rawValue = message.SPN3High_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN2High_DM32rawValue = message.SPN2High_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM32rawValue = message.SPN1High_DM32.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Actual_Ignition_TimingrawValue = message.Engine_Actual_Ignition_Timing.GetRawValue(canData);
            ara::core::Optional<progressiveshiftdisable_0> progressiveshiftdisable_0rawValue = message.progressiveshiftdisable_0.GetRawValue(canData);
            ara::core::Optional<engmomentaryoverspeedenable_0> engmomentaryoverspeedenable_0rawValue = message.engmomentaryoverspeedenable_0.GetRawValue(canData);
            ara::core::Optional<transshiftinprocess_0> transshiftinprocess_0rawValue = message.transshiftinprocess_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> transinputshaftspeed_0rawValue = message.transinputshaftspeed_0.GetRawValue(canData);
            ara::core::Optional<transdrivelineengaged_0> transdrivelineengaged_0rawValue = message.transdrivelineengaged_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Trans_Output_Shaft_SpeedrawValue = message.Trans_Output_Shaft_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Operating_Mode_StatusrawValue = message.Operating_Mode_Status.GetRawValue(canData);
            ara::core::Optional<Cold_Start_Relay_Diag> Cold_Start_Relay_DiagrawValue = message.Cold_Start_Relay_Diag.GetRawValue(canData);
            ara::core::Optional<Cold_Start_Relay> Cold_Start_RelayrawValue = message.Cold_Start_Relay.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Cold_Start_Fuel_IgniterrawValue = message.Cold_Start_Fuel_Igniter.GetRawValue(canData);
            ara::core::Optional<Engine_Crank_Inhibit_Flame_Start> Engine_Crank_Inhibit_Flame_StartrawValue = message.Engine_Crank_Inhibit_Flame_Start.GetRawValue(canData);
            ara::core::Optional<Cold_Start_Fuel_Solenoid> Cold_Start_Fuel_SolenoidrawValue = message.Cold_Start_Fuel_Solenoid.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> seat_belt_switchrawValue = message.seat_belt_switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Estimated_Percent_Fan_Speed_ArawValue = message.Estimated_Percent_Fan_Speed_A.GetRawValue(canData);
            ara::core::Optional<DPF_Regen_Inhibit_Switch> DPF_Regen_Inhibit_SwitchrawValue = message.DPF_Regen_Inhibit_Switch.GetRawValue(canData);
            ara::core::Optional<DPF_Regen_Force_Switch> DPF_Regen_Force_SwitchrawValue = message.DPF_Regen_Force_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TestLimitMinimum_DM8rawValue = message.TestLimitMinimum_DM8.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TestLimitMaximum_DM8rawValue = message.TestLimitMaximum_DM8.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TestValue_DM8rawValue = message.TestValue_DM8.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestType_DM8rawValue = message.TestType_DM8.GetRawValue(canData);
            ara::core::Optional<TestIdentifier_DM8_E> TestIdentifier_DM8_ErawValue = message.TestIdentifier_DM8_E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN10_DM6rawValue = message.CM_SPN10_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN10_DM6rawValue = message.OC_SPN10_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN10High_DM6rawValue = message.SPN10High_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN10_DM6> FMI_SPN10_DM6rawValue = message.FMI_SPN10_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN10_DM6rawValue = message.SPN10_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN9_DM6rawValue = message.CM_SPN9_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN9_DM6rawValue = message.OC_SPN9_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN9High_DM6rawValue = message.SPN9High_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN9_DM6> FMI_SPN9_DM6rawValue = message.FMI_SPN9_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN9_DM6rawValue = message.SPN9_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN8_DM6rawValue = message.CM_SPN8_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN8_DM6rawValue = message.OC_SPN8_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN8High_DM6rawValue = message.SPN8High_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN8_DM6> FMI_SPN8_DM6rawValue = message.FMI_SPN8_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN8_DM6rawValue = message.SPN8_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN7_DM6rawValue = message.CM_SPN7_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN7_DM6rawValue = message.OC_SPN7_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN7High_DM6rawValue = message.SPN7High_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN7_DM6> FMI_SPN7_DM6rawValue = message.FMI_SPN7_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN7_DM6rawValue = message.SPN7_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN6_DM6rawValue = message.CM_SPN6_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN6_DM6rawValue = message.OC_SPN6_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN6High_DM6rawValue = message.SPN6High_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN6_DM6> FMI_SPN6_DM6rawValue = message.FMI_SPN6_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN6_DM6rawValue = message.SPN6_DM6.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp_DM6> FlashMalfuncIndicatorLamp_DM6rawValue = message.FlashMalfuncIndicatorLamp_DM6.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp_DM6> FlashRedStopLamp_DM6rawValue = message.FlashRedStopLamp_DM6.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp_DM6> FlashAmberWarningLamp_DM6rawValue = message.FlashAmberWarningLamp_DM6.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp_DM6> FlashProtectLamp_DM6rawValue = message.FlashProtectLamp_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN5_DM6rawValue = message.SPN5_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN4_DM6rawValue = message.SPN4_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN3_DM6rawValue = message.SPN3_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN2_DM6rawValue = message.SPN2_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM6rawValue = message.SPN1_DM6.GetRawValue(canData);
            ara::core::Optional<RedStopLampStatus_DM6> RedStopLampStatus_DM6rawValue = message.RedStopLampStatus_DM6.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatusDM6> ProtectLampStatusDM6rawValue = message.ProtectLampStatusDM6.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus_DM6> AmberWarningLampStatus_DM6rawValue = message.AmberWarningLampStatus_DM6.GetRawValue(canData);
            ara::core::Optional<MalfunIndicatorLampStatus_DM6> MalfunIndicatorLampStatus_DM6rawValue = message.MalfunIndicatorLampStatus_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN5_DM6> FMI_SPN5_DM6rawValue = message.FMI_SPN5_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN4_DM6> FMI_SPN4_DM6rawValue = message.FMI_SPN4_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN3_DM6> FMI_SPN3_DM6rawValue = message.FMI_SPN3_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN2_DM6> FMI_SPN2_DM6rawValue = message.FMI_SPN2_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN5_DM6rawValue = message.CM_SPN5_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN4_DM6rawValue = message.CM_SPN4_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN3_DM6rawValue = message.CM_SPN3_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN2_DM6rawValue = message.CM_SPN2_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN5_DM6rawValue = message.OC_SPN5_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN4_DM6rawValue = message.OC_SPN4_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN3_DM6rawValue = message.OC_SPN3_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN2_DM6rawValue = message.OC_SPN2_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN1_DM6rawValue = message.OC_SPN1_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN1_DM6rawValue = message.CM_SPN1_DM6.GetRawValue(canData);
            ara::core::Optional<FMI_SPN1_DM6> FMI_SPN1_DM6rawValue = message.FMI_SPN1_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN5High_DM6rawValue = message.SPN5High_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN4High_DM6rawValue = message.SPN4High_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN3High_DM6rawValue = message.SPN3High_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN2High_DM6rawValue = message.SPN2High_DM6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM6rawValue = message.SPN1High_DM6.GetRawValue(canData);
            ara::core::Optional<PMFilterMonStatus_DM5> PMFilterMonStatus_DM5rawValue = message.PMFilterMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<NOxCnvrtctlystMonStatus_DM5> NOxCnvrtctlystMonStatus_DM5rawValue = message.NOxCnvrtctlystMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<NHMCCnvrtctlystMonStatus_DM5> NHMCCnvrtctlystMonStatus_DM5rawValue = message.NHMCCnvrtctlystMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<BstPressCtrlSystemMonStatus_DM5> BstPressCtrlSystemMonStatus_DM5rawValue = message.BstPressCtrlSystemMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<NOxCnvrtctlystMonSupp_DM5> NOxCnvrtctlystMonSupp_DM5rawValue = message.NOxCnvrtctlystMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<PMFilterMonSupp_DM5> PMFilterMonSupp_DM5rawValue = message.PMFilterMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<NHMCCnvrtctlystMonSupp_DM5> NHMCCnvrtctlystMonSupp_DM5rawValue = message.NHMCCnvrtctlystMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<BstPressCtrlSystemMonSupp_DM5> BstPressCtrlSystemMonSupp_DM5rawValue = message.BstPressCtrlSystemMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<MisfireMonitoringSupport_DM5> MisfireMonitoringSupport_DM5rawValue = message.MisfireMonitoringSupport_DM5.GetRawValue(canData);
            ara::core::Optional<OBDCompliance_DM5> OBDCompliance_DM5rawValue = message.OBDCompliance_DM5.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PreviouslyActiveTroubleCodes_DM5rawValue = message.PreviouslyActiveTroubleCodes_DM5.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ActiveTroubleCodes_DM5rawValue = message.ActiveTroubleCodes_DM5.GetRawValue(canData);
            ara::core::Optional<SecondAirSystemMonStatus_DM5> SecondAirSystemMonStatus_DM5rawValue = message.SecondAirSystemMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<EvaporativeSystemMonStatus_DM5> EvaporativeSystemMonStatus_DM5rawValue = message.EvaporativeSystemMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<ACSystemRefrigerantMonStatus_DM5> ACSystemRefrigerantMonStatus_DM5rawValue = message.ACSystemRefrigerantMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<OxygenSensorHeaterMonStatus_DM5> OxygenSensorHeaterMonStatus_DM5rawValue = message.OxygenSensorHeaterMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<OxygenSensorMonStatus_DM5> OxygenSensorMonStatus_DM5rawValue = message.OxygenSensorMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<EGRSystemMonitoringStatus_DM5> EGRSystemMonitoringStatus_DM5rawValue = message.EGRSystemMonitoringStatus_DM5.GetRawValue(canData);
            ara::core::Optional<CatalystMonStatus_DM5> CatalystMonStatus_DM5rawValue = message.CatalystMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<ColdStartAidSystemMonStatus_DM5> ColdStartAidSystemMonStatus_DM5rawValue = message.ColdStartAidSystemMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<HeatedCatalystMonStatus_DM5> HeatedCatalystMonStatus_DM5rawValue = message.HeatedCatalystMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<CatalystMonSupp_DM5> CatalystMonSupp_DM5rawValue = message.CatalystMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<ColdStartAidSystemMonSupp_DM5> ColdStartAidSystemMonSupp_DM5rawValue = message.ColdStartAidSystemMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<HeatedCatalystMonSupp_DM5> HeatedCatalystMonSupp_DM5rawValue = message.HeatedCatalystMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<EvaporativeSystemMonSupp_DM5> EvaporativeSystemMonSupp_DM5rawValue = message.EvaporativeSystemMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<SecondAirSystemMonSupp_DM5> SecondAirSystemMonSupp_DM5rawValue = message.SecondAirSystemMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<ACSystemRefrigerantMonSupp_DM5> ACSystemRefrigerantMonSupp_DM5rawValue = message.ACSystemRefrigerantMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<OxygenSensorMonSupp_DM5> OxygenSensorMonSupp_DM5rawValue = message.OxygenSensorMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<OxygenSensorHeaterMonSupp_DM5> OxygenSensorHeaterMonSupp_DM5rawValue = message.OxygenSensorHeaterMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<EGRSystemMonitoringSupp_DM5> EGRSystemMonitoringSupp_DM5rawValue = message.EGRSystemMonitoringSupp_DM5.GetRawValue(canData);
            ara::core::Optional<MisfireMonitoringStatus_DM5> MisfireMonitoringStatus_DM5rawValue = message.MisfireMonitoringStatus_DM5.GetRawValue(canData);
            ara::core::Optional<ComprehensiveCompMonStatus_DM5> ComprehensiveCompMonStatus_DM5rawValue = message.ComprehensiveCompMonStatus_DM5.GetRawValue(canData);
            ara::core::Optional<FuelSystemMonitoringStatus_DM5> FuelSystemMonitoringStatus_DM5rawValue = message.FuelSystemMonitoringStatus_DM5.GetRawValue(canData);
            ara::core::Optional<CompreComponentMonSupp_DM5> CompreComponentMonSupp_DM5rawValue = message.CompreComponentMonSupp_DM5.GetRawValue(canData);
            ara::core::Optional<FuelSystemMonitoringSupport_DM5> FuelSystemMonitoringSupport_DM5rawValue = message.FuelSystemMonitoringSupport_DM5.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM4rawValue = message.SPN1_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Speed_DM4rawValue = message.Engine_Speed_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Intake_Manifold_Pressure_DM4rawValue = message.Intake_Manifold_Pressure_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Load_DM4rawValue = message.Engine_Load_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Vehicle_Speed_DM4rawValue = message.Vehicle_Speed_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN1_DM4rawValue = message.OC_SPN1_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN1_DM4rawValue = message.CM_SPN1_DM4.GetRawValue(canData);
            ara::core::Optional<FMI_SPN1_DM4> FMI_SPN1_DM4rawValue = message.FMI_SPN1_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> FreezeFrameLength_DM4rawValue = message.FreezeFrameLength_DM4.GetRawValue(canData);
            ara::core::Optional<Engine_Torque_Mode_DM4> Engine_Torque_Mode_DM4rawValue = message.Engine_Torque_Mode_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Coolant_Temperature_DM4rawValue = message.Engine_Coolant_Temperature_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM4rawValue = message.SPN1High_DM4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN10_DM23rawValue = message.CM_SPN10_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN10_DM23rawValue = message.OC_SPN10_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN10High_DM23rawValue = message.SPN10High_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN10_DM23> FMI_SPN10_DM23rawValue = message.FMI_SPN10_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN10_DM23rawValue = message.SPN10_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN9_DM23rawValue = message.CM_SPN9_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN9_DM23rawValue = message.OC_SPN9_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN9High_DM23rawValue = message.SPN9High_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN9_DM23> FMI_SPN9_DM23rawValue = message.FMI_SPN9_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN9_DM23rawValue = message.SPN9_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN8_DM23rawValue = message.CM_SPN8_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN8_DM23rawValue = message.OC_SPN8_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN8High_DM23rawValue = message.SPN8High_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN8_DM23> FMI_SPN8_DM23rawValue = message.FMI_SPN8_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN8_DM23rawValue = message.SPN8_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN7_DM23rawValue = message.CM_SPN7_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN7_DM23rawValue = message.OC_SPN7_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN7High_DM23rawValue = message.SPN7High_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN7_DM23> FMI_SPN7_DM23rawValue = message.FMI_SPN7_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN7_DM23rawValue = message.SPN7_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN6_DM23rawValue = message.CM_SPN6_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN6_DM23rawValue = message.OC_SPN6_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN6High_DM23rawValue = message.SPN6High_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN6_DM23> FMI_SPN6_DM23rawValue = message.FMI_SPN6_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN6_DM23rawValue = message.SPN6_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN5_DM23> FMI_SPN5_DM23rawValue = message.FMI_SPN5_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN4_DM23> FMI_SPN4_DM23rawValue = message.FMI_SPN4_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN3_DM23> FMI_SPN3_DM23rawValue = message.FMI_SPN3_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN2_DM23> FMI_SPN2_DM23rawValue = message.FMI_SPN2_DM23.GetRawValue(canData);
            ara::core::Optional<FMI_SPN1_DM23> FMI_SPN1_DM23rawValue = message.FMI_SPN1_DM23.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp_DM23> FlashRedStopLamp_DM23rawValue = message.FlashRedStopLamp_DM23.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp_DM23> FlashProtectLamp_DM23rawValue = message.FlashProtectLamp_DM23.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp_DM23> FlashMalfuncIndicatorLamp_DM23rawValue = message.FlashMalfuncIndicatorLamp_DM23.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp_DM23> FlashAmberWarningLamp_DM23rawValue = message.FlashAmberWarningLamp_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN5_DM23rawValue = message.SPN5_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN4_DM23rawValue = message.SPN4_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN3_DM23rawValue = message.SPN3_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN2_DM23rawValue = message.SPN2_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM23rawValue = message.SPN1_DM23.GetRawValue(canData);
            ara::core::Optional<RedStopLampStatus_DM23> RedStopLampStatus_DM23rawValue = message.RedStopLampStatus_DM23.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatusDM23> ProtectLampStatusDM23rawValue = message.ProtectLampStatusDM23.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus_DM23> AmberWarningLampStatus_DM23rawValue = message.AmberWarningLampStatus_DM23.GetRawValue(canData);
            ara::core::Optional<MalfunIndicatorLampStatus_DM23> MalfunIndicatorLampStatus_DM23rawValue = message.MalfunIndicatorLampStatus_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN1_DM23rawValue = message.OC_SPN1_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN1_DM23rawValue = message.CM_SPN1_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN5_DM23rawValue = message.CM_SPN5_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN4_DM23rawValue = message.CM_SPN4_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN3_DM23rawValue = message.CM_SPN3_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN2_DM23rawValue = message.CM_SPN2_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN5_DM23rawValue = message.OC_SPN5_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN4_DM23rawValue = message.OC_SPN4_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN3_DM23rawValue = message.OC_SPN3_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN2_DM23rawValue = message.OC_SPN2_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN5High_DM23rawValue = message.SPN5High_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN4High_DM23rawValue = message.SPN4High_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN3High_DM23rawValue = message.SPN3High_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN2High_DM23rawValue = message.SPN2High_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM23rawValue = message.SPN1High_DM23.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TimeSinceDTCsCleared_DM21rawValue = message.TimeSinceDTCsCleared_DM21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> MinutesRunByEngMILActivated_DM21rawValue = message.MinutesRunByEngMILActivated_DM21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DistanceSinceDTCsCleared_DM21rawValue = message.DistanceSinceDTCsCleared_DM21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DistanceWhileMILisActivated_DM21rawValue = message.DistanceWhileMILisActivated_DM21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN10_DM12rawValue = message.CM_SPN10_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN10_DM12rawValue = message.OC_SPN10_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN10High_DM12rawValue = message.SPN10High_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN10_DM12> FMI_SPN10_DM12rawValue = message.FMI_SPN10_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN10_DM12rawValue = message.SPN10_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN9_DM12rawValue = message.CM_SPN9_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN9_DM12rawValue = message.OC_SPN9_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN9High_DM12rawValue = message.SPN9High_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN9_DM12> FMI_SPN9_DM12rawValue = message.FMI_SPN9_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN9_DM12rawValue = message.SPN9_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN8_DM12rawValue = message.CM_SPN8_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN8_DM12rawValue = message.OC_SPN8_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN8High_DM12rawValue = message.SPN8High_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN8_DM12> FMI_SPN8_DM12rawValue = message.FMI_SPN8_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN8_DM12rawValue = message.SPN8_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN7_DM12rawValue = message.CM_SPN7_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN7_DM12rawValue = message.OC_SPN7_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN7High_DM12rawValue = message.SPN7High_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN7_DM12> FMI_SPN7_DM12rawValue = message.FMI_SPN7_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN7_DM12rawValue = message.SPN7_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN6_DM12rawValue = message.CM_SPN6_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN6_DM12rawValue = message.OC_SPN6_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN6High_DM12rawValue = message.SPN6High_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN6_DM12> FMI_SPN6_DM12rawValue = message.FMI_SPN6_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN6_DM12rawValue = message.SPN6_DM12.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp_DM12> FlashRedStopLamp_DM12rawValue = message.FlashRedStopLamp_DM12.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp_DM12> FlashProtectLamp_DM12rawValue = message.FlashProtectLamp_DM12.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp_DM12> FlashMalfuncIndicatorLamp_DM12rawValue = message.FlashMalfuncIndicatorLamp_DM12.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp_DM12> FlashAmberWarningLamp_DM12rawValue = message.FlashAmberWarningLamp_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN5_DM12> FMI_SPN5_DM12rawValue = message.FMI_SPN5_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN4_DM12> FMI_SPN4_DM12rawValue = message.FMI_SPN4_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN3_DM12> FMI_SPN3_DM12rawValue = message.FMI_SPN3_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN2_DM12> FMI_SPN2_DM12rawValue = message.FMI_SPN2_DM12.GetRawValue(canData);
            ara::core::Optional<FMI_SPN1_DM12> FMI_SPN1_DM12rawValue = message.FMI_SPN1_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN5_DM12rawValue = message.SPN5_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN4_DM12rawValue = message.SPN4_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN3_DM12rawValue = message.SPN3_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN2_DM12rawValue = message.SPN2_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SPN1_DM12rawValue = message.SPN1_DM12.GetRawValue(canData);
            ara::core::Optional<RedStopLampStatus_DM12> RedStopLampStatus_DM12rawValue = message.RedStopLampStatus_DM12.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatusDM12> ProtectLampStatusDM12rawValue = message.ProtectLampStatusDM12.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus_DM12> AmberWarningLampStatus_DM12rawValue = message.AmberWarningLampStatus_DM12.GetRawValue(canData);
            ara::core::Optional<MalfunIndicatorLampStatus_DM12> MalfunIndicatorLampStatus_DM12rawValue = message.MalfunIndicatorLampStatus_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN1_DM12rawValue = message.OC_SPN1_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN1_DM12rawValue = message.CM_SPN1_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN5_DM12rawValue = message.CM_SPN5_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN4_DM12rawValue = message.CM_SPN4_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN3_DM12rawValue = message.CM_SPN3_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CM_SPN2_DM12rawValue = message.CM_SPN2_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN5_DM12rawValue = message.OC_SPN5_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN4_DM12rawValue = message.OC_SPN4_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN3_DM12rawValue = message.OC_SPN3_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OC_SPN2_DM12rawValue = message.OC_SPN2_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN5High_DM12rawValue = message.SPN5High_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN4High_DM12rawValue = message.SPN4High_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN3High_DM12rawValue = message.SPN3High_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN2High_DM12rawValue = message.SPN2High_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM12rawValue = message.SPN1High_DM12.GetRawValue(canData);
            ara::core::Optional<std::uint64_t> ManufAssignedTest16to64_DM10rawValue = message.ManufAssignedTest16to64_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ManufAssignedTest10to15_DM10rawValue = message.ManufAssignedTest10to15_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestNine_DM10rawValue = message.TestNine_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestOne_DM10rawValue = message.TestOne_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestTwo_DM10rawValue = message.TestTwo_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestThree_DM10rawValue = message.TestThree_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestFour_DM10rawValue = message.TestFour_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestFive_DM10rawValue = message.TestFive_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestSix_DM10rawValue = message.TestSix_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestSeven_DM10rawValue = message.TestSeven_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TestEight_DM10rawValue = message.TestEight_DM10.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Desi_Abs_IMP_Turbo_Boost_LimitrawValue = message.Desi_Abs_IMP_Turbo_Boost_Limit.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engfuel1injectorail1pressure_0rawValue = message.engfuel1injectorail1pressure_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engfuelinjectionctlpressure_0rawValue = message.engfuelinjectionctlpressure_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineFluidLevelPressure2rawValue = message.EngineFluidLevelPressure2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineCoolantPressure_1rawValue = message.EngineCoolantPressure_1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_exhaustflowraterawValue = message.engine_exhaustflowrate.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> ctrlrqrawValue = message.ctrlrq.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> htrswstatrawValue = message.htrswstat.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> spdrpmrq_1rawValue = message.spdrpmrq_1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> spdrpmrqrawValue = message.spdrpmrq.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> hvcurrph_eabcrawValue = message.hvcurrph_eabc.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> hvcurrph_ehpsrawValue = message.hvcurrph_ehps.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engturbocharger1intakepressrawValue = message.engturbocharger1intakepress.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> src_add_cont_dev_retd_control_0rawValue = message.src_add_cont_dev_retd_control_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> intendedretarderpercenttorque_0rawValue = message.intendedretarderpercenttorque_0.GetRawValue(canData);
            ara::core::Optional<ret_enbl_shift_sssist_switch_0> ret_enbl_shift_sssist_switch_0rawValue = message.ret_enbl_shift_sssist_switch_0.GetRawValue(canData);
            ara::core::Optional<ret_enbl_brake_assist_awitch_0> ret_enbl_brake_assist_awitch_0rawValue = message.ret_enbl_brake_assist_awitch_0.GetRawValue(canData);
            ara::core::Optional<retarder_torque_mode_0> retarder_torque_mode_0rawValue = message.retarder_torque_mode_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> retarderpercenttorque_0rawValue = message.retarderpercenttorque_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> urge_to_move_abortedrawValue = message.urge_to_move_aborted.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engdesiredabsintakemanifoldpressrawValue = message.engdesiredabsintakemanifoldpress.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engturbo4boostpressrawValue = message.engturbo4boostpress.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engturbo3boostpressrawValue = message.engturbo3boostpress.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engturbo2boostpressrawValue = message.engturbo2boostpress.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engturbo1boostpressrawValue = message.engturbo1boostpress.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> drive_axle_locationrawValue = message.drive_axle_location.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> drive_axle_temperaturerawValue = message.drive_axle_temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engchargeaircoolerthermostat_21rawValue = message.engchargeaircoolerthermostat_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engineintercoolertemp_21rawValue = message.engineintercoolertemp_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engineturbocharger1oiltemp_21rawValue = message.engineturbocharger1oiltemp_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Oil_Temperature_21rawValue = message.Engine_Oil_Temperature_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Fuel_Temperature_21rawValue = message.Engine_Fuel_Temperature_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Coolant_Temperature_21rawValue = message.Engine_Coolant_Temperature_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transoillvlmeasurementstatus_21rawValue = message.transoillvlmeasurementstatus_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionoillevel1cnttimer_21rawValue = message.transmissionoillevel1cnttimer_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmission1oilpressure_21rawValue = message.transmission1oilpressure_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionfilterdiffpress_21rawValue = message.transmissionfilterdiffpress_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionclutch1pressure_21rawValue = message.transmissionclutch1pressure_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmission_oil_level_21rawValue = message.transmission_oil_level_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> transmission_oil_temperature1_21rawValue = message.transmission_oil_temperature1_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Transmission_Oil_Level_HiLo_21rawValue = message.Transmission_Oil_Level_HiLo_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> hydraulic_brk_fluid_lvlswitch_21rawValue = message.hydraulic_brk_fluid_lvlswitch_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> upshift_indicatorrawValue = message.upshift_indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> adjustable_road_speedlim_statusrawValue = message.adjustable_road_speedlim_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> veh_spdlimit_override_expistatusrawValue = message.veh_spdlimit_override_expistatus.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> shift_synchronization_statusrawValue = message.shift_synchronization_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> vehicle_speedlimit_overridstatusrawValue = message.vehicle_speedlimit_overridstatus.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> driver_reward_lvl_update_approacrawValue = message.driver_reward_lvl_update_approac.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> driver_reward_levelrawValue = message.driver_reward_level.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> front_axle_group_engage_statusrawValue = message.front_axle_group_engage_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> hydraulic_brk_fluid_lvlswitchrawValue = message.hydraulic_brk_fluid_lvlswitch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> speed_sign_type_unitsrawValue = message.speed_sign_type_units.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> lanetrackingstatusleftside_e8rawValue = message.lanetrackingstatusleftside_e8.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> lanetrackingstatusrightside_e8rawValue = message.lanetrackingstatusrightside_e8.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> forward_lane_imager_statusrawValue = message.forward_lane_imager_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> lane_departure_enable_statusrawValue = message.lane_departure_enable_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> advan_emer_braking_sys_staterawValue = message.advan_emer_braking_sys_state.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> traffic_speed_limit_sign_valuerawValue = message.traffic_speed_limit_sign_value.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> excessive_overspeedrawValue = message.excessive_overspeed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> speed_limit_exceededrawValue = message.speed_limit_exceeded.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> dynamometer_moderawValue = message.dynamometer_mode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> acc_sensor_blockedrawValue = message.acc_sensor_blocked.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> brake_overuserawValue = message.brake_overuse.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> audible_feedback_controlrawValue = message.audible_feedback_control.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> cms_braking_statusrawValue = message.cms_braking_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Right_Turn_Signal_CommandrawValue = message.Rear_Right_Turn_Signal_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Left_Turn_Signal_CommandrawValue = message.Rear_Left_Turn_Signal_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Right_High_Beam_Headlamp_CommandrawValue = message.Right_High_Beam_Headlamp_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Left_High_Beam_Headlamp_CommandrawValue = message.Left_High_Beam_Headlamp_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Right_Low_Beam_Headlamp_CommandrawValue = message.Right_Low_Beam_Headlamp_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Left_Low_Beam_Headlamp_CommandrawValue = message.Left_Low_Beam_Headlamp_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Air_Horn_CommandrawValue = message.Air_Horn_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Electric_Horn_CommandrawValue = message.Electric_Horn_Command.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Turn_Signal_SwitchrawValue = message.Turn_Signal_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Main_Light_SwitchrawValue = message.Main_Light_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Operator_Washer_SwitchrawValue = message.Front_Operator_Washer_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Nonoperator_Washer_SwitchrawValue = message.Front_Nonoperator_Washer_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Operator_Wiper_SwitchrawValue = message.Front_Operator_Wiper_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Nonoperator_Wiper_SwitchrawValue = message.Front_Nonoperator_Wiper_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Net_Battery_CurrentrawValue = message.Net_Battery_Current.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Transmission_Neutral_SwitchrawValue = message.Transmission_Neutral_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> ParameterGroupNumberrawValue = message.ParameterGroupNumber.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TrailerVdcActive_3ErawValue = message.TrailerVdcActive_3E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> YcEngineControlActive_3ErawValue = message.YcEngineControlActive_3E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> YcBrakeControlActive_3ErawValue = message.YcBrakeControlActive_3E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> RopEngineControlActive_3ErawValue = message.RopEngineControlActive_3E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> RopBrakeControlActive_3ErawValue = message.RopBrakeControlActive_3E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> VdcFullyOperational_3ErawValue = message.VdcFullyOperational_3E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> VdcBrakeLightRequest_3ErawValue = message.VdcBrakeLightRequest_3E.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Electronic_Stability_Control_3ErawValue = message.Electronic_Stability_Control_3E.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_Distance_HR_0rawValue = message.Trip_Distance_HR_0.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Vehicle_Distance_HR_0rawValue = message.Total_Vehicle_Distance_HR_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Lane_Departure_RightrawValue = message.Lane_Departure_Right.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Lane_Departure_LeftrawValue = message.Lane_Departure_Left.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Lane_Departure_Imm_RightSiderawValue = message.Lane_Departure_Imm_RightSide.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Lane_Departure_Imm_LeftSiderawValue = message.Lane_Departure_Imm_LeftSide.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tractormnttrailerabswarning_BrawValue = message.tractormnttrailerabswarning_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> halt_brake_switch_BrawValue = message.halt_brake_switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> railroad_mode_switch_BrawValue = message.railroad_mode_switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> src_add_ctrl_dev_brake_ctrl_BrawValue = message.src_add_ctrl_dev_brake_ctrl_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> abs_fully_operational_BrawValue = message.abs_fully_operational_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tractionctrloverrideswitch_BrawValue = message.tractionctrloverrideswitch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_hill_holder_switch_BrawValue = message.asr_hill_holder_switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_off_road_switch_BrawValue = message.asr_off_road_switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> abs_off_road_switch_BrawValue = message.abs_off_road_switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> brake_pedal_position_BrawValue = message.brake_pedal_position_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ebs_brake_switch_BrawValue = message.ebs_brake_switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> antilock_braking_abs_active_BrawValue = message.antilock_braking_abs_active_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_brake_control_active_BrawValue = message.asr_brake_control_active_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_engine_control_active_BrawValue = message.asr_engine_control_active_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Trailer_ABSrawValue = message.Trailer_ABS.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ATC_ASR_Info_SignalrawValue = message.ATC_ASR_Info_Signal.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ABS_EBS_Amber_Warning_SignalrawValue = message.ABS_EBS_Amber_Warning_Signal.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EBS_Red_Warning_SignalrawValue = message.EBS_Red_Warning_Signal.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Retarder_Selection_BrawValue = message.Engine_Retarder_Selection_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Remote_Accel_Enable_Switch_BrawValue = message.Remote_Accel_Enable_Switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Aux_Engine_Shutdown_Switch_BrawValue = message.Aux_Engine_Shutdown_Switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Derate_Switch_BrawValue = message.Engine_Derate_Switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Accelerator_Interlock_Switch_BrawValue = message.Accelerator_Interlock_Switch_B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SCR_Oper_Indu_Act_Trav_DistrawValue = message.SCR_Oper_Indu_Act_Trav_Dist.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Catalyst_Conv_EfficiencyrawValue = message.Catalyst_Conv_Efficiency.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Aux_Cooler_Supply2rawValue = message.Eng_Aux_Cooler_Supply2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Aux_Cooler_Supply1rawValue = message.Eng_Aux_Cooler_Supply1.GetRawValue(canData);
            ara::core::Optional<AFT_Fuel_Enable_Control> AFT_Fuel_Enable_ControlrawValue = message.AFT_Fuel_Enable_Control.GetRawValue(canData);
            ara::core::Optional<Regeneration_Status_A> Regeneration_Status_ArawValue = message.Regeneration_Status_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Fuel_Pressure_ControlrawValue = message.Fuel_Pressure_Control.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Fuel_Rate_BrawValue = message.Fuel_Rate_B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Fuel_Pressure_1_ArawValue = message.Fuel_Pressure_1_A.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> trip_distancerawValue = message.trip_distance.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Vehicle_DistancerawValue = message.Total_Vehicle_Distance.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ForwardCollisionWarningrawValue = message.ForwardCollisionWarning.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ACCDistanceAlertSignalrawValue = message.ACCDistanceAlertSignal.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ACCSystemShutoffWarningrawValue = message.ACCSystemShutoffWarning.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ACCTargetDetectedrawValue = message.ACCTargetDetected.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> RoadCurvaturerawValue = message.RoadCurvature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ACCSetDistanceModerawValue = message.ACCSetDistanceMode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ACCModerawValue = message.ACCMode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ACCSetSpeedrawValue = message.ACCSetSpeed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DistanceToTheForwardVehiclerawValue = message.DistanceToTheForwardVehicle.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SpeedOfTheForwardVehiclerawValue = message.SpeedOfTheForwardVehicle.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> ParameterGroupNumber_ACKM_RXrawValue = message.ParameterGroupNumber_ACKM_RX.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AddressAccessDenied_ACKM_RXrawValue = message.AddressAccessDenied_ACKM_RX.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AddressAcknowledged_ACKM__RXrawValue = message.AddressAcknowledged_ACKM__RX.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AddressBusy_ACKM_RXrawValue = message.AddressBusy_ACKM_RX.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AddressNegativeAck_ACKM_RXrawValue = message.AddressNegativeAck_ACKM_RX.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> GroupFunctionValue_ACKM_RXrawValue = message.GroupFunctionValue_ACKM_RX.GetRawValue(canData);
            ara::core::Optional<ControlByte_ACKM_RX> ControlByte_ACKM_RXrawValue = message.ControlByte_ACKM_RX.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AltituderawValue = message.Altitude.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> HorizontalDilutionofPrecisionrawValue = message.HorizontalDilutionofPrecision.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> NavigationBasedVehicleSpeedrawValue = message.NavigationBasedVehicleSpeed.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> CompassBearingrawValue = message.CompassBearing.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BCM_UDS_TX_4A_placeholderrawValue = message.BCM_UDS_TX_4A_placeholder.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SuspendDurationrawValue = message.SuspendDuration.GetRawValue(canData);
            ara::core::Optional<HoldSignal> HoldSignalrawValue = message.HoldSignal.GetRawValue(canData);
            ara::core::Optional<SuspendSignal> SuspendSignalrawValue = message.SuspendSignal.GetRawValue(canData);
            ara::core::Optional<J1939Network3> J1939Network3rawValue = message.J1939Network3.GetRawValue(canData);
            ara::core::Optional<J1939Network2> J1939Network2rawValue = message.J1939Network2.GetRawValue(canData);
            ara::core::Optional<ISO9141> ISO9141rawValue = message.ISO9141.GetRawValue(canData);
            ara::core::Optional<SAE_J1850> SAE_J1850rawValue = message.SAE_J1850.GetRawValue(canData);
            ara::core::Optional<ManufacturerSpecificPort> ManufacturerSpecificPortrawValue = message.ManufacturerSpecificPort.GetRawValue(canData);
            ara::core::Optional<CurrentDataLink> CurrentDataLinkrawValue = message.CurrentDataLink.GetRawValue(canData);
            ara::core::Optional<SAE_J1587> SAE_J1587rawValue = message.SAE_J1587.GetRawValue(canData);
            ara::core::Optional<SAE_J1922> SAE_J1922rawValue = message.SAE_J1922.GetRawValue(canData);
            ara::core::Optional<J1939Network1> J1939Network1rawValue = message.J1939Network1.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BCM_UDS_TX_4B_placeholderrawValue = message.BCM_UDS_TX_4B_placeholder.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_InhibitrawValue = message.OnBoard_Prgm_Initiate_Inhibit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_RqstrawValue = message.OnBoard_Prgm_Initiate_Rqst.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_ECU_AddrrawValue = message.OnBoard_Prgm_Initiate_ECU_Addr.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Programming_ProgressrawValue = message.OnBoard_Programming_Progress.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Key_Switch_RequestrawValue = message.OBP_Key_Switch_Request.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Tgt_Not_ReadyrawValue = message.OBP_Interlock_Tgt_Not_Ready.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Pwr_Supply_InsuffrawValue = message.OBP_Interlock_Pwr_Supply_Insuff.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Park_BrakerawValue = message.OBP_Interlock_Park_Brake.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Key_Switch_PosrawValue = message.OBP_Interlock_Key_Switch_Pos.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Engine_SpeedrawValue = message.OBP_Interlock_Engine_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Overall_StatusrawValue = message.OBP_Interlock_Overall_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Programming_StatusrawValue = message.OnBoard_Programming_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Target_ECU_AddressrawValue = message.OnBoard_Prgm_Target_ECU_Address.GetRawValue(canData);
            ara::core::Optional<Remote_Lock_Unlock_Req_21_4B> Remote_Lock_Unlock_Req_21_4BrawValue = message.Remote_Lock_Unlock_Req_21_4B.GetRawValue(canData);
            ara::core::Optional<V2X_Vehicle_Request_21_4B> V2X_Vehicle_Request_21_4BrawValue = message.V2X_Vehicle_Request_21_4B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Message_Checksum_ValuerawValue = message.Message_Checksum_Value.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Message_Counter_ValuerawValue = message.Message_Counter_Value.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Est_Remaining_Programming_timerawValue = message.Est_Remaining_Programming_time.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Programming_TyperawValue = message.Programming_Type.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Crank_Inhibit_RequestrawValue = message.Crank_Inhibit_Request.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Comm_Header_ByterawValue = message.Comm_Header_Byte.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripFanOnTimeDueToAC_00rawValue = message.TripFanOnTimeDueToAC_00.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AuxHeaterOutputCoolantTemp_19rawValue = message.AuxHeaterOutputCoolantTemp_19.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AuxiliaryHeaterMode_19rawValue = message.AuxiliaryHeaterMode_19.GetRawValue(canData);
            ara::core::Optional<LiftAxle1Position_2F> LiftAxle1Position_2FrawValue = message.LiftAxle1Position_2F.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> RollAngle_03rawValue = message.RollAngle_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> PitchRate_03rawValue = message.PitchRate_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> PitchAngle_03rawValue = message.PitchAngle_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CrankAtmpCntonPresentStartAtt_00rawValue = message.CrankAtmpCntonPresentStartAtt_00.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> CargoWeight_21rawValue = message.CargoWeight_21.GetRawValue(canData);
            }
            break;
        case 0x9CFEBE00: {
            constexpr auto message = canID_0x9CFEBE00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FF20D9: {
            constexpr auto message = canID_0x98FF20D9{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> tmpractrawValue = message.tmpract.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> tmprsetrawValue = message.tmprset.GetRawValue(canData);
            }
            break;
        case 0x98FDE500: {
            constexpr auto message = canID_0x98FDE500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FEDB00: {
            constexpr auto message = canID_0x98FEDB00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FC4B00: {
            constexpr auto message = canID_0x98FC4B00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FECE3D: {
            constexpr auto message = canID_0x98FECE3D{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FECE27: {
            constexpr auto message = canID_0x98FECE27{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FECE17: {
            constexpr auto message = canID_0x98FECE17{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FECE00: {
            constexpr auto message = canID_0x98FECE00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98C1FF00: {
            constexpr auto message = canID_0x98C1FF00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x8CFD0E00: {
            constexpr auto message = canID_0x8CFD0E00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x8CFD1000: {
            constexpr auto message = canID_0x8CFD1000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FD6E00: {
            constexpr auto message = canID_0x98FD6E00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x9CDEFF17: {
            constexpr auto message = canID_0x9CDEFF17{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<ServiceCompIdentification_17> ServiceCompIdentification_17rawValue = message.ServiceCompIdentification_17.GetRawValue(canData);
            }
            break;
        case 0x98FEC000: {
            constexpr auto message = canID_0x98FEC000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> ServiceDelayOpTimeBased_00rawValue = message.ServiceDelayOpTimeBased_00.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> ServiceDistance_00rawValue = message.ServiceDistance_00.GetRawValue(canData);
            }
            break;
        case 0x98FB4D00: {
            constexpr auto message = canID_0x98FB4D00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> FuelFilterLifeRemain_00rawValue = message.FuelFilterLifeRemain_00.GetRawValue(canData);
            }
            break;
        case 0x98FCD500: {
            constexpr auto message = canID_0x98FCD500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> DefFilterLife_00rawValue = message.DefFilterLife_00.GetRawValue(canData);
            }
            break;
        case 0x8CFE572F: {
            constexpr auto message = canID_0x8CFE572F{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<ElecShockAbsorberCtlMode_2F> ElecShockAbsorberCtlMode_2FrawValue = message.ElecShockAbsorberCtlMode_2F.GetRawValue(canData);
            }
            break;
        case 0x8CFE4121: {
            constexpr auto message = canID_0x8CFE4121{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<RightTurnSignalLightsCmd_21> RightTurnSignalLightsCmd_21rawValue = message.RightTurnSignalLightsCmd_21.GetRawValue(canData);
            ara::core::Optional<LeftTurnSignalLightsCmd_21> LeftTurnSignalLightsCmd_21rawValue = message.LeftTurnSignalLightsCmd_21.GetRawValue(canData);
            }
            break;
        case 0x98E34A00: {
            constexpr auto message = canID_0x98E34A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FECF00: {
            constexpr auto message = canID_0x98FECF00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FCC700: {
            constexpr auto message = canID_0x98FCC700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98A04A00: {
            constexpr auto message = canID_0x98A04A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98A14A00: {
            constexpr auto message = canID_0x98A14A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98A44A3D: {
            constexpr auto message = canID_0x98A44A3D{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98A44A00: {
            constexpr auto message = canID_0x98A44A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x989E4A00: {
            constexpr auto message = canID_0x989E4A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FD8000: {
            constexpr auto message = canID_0x98FD8000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FDB700: {
            constexpr auto message = canID_0x98FDB700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FDB600: {
            constexpr auto message = canID_0x98FDB600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FDB500: {
            constexpr auto message = canID_0x98FDB500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FED400: {
            constexpr auto message = canID_0x98FED400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1700: {
            constexpr auto message = canID_0x98FB1700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1600: {
            constexpr auto message = canID_0x98FB1600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1500: {
            constexpr auto message = canID_0x98FB1500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1400: {
            constexpr auto message = canID_0x98FB1400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1300: {
            constexpr auto message = canID_0x98FB1300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1200: {
            constexpr auto message = canID_0x98FB1200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1100: {
            constexpr auto message = canID_0x98FB1100{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB1000: {
            constexpr auto message = canID_0x98FB1000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0F00: {
            constexpr auto message = canID_0x98FB0F00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0E00: {
            constexpr auto message = canID_0x98FB0E00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0D00: {
            constexpr auto message = canID_0x98FB0D00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0C00: {
            constexpr auto message = canID_0x98FB0C00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0B00: {
            constexpr auto message = canID_0x98FB0B00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0A00: {
            constexpr auto message = canID_0x98FB0A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0900: {
            constexpr auto message = canID_0x98FB0900{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0800: {
            constexpr auto message = canID_0x98FB0800{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0700: {
            constexpr auto message = canID_0x98FB0700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0600: {
            constexpr auto message = canID_0x98FB0600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0500: {
            constexpr auto message = canID_0x98FB0500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0400: {
            constexpr auto message = canID_0x98FB0400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0300: {
            constexpr auto message = canID_0x98FB0300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0200: {
            constexpr auto message = canID_0x98FB0200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x8CFD0C00: {
            constexpr auto message = canID_0x8CFD0C00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x9CFBE800: {
            constexpr auto message = canID_0x9CFBE800{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FAFA00: {
            constexpr auto message = canID_0x98FAFA00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FAF900: {
            constexpr auto message = canID_0x98FAF900{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FAF700: {
            constexpr auto message = canID_0x98FAF700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x8CF0FB00: {
            constexpr auto message = canID_0x8CF0FB00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98F01000: {
            constexpr auto message = canID_0x98F01000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0100: {
            constexpr auto message = canID_0x98FB0100{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FB0000: {
            constexpr auto message = canID_0x98FB0000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FAFF00: {
            constexpr auto message = canID_0x98FAFF00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FAFE00: {
            constexpr auto message = canID_0x98FAFE00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FAFD00: {
            constexpr auto message = canID_0x98FAFD00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FAFC00: {
            constexpr auto message = canID_0x98FAFC00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x9CFE8700: {
            constexpr auto message = canID_0x9CFE8700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x9CFE8300: {
            constexpr auto message = canID_0x9CFE8300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x9CFE8200: {
            constexpr auto message = canID_0x9CFE8200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FDD000: {
            constexpr auto message = canID_0x98FDD000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x90F01A00: {
            constexpr auto message = canID_0x90F01A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FE9600: {
            constexpr auto message = canID_0x98FE9600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FDC300: {
            constexpr auto message = canID_0x98FDC300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FDC200: {
            constexpr auto message = canID_0x98FDC200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FCD000: {
            constexpr auto message = canID_0x98FCD000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98FBD600: {
            constexpr auto message = canID_0x98FBD600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x8CF00327: {
            constexpr auto message = canID_0x8CF00327{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> CarbRawDatarawValue = message.CarbRawData.GetRawValue(canData);
            }
            break;
        case 0x98F0090B: {
            constexpr auto message = canID_0x98F0090B{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> LongitudinalAcc_0BrawValue = message.LongitudinalAcc_0B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SteeringWheelAngle_0BrawValue = message.SteeringWheelAngle_0B.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> YawRaterawValue = message.YawRate.GetRawValue(canData);
            }
            break;
        case 0x98FEE621: {
            constexpr auto message = canID_0x98FEE621{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> local_minute_offsetrawValue = message.local_minute_offset.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> local_hour_offsetrawValue = message.local_hour_offset.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> YearrawValue = message.Year.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SecondsrawValue = message.Seconds.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MonthrawValue = message.Month.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MinutesrawValue = message.Minutes.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> HoursrawValue = message.Hours.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DayrawValue = message.Day.GetRawValue(canData);
            }
            break;
        case 0x98FE8D00: {
            constexpr auto message = canID_0x98FE8D00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> battery_potential_powerInput2rawValue = message.battery_potential_powerInput2.GetRawValue(canData);
            }
            break;
        case 0x98FF3121: {
            constexpr auto message = canID_0x98FF3121{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Wheelchair_Lift_Door_statusrawValue = message.Wheelchair_Lift_Door_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> StopArm_statusrawValue = message.StopArm_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> StopArmLamp_statusrawValue = message.StopArmLamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Right_Red_Lamp_statusrawValue = message.Rear_Right_Red_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Right_Amber_Lamp_statusrawValue = message.Rear_Right_Amber_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Left_Red_Lamp_statusrawValue = message.Rear_Left_Red_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Left_Amber_Lamp_statusrawValue = message.Rear_Left_Amber_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Right_Red_Lamp_statusrawValue = message.Front_Right_Red_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Right_Amber_Lamp_statusrawValue = message.Front_Right_Amber_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Left_Red_Lamp_statusrawValue = message.Front_Left_Red_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Left_Amber_Lamp_statusrawValue = message.Front_Left_Amber_Lamp_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Emergency_Door_StatusrawValue = message.Emergency_Door_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CrossingGate_StatusrawValue = message.CrossingGate_Status.GetRawValue(canData);
            }
            break;
        case 0x98FF23F3: {
            constexpr auto message = canID_0x98FF23F3{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> b2v_totdischgenergyrawValue = message.b2v_totdischgenergy.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> b2v_totchgenergyrawValue = message.b2v_totchgenergy.GetRawValue(canData);
            }
            break;
        case 0x98FAC330: {
            constexpr auto message = canID_0x98FAC330{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> AirDryerCartridgeLifeRemain_30rawValue = message.AirDryerCartridgeLifeRemain_30.GetRawValue(canData);
            }
            break;
        case 0x98FF7321: {
            constexpr auto message = canID_0x98FF7321{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> StarterPrognosticsTxStatus_21rawValue = message.StarterPrognosticsTxStatus_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AirSystemLeakTxStatus_21rawValue = message.AirSystemLeakTxStatus_21.GetRawValue(canData);
            }
            break;
        case 0x98FF7221: {
            constexpr auto message = canID_0x98FF7221{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> SecondaryAirPressureDelta_21rawValue = message.SecondaryAirPressureDelta_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> PrimaryAirPressureDelta_21rawValue = message.PrimaryAirPressureDelta_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineOffDuration_21rawValue = message.EngineOffDuration_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AmbientAirTempDelta_21rawValue = message.AmbientAirTempDelta_21.GetRawValue(canData);
            }
            break;
        case 0x98FF7121: {
            constexpr auto message = canID_0x98FF7121{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> EngineOilTempInCrank_21rawValue = message.EngineOilTempInCrank_21.GetRawValue(canData);
            ara::core::Optional<CrankStartStatus_21> CrankStartStatus_21rawValue = message.CrankStartStatus_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> CrankDuration_21rawValue = message.CrankDuration_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BarometricPressureInCrank_21rawValue = message.BarometricPressureInCrank_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AverageCrankingSpeed_21rawValue = message.AverageCrankingSpeed_21.GetRawValue(canData);
            }
            break;
        case 0x98FF4327: {
            constexpr auto message = canID_0x98FF4327{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> FuelWaterSeperatorTxStatus_27rawValue = message.FuelWaterSeperatorTxStatus_27.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineAirFilterTxStatus_27rawValue = message.EngineAirFilterTxStatus_27.GetRawValue(canData);
            }
            break;
        case 0x98FF00D7: {
            constexpr auto message = canID_0x98FF00D7{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> WMSTransmitterStatus_D7rawValue = message.WMSTransmitterStatus_D7.GetRawValue(canData);
            }
            break;
        case 0x98FE700B: {
            constexpr auto message = canID_0x98FE700B{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Comb_Veh_Weight_0BrawValue = message.Comb_Veh_Weight_0B.GetRawValue(canData);
            }
            break;
        case 0x98FF634A: {
            constexpr auto message = canID_0x98FF634A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> StarterPrognosticsRxStatus_4ArawValue = message.StarterPrognosticsRxStatus_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AirSystemLeakRxStatus_4ArawValue = message.AirSystemLeakRxStatus_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineAirFilterRxStatus_4ArawValue = message.EngineAirFilterRxStatus_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> FuelWaterSeperatorRxStatus_4ArawValue = message.FuelWaterSeperatorRxStatus_4A.GetRawValue(canData);
            }
            break;
        case 0x98FF624A: {
            constexpr auto message = canID_0x98FF624A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> WMSReceiverStatus_4ArawValue = message.WMSReceiverStatus_4A.GetRawValue(canData);
            }
            break;
        case 0x98FF2721: {
            constexpr auto message = canID_0x98FF2721{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<RemoteLockUnlockStatus_21> RemoteLockUnlockStatus_21rawValue = message.RemoteLockUnlockStatus_21.GetRawValue(canData);
            }
            break;
        case 0x98EEFEFE: {
            constexpr auto message = canID_0x98EEFEFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> ArbitrationAddressCapable_FE_FErawValue = message.ArbitrationAddressCapable_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> IndustryGroup_FE_FErawValue = message.IndustryGroup_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> VehicleSystem_FE_FErawValue = message.VehicleSystem_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> VehicleSystemInstance_FE_FErawValue = message.VehicleSystemInstance_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Function_FE_FErawValue = message.Function_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> FunctionInstance_FE_FErawValue = message.FunctionInstance_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> ManufacturerCode_FE_FErawValue = message.ManufacturerCode_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECUInstance_FE_FErawValue = message.ECUInstance_FE_FE.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> IdentityNumber_FE_FErawValue = message.IdentityNumber_FE_FE.GetRawValue(canData);
            }
            break;
        case 0x98D9034A: {
            constexpr auto message = canID_0x98D9034A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98D4034A: {
            constexpr auto message = canID_0x98D4034A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98D7034A: {
            constexpr auto message = canID_0x98D7034A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x8CFF224A: {
            constexpr auto message = canID_0x8CFF224A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> BCMCommData_21_4ArawValue = message.BCMCommData_21_4A.GetRawValue(canData);
            }
            break;
        case 0x98FF2A21: {
            constexpr auto message = canID_0x98FF2A21{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> BcmAlarmRequest_21rawValue = message.BcmAlarmRequest_21.GetRawValue(canData);
            }
            break;
        case 0x98FF96F3: {
            constexpr auto message = canID_0x98FF96F3{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> B2v_TotDischgEnergy_F3rawValue = message.B2v_TotDischgEnergy_F3.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> B2v_TotChgEnergy_F3rawValue = message.B2v_TotChgEnergy_F3.GetRawValue(canData);
            }
            break;
        case 0x80F02B53: {
            constexpr auto message = canID_0x80F02B53{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Crash_Checksum_53rawValue = message.Crash_Checksum_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Crash_Counter_53rawValue = message.Crash_Counter_53.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Crash_Type_53rawValue = message.Crash_Type_53.GetRawValue(canData);
            }
            break;
        case 0x98FEEC5A: {
            constexpr auto message = canID_0x98FEEC5A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            }
            break;
        case 0x98EAFF11: {
            constexpr auto message = canID_0x98EAFF11{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> ParameterGroupNumberrawValue = message.ParameterGroupNumber.GetRawValue(canData);
            }
            break;
        case 0x98FF305A: {
            constexpr auto message = canID_0x98FF305A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> hves_time_to_charge_5arawValue = message.hves_time_to_charge_5a.GetRawValue(canData);
            }
            break;
        case 0x98FD0900: {
            constexpr auto message = canID_0x98FD0900{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> highresolutionengtripfuel_0rawValue = message.highresolutionengtripfuel_0.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> highresolutionengtotalfuelused_0rawValue = message.highresolutionengtotalfuelused_0.GetRawValue(canData);
            }
            break;
        case 0x98FD0700: {
            constexpr auto message = canID_0x98FD0700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> EngineInterCoolerTempHighCmd_0rawValue = message.EngineInterCoolerTempHighCmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFuelTempHighLampCmd_0rawValue = message.EngineFuelTempHighLampCmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineOilTempHighLampCmd_0rawValue = message.EngineOilTempHighLampCmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineStopStartEnabledLampCmd_0rawValue = message.EngineStopStartEnabledLampCmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngStopStartAutoStartFailedCmd_0rawValue = message.EngStopStartAutoStartFailedCmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngStopStartAutomStopActiveCmd_0rawValue = message.EngStopStartAutomStopActiveCmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngIdleMgmtPendEventLampCmd_0rawValue = message.EngIdleMgmtPendEventLampCmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_brake_active_lamp_cmd_0rawValue = message.engine_brake_active_lamp_cmd_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> obd_malfunction_indic_lamp_cmdrawValue = message.obd_malfunction_indic_lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_redstop_lamp_cmdrawValue = message.engine_redstop_lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_amberwarning_lamp_cmdrawValue = message.engine_amberwarning_lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_protect_lamp_commandrawValue = message.engine_protect_lamp_command.GetRawValue(canData);
            ara::core::Optional<CmprssnBrkEnblSwtchIndctrLampCmd> CmprssnBrkEnblSwtchIndctrLampCmdrawValue = message.CmprssnBrkEnblSwtchIndctrLampCmd.GetRawValue(canData);
            ara::core::Optional<engoilpresslowlampcmd> engoilpresslowlampcmdrawValue = message.engoilpresslowlampcmd.GetRawValue(canData);
            ara::core::Optional<engcoolanttemphighlampcmd_0> engcoolanttemphighlampcmd_0rawValue = message.engcoolanttemphighlampcmd_0.GetRawValue(canData);
            ara::core::Optional<engcoolantlevellowlampcmd_0> engcoolantlevellowlampcmd_0rawValue = message.engcoolantlevellowlampcmd_0.GetRawValue(canData);
            ara::core::Optional<engidlemanagementactivelampcmd_0> engidlemanagementactivelampcmd_0rawValue = message.engidlemanagementactivelampcmd_0.GetRawValue(canData);
            ara::core::Optional<engairfilterrestrictionlampcmd_0> engairfilterrestrictionlampcmd_0rawValue = message.engairfilterrestrictionlampcmd_0.GetRawValue(canData);
            ara::core::Optional<engfuelfilterrestrictedlampcmd_0> engfuelfilterrestrictedlampcmd_0rawValue = message.engfuelfilterrestrictedlampcmd_0.GetRawValue(canData);
            ara::core::Optional<engctrlmodule1rdyforuselampcmd_0> engctrlmodule1rdyforuselampcmd_0rawValue = message.engctrlmodule1rdyforuselampcmd_0.GetRawValue(canData);
            ara::core::Optional<engctrlmodule2rdyforuselampcmd_0> engctrlmodule2rdyforuselampcmd_0rawValue = message.engctrlmodule2rdyforuselampcmd_0.GetRawValue(canData);
            ara::core::Optional<engctrlmodule3rdyforuselampcmd_0> engctrlmodule3rdyforuselampcmd_0rawValue = message.engctrlmodule3rdyforuselampcmd_0.GetRawValue(canData);
            ara::core::Optional<engspeedhighlampcmd_0> engspeedhighlampcmd_0rawValue = message.engspeedhighlampcmd_0.GetRawValue(canData);
            ara::core::Optional<engspeedveryhighlampcmd_0> engspeedveryhighlampcmd_0rawValue = message.engspeedveryhighlampcmd_0.GetRawValue(canData);
            ara::core::Optional<vhclaccelratelimitlampcmd_0> vhclaccelratelimitlampcmd_0rawValue = message.vhclaccelratelimitlampcmd_0.GetRawValue(canData);
            }
            break;
        case 0x8CF0955A: {
            constexpr auto message = canID_0x8CF0955A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> hvesstemprawValue = message.hvesstemp.GetRawValue(canData);
            }
            break;
        case 0x98FF1480: {
            constexpr auto message = canID_0x98FF1480{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> controlpilot_moderawValue = message.controlpilot_mode.GetRawValue(canData);
            }
            break;
        case 0x98DF11FE: {
            constexpr auto message = canID_0x98DF11FE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<SuspendSignal> SuspendSignalrawValue = message.SuspendSignal.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SuspendDurationrawValue = message.SuspendDuration.GetRawValue(canData);
            ara::core::Optional<SAE_J1922> SAE_J1922rawValue = message.SAE_J1922.GetRawValue(canData);
            ara::core::Optional<SAE_J1850> SAE_J1850rawValue = message.SAE_J1850.GetRawValue(canData);
            ara::core::Optional<SAE_J1587> SAE_J1587rawValue = message.SAE_J1587.GetRawValue(canData);
            ara::core::Optional<ManufacturerSpecificPort> ManufacturerSpecificPortrawValue = message.ManufacturerSpecificPort.GetRawValue(canData);
            ara::core::Optional<J1939Network3> J1939Network3rawValue = message.J1939Network3.GetRawValue(canData);
            ara::core::Optional<J1939Network2> J1939Network2rawValue = message.J1939Network2.GetRawValue(canData);
            ara::core::Optional<J1939Network1> J1939Network1rawValue = message.J1939Network1.GetRawValue(canData);
            ara::core::Optional<ISO9141> ISO9141rawValue = message.ISO9141.GetRawValue(canData);
            ara::core::Optional<HoldSignal> HoldSignalrawValue = message.HoldSignal.GetRawValue(canData);
            ara::core::Optional<CurrentDataLink> CurrentDataLinkrawValue = message.CurrentDataLink.GetRawValue(canData);
            }
            break;
        case 0x98FED65A: {
            constexpr auto message = canID_0x98FED65A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> hybsysoffbrdchrgrconnstatrawValue = message.hybsysoffbrdchrgrconnstat.GetRawValue(canData);
            }
            break;
        case 0x9CFEAF00: {
            constexpr auto message = canID_0x9CFEAF00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> total_fuel_used_gaseousrawValue = message.total_fuel_used_gaseous.GetRawValue(canData);
            }
            break;
        case 0x98FCC25A: {
            constexpr auto message = canID_0x98FCC25A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> HybridSystemRegenBrakeInd_5arawValue = message.HybridSystemRegenBrakeInd_5a.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> HybridSystemOverheatIndic_5arawValue = message.HybridSystemOverheatIndic_5a.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> HybridSystemStopIndic_5arawValue = message.HybridSystemStopIndic_5a.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> HybridSystemInitializationInd_5arawValue = message.HybridSystemInitializationInd_5a.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> HybridSystemWarningIndic_5arawValue = message.HybridSystemWarningIndic_5a.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> strdengysrclvlrawValue = message.strdengysrclvl.GetRawValue(canData);
            }
            break;
        case 0x98FF005A: {
            constexpr auto message = canID_0x98FF005A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> clntloopclvllowrawValue = message.clntloopclvllow.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> clntloopblvllowrawValue = message.clntloopblvllow.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> clntloopalvllowrawValue = message.clntloopalvllow.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> clntloopclvlrawValue = message.clntloopclvl.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> clntloopblvlrawValue = message.clntloopblvl.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> clntloopalvlrawValue = message.clntloopalvl.GetRawValue(canData);
            }
            break;
        case 0x88FF00EF: {
            constexpr auto message = canID_0x88FF00EF{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> mtrspdrawValue = message.mtrspd.GetRawValue(canData);
            }
            break;
        case 0x98FF06EF: {
            constexpr auto message = canID_0x98FF06EF{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> swverBranchrawValue = message.swverBranch.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> swverBuildrawValue = message.swverBuild.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> swverminorrawValue = message.swverminor.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> swvermajorrawValue = message.swvermajor.GetRawValue(canData);
            }
            break;
        case 0x98FFD6B3: {
            constexpr auto message = canID_0x98FFD6B3{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> CoolTmprrawValue = message.CoolTmpr.GetRawValue(canData);
            }
            break;
        case 0x98FF2019: {
            constexpr auto message = canID_0x98FF2019{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> tmpractrawValue = message.tmpract.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> tmprsetrawValue = message.tmprset.GetRawValue(canData);
            }
            break;
        case 0x98FF03EF: {
            constexpr auto message = canID_0x98FF03EF{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> trqavailregenrawValue = message.trqavailregen.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> trqavailmtrrawValue = message.trqavailmtr.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> trqestrawValue = message.trqest.GetRawValue(canData);
            }
            break;
        case 0x8C02FF5A: {
            constexpr auto message = canID_0x8C02FF5A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> mtrgrtr1temp1rawValue = message.mtrgrtr1temp1.GetRawValue(canData);
            }
            break;
        case 0x98FF0ACA: {
            constexpr auto message = canID_0x98FF0ACA{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> btmspumpflowestrawValue = message.btmspumpflowest.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> btmspumpspdrawValue = message.btmspumpspd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> btmsmoderawValue = message.btmsmode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> BtmsCoolTmprOutrawValue = message.BtmsCoolTmprOut.GetRawValue(canData);
            }
            break;
        case 0x98FF0BCA: {
            constexpr auto message = canID_0x98FF0BCA{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> btmsheatpwrrawValue = message.btmsheatpwr.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> btmschillpwrrawValue = message.btmschillpwr.GetRawValue(canData);
            }
            break;
        case 0x98FF0BEF: {
            constexpr auto message = canID_0x98FF0BEF{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> MtrRotTotMtrrawValue = message.MtrRotTotMtr.GetRawValue(canData);
            }
            break;
        case 0x98FF12F3: {
            constexpr auto message = canID_0x98FF12F3{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> b2v_st2_maxlmt_dischgcurrrawValue = message.b2v_st2_maxlmt_dischgcurr.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> b2v_st2_maxlmt_chgcurrrawValue = message.b2v_st2_maxlmt_chgcurr.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> b2v_st2_currentrawValue = message.b2v_st2_current.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> b2v_st2_sohrawValue = message.b2v_st2_soh.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> b2v_st2_socrawValue = message.b2v_st2_soc.GetRawValue(canData);
            }
            break;
        case 0x98FF14F3: {
            constexpr auto message = canID_0x98FF14F3{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> b2v_st4_avg_temprawValue = message.b2v_st4_avg_temp.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> b2v_st4_min_temprawValue = message.b2v_st4_min_temp.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> b2v_st4_max_temprawValue = message.b2v_st4_max_temp.GetRawValue(canData);
            }
            break;
        case 0x98FF3019: {
            constexpr auto message = canID_0x98FF3019{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> hvpwractrawValue = message.hvpwract.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> instancestatrawValue = message.instancestat.GetRawValue(canData);
            }
            break;
        case 0x98FFC882: {
            constexpr auto message = canID_0x98FFC882{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint64_t> fms_IdentificationrawValue = message.fms_Identification.GetRawValue(canData);
            }
            break;
        case 0x98FFCB82: {
            constexpr auto message = canID_0x98FFCB82{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> fms_healthrawValue = message.fms_health.GetRawValue(canData);
            }
            break;
        case 0x98FFC982: {
            constexpr auto message = canID_0x98FFC982{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> eng_airfilter6_diff_pressurerawValue = message.eng_airfilter6_diff_pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> eng_airfilter5_diff_pressurerawValue = message.eng_airfilter5_diff_pressure.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> lube_dp_adv_resrawValue = message.lube_dp_adv_res.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> stage2fueldpadvresrawValue = message.stage2fueldpadvres.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> stage1fueldpadvresrawValue = message.stage1fueldpadvres.GetRawValue(canData);
            }
            break;
        case 0x8CF0965A: {
            constexpr auto message = canID_0x8CF0965A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> hvessintchrgrstatrawValue = message.hvessintchrgrstat.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> hvessopstatrawValue = message.hvessopstat.GetRawValue(canData);
            }
            break;
        case 0x98FCB75A: {
            constexpr auto message = canID_0x98FCB75A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> hybridsystemenginestop_5arawValue = message.hybridsystemenginestop_5a.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> distance_to_emptyrawValue = message.distance_to_empty.GetRawValue(canData);
            }
            break;
        case 0x8CFB745A: {
            constexpr auto message = canID_0x8CFB745A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> mot_gen_status_5arawValue = message.mot_gen_status_5a.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> propulsion_torque_percentrawValue = message.propulsion_torque_percent.GetRawValue(canData);
            }
            break;
        case 0x98FEFC82: {
            constexpr auto message = canID_0x98FEFC82{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> enginefuelfilterdiffpressure_82rawValue = message.enginefuelfilterdiffpressure_82.GetRawValue(canData);
            }
            break;
        case 0x98FEF682: {
            constexpr auto message = canID_0x98FEF682{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> eng_airfilter1_dif_pressure_82rawValue = message.eng_airfilter1_dif_pressure_82.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> enginecoolantfilterdiffpress_82rawValue = message.enginecoolantfilterdiffpress_82.GetRawValue(canData);
            }
            break;
        case 0x90F03300: {
            constexpr auto message = canID_0x90F03300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> aft1_scr_intermediate_nh3rawValue = message.aft1_scr_intermediate_nh3.GetRawValue(canData);
            }
            break;
        case 0x98FCC500: {
            constexpr auto message = canID_0x98FCC500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> aft1_scr_intrmediate_temperaturerawValue = message.aft1_scr_intrmediate_temperature.GetRawValue(canData);
            }
            break;
        case 0x9CFD0800: {
            constexpr auto message = canID_0x9CFD0800{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> engine_oil_temperature3rawValue = message.engine_oil_temperature3.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_oil_relative_dielectricrawValue = message.engine_oil_relative_dielectric.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_oil_densityrawValue = message.engine_oil_density.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_oil_viscosityrawValue = message.engine_oil_viscosity.GetRawValue(canData);
            }
            break;
        case 0x98FE9A00: {
            constexpr auto message = canID_0x98FE9A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> engine_turbocharger1_intaketemprawValue = message.engine_turbocharger1_intaketemp.GetRawValue(canData);
            }
            break;
        case 0x98FE6A82: {
            constexpr auto message = canID_0x98FE6A82{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> eng_fuelfilterpressuresuction_82rawValue = message.eng_fuelfilterpressuresuction_82.GetRawValue(canData);
            }
            break;
        case 0x98FE6A00: {
            constexpr auto message = canID_0x98FE6A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> engine_oilfilteroutletpressurerawValue = message.engine_oilfilteroutletpressure.GetRawValue(canData);
            }
            break;
        case 0x8CF02300: {
            constexpr auto message = canID_0x8CF02300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> aft1_diesel_exhaust_fluid_actrawValue = message.aft1_diesel_exhaust_fluid_act.GetRawValue(canData);
            }
            break;
        case 0x98FDD082: {
            constexpr auto message = canID_0x98FDD082{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> engine_air_filter4_diff_pressurerawValue = message.engine_air_filter4_diff_pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_air_filter3_diff_pressurerawValue = message.engine_air_filter3_diff_pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_air_filter2_diff_pressurerawValue = message.engine_air_filter2_diff_pressure.GetRawValue(canData);
            }
            break;
        case 0x98FCE48C: {
            constexpr auto message = canID_0x98FCE48C{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> engine_fuel1_temperature1rawValue = message.engine_fuel1_temperature1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_fuel_dielectricityrawValue = message.engine_fuel_dielectricity.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_fuel_densityrawValue = message.engine_fuel_density.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_fuel_dynamic_viscosityrawValue = message.engine_fuel_dynamic_viscosity.GetRawValue(canData);
            }
            break;
        case 0x9CFE9200: {
            constexpr auto message = canID_0x9CFE9200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> EngFuelSys1GasMassFlowRate_00rawValue = message.EngFuelSys1GasMassFlowRate_00.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_oil_filter_intake_pressrawValue = message.engine_oil_filter_intake_press.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engine_exhaust_pressure_1rawValue = message.engine_exhaust_pressure_1.GetRawValue(canData);
            }
            break;
        case 0x9CFE8C00: {
            constexpr auto message = canID_0x9CFE8C00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> auxiliary_temperature_1rawValue = message.auxiliary_temperature_1.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> auxiliary_pressure_1rawValue = message.auxiliary_pressure_1.GetRawValue(canData);
            }
            break;
        case 0x98FEDD00: {
            constexpr auto message = canID_0x98FEDD00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> engine_turbocharger_1_speedrawValue = message.engine_turbocharger_1_speed.GetRawValue(canData);
            }
            break;
        case 0x98FFDD4A: {
            constexpr auto message = canID_0x98FFDD4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98FD82FE: {
            constexpr auto message = canID_0x98FD82FE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> SPNConversionMethod2rawValue = message.SPNConversionMethod2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OccurrenceCount2rawValue = message.OccurrenceCount2.GetRawValue(canData);
            ara::core::Optional<FailureModeIdentifier2> FailureModeIdentifier2rawValue = message.FailureModeIdentifier2.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> SuspectParameterNumber2rawValue = message.SuspectParameterNumber2.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp2> FlashMalfuncIndicatorLamp2rawValue = message.FlashMalfuncIndicatorLamp2.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp2> FlashRedStopLamp2rawValue = message.FlashRedStopLamp2.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp2> FlashAmberWarningLamp2rawValue = message.FlashAmberWarningLamp2.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp2> FlashProtectLamp2rawValue = message.FlashProtectLamp2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MalfunctionIndicatorLampStatus2rawValue = message.MalfunctionIndicatorLampStatus2.GetRawValue(canData);
            ara::core::Optional<RedStopLampStatus2> RedStopLampStatus2rawValue = message.RedStopLampStatus2.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus2> AmberWarningLampStatus2rawValue = message.AmberWarningLampStatus2.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatus2> ProtectLampStatus2rawValue = message.ProtectLampStatus2.GetRawValue(canData);
            }
            break;
        case 0x98F00100: {
            constexpr auto message = canID_0x98F00100{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> tractormnttrailerabswarning_0rawValue = message.tractormnttrailerabswarning_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> halt_brake_switch_0rawValue = message.halt_brake_switch_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> railroad_mode_switch_0rawValue = message.railroad_mode_switch_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> src_add_ctrl_dev_brake_ctrl_0rawValue = message.src_add_ctrl_dev_brake_ctrl_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> abs_fully_operational_0rawValue = message.abs_fully_operational_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tractionctrloverrideswitch_0rawValue = message.tractionctrloverrideswitch_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_hill_holder_switch_0rawValue = message.asr_hill_holder_switch_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_off_road_switch_0rawValue = message.asr_off_road_switch_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> abs_off_road_switch_0rawValue = message.abs_off_road_switch_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> brake_pedal_position_0rawValue = message.brake_pedal_position_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ebs_brake_switch_0rawValue = message.ebs_brake_switch_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> antilock_braking_abs_active_0rawValue = message.antilock_braking_abs_active_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_brake_control_active_0rawValue = message.asr_brake_control_active_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_engine_control_active_0rawValue = message.asr_engine_control_active_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ABS_EBS_Amber_Warning_0rawValue = message.ABS_EBS_Amber_Warning_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Trailer_ABS_0rawValue = message.Trailer_ABS_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ATC_ASR_Info_Signal_0rawValue = message.ATC_ASR_Info_Signal_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EBS_Red_Warning_Signal_0rawValue = message.EBS_Red_Warning_Signal_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Retarder_Selection_B_0rawValue = message.Engine_Retarder_Selection_B_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Remote_Accel_Enable_Switch_B_0rawValue = message.Remote_Accel_Enable_Switch_B_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Aux_Engine_Shutdown_Switch_B_0rawValue = message.Aux_Engine_Shutdown_Switch_B_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Derate_Switch_B_0rawValue = message.Engine_Derate_Switch_B_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Accelerator_Interlock_Switch_B_0rawValue = message.Accelerator_Interlock_Switch_B_0.GetRawValue(canData);
            }
            break;
        case 0x98FF2921: {
            constexpr auto message = canID_0x98FF2921{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> check_acrawValue = message.check_ac.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> distancetoemptydefrawValue = message.distancetoemptydef.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engineoiltemperaturerawValue = message.engineoiltemperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> frontaxleengagedrawValue = message.frontaxleengaged.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> distancetoemptyfuelrawValue = message.distancetoemptyfuel.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> diagnosticmodecmdrawValue = message.diagnosticmodecmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ptob_engagedrawValue = message.ptob_engaged.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ptoa_engagedrawValue = message.ptoa_engaged.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> alarmrequestrawValue = message.alarmrequest.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> airfilterrestrictionrawValue = message.airfilterrestriction.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> powerdividerlockrawValue = message.powerdividerlock.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> lowcoolantlevelrawValue = message.lowcoolantlevel.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> oilpressurerawValue = message.oilpressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> retarderovertemprawValue = message.retarderovertemp.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> retarderactiverawValue = message.retarderactive.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> lowoillevelrawValue = message.lowoillevel.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> lowwasherfluidrawValue = message.lowwasherfluid.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> servicebrakerawValue = message.servicebrake.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> changeoilfilterrawValue = message.changeoilfilter.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> highcoolanttemperaturerawValue = message.highcoolanttemperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> servicefuelfilterrawValue = message.servicefuelfilter.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> waterinfuelrawValue = message.waterinfuel.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> brakefluidlow_narawValue = message.brakefluidlow_na.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> brakefluidlow_exportrawValue = message.brakefluidlow_export.GetRawValue(canData);
            }
            break;
        case 0x98DFFFFE: {
            constexpr auto message = canID_0x98DFFFFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> SuspendDurationrawValue = message.SuspendDuration.GetRawValue(canData);
            ara::core::Optional<HoldSignal> HoldSignalrawValue = message.HoldSignal.GetRawValue(canData);
            ara::core::Optional<SuspendSignal> SuspendSignalrawValue = message.SuspendSignal.GetRawValue(canData);
            ara::core::Optional<J1939Network3> J1939Network3rawValue = message.J1939Network3.GetRawValue(canData);
            ara::core::Optional<J1939Network2> J1939Network2rawValue = message.J1939Network2.GetRawValue(canData);
            ara::core::Optional<ISO9141> ISO9141rawValue = message.ISO9141.GetRawValue(canData);
            ara::core::Optional<SAE_J1850> SAE_J1850rawValue = message.SAE_J1850.GetRawValue(canData);
            ara::core::Optional<ManufacturerSpecificPort> ManufacturerSpecificPortrawValue = message.ManufacturerSpecificPort.GetRawValue(canData);
            ara::core::Optional<CurrentDataLink> CurrentDataLinkrawValue = message.CurrentDataLink.GetRawValue(canData);
            ara::core::Optional<SAE_J1587> SAE_J1587rawValue = message.SAE_J1587.GetRawValue(canData);
            ara::core::Optional<SAE_J1922> SAE_J1922rawValue = message.SAE_J1922.GetRawValue(canData);
            ara::core::Optional<J1939Network1> J1939Network1rawValue = message.J1939Network1.GetRawValue(canData);
            }
            break;
        case 0x98FF3021: {
            constexpr auto message = canID_0x98FF3021{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Wheelchair_Lift_Door_cmdrawValue = message.Wheelchair_Lift_Door_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CrossingGate_CmdrawValue = message.CrossingGate_Cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> StopArm_cmdrawValue = message.StopArm_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> StopArmLamp_cmdrawValue = message.StopArmLamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Right_Red_Lamp_cmdrawValue = message.Rear_Right_Red_Lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Left_Red_Lamp_cmdrawValue = message.Rear_Left_Red_Lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Right_Red_Lamp_cmdrawValue = message.Front_Right_Red_Lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Left_Red_Lamp_cmdrawValue = message.Front_Left_Red_Lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Right_Amber_Lamp_cmdrawValue = message.Rear_Right_Amber_Lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Left_Amber_Lamp_cmdrawValue = message.Rear_Left_Amber_Lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Right_Amber_Lamp_cmdrawValue = message.Front_Right_Amber_Lamp_cmd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Front_Left_Amber_Lamp_cmdrawValue = message.Front_Left_Amber_Lamp_cmd.GetRawValue(canData);
            }
            break;
        case 0x98FF3321: {
            constexpr auto message = canID_0x98FF3321{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Rear_Right_Turn_Signal_StatusrawValue = message.Rear_Right_Turn_Signal_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Rear_Left_Turn_Signal_StatusrawValue = message.Rear_Left_Turn_Signal_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Right_High_Beam_Headlamp_StatusrawValue = message.Right_High_Beam_Headlamp_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Left_High_Beam_Headlamp_StatusrawValue = message.Left_High_Beam_Headlamp_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Right_Low_Beam_Headlamp_StatusrawValue = message.Right_Low_Beam_Headlamp_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Left_Low_Beam_Headlamp_StatusrawValue = message.Left_Low_Beam_Headlamp_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Aggregate_Door_Switch_StatusrawValue = message.Aggregate_Door_Switch_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Air_Horn_StatusrawValue = message.Air_Horn_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Electric_Horn_StatusrawValue = message.Electric_Horn_Status.GetRawValue(canData);
            }
            break;
        case 0x9CFEC321: {
            constexpr auto message = canID_0x9CFEC321{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Transmission_Neutral_Switch_21rawValue = message.Transmission_Neutral_Switch_21.GetRawValue(canData);
            }
            break;
        case 0x98FEF803: {
            constexpr auto message = canID_0x98FEF803{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> transoillvlmeasurementstatus_03rawValue = message.transoillvlmeasurementstatus_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionoillevel1cnttimer_03rawValue = message.transmissionoillevel1cnttimer_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmission1oilpressure_03rawValue = message.transmission1oilpressure_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionfilterdiffpress_03rawValue = message.transmissionfilterdiffpress_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionclutch1pressure_03rawValue = message.transmissionclutch1pressure_03.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmission_oil_levelrawValue = message.transmission_oil_level.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> transmission_oil_temperature1rawValue = message.transmission_oil_temperature1.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Transmission_Oil_Level_HiLorawValue = message.Transmission_Oil_Level_HiLo.GetRawValue(canData);
            }
            break;
        case 0x98DA4A21: {
            constexpr auto message = canID_0x98DA4A21{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98DAFAFE: {
            constexpr auto message = canID_0x98DAFAFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98DAFEFA: {
            constexpr auto message = canID_0x98DAFEFA{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> BCM_UDS_TX_4A_placeholderrawValue = message.BCM_UDS_TX_4A_placeholder.GetRawValue(canData);
            }
            break;
        case 0x98DAF9FE: {
            constexpr auto message = canID_0x98DAF9FE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98DAFEF9: {
            constexpr auto message = canID_0x98DAFEF9{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> BCM_UDS_TX_4A_placeholderrawValue = message.BCM_UDS_TX_4A_placeholder.GetRawValue(canData);
            }
            break;
        case 0x98DAFE4A: {
            constexpr auto message = canID_0x98DAFE4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> BCM_UDS_TX_4A_placeholderrawValue = message.BCM_UDS_TX_4A_placeholder.GetRawValue(canData);
            }
            break;
        case 0x98FEF121: {
            constexpr auto message = canID_0x98FEF121{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<ParkBrakeReleaseInhibitRqst_21> ParkBrakeReleaseInhibitRqst_21rawValue = message.ParkBrakeReleaseInhibitRqst_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlPauseSwitch_21> CruiseControlPauseSwitch_21rawValue = message.CruiseControlPauseSwitch_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngShutdownOverrideSwitchA_21rawValue = message.EngShutdownOverrideSwitchA_21.GetRawValue(canData);
            ara::core::Optional<Engine_Test_Mode_Switch_A_21> Engine_Test_Mode_Switch_A_21rawValue = message.Engine_Test_Mode_Switch_A_21.GetRawValue(canData);
            ara::core::Optional<Engine_Idle_Decrement_Switch_21> Engine_Idle_Decrement_Switch_21rawValue = message.Engine_Idle_Decrement_Switch_21.GetRawValue(canData);
            ara::core::Optional<Engine_Idle_Increment_Switch_21> Engine_Idle_Increment_Switch_21rawValue = message.Engine_Idle_Increment_Switch_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlState_21> CruiseControlState_21rawValue = message.CruiseControlState_21.GetRawValue(canData);
            ara::core::Optional<PTOState_21> PTOState_21rawValue = message.PTOState_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> CruiseControlSetSpeed_21rawValue = message.CruiseControlSetSpeed_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlAccelerateSwitch_21> CruiseControlAccelerateSwitch_21rawValue = message.CruiseControlAccelerateSwitch_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlResumeSwitch_21> CruiseControlResumeSwitch_21rawValue = message.CruiseControlResumeSwitch_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlCoastSwitch_21> CruiseControlCoastSwitch_21rawValue = message.CruiseControlCoastSwitch_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlSetSwitch_21> CruiseControlSetSwitch_21rawValue = message.CruiseControlSetSwitch_21.GetRawValue(canData);
            ara::core::Optional<ClutchSwitch_21> ClutchSwitch_21rawValue = message.ClutchSwitch_21.GetRawValue(canData);
            ara::core::Optional<brakeswitch_21> brakeswitch_21rawValue = message.brakeswitch_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlEnableSwitch_21> CruiseControlEnableSwitch_21rawValue = message.CruiseControlEnableSwitch_21.GetRawValue(canData);
            ara::core::Optional<CruiseControlActive_21> CruiseControlActive_21rawValue = message.CruiseControlActive_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> WheelBasedVehicleSpeed_21rawValue = message.WheelBasedVehicleSpeed_21.GetRawValue(canData);
            ara::core::Optional<ParkingBrakeSwitch_21> ParkingBrakeSwitch_21rawValue = message.ParkingBrakeSwitch_21.GetRawValue(canData);
            ara::core::Optional<TwoSpeedAxleSwitch_21> TwoSpeedAxleSwitch_21rawValue = message.TwoSpeedAxleSwitch_21.GetRawValue(canData);
            }
            break;
        case 0x98FEF721: {
            constexpr auto message = canID_0x98FEF721{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> sli_battery1_net_current_21rawValue = message.sli_battery1_net_current_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> charging_system_potential_21rawValue = message.charging_system_potential_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> alternator_current_21rawValue = message.alternator_current_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Keyswitch_Battery_Potential_21rawValue = message.Keyswitch_Battery_Potential_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Battery_Potential_21rawValue = message.Battery_Potential_21.GetRawValue(canData);
            }
            break;
        case 0x98F0010B: {
            constexpr auto message = canID_0x98F0010B{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> tractormnttrailerabswarning_B_BrawValue = message.tractormnttrailerabswarning_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> halt_brake_switch_B_BrawValue = message.halt_brake_switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> railroad_mode_switch_B_BrawValue = message.railroad_mode_switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> src_add_ctrl_dev_brake_ctrl_B_BrawValue = message.src_add_ctrl_dev_brake_ctrl_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> abs_fully_operational_B_BrawValue = message.abs_fully_operational_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tractionctrloverrideswitch_B_BrawValue = message.tractionctrloverrideswitch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_hill_holder_switch_B_BrawValue = message.asr_hill_holder_switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_off_road_switch_B_BrawValue = message.asr_off_road_switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> abs_off_road_switch_B_BrawValue = message.abs_off_road_switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> brake_pedal_position_B_BrawValue = message.brake_pedal_position_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ebs_brake_switch_B_BrawValue = message.ebs_brake_switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> antilock_braking_abs_active_B_BrawValue = message.antilock_braking_abs_active_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_brake_control_active_B_BrawValue = message.asr_brake_control_active_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> asr_engine_control_active_B_BrawValue = message.asr_engine_control_active_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Trailer_ABS_BrawValue = message.Trailer_ABS_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ATC_ASR_Info_Signal_BrawValue = message.ATC_ASR_Info_Signal_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ABS_EBS_Amber_Warning_Signal_BrawValue = message.ABS_EBS_Amber_Warning_Signal_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EBS_Red_Warning_Signal_BrawValue = message.EBS_Red_Warning_Signal_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Retarder_Selection_B_BrawValue = message.Engine_Retarder_Selection_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Remote_Accel_Enable_Switch_B_BrawValue = message.Remote_Accel_Enable_Switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Aux_Engine_Shutdown_Switch_B_BrawValue = message.Aux_Engine_Shutdown_Switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Derate_Switch_B_BrawValue = message.Engine_Derate_Switch_B_B.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Accelerator_Interlock_Switch_B_BrawValue = message.Accelerator_Interlock_Switch_B_B.GetRawValue(canData);
            }
            break;
        case 0x98FEFA21: {
            constexpr auto message = canID_0x98FEFA21{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> parkingbrakeredwarningsignal_21rawValue = message.parkingbrakeredwarningsignal_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> parkingbrakeactuator_21rawValue = message.parkingbrakeactuator_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> brakeapplicationpressure_21rawValue = message.brakeapplicationpressure_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Brake_Sec_PressurerawValue = message.Brake_Sec_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Brake_Prim_PressurerawValue = message.Brake_Prim_Pressure.GetRawValue(canData);
            }
            break;
        case 0x98FE4F0B: {
            constexpr auto message = canID_0x98FE4F0B{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> trailer_vdc_activerawValue = message.trailer_vdc_active.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> yc_brake_control_activerawValue = message.yc_brake_control_active.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> yc_engine_control_activerawValue = message.yc_engine_control_active.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> rop_brake_control_activerawValue = message.rop_brake_control_active.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> rop_engine_control_activerawValue = message.rop_engine_control_active.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> vdcbrakelightrequestrawValue = message.vdcbrakelightrequest.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> vdcfullyoperationalrawValue = message.vdcfullyoperational.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Electronic_Stability_ControlrawValue = message.Electronic_Stability_Control.GetRawValue(canData);
            }
            break;
        case 0x98FECC4A: {
            constexpr auto message = canID_0x98FECC4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::int8_t> DM3_SignalrawValue = message.DM3_Signal.GetRawValue(canData);
            }
            break;
        case 0x98FED34A: {
            constexpr auto message = canID_0x98FED34A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::int8_t> DM11_SignalrawValue = message.DM11_Signal.GetRawValue(canData);
            }
            break;
        case 0x98EA4AFE: {
            constexpr auto message = canID_0x98EA4AFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> ParameterGroupNumber_RQST_RXrawValue = message.ParameterGroupNumber_RQST_RX.GetRawValue(canData);
            }
            break;
        case 0x98FEEBFE: {
            constexpr auto message = canID_0x98FEEBFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> UnitNumberrawValue = message.UnitNumber.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SerialNumberrawValue = message.SerialNumber.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> ModelrawValue = message.Model.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MakerawValue = message.Make.GetRawValue(canData);
            }
            break;
        case 0x9CFEAB03: {
            constexpr auto message = canID_0x9CFEAB03{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Highest_Poss_GearrawValue = message.Highest_Poss_Gear.GetRawValue(canData);
            }
            break;
        case 0x98FEF433: {
            constexpr auto message = canID_0x98FEF433{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> extended_tire_pressure_supportrawValue = message.extended_tire_pressure_support.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tirepressure_threshold_detectionrawValue = message.tirepressure_threshold_detection.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> tire_air_leakage_raterawValue = message.tire_air_leakage_rate.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tire_sensor_electrical_faultrawValue = message.tire_sensor_electrical_fault.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tire_statusrawValue = message.tire_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> tire_sensor_enable_statusrawValue = message.tire_sensor_enable_status.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Tire_TemperaturerawValue = message.Tire_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Tire_PressurerawValue = message.Tire_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Tire_LocationrawValue = message.Tire_Location.GetRawValue(canData);
            }
            break;
        case 0x98EAFE4A: {
            constexpr auto message = canID_0x98EAFE4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> ParameterGroupNumberrawValue = message.ParameterGroupNumber.GetRawValue(canData);
            }
            break;
        case 0x98D0FF37: {
            constexpr auto message = canID_0x98D0FF37{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Illumination_Brightness_PercentrawValue = message.Illumination_Brightness_Percent.GetRawValue(canData);
            }
            break;
        case 0x98FDC5FE: {
            constexpr auto message = canID_0x98FDC5FE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> ECU_TyperawValue = message.ECU_Type.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_Serial_NumberrawValue = message.ECU_Serial_Number.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_Part_NumberrawValue = message.ECU_Part_Number.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_Manufacturer_NamerawValue = message.ECU_Manufacturer_Name.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_LocationrawValue = message.ECU_Location.GetRawValue(canData);
            }
            break;
        case 0x98FDC54A: {
            constexpr auto message = canID_0x98FDC54A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> ECU_Manufacturer_NamerawValue = message.ECU_Manufacturer_Name.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_TyperawValue = message.ECU_Type.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_LocationrawValue = message.ECU_Location.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_Part_NumberrawValue = message.ECU_Part_Number.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> ECU_Serial_NumberrawValue = message.ECU_Serial_Number.GetRawValue(canData);
            }
            break;
        case 0x98DA214A: {
            constexpr auto message = canID_0x98DA214A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> BCM_UDS_TX_4A_placeholderrawValue = message.BCM_UDS_TX_4A_placeholder.GetRawValue(canData);
            }
            break;
        case 0x98DA4AFE: {
            constexpr auto message = canID_0x98DA4AFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98C2FFFE: {
            constexpr auto message = canID_0x98C2FFFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> ApplSysMonitorNumerator_DM20rawValue = message.ApplSysMonitorNumerator_DM20.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> ApplSysMonitorDenominator_DM20rawValue = message.ApplSysMonitorDenominator_DM20.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> SPNofApplSysMonitor_DM20rawValue = message.SPNofApplSysMonitor_DM20.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> OBDMonitoringCondEncount_DM20rawValue = message.OBDMonitoringCondEncount_DM20.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> IgnitionCycleCounter_DM20rawValue = message.IgnitionCycleCounter_DM20.GetRawValue(canData);
            }
            break;
        case 0x98684B21: {
            constexpr auto message = canID_0x98684B21{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> OnBoard_Programming_ProgressrawValue = message.OnBoard_Programming_Progress.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Key_Switch_RequestrawValue = message.OBP_Key_Switch_Request.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Tgt_Not_ReadyrawValue = message.OBP_Interlock_Tgt_Not_Ready.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Pwr_Supply_InsuffrawValue = message.OBP_Interlock_Pwr_Supply_Insuff.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Park_BrakerawValue = message.OBP_Interlock_Park_Brake.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Key_Switch_PosrawValue = message.OBP_Interlock_Key_Switch_Pos.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Engine_SpeedrawValue = message.OBP_Interlock_Engine_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Overall_StatusrawValue = message.OBP_Interlock_Overall_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Programming_StatusrawValue = message.OnBoard_Programming_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Target_ECU_AddressrawValue = message.OnBoard_Prgm_Target_ECU_Address.GetRawValue(canData);
            }
            break;
        case 0x98D3FFFE: {
            constexpr auto message = canID_0x98D3FFFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> CalibrationID4rawValue = message.CalibrationID4.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> CalibrationID3rawValue = message.CalibrationID3.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> CalibrationID2rawValue = message.CalibrationID2.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> CalibrationID1rawValue = message.CalibrationID1.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> CalibrationVerificationNumberrawValue = message.CalibrationVerificationNumber.GetRawValue(canData);
            }
            break;
        case 0x98FECBFE: {
            constexpr auto message = canID_0x98FECBFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> SPNConversionMethod2rawValue = message.SPNConversionMethod2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OccurrenceCount2rawValue = message.OccurrenceCount2.GetRawValue(canData);
            ara::core::Optional<FailureModeIdentifier2> FailureModeIdentifier2rawValue = message.FailureModeIdentifier2.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> SuspectParameterNumber2rawValue = message.SuspectParameterNumber2.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp2> FlashMalfuncIndicatorLamp2rawValue = message.FlashMalfuncIndicatorLamp2.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp2> FlashRedStopLamp2rawValue = message.FlashRedStopLamp2.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp2> FlashAmberWarningLamp2rawValue = message.FlashAmberWarningLamp2.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp2> FlashProtectLamp2rawValue = message.FlashProtectLamp2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MalfunctionIndicatorLampStatus2rawValue = message.MalfunctionIndicatorLampStatus2.GetRawValue(canData);
            ara::core::Optional<RedStopLampStatus2> RedStopLampStatus2rawValue = message.RedStopLampStatus2.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus2> AmberWarningLampStatus2rawValue = message.AmberWarningLampStatus2.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatus2> ProtectLampStatus2rawValue = message.ProtectLampStatus2.GetRawValue(canData);
            }
            break;
        case 0x98FEDA4A: {
            constexpr auto message = canID_0x98FEDA4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> SoftwareIdentificationrawValue = message.SoftwareIdentification.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> NumberOfSoftwareIdFieldsrawValue = message.NumberOfSoftwareIdFields.GetRawValue(canData);
            }
            break;
        case 0x98FEEB4A: {
            constexpr auto message = canID_0x98FEEB4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> UnitNumberrawValue = message.UnitNumber.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> SerialNumberrawValue = message.SerialNumber.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> ModelrawValue = message.Model.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MakerawValue = message.Make.GetRawValue(canData);
            }
            break;
        case 0x98FECB4A: {
            constexpr auto message = canID_0x98FECB4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> SPNConversionMethod2rawValue = message.SPNConversionMethod2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OccurrenceCount2rawValue = message.OccurrenceCount2.GetRawValue(canData);
            ara::core::Optional<FailureModeIdentifier2> FailureModeIdentifier2rawValue = message.FailureModeIdentifier2.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> SuspectParameterNumber2rawValue = message.SuspectParameterNumber2.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp2> FlashMalfuncIndicatorLamp2rawValue = message.FlashMalfuncIndicatorLamp2.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp2> FlashRedStopLamp2rawValue = message.FlashRedStopLamp2.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp2> FlashAmberWarningLamp2rawValue = message.FlashAmberWarningLamp2.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp2> FlashProtectLamp2rawValue = message.FlashProtectLamp2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MalfunctionIndicatorLampStatus2rawValue = message.MalfunctionIndicatorLampStatus2.GetRawValue(canData);
            ara::core::Optional<RedStopLampStatus2> RedStopLampStatus2rawValue = message.RedStopLampStatus2.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus2> AmberWarningLampStatus2rawValue = message.AmberWarningLampStatus2.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatus2> ProtectLampStatus2rawValue = message.ProtectLampStatus2.GetRawValue(canData);
            }
            break;
        case 0x98FB8121: {
            constexpr auto message = canID_0x98FB8121{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_InhibitrawValue = message.OnBoard_Prgm_Initiate_Inhibit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_RqstrawValue = message.OnBoard_Prgm_Initiate_Rqst.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_ECU_AddrrawValue = message.OnBoard_Prgm_Initiate_ECU_Addr.GetRawValue(canData);
            }
            break;
        case 0x9868214A: {
            constexpr auto message = canID_0x9868214A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> OnBoard_Programming_ProgressrawValue = message.OnBoard_Programming_Progress.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Key_Switch_RequestrawValue = message.OBP_Key_Switch_Request.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Tgt_Not_ReadyrawValue = message.OBP_Interlock_Tgt_Not_Ready.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Pwr_Supply_InsuffrawValue = message.OBP_Interlock_Pwr_Supply_Insuff.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Park_BrakerawValue = message.OBP_Interlock_Park_Brake.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Key_Switch_PosrawValue = message.OBP_Interlock_Key_Switch_Pos.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Engine_SpeedrawValue = message.OBP_Interlock_Engine_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Overall_StatusrawValue = message.OBP_Interlock_Overall_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Programming_StatusrawValue = message.OnBoard_Programming_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Target_ECU_AddressrawValue = message.OnBoard_Prgm_Target_ECU_Address.GetRawValue(canData);
            }
            break;
        case 0x98FFDD00: {
            constexpr auto message = canID_0x98FFDD00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x9CFEB700: {
            constexpr auto message = canID_0x9CFEB700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Total_Engine_Cruise_TimerawValue = message.Total_Engine_Cruise_Time.GetRawValue(canData);
            }
            break;
        case 0x98FFF821: {
            constexpr auto message = canID_0x98FFF821{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<VehicleOperationalMode_21> VehicleOperationalMode_21rawValue = message.VehicleOperationalMode_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Vehicle_State_MsgCounterrawValue = message.Vehicle_State_MsgCounter.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MsgCtr_OddParity_BitrawValue = message.MsgCtr_OddParity_Bit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Crank_GridStaterawValue = message.Crank_GridState.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Ign_GridStaterawValue = message.Ign_GridState.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Acc_GridStaterawValue = message.Acc_GridState.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Load_Control_StatesrawValue = message.Load_Control_States.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Vehicle_AutoTheft_StatusrawValue = message.Vehicle_AutoTheft_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Vehicle_AutoSS_StatusrawValue = message.Vehicle_AutoSS_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Vehicle_SleepModerawValue = message.Vehicle_SleepMode.GetRawValue(canData);
            }
            break;
        case 0x98EF174A: {
            constexpr auto message = canID_0x98EF174A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98EF214A: {
            constexpr auto message = canID_0x98EF214A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<RemoteLockUnlockReq_21_4A> RemoteLockUnlockReq_21_4ArawValue = message.RemoteLockUnlockReq_21_4A.GetRawValue(canData);
            ara::core::Optional<V2xVehicleRequest_21_4A> V2xVehicleRequest_21_4ArawValue = message.V2xVehicleRequest_21_4A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Message_Checksum_ValuerawValue = message.Message_Checksum_Value.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Message_Counter_ValuerawValue = message.Message_Counter_Value.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Est_Remaining_Programming_timerawValue = message.Est_Remaining_Programming_time.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Programming_TyperawValue = message.Programming_Type.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Crank_Inhibit_RequestrawValue = message.Crank_Inhibit_Request.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Comm_Header_ByterawValue = message.Comm_Header_Byte.GetRawValue(canData);
            }
            break;
        case 0x98EF4A17: {
            constexpr auto message = canID_0x98EF4A17{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Freeform_Text_MessagingrawValue = message.Freeform_Text_Messaging.GetRawValue(canData);
            }
            break;
        case 0x98FB814A: {
            constexpr auto message = canID_0x98FB814A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_InhibitrawValue = message.OnBoard_Prgm_Initiate_Inhibit.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_RqstrawValue = message.OnBoard_Prgm_Initiate_Rqst.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Initiate_ECU_AddrrawValue = message.OnBoard_Prgm_Initiate_ECU_Addr.GetRawValue(canData);
            }
            break;
        case 0x98684A21: {
            constexpr auto message = canID_0x98684A21{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> OnBoard_Programming_ProgressrawValue = message.OnBoard_Programming_Progress.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Key_Switch_RequestrawValue = message.OBP_Key_Switch_Request.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Tgt_Not_ReadyrawValue = message.OBP_Interlock_Tgt_Not_Ready.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Pwr_Supply_InsuffrawValue = message.OBP_Interlock_Pwr_Supply_Insuff.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Park_BrakerawValue = message.OBP_Interlock_Park_Brake.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Key_Switch_PosrawValue = message.OBP_Interlock_Key_Switch_Pos.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Engine_SpeedrawValue = message.OBP_Interlock_Engine_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OBP_Interlock_Overall_StatusrawValue = message.OBP_Interlock_Overall_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Programming_StatusrawValue = message.OnBoard_Programming_Status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OnBoard_Prgm_Target_ECU_AddressrawValue = message.OnBoard_Prgm_Target_ECU_Address.GetRawValue(canData);
            }
            break;
        case 0x98FEDAFE: {
            constexpr auto message = canID_0x98FEDAFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> SoftwareIdentificationrawValue = message.SoftwareIdentification.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> NumberOfSoftwareIdFieldsrawValue = message.NumberOfSoftwareIdFields.GetRawValue(canData);
            }
            break;
        case 0x98FECA4A: {
            constexpr auto message = canID_0x98FECA4A{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> SPNConversionMethodrawValue = message.SPNConversionMethod.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> OccurrenceCountrawValue = message.OccurrenceCount.GetRawValue(canData);
            ara::core::Optional<FailureModeIdentifier> FailureModeIdentifierrawValue = message.FailureModeIdentifier.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> SuspectParameterNumberrawValue = message.SuspectParameterNumber.GetRawValue(canData);
            ara::core::Optional<FlashMalfuncIndicatorLamp> FlashMalfuncIndicatorLamprawValue = message.FlashMalfuncIndicatorLamp.GetRawValue(canData);
            ara::core::Optional<FlashRedStopLamp> FlashRedStopLamprawValue = message.FlashRedStopLamp.GetRawValue(canData);
            ara::core::Optional<FlashAmberWarningLamp> FlashAmberWarningLamprawValue = message.FlashAmberWarningLamp.GetRawValue(canData);
            ara::core::Optional<FlashProtectLamp> FlashProtectLamprawValue = message.FlashProtectLamp.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> MalfunctionIndicatorLampStatusrawValue = message.MalfunctionIndicatorLampStatus.GetRawValue(canData);
            ara::core::Optional<RedStopLampState> RedStopLampStaterawValue = message.RedStopLampState.GetRawValue(canData);
            ara::core::Optional<AmberWarningLampStatus> AmberWarningLampStatusrawValue = message.AmberWarningLampStatus.GetRawValue(canData);
            ara::core::Optional<ProtectLampStatus> ProtectLampStatusrawValue = message.ProtectLampStatus.GetRawValue(canData);
            }
            break;
        case 0x98DF4AFE: {
            constexpr auto message = canID_0x98DF4AFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> SuspendDurationrawValue = message.SuspendDuration.GetRawValue(canData);
            ara::core::Optional<HoldSignal> HoldSignalrawValue = message.HoldSignal.GetRawValue(canData);
            ara::core::Optional<SuspendSignal> SuspendSignalrawValue = message.SuspendSignal.GetRawValue(canData);
            ara::core::Optional<J1939Network3> J1939Network3rawValue = message.J1939Network3.GetRawValue(canData);
            ara::core::Optional<J1939Network2> J1939Network2rawValue = message.J1939Network2.GetRawValue(canData);
            ara::core::Optional<ISO9141> ISO9141rawValue = message.ISO9141.GetRawValue(canData);
            ara::core::Optional<SAE_J1850> SAE_J1850rawValue = message.SAE_J1850.GetRawValue(canData);
            ara::core::Optional<ManufacturerSpecificPort> ManufacturerSpecificPortrawValue = message.ManufacturerSpecificPort.GetRawValue(canData);
            ara::core::Optional<CurrentDataLink> CurrentDataLinkrawValue = message.CurrentDataLink.GetRawValue(canData);
            ara::core::Optional<SAE_J1587> SAE_J1587rawValue = message.SAE_J1587.GetRawValue(canData);
            ara::core::Optional<SAE_J1922> SAE_J1922rawValue = message.SAE_J1922.GetRawValue(canData);
            ara::core::Optional<J1939Network1> J1939Network1rawValue = message.J1939Network1.GetRawValue(canData);
            }
            break;
        case 0x98FE5600: {
            constexpr auto message = canID_0x98FE5600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Aft1DsllExhaustFluidTnk1Heater_0rawValue = message.Aft1DsllExhaustFluidTnk1Heater_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Aft1DslExhaustFluidTnk1TmpFmi_0rawValue = message.Aft1DslExhaustFluidTnk1TmpFmi_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> aft1deftankheater_0rawValue = message.aft1deftankheater_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> aft1deftanklevel_0rawValue = message.aft1deftanklevel_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> aft1deftanklevelpreliminaryfmi_0rawValue = message.aft1deftanklevelpreliminaryfmi_0.GetRawValue(canData);
            ara::core::Optional<SCR_Oper_Induc_Severity> SCR_Oper_Induc_SeverityrawValue = message.SCR_Oper_Induc_Severity.GetRawValue(canData);
            ara::core::Optional<SCR_Oper_Induc_Act> SCR_Oper_Induc_ActrawValue = message.SCR_Oper_Induc_Act.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DEF_Tank_TemperaturerawValue = message.DEF_Tank_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DEF_Tank_Level_BrawValue = message.DEF_Tank_Level_B.GetRawValue(canData);
            }
            break;
        case 0x98FC2A00: {
            constexpr auto message = canID_0x98FC2A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> oi_torque_derate_l2rawValue = message.oi_torque_derate_l2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> oi_torque_derate_l1rawValue = message.oi_torque_derate_l1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> OI_Time_Trq_Derate_L2rawValue = message.OI_Time_Trq_Derate_L2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> OI_Time_Trq_Derate_L1rawValue = message.OI_Time_Trq_Derate_L1.GetRawValue(canData);
            }
            break;
        case 0x90FDA300: {
            constexpr auto message = canID_0x90FDA300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> VGT_posn_desrawValue = message.VGT_posn_des.GetRawValue(canData);
            }
            break;
        case 0x98FD9F00: {
            constexpr auto message = canID_0x98FD9F00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<AFT1_Purge_Air_Actuator> AFT1_Purge_Air_ActuatorrawValue = message.AFT1_Purge_Air_Actuator.GetRawValue(canData);
            }
            break;
        case 0x98FD7900: {
            constexpr auto message = canID_0x98FD7900{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Trip_No_Of_Active_Regens_BrawValue = message.Trip_No_Of_Active_Regens_B.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_Act_Regen_Time_BrawValue = message.Trip_Act_Regen_Time_B.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_Fuel_UsedrawValue = message.Trip_Fuel_Used.GetRawValue(canData);
            }
            break;
        case 0x94FD3E00: {
            constexpr auto message = canID_0x94FD3E00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Catalyst_Outlet_Gas_TemprawValue = message.Catalyst_Outlet_Gas_Temp.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Catalyst_Intake_Gas_TemprawValue = message.Catalyst_Intake_Gas_Temp.GetRawValue(canData);
            }
            break;
        case 0x98FD9B00: {
            constexpr auto message = canID_0x98FD9B00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Aft1DieselExhaustFluidTemp2_0rawValue = message.Aft1DieselExhaustFluidTemp2_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DEF_ConcentrationrawValue = message.DEF_Concentration.GetRawValue(canData);
            }
            break;
        case 0x98FC9600: {
            constexpr auto message = canID_0x98FC9600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> engineoilfilterintakepresshighrawValue = message.engineoilfilterintakepresshigh.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Exhaust_Back_Pressure_ExtRangerawValue = message.Exhaust_Back_Pressure_ExtRange.GetRawValue(canData);
            }
            break;
        case 0x9CFEB000: {
            constexpr auto message = canID_0x9CFEB000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Trip_Engine_Idle_TimerawValue = message.Trip_Engine_Idle_Time.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_Engine_Run_TimerawValue = message.Trip_Engine_Run_Time.GetRawValue(canData);
            }
            break;
        case 0x9CFEB900: {
            constexpr auto message = canID_0x9CFEB900{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> TripDriveFuelEconomy_4A_00rawValue = message.TripDriveFuelEconomy_4A_00.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> TripCruiseFuelUsed_4A_00rawValue = message.TripCruiseFuelUsed_4A_00.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> trip_drive_fuel_usedrawValue = message.trip_drive_fuel_used.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_Vehicle_Idle_Fuel_UsedrawValue = message.Trip_Vehicle_Idle_Fuel_Used.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_PTO_Gov_NonMove_Fuel_UsedrawValue = message.Trip_PTO_Gov_NonMove_Fuel_Used.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Trip_PTO_Gov_Move_Fuel_UsedrawValue = message.Trip_PTO_Gov_Move_Fuel_Used.GetRawValue(canData);
            }
            break;
        case 0x98F00F00: {
            constexpr auto message = canID_0x98F00F00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> aft1_outlet_percent_oxygen1rawValue = message.aft1_outlet_percent_oxygen1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> NOx_Out_ConcentrationrawValue = message.NOx_Out_Concentration.GetRawValue(canData);
            }
            break;
        case 0x98FEFF00: {
            constexpr auto message = canID_0x98FEFF00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> OilFilterLife_00rawValue = message.OilFilterLife_00.GetRawValue(canData);
            ara::core::Optional<Water_In_Fuel> Water_In_FuelrawValue = message.Water_In_Fuel.GetRawValue(canData);
            }
            break;
        case 0x98FEF000: {
            constexpr auto message = canID_0x98FEF000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> power_take_off_set_speedrawValue = message.power_take_off_set_speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> power_takeoff_oil_temperaturerawValue = message.power_takeoff_oil_temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_pto_gov_disable_switchrawValue = message.engine_pto_gov_disable_switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> auxiliary_input_ignore_switchrawValue = message.auxiliary_input_ignore_switch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> remoteptogovpreprgmspeedswitch2rawValue = message.remoteptogovpreprgmspeedswitch2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> operatorengineptogovmemorySwitchrawValue = message.operatorengineptogovmemorySwitch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engineptogovaccelerateswitchrawValue = message.engineptogovaccelerateswitch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engineptogovernorresumeswitchrawValue = message.engineptogovernorresumeswitch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engineptogovcoastdecelswitchrawValue = message.engineptogovcoastdecelswitch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_pto_governor_set_switchrawValue = message.engine_pto_governor_set_switch.GetRawValue(canData);
            ara::core::Optional<Remote_PTO_Speed_Cntrl_Switch_A> Remote_PTO_Speed_Cntrl_Switch_ArawValue = message.Remote_PTO_Speed_Cntrl_Switch_A.GetRawValue(canData);
            ara::core::Optional<PTO_Gov_Pre_Spd_Cntrl_Switch_A> PTO_Gov_Pre_Spd_Cntrl_Switch_ArawValue = message.PTO_Gov_Pre_Spd_Cntrl_Switch_A.GetRawValue(canData);
            ara::core::Optional<PTO_Enable_Switch> PTO_Enable_SwitchrawValue = message.PTO_Enable_Switch.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Power_Take_Off_Speed_0rawValue = message.Power_Take_Off_Speed_0.GetRawValue(canData);
            }
            break;
        case 0x98FEE500: {
            constexpr auto message = canID_0x98FEE500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Engine_Total_RevolutionsrawValue = message.Engine_Total_Revolutions.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Engine_Hours_ArawValue = message.Total_Engine_Hours_A.GetRawValue(canData);
            }
            break;
        case 0x98FD9400: {
            constexpr auto message = canID_0x98FD9400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> CC_Oil_Separator_SpeedrawValue = message.CC_Oil_Separator_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EGR_Position_ArawValue = message.EGR_Position_A.GetRawValue(canData);
            }
            break;
        case 0x98FD7C00: {
            constexpr auto message = canID_0x98FD7C00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<regeninhibitduetotresher> regeninhibitduetotresherrawValue = message.regeninhibitduetotresher.GetRawValue(canData);
            ara::core::Optional<filterconditionnotmetactiveregen> filterconditionnotmetactiveregenrawValue = message.filterconditionnotmetactiveregen.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> regennhibitduelowexhaustpressurerawValue = message.regennhibitduelowexhaustpressure.GetRawValue(canData);
            ara::core::Optional<hydrocarbondoserpurgingenable> hydrocarbondoserpurgingenablerawValue = message.hydrocarbondoserpurgingenable.GetRawValue(canData);
            ara::core::Optional<regenforcedstatus> regenforcedstatusrawValue = message.regenforcedstatus.GetRawValue(canData);
            ara::core::Optional<regeninitialconfiguration> regeninitialconfigurationrawValue = message.regeninitialconfiguration.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> regeninhibitduevehiclespeedbelowrawValue = message.regeninhibitduevehiclespeedbelow.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> regeninhibitduetopermanentsystemrawValue = message.regeninhibitduetopermanentsystem.GetRawValue(canData);
            ara::core::Optional<regeninhibitduetotemporarysystem> regeninhibitduetotemporarysystemrawValue = message.regeninhibitduetotemporarysystem.GetRawValue(canData);
            ara::core::Optional<regeninhibitedduetosystemtimeout> regeninhibitedduetosystemtimeoutrawValue = message.regeninhibitedduetosystemtimeout.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> passive_regeneration_statusrawValue = message.passive_regeneration_status.GetRawValue(canData);
            ara::core::Optional<regen_availability_status> regen_availability_statusrawValue = message.regen_availability_status.GetRawValue(canData);
            ara::core::Optional<High_Exhaust_Temp_Lamp> High_Exhaust_Temp_LamprawValue = message.High_Exhaust_Temp_Lamp.GetRawValue(canData);
            ara::core::Optional<Reg_Inh_Eng_Not_Warm_Up> Reg_Inh_Eng_Not_Warm_UprawValue = message.Reg_Inh_Eng_Not_Warm_Up.GetRawValue(canData);
            ara::core::Optional<Reg_Inh_Sys_Fault_Active> Reg_Inh_Sys_Fault_ActiverawValue = message.Reg_Inh_Sys_Fault_Active.GetRawValue(canData);
            ara::core::Optional<Reg_Inh_Low_Exhaust_Gas_Temp> Reg_Inh_Low_Exhaust_Gas_TemprawValue = message.Reg_Inh_Low_Exhaust_Gas_Temp.GetRawValue(canData);
            ara::core::Optional<Reg_Inh_Prk_Brk_Not_Set> Reg_Inh_Prk_Brk_Not_SetrawValue = message.Reg_Inh_Prk_Brk_Not_Set.GetRawValue(canData);
            ara::core::Optional<Reg_Inh_Veh_Speed_Too_High> Reg_Inh_Veh_Speed_Too_HighrawValue = message.Reg_Inh_Veh_Speed_Too_High.GetRawValue(canData);
            ara::core::Optional<Reg_Inhibit_Out_of_Neutral> Reg_Inhibit_Out_of_NeutralrawValue = message.Reg_Inhibit_Out_of_Neutral.GetRawValue(canData);
            ara::core::Optional<Reg_Inhibit_Accel_Off_Idle> Reg_Inhibit_Accel_Off_IdlerawValue = message.Reg_Inhibit_Accel_Off_Idle.GetRawValue(canData);
            ara::core::Optional<Regen_Inhibit_PTO_Active> Regen_Inhibit_PTO_ActiverawValue = message.Regen_Inhibit_PTO_Active.GetRawValue(canData);
            ara::core::Optional<Reg_Inh_Serv_Brake_Active> Reg_Inh_Serv_Brake_ActiverawValue = message.Reg_Inh_Serv_Brake_Active.GetRawValue(canData);
            ara::core::Optional<Reg_Inhibit_Clutch_Diseng> Reg_Inhibit_Clutch_DisengrawValue = message.Reg_Inhibit_Clutch_Diseng.GetRawValue(canData);
            ara::core::Optional<Reg_Inhibit_Inhibit_Switch> Reg_Inhibit_Inhibit_SwitchrawValue = message.Reg_Inhibit_Inhibit_Switch.GetRawValue(canData);
            ara::core::Optional<Regen_Inhibit_Status> Regen_Inhibit_StatusrawValue = message.Regen_Inhibit_Status.GetRawValue(canData);
            ara::core::Optional<DPF_Status_B> DPF_Status_BrawValue = message.DPF_Status_B.GetRawValue(canData);
            ara::core::Optional<Regen_Status> Regen_StatusrawValue = message.Regen_Status.GetRawValue(canData);
            ara::core::Optional<DPF_Lamp_Command> DPF_Lamp_CommandrawValue = message.DPF_Lamp_Command.GetRawValue(canData);
            }
            break;
        case 0x98FEE900: {
            constexpr auto message = canID_0x98FEE900{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Total_Engine_Fuel_UsedrawValue = message.Total_Engine_Fuel_Used.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Engine_Trip_FuelrawValue = message.Engine_Trip_Fuel.GetRawValue(canData);
            }
            break;
        case 0x98FEF700: {
            constexpr auto message = canID_0x98FEF700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> charging_system_potential_0rawValue = message.charging_system_potential_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> sli_battery1_net_current_0rawValue = message.sli_battery1_net_current_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> alternator_current_0rawValue = message.alternator_current_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Keyswitch_Battery_PotentialrawValue = message.Keyswitch_Battery_Potential.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Battery_PotentialrawValue = message.Battery_Potential.GetRawValue(canData);
            }
            break;
        case 0x98FEF200: {
            constexpr auto message = canID_0x98FEF200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> engine_throttle_valve2_positionrawValue = message.engine_throttle_valve2_position.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_throttle_valve1_position1rawValue = message.engine_throttle_valve1_position1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Average_Fuel_EconomyrawValue = message.Average_Fuel_Economy.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Instantaneous_Fuel_EconomyrawValue = message.Instantaneous_Fuel_Economy.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Fuel_Rate_BrawValue = message.Engine_Fuel_Rate_B.GetRawValue(canData);
            }
            break;
        case 0x98FECAFE: {
            constexpr auto message = canID_0x98FECAFE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> SPN2High_DM1rawValue = message.SPN2High_DM1.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SPN1High_DM1rawValue = message.SPN1High_DM1.GetRawValue(canData);
            }
            break;
        case 0x98FEDC00: {
            constexpr auto message = canID_0x98FEDC00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Total_Idle_HoursrawValue = message.Total_Idle_Hours.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Idle_Fuel_UsedrawValue = message.Total_Idle_Fuel_Used.GetRawValue(canData);
            }
            break;
        case 0x98FEF600: {
            constexpr auto message = canID_0x98FEF600{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> engine_airfilter_1_diff_pressurerawValue = message.engine_airfilter_1_diff_pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> enginecoolantfilterdiffpressrawValue = message.enginecoolantfilterdiffpress.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Exhaust_Gas_TemperaturerawValue = message.Exhaust_Gas_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_intake_air_pressurerawValue = message.engine_intake_air_pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Intake_Manifold_TemperaturerawValue = message.Intake_Manifold_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Intake_Manifold_PressurerawValue = message.Intake_Manifold_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DPF_Inlet_PressurerawValue = message.DPF_Inlet_Pressure.GetRawValue(canData);
            }
            break;
        case 0x98FD9800: {
            constexpr auto message = canID_0x98FD9800{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Avg_Time_Bet_Act_DPF_RegrawValue = message.Avg_Time_Bet_Act_DPF_Reg.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Tot_No_Of_Act_Reg_Man_ReqsrawValue = message.Tot_No_Of_Act_Reg_Man_Reqs.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Tot_No_Of_Act_Reg_Inh_ReqsrawValue = message.Tot_No_Of_Act_Reg_Inh_Reqs.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_No_Of_Active_RegensrawValue = message.Total_No_Of_Active_Regens.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Disabled_TimerawValue = message.Total_Disabled_Time.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Regen_TimerawValue = message.Total_Regen_Time.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Fuel_UsedrawValue = message.Total_Fuel_Used.GetRawValue(canData);
            }
            break;
        case 0x98FDB800: {
            constexpr auto message = canID_0x98FDB800{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<NHMCCnvrtctlystMonComplete_DM26> NHMCCnvrtctlystMonComplete_DM26rawValue = message.NHMCCnvrtctlystMonComplete_DM26.GetRawValue(canData);
            ara::core::Optional<NOxCnvrtctlystMonComplete_DM26> NOxCnvrtctlystMonComplete_DM26rawValue = message.NOxCnvrtctlystMonComplete_DM26.GetRawValue(canData);
            ara::core::Optional<PMFilterMonComplete_DM26> PMFilterMonComplete_DM26rawValue = message.PMFilterMonComplete_DM26.GetRawValue(canData);
            ara::core::Optional<BstPressCtrlSystemMonComp_DM26> BstPressCtrlSystemMonComp_DM26rawValue = message.BstPressCtrlSystemMonComp_DM26.GetRawValue(canData);
            ara::core::Optional<ColdStartAidSystemMonComp_DM26> ColdStartAidSystemMonComp_DM26rawValue = message.ColdStartAidSystemMonComp_DM26.GetRawValue(canData);
            ara::core::Optional<EGRSystemMonitoringComplete_DM26> EGRSystemMonitoringComplete_DM26rawValue = message.EGRSystemMonitoringComplete_DM26.GetRawValue(canData);
            ara::core::Optional<OxygenSensorHeaterMonComp_DM26> OxygenSensorHeaterMonComp_DM26rawValue = message.OxygenSensorHeaterMonComp_DM26.GetRawValue(canData);
            ara::core::Optional<OxygenSensorMonComplete_DM26> OxygenSensorMonComplete_DM26rawValue = message.OxygenSensorMonComplete_DM26.GetRawValue(canData);
            ara::core::Optional<ACSystemRefrigerantMonComp_DM26> ACSystemRefrigerantMonComp_DM26rawValue = message.ACSystemRefrigerantMonComp_DM26.GetRawValue(canData);
            ara::core::Optional<SecondAirSystemMonComplete_DM26> SecondAirSystemMonComplete_DM26rawValue = message.SecondAirSystemMonComplete_DM26.GetRawValue(canData);
            ara::core::Optional<EvaporativeSystemMonComp_DM26> EvaporativeSystemMonComp_DM26rawValue = message.EvaporativeSystemMonComp_DM26.GetRawValue(canData);
            ara::core::Optional<HeatedCatalystMonComplete_DM26> HeatedCatalystMonComplete_DM26rawValue = message.HeatedCatalystMonComplete_DM26.GetRawValue(canData);
            ara::core::Optional<CatalystMonComplete_DM26> CatalystMonComplete_DM26rawValue = message.CatalystMonComplete_DM26.GetRawValue(canData);
            ara::core::Optional<NHMCCnvrtctlystMonEnabled_DM26> NHMCCnvrtctlystMonEnabled_DM26rawValue = message.NHMCCnvrtctlystMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<NOxCnvrtctlystMonEnabled_DM26> NOxCnvrtctlystMonEnabled_DM26rawValue = message.NOxCnvrtctlystMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<PMFilterMonEnabled_DM26> PMFilterMonEnabled_DM26rawValue = message.PMFilterMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<BstPressCtrlSysMonEnabled_DM26> BstPressCtrlSysMonEnabled_DM26rawValue = message.BstPressCtrlSysMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<ColdStartAidSysMonEnabled_DM26> ColdStartAidSysMonEnabled_DM26rawValue = message.ColdStartAidSysMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<EGRSystemMonitoringEnabled_DM26> EGRSystemMonitoringEnabled_DM26rawValue = message.EGRSystemMonitoringEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<OxygenSensorHtrMonEnabled_DM26> OxygenSensorHtrMonEnabled_DM26rawValue = message.OxygenSensorHtrMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<OxygenSensorMonEnabled_DM26> OxygenSensorMonEnabled_DM26rawValue = message.OxygenSensorMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<ACSystemRefrigMonEnabled_DM26> ACSystemRefrigMonEnabled_DM26rawValue = message.ACSystemRefrigMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<SecondAirSystemMonEnabled_DM26> SecondAirSystemMonEnabled_DM26rawValue = message.SecondAirSystemMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<EvaporativeSystemMonEnabled_DM26> EvaporativeSystemMonEnabled_DM26rawValue = message.EvaporativeSystemMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<HeatedCatalystMonEnabled_DM26> HeatedCatalystMonEnabled_DM26rawValue = message.HeatedCatalystMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<CatalystMonEnabled_DM26> CatalystMonEnabled_DM26rawValue = message.CatalystMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<ComprehensiveComptMonComp_DM26> ComprehensiveComptMonComp_DM26rawValue = message.ComprehensiveComptMonComp_DM26.GetRawValue(canData);
            ara::core::Optional<FuelSystemMonitoringComp_DM26> FuelSystemMonitoringComp_DM26rawValue = message.FuelSystemMonitoringComp_DM26.GetRawValue(canData);
            ara::core::Optional<MisfireMonitoringComplete_DM26> MisfireMonitoringComplete_DM26rawValue = message.MisfireMonitoringComplete_DM26.GetRawValue(canData);
            ara::core::Optional<ComprehensiveCompMonEnabled_DM26> ComprehensiveCompMonEnabled_DM26rawValue = message.ComprehensiveCompMonEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<FuelSystemMonitoringEnabled_DM26> FuelSystemMonitoringEnabled_DM26rawValue = message.FuelSystemMonitoringEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<MisfireMonitoringEnabled_DM26> MisfireMonitoringEnabled_DM26rawValue = message.MisfireMonitoringEnabled_DM26.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> NumOfWarmUpsSinceDTCsClear_DM26rawValue = message.NumOfWarmUpsSinceDTCsClear_DM26.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TimeSinceEngineStart_DM26rawValue = message.TimeSinceEngineStart_DM26.GetRawValue(canData);
            }
            break;
        case 0x98FEE700: {
            constexpr auto message = canID_0x98FEE700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> total_vehicle_hours_0rawValue = message.total_vehicle_hours_0.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Power_Take_Off_HoursrawValue = message.Total_Power_Take_Off_Hours.GetRawValue(canData);
            }
            break;
        case 0x98FEC117: {
            constexpr auto message = canID_0x98FEC117{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Trip_Distance_HRrawValue = message.Trip_Distance_HR.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Vehicle_Distance_HRrawValue = message.Total_Vehicle_Distance_HR.GetRawValue(canData);
            }
            break;
        case 0x98FEE400: {
            constexpr auto message = canID_0x98FEE400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> ac_highpressure_fan_switchrawValue = message.ac_highpressure_fan_switch.GetRawValue(canData);
            ara::core::Optional<EWPS_Configuration> EWPS_ConfigurationrawValue = message.EWPS_Configuration.GetRawValue(canData);
            ara::core::Optional<EWPS_State> EWPS_StaterawValue = message.EWPS_State.GetRawValue(canData);
            ara::core::Optional<EWPS_Timer_Override> EWPS_Timer_OverriderawValue = message.EWPS_Timer_Override.GetRawValue(canData);
            ara::core::Optional<EWPS_Approaching_Shutdown> EWPS_Approaching_ShutdownrawValue = message.EWPS_Approaching_Shutdown.GetRawValue(canData);
            ara::core::Optional<EWPS_Has_Shutdown_Engine> EWPS_Has_Shutdown_EnginerawValue = message.EWPS_Has_Shutdown_Engine.GetRawValue(canData);
            ara::core::Optional<Wait_To_Start_Lamp_A> Wait_To_Start_Lamp_ArawValue = message.Wait_To_Start_Lamp_A.GetRawValue(canData);
            ara::core::Optional<IST_Timer_Function> IST_Timer_FunctionrawValue = message.IST_Timer_Function.GetRawValue(canData);
            ara::core::Optional<IST_Timer_State> IST_Timer_StaterawValue = message.IST_Timer_State.GetRawValue(canData);
            ara::core::Optional<IST_Timer_Override> IST_Timer_OverriderawValue = message.IST_Timer_Override.GetRawValue(canData);
            ara::core::Optional<IST_Drive_Alert_Mode> IST_Drive_Alert_ModerawValue = message.IST_Drive_Alert_Mode.GetRawValue(canData);
            ara::core::Optional<IST_Has_Shutdown_Engine> IST_Has_Shutdown_EnginerawValue = message.IST_Has_Shutdown_Engine.GetRawValue(canData);
            }
            break;
        case 0x9CFEB300: {
            constexpr auto message = canID_0x9CFEB300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Total_PTO_Fuel_UsedrawValue = message.Total_PTO_Fuel_Used.GetRawValue(canData);
            }
            break;
        case 0x98FEBD00: {
            constexpr auto message = canID_0x98FEBD00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Engine_Fan_SpeedrawValue = message.Engine_Fan_Speed.GetRawValue(canData);
            ara::core::Optional<Engine_Fan_State> Engine_Fan_StaterawValue = message.Engine_Fan_State.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Estimated_Percent_Fan_SpeedrawValue = message.Estimated_Percent_Fan_Speed.GetRawValue(canData);
            }
            break;
        case 0x98FEA400: {
            constexpr auto message = canID_0x98FEA400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> engine_exhaustgas_diffpressrawValue = message.engine_exhaustgas_diffpress.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EGR_TemperaturerawValue = message.EGR_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Oil_Temperature_2rawValue = message.Engine_Oil_Temperature_2.GetRawValue(canData);
            }
            break;
        case 0x98FEEE00: {
            constexpr auto message = canID_0x98FEEE00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> engchargeaircoolerthermostat_0rawValue = message.engchargeaircoolerthermostat_0.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engineintercoolertempe_0rawValue = message.engineintercoolertempe_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> engineturbocharger1oiltemp_0rawValue = message.engineturbocharger1oiltemp_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Oil_TemperaturerawValue = message.Engine_Oil_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Fuel_TemperaturerawValue = message.Engine_Fuel_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Coolant_TemperaturerawValue = message.Engine_Coolant_Temperature.GetRawValue(canData);
            }
            break;
        case 0x9CFEB100: {
            constexpr auto message = canID_0x9CFEB100{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> Total_ECM_Run_TimerawValue = message.Total_ECM_Run_Time.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> Total_Vehicle_Distance_ECM_ArawValue = message.Total_Vehicle_Distance_ECM_A.GetRawValue(canData);
            }
            break;
        case 0x8CF00A00: {
            constexpr auto message = canID_0x8CF00A00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Eng_exhaust_gas_massflow_raterawValue = message.Eng_exhaust_gas_massflow_rate.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Mass_Air_Flow_ArawValue = message.Mass_Air_Flow_A.GetRawValue(canData);
            }
            break;
        case 0x98FEFC21: {
            constexpr auto message = canID_0x98FEFC21{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> cargo_ambient_temperature_21rawValue = message.cargo_ambient_temperature_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> fuellevel2_21rawValue = message.fuellevel2_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_fuel_filter_diff_press_21rawValue = message.engine_fuel_filter_diff_press_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_oil_filter_diff_press1_21rawValue = message.engine_oil_filter_diff_press1_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> washer_fluid_level_21rawValue = message.washer_fluid_level_21.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> engine_oil_filter_diff_pressrawValue = message.engine_oil_filter_diff_press.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Fuel_LevelrawValue = message.Fuel_Level.GetRawValue(canData);
            }
            break;
        case 0x98FD2000: {
            constexpr auto message = canID_0x98FD2000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> DOC_Outlet_Temperature_ArawValue = message.DOC_Outlet_Temperature_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DOC_Inlet_Temperature_CrawValue = message.DOC_Inlet_Temperature_C.GetRawValue(canData);
            }
            break;
        case 0x98FD7B00: {
            constexpr auto message = canID_0x98FD7B00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> DPF_Soot_Load_Regen_Thresh_ArawValue = message.DPF_Soot_Load_Regen_Thresh_A.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> DPF_Time_Since_Last_Act_Regen_ArawValue = message.DPF_Time_Since_Last_Act_Regen_A.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DPF_Ash_LoadrawValue = message.DPF_Ash_Load.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> DPF_Soot_LoadrawValue = message.DPF_Soot_Load.GetRawValue(canData);
            }
            break;
        case 0x98FD8C00: {
            constexpr auto message = canID_0x98FD8C00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> DPF_Outlet_Pressure_2rawValue = message.DPF_Outlet_Pressure_2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DPF_Inlet_Pressure_1rawValue = message.DPF_Inlet_Pressure_1.GetRawValue(canData);
            }
            break;
        case 0x8CFD9200: {
            constexpr auto message = canID_0x8CFD9200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<engine_controlld_shutdwn_request> engine_controlld_shutdwn_requestrawValue = message.engine_controlld_shutdwn_request.GetRawValue(canData);
            ara::core::Optional<engine_derate_request> engine_derate_requestrawValue = message.engine_derate_request.GetRawValue(canData);
            ara::core::Optional<Engine_Operating_State> Engine_Operating_StaterawValue = message.Engine_Operating_State.GetRawValue(canData);
            }
            break;
        case 0x98F00E00: {
            constexpr auto message = canID_0x98F00E00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<O2_Heater_A> O2_Heater_ArawValue = message.O2_Heater_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AFT_O2_Inlet_PercentrawValue = message.AFT_O2_Inlet_Percent.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> NOx_Inlet_Concentration_DrawValue = message.NOx_Inlet_Concentration_D.GetRawValue(canData);
            }
            break;
        case 0x90FCFD00: {
            constexpr auto message = canID_0x90FCFD00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Fuel_Rail_Pressure_DesiredrawValue = message.Fuel_Rail_Pressure_Desired.GetRawValue(canData);
            }
            break;
        case 0x98FEEF00: {
            constexpr auto message = canID_0x98FEEF00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> eng_extended_crankcase_pressurerawValue = message.eng_extended_crankcase_pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Coolant_LevelrawValue = message.Engine_Coolant_Level.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Coolant_PressurerawValue = message.Engine_Coolant_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Crankcase_PressurerawValue = message.Engine_Crankcase_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Oil_PressurerawValue = message.Engine_Oil_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Oil_LevelrawValue = message.Engine_Oil_Level.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Fuel_Delivery_PressurerawValue = message.Fuel_Delivery_Pressure.GetRawValue(canData);
            }
            break;
        case 0x98FDD500: {
            constexpr auto message = canID_0x98FDD500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> VGT_Actuator_PosrawValue = message.VGT_Actuator_Pos.GetRawValue(canData);
            ara::core::Optional<VGT_Ctrl_Mode> VGT_Ctrl_ModerawValue = message.VGT_Ctrl_Mode.GetRawValue(canData);
            ara::core::Optional<Fuel_Control_System_Status> Fuel_Control_System_StatusrawValue = message.Fuel_Control_System_Status.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EGR_ControlrawValue = message.EGR_Control.GetRawValue(canData);
            }
            break;
        case 0x8CF00300: {
            constexpr auto message = canID_0x8CF00300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> estimatedpumpingpercentTorquerawValue = message.estimatedpumpingpercentTorque.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> scr_thermal_management_activerawValue = message.scr_thermal_management_active.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> dpf_thermal_management_activerawValue = message.dpf_thermal_management_active.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> momentaryengmaxpowerenablerawValue = message.momentaryengmaxpowerenable.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> acceleratorpedal2lowIdleswitchrawValue = message.acceleratorpedal2lowIdleswitch.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> accelerator_pedal_position_2rawValue = message.accelerator_pedal_position_2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> veh_accele_rate_limit_statusrawValue = message.veh_accele_rate_limit_status.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Actual_Max_Avg_Eng_Perc_TorquerawValue = message.Actual_Max_Avg_Eng_Perc_Torque.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Remote_Accel_Pedal_PositionrawValue = message.Remote_Accel_Pedal_Position.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_LoadrawValue = message.Engine_Load.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Accelerator_Pedal_Position_1rawValue = message.Accelerator_Pedal_Position_1.GetRawValue(canData);
            ara::core::Optional<Road_Speed_Limit_Status> Road_Speed_Limit_StatusrawValue = message.Road_Speed_Limit_Status.GetRawValue(canData);
            ara::core::Optional<Accel_Ped_Kickdown_Switch> Accel_Ped_Kickdown_SwitchrawValue = message.Accel_Ped_Kickdown_Switch.GetRawValue(canData);
            ara::core::Optional<Accel_Pedal_Low_Idle_Switch> Accel_Pedal_Low_Idle_SwitchrawValue = message.Accel_Pedal_Low_Idle_Switch.GetRawValue(canData);
            }
            break;
        case 0x98FEED00: {
            constexpr auto message = canID_0x98FEED00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Cruise_Cntrl_Low_Set_Lim_SpeedrawValue = message.Cruise_Cntrl_Low_Set_Lim_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Cruise_Cntrl_High_Set_Lim_SpeedrawValue = message.Cruise_Cntrl_High_Set_Lim_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Maximum_Vehicle_Speed_LimitrawValue = message.Maximum_Vehicle_Speed_Limit.GetRawValue(canData);
            }
            break;
        case 0x98FE7011: {
            constexpr auto message = canID_0x98FE7011{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Comb_Veh_WeightrawValue = message.Comb_Veh_Weight.GetRawValue(canData);
            }
            break;
        case 0x8CF0D311: {
            constexpr auto message = canID_0x8CF0D311{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> RoadwayNextGradeDistancerawValue = message.RoadwayNextGradeDistance.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PCCNextGraderawValue = message.PCCNextGrade.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PCCPresentGraderawValue = message.PCCPresentGrade.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PCCStaterawValue = message.PCCState.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> PCCSetSpeedOffsetrawValue = message.PCCSetSpeedOffset.GetRawValue(canData);
            }
            break;
        case 0x98851100: {
            constexpr auto message = canID_0x98851100{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> PCCMaximumNegativeOffsetrawValue = message.PCCMaximumNegativeOffset.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PCCMaximumPositiveOffsetrawValue = message.PCCMaximumPositiveOffset.GetRawValue(canData);
            }
            break;
        case 0x98FCDC00: {
            constexpr auto message = canID_0x98FCDC00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Cruise_Ctrl_Set_Speed_HRrawValue = message.Cruise_Ctrl_Set_Speed_HR.GetRawValue(canData);
            ara::core::Optional<PCCSetSpeedOffsetStatus> PCCSetSpeedOffsetStatusrawValue = message.PCCSetSpeedOffsetStatus.GetRawValue(canData);
            ara::core::Optional<Adaptive_Cruise_Ctrl_Ready_Sts> Adaptive_Cruise_Ctrl_Ready_StsrawValue = message.Adaptive_Cruise_Ctrl_Ready_Sts.GetRawValue(canData);
            }
            break;
        case 0x98F00503: {
            constexpr auto message = canID_0x98F00503{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Transmission_Requested_RangerawValue = message.Transmission_Requested_Range.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Transmission_Current_RangerawValue = message.Transmission_Current_Range.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TransmissionCurrentGearrawValue = message.TransmissionCurrentGear.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TransmissionActualGearRatiorawValue = message.TransmissionActualGearRatio.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Transmission_Selected_Gear_ArawValue = message.Transmission_Selected_Gear_A.GetRawValue(canData);
            }
            break;
        case 0x98F0000F: {
            constexpr auto message = canID_0x98F0000F{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Act_Max_Ava_Retarder_Perc_TorquerawValue = message.Act_Max_Ava_Retarder_Perc_Torque.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Drivers_Dmnd_Retd_Percent_TorquerawValue = message.Drivers_Dmnd_Retd_Percent_Torque.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Src_Add_Cont_Dev_Retd_ControlrawValue = message.Src_Add_Cont_Dev_Retd_Control.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Intended_Retarder_Percent_TorquerawValue = message.Intended_Retarder_Percent_Torque.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> RetarderPercentTorquerawValue = message.RetarderPercentTorque.GetRawValue(canData);
            ara::core::Optional<Ret_Enbl_Shift_Assist_Switch> Ret_Enbl_Shift_Assist_SwitchrawValue = message.Ret_Enbl_Shift_Assist_Switch.GetRawValue(canData);
            ara::core::Optional<Ret_Enbl_Brake_Assist_Switch> Ret_Enbl_Brake_Assist_SwitchrawValue = message.Ret_Enbl_Brake_Assist_Switch.GetRawValue(canData);
            ara::core::Optional<Retarder_Torque_Mode> Retarder_Torque_ModerawValue = message.Retarder_Torque_Mode.GetRawValue(canData);
            }
            break;
        case 0x98FEDF00: {
            constexpr auto message = canID_0x98FEDF00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Exhaust_Gas_Mass_FlowrawValue = message.Exhaust_Gas_Mass_Flow.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EstEngParLossPercentTorquerawValue = message.EstEngParLossPercentTorque.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Dsired_Oper_Speed_Assm_AdjrawValue = message.Eng_Dsired_Oper_Speed_Assm_Adj.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Desired_Operating_SpdrawValue = message.Engine_Desired_Operating_Spd.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TorqueNorminalFrictionPercentrawValue = message.TorqueNorminalFrictionPercent.GetRawValue(canData);
            }
            break;
        case 0x8CF00400: {
            constexpr auto message = canID_0x8CF00400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> actual_eng_percent_torque_fracrawValue = message.actual_eng_percent_torque_frac.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Engine_Demand_Percent_Torque_ArawValue = message.Engine_Demand_Percent_Torque_A.GetRawValue(canData);
            ara::core::Optional<Engine_Starter_Mode> Engine_Starter_ModerawValue = message.Engine_Starter_Mode.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Src_Add_Cont_Dev_Eng_Control_ArawValue = message.Src_Add_Cont_Dev_Eng_Control_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedrawValue = message.EngineSpeed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TorqueEngineActualPercentrawValue = message.TorqueEngineActualPercent.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TorqueDriversEngineDemandrawValue = message.TorqueDriversEngineDemand.GetRawValue(canData);
            ara::core::Optional<Engine_Torque_Mode> Engine_Torque_ModerawValue = message.Engine_Torque_Mode.GetRawValue(canData);
            }
            break;
        case 0x98FEE300: {
            constexpr auto message = canID_0x98FEE300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> Eng_Default_Idle_Tq_LimrawValue = message.Eng_Default_Idle_Tq_Lim.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Engine_Default_Torque_LimitrawValue = message.Engine_Default_Torque_Limit.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineMomentOfInertiarawValue = message.EngineMomentOfInertia.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Req_Trq_Cntrl_Rg_Up_LimrawValue = message.Eng_Req_Trq_Cntrl_Rg_Up_Lim.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Req_Trq_Cntrl_Rg_Low_LimrawValue = message.Eng_Req_Trq_Cntrl_Rg_Low_Lim.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Req_Spd_Cntrl_Rg_Up_LimrawValue = message.Eng_Req_Spd_Cntrl_Rg_Up_Lim.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Req_Spd_Cntrl_Rg_Low_LimrawValue = message.Eng_Req_Spd_Cntrl_Rg_Low_Lim.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Eng_Max_Mom_Over_Time_LimitrawValue = message.Eng_Max_Mom_Over_Time_Limit.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Max_Mom_Eng_Ov_Spd_At_Point_7_ArawValue = message.Max_Mom_Eng_Ov_Spd_At_Point_7_A.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> ReferenceEngineTorquerawValue = message.ReferenceEngineTorque.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Eng_Gain_Of_The_Endspeed_GovrawValue = message.Eng_Gain_Of_The_Endspeed_Gov.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedAtHighIdlePoint6rawValue = message.EngineSpeedAtHighIdlePoint6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PercentTorqueAtPoint5rawValue = message.PercentTorqueAtPoint5.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedAtPoint5rawValue = message.EngineSpeedAtPoint5.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PercentTorqueAtPoint4rawValue = message.PercentTorqueAtPoint4.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedAtPoint4rawValue = message.EngineSpeedAtPoint4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PercentTorqueAtPoint3rawValue = message.PercentTorqueAtPoint3.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedAtPoint3rawValue = message.EngineSpeedAtPoint3.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PercentTorqueAtPoint2rawValue = message.PercentTorqueAtPoint2.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedAtPoint2rawValue = message.EngineSpeedAtPoint2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> PercentTorqueAtPoint1rawValue = message.PercentTorqueAtPoint1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> EngineSpeedAtPoint1rawValue = message.EngineSpeedAtPoint1.GetRawValue(canData);
            }
            break;
*/

        case 0x98FEF500: {
            constexpr auto message = canID_0x98FEF500{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> road_surface_temperaturerawValue = message.road_surface_temperature.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> cab_interior_temperaturerawValue = message.cab_interior_temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Air_Intake_TemperaturerawValue = message.Air_Intake_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Ambient_Air_TemperaturerawValue = message.Ambient_Air_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Barometric_PressurerawValue = message.Barometric_Pressure.GetRawValue(canData);
            }
            break;
        case 0x98FDB400: {
            constexpr auto message = canID_0x98FDB400{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> aft1_exhaust_temperature1rawValue = message.aft1_exhaust_temperature1.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> DPF_Inlet_TemperaturerawValue = message.DPF_Inlet_Temperature.GetRawValue(canData);
            }
            break;
        case 0x98FDB200: {
            constexpr auto message = canID_0x98FDB200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> AFT_DPF_Diff_PressurerawValue = message.AFT_DPF_Diff_Pressure.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> aft1dpfintermediatetemperature_0rawValue = message.aft1dpfintermediatetemperature_0.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Exhaust_Gas_Temp_2rawValue = message.Exhaust_Gas_Temp_2.GetRawValue(canData);
            }
            break;
        case 0x98FDB300: {
            constexpr auto message = canID_0x98FDB300{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> DPF_Outlet_TemperaturerawValue = message.DPF_Outlet_Temperature.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> Exhaust_Gas_Temp_3rawValue = message.Exhaust_Gas_Temp_3.GetRawValue(canData);
            }
            break;
        case 0x98FEEC00: {
            constexpr auto message = canID_0x98FEEC00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            }
            break;
        case 0x98F0F611: {
            constexpr auto message = canID_0x98F0F611{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> eHorizonData_HrawValue = message.eHorizonData_H.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> eHorizonData_LrawValue = message.eHorizonData_L.GetRawValue(canData);
            }
            break;
        case 0x98FFC200: {
            constexpr auto message = canID_0x98FFC200{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> NGCCGenericSPNrawValue = message.NGCCGenericSPN.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TopGearNumberrawValue = message.TopGearNumber.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_18rawValue = message.GearRatio_18.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_17rawValue = message.GearRatio_17.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_16rawValue = message.GearRatio_16.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_15rawValue = message.GearRatio_15.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_14rawValue = message.GearRatio_14.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_13rawValue = message.GearRatio_13.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_12rawValue = message.GearRatio_12.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_11rawValue = message.GearRatio_11.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_10rawValue = message.GearRatio_10.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_09rawValue = message.GearRatio_09.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_08rawValue = message.GearRatio_08.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_07rawValue = message.GearRatio_07.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_06rawValue = message.GearRatio_06.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_05rawValue = message.GearRatio_05.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_04rawValue = message.GearRatio_04.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_03rawValue = message.GearRatio_03.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_02rawValue = message.GearRatio_02.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> GearRatio_01rawValue = message.GearRatio_01.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> TireRadiusrawValue = message.TireRadius.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> RearAxleRatioHighrawValue = message.RearAxleRatioHigh.GetRawValue(canData);
            }
            break;
        case 0x98FEEA21: {
            constexpr auto message = canID_0x98FEEA21{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> TrailerWeight_21rawValue = message.TrailerWeight_21.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> AxleWeightrawValue = message.AxleWeight.GetRawValue(canData);
            }
            break;
        case 0x98FE4A03: {
            constexpr auto message = canID_0x98FE4A03{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> activeshiftconsoleindicatorrawValue = message.activeshiftconsoleindicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> trans_air_supply_press_indicatorrawValue = message.trans_air_supply_press_indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TransmissionMode3IndicatorrawValue = message.TransmissionMode3Indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionmode8indicatorrawValue = message.transmissionmode8indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionmode7indicatorrawValue = message.transmissionmode7indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionmode5indicatorrawValue = message.transmissionmode5indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> transmissionmode6indicatorrawValue = message.transmissionmode6indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Transmission_Warning_IndicatorrawValue = message.Transmission_Warning_Indicator.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TransmissionMode1IndicatorrawValue = message.TransmissionMode1Indicator.GetRawValue(canData);
            }
            break;
        case 0x98EA11FE: {
            constexpr auto message = canID_0x98EA11FE{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> ParameterGroupNumber_RQST_RXrawValue = message.ParameterGroupNumber_RQST_RX.GetRawValue(canData);
            }
            break;
        case 0x8CFFC411: {
            constexpr auto message = canID_0x8CFFC411{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint32_t> NGCCFaultLatituderawValue = message.NGCCFaultLatitude.GetRawValue(canData);
            ara::core::Optional<std::uint32_t> NGCCFaultLongituderawValue = message.NGCCFaultLongitude.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> NGCCFaultNumberrawValue = message.NGCCFaultNumber.GetRawValue(canData);
            }
            break;
        case 0x98FCE700: {
            constexpr auto message = canID_0x98FCE700{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> EngineFrictionTorquePoint7rawValue = message.EngineFrictionTorquePoint7.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFrictionTorquePoint6rawValue = message.EngineFrictionTorquePoint6.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFrictionTorquePoint5rawValue = message.EngineFrictionTorquePoint5.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFrictionTorquePoint4rawValue = message.EngineFrictionTorquePoint4.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFrictionTorquePoint3rawValue = message.EngineFrictionTorquePoint3.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFrictionTorquePoint2rawValue = message.EngineFrictionTorquePoint2.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> EngineFrictionTorqueAtIdlePoint1rawValue = message.EngineFrictionTorqueAtIdlePoint1.GetRawValue(canData);
            }
            break;
        case 0x8CFFC111: {
            constexpr auto message = canID_0x8CFFC111{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> NGCCOptimizationGearNumberrawValue = message.NGCCOptimizationGearNumber.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> NeutralCoastStatusrawValue = message.NeutralCoastStatus.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> NGCCPresentSetSpeedOffsetrawValue = message.NGCCPresentSetSpeedOffset.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> NGCCAveragePercentGraderawValue = message.NGCCAveragePercentGrade.GetRawValue(canData);
            ara::core::Optional<std::uint16_t> NGCCFutureSetSpeedOffsetrawValue = message.NGCCFutureSetSpeedOffset.GetRawValue(canData);
            }
            break;
        case 0x98FFC000: {
            constexpr auto message = canID_0x98FFC000{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint8_t> NeutralCoastPPrawValue = message.NeutralCoastPP.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> TransmissionTyperawValue = message.TransmissionType.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> AccelDecelTimeOffsetrawValue = message.AccelDecelTimeOffset.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> FutureDistancerawValue = message.FutureDistance.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> SpeedTradeoffFactorrawValue = message.SpeedTradeoffFactor.GetRawValue(canData);
            }
            break;

        case 0x98FEBD00: {
            constexpr auto message = canID_0x98FEBD00{};
            std::cout << typeid(message).name() << std::endl;
            std::cout << "GetCanIdentifier: " << message.GetRawIdentifier() << std::endl;
            ara::core::Optional<std::uint16_t> Engine_Fan_SpeedrawValue = message.Engine_Fan_Speed.GetRawValue(canData);
            ara::core::Optional<std::uint8_t> Estimated_Percent_Fan_SpeedrawValue = message.Estimated_Percent_Fan_Speed.GetRawValue(canData);
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
    std::cout << "Creating application..." << std::endl;
    //std::string idstring = "0x9CFEBE00";
    //ara::core::Span<uint8_t> payload1 = 0xFF0CFDF0; // Example for EEC4_00
    //processCanMessage(idstring, payload1);
    //exit (1);

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
