#define CanMessage ::util::CanMessage
#define CanSignalBe ::util::CanSignalBe
#define CanSignalLe ::util::CanSignalLe

struct VECTOR__INDEPENDENT_SIG_MSG : public CanMessage<0xC0000000, 0> {
  enum class Engine_Starter_Mode : std::uint8_t {
    STARTNOTRQED = 0,
    STARTERACTIVEGEARNOTENGAGED = 1,
    STARTERACTIVEGEARENGAGED = 2,
    STARTFINISHED = 3,
    STARTERINHIBITEDDUEENGALREADYRUN = 4,
    STARTERINHIDUEENGNOTRDYSTART = 5,
    STARTERINHIDUEDRIVELINEENGAGED = 6,
    STARTERINHIDUEACTIVEIMMOBILIZER = 7,
    STARTERINHIDUESTARTEROVER_TEMP = 8,
    STRTINHIDUEINTAKEAIRSHUTVALVEACT = 9,
    STARTERINHIDUEACTIVESCRINDUC = 10,
    RESERVED = 11,
    SARTERINHIREASONUNKNOWN = 12,
    ERROR_1 = 13,
    ERROR_2 = 14,
    NOTAVAILABLE = 15,
  };

  enum class OtaTimerStatus_17 : std::uint8_t {
    TIMER_INACTIVE = 0,
    TIMER_ACTIVE = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class V2gStatusMode_5A : std::uint8_t {
    NOT_ACTIVE = 0,
    STAND_BY = 1,
    CHARGING = 2,
    V2G_DISCHARGE = 3,
    V2I_DISCHARGE = 4,
    RESERVED_1 = 5,
    RESERVED_2 = 6,
    RESERVED_3 = 7,
    RESERVED_4 = 8,
    RESERVED_5 = 9,
    RESERVED_6 = 10,
    RESERVED_7 = 11,
    RESERVED_8 = 12,
    RESERVED_9 = 13,
    ERROR = 14,
    NOT_AVAILABLE = 15,
  };

  enum class V2gAllowedPermissions_5A : std::uint8_t {
    NOT_ALLOWED = 0,
    ALLOWED = 1,
    STAND_BY = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Grid_V2X_Request_5A_4B : std::uint8_t {
    NOT_REQUESTED = 0,
    V2G_DISCHARGE_REQUEST = 1,
    V2G_CHARGE_REQUEST = 2,
    V2I_REQUEST = 3,
    RESERVED_1 = 4,
    RESERVED_2 = 5,
    RESERVED_3 = 6,
    RESERVED_4 = 7,
    RESERVED_5 = 8,
    RESERVED_6 = 9,
    RESERVED_7 = 10,
    RESERVED_8 = 11,
    RESERVED_9 = 12,
    RESERVED_10 = 13,
    ERROR = 14,
    NOT_AVAILABLE = 15,
  };

  enum class Grid_V2X_Confirmation_5A_4B : std::uint8_t {
    REJECTED = 0,
    ACCEPTED = 1,
    RESERVED = 2,
    NA = 3,
  };

  enum class GridV2xConfirmation_5A_4A : std::uint8_t {
    REJECTED = 0,
    ACCEPTED = 1,
    RESERVED = 2,
    NA = 3,
  };

  enum class GridV2xRequest_5A_4A : std::uint8_t {
    NOT_REQUESTED = 0,
    V2G_DISCHARGE_REQUEST = 1,
    V2G_CHARGE_REQUEST = 2,
    V2I_REQUEST = 3,
    RESERVED_1 = 4,
    RESERVED_2 = 5,
    RESERVED_3 = 6,
    RESERVED_4 = 7,
    RESERVED_5 = 8,
    RESERVED_6 = 9,
    RESERVED_7 = 10,
    RESERVED_8 = 11,
    RESERVED_9 = 12,
    RESERVED_10 = 13,
    ERROR = 14,
    NOT_AVAILABLE = 15,
  };

  enum class OvrdCtrlModePriority_7 : std::uint8_t {
    HIGHEST_PRIORITY = 0,
    HIGH_PRIORITY = 1,
    MEDIUM_PRIORITY = 2,
    LOW_PRIORITY = 3,
  };

  enum class EngRqstdSpdCtrlCondns_7 : std::uint8_t {
    TRANSIENT_DISENGAGED_NON_LOCKUP = 0,
    STABILITY_DISENGAGED_NON_LOCKUP = 1,
    STABILITY_ENGAGE_IN_LOCKUP_COND1 = 2,
    STABILITY_ENGAGE_IN_LOCKUP_COND2 = 3,
  };

  enum class EngOvrdCtrlMode_7 : std::uint8_t {
    OVERRIDE_DISABLED = 0,
    SPEED_CONTROL = 1,
    TORQUE_CONTROL = 2,
    SPEED_TORQUE_LIMIT_CONTROL = 3,
  };

  enum class AtleastOnePtoEngaged_3 : std::uint8_t {
    NO_PTO_DRIVE_IS_ENGAGED = 0,
    AT_LEAST_ONE_PTO_DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EngageMntcnstPtoEngFlyWheel_3 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EngagementStatusPtoEngFlyWheel_3 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntTrnsFrcsOtputShaftPto_3 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntTrnsFrcsOtptShaftPto_3 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntTransOutputShaftPto_3 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntTransInputShaftPto2_3 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntTransInputShaftPto1_3 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntSttsPtoAccessoryDrive2_3 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntSttsPtoAccessoryDrive1_3 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntStsTransOutputShaftPto_3 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntStsTransInputShaftPto2_3 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntStsTransInputShaftPto1_3 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntcnsntPtoAccssoryDrive2_3 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnggMntcnsntPtoAccssoryDrive1_3 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnblSwtchTrnscseOutputShftPto_3 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnbleSwitchTransOutputShftPto_3 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnbleSwitchTransInputShftPto1_3 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnbleSwitchPtoAccessoryDrive2_3 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnbleSwitchPtoAccessoryDrive1_3 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnableSwtchTransInputShftPto2_3 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class EnableSwitchPtoEngFlyWheel_3 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enblswtchtrnscseoutputshftpto_0 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enbleswitchtransoutputshftpto_0 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enableswtchtransinputshftpto2_0 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enbleswitchtransinputshftpto1_0 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enableswitchptoengflywheel_0 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enbleswitchptoaccessorydrive1_0 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enbleswitchptoaccessorydrive2_0 : std::uint8_t {
    ENABLE_SWITCH_OFF = 0,
    ENABLE_SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmnttrnsfrcsotptshaftpto_0 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmnttransoutputshaftpto_0 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmnttransinputshaftpto2_0 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmnttransinputshaftpto1_0 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engagemntcnstptoengflywheel_0 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmntcnsntptoaccssorydrive1_0 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmntcnsntptoaccssorydrive2_0 : std::uint8_t {
    CONSENT_NOT_GIVEN = 0,
    CONSENT_GIVEN = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmnttrnsfrcsotputshaftpto_0 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmntststransoutputshaftpto_0 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmntststransinputshaftpto2_0 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmntststransinputshaftpto1_0 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engagementstatusptoengflywheel_0 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmntsttsptoaccessorydrive1_0 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class enggmntsttsptoaccessorydrive2_0 : std::uint8_t {
    DRIVE_NOT_ENGAGED = 0,
    DRIVE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class atleastoneptoengaged_0 : std::uint8_t {
    NO_PTO_DRIVE_IS_ENGAGED = 0,
    AT_LEAST_ONE_PTO_DRIVE_IS_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engovrdctrlmode_0 : std::uint8_t {
    OVERRIDE_DISABLED = 0,
    SPEED_CONTROL = 1,
    TORQUE_CONTROL = 2,
    SPEED_TORQUE_LIMIT_CONTROL = 3,
  };

  enum class RunningLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class AltBeamHeadLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class LowBeamHeadLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class HighBeamHeadLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TractorFrontFogLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class RotatingBeaconLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class BackUpLightAndAlarmHornCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class CenterStopLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class Right_Stop_Light_Command : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class Left_Stop_Light_Command : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementClearanceLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TractorClearanceLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementMarkerLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TractorMarkerLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class RearFogLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TrctrUndrsdeMountedWorkLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TrctrRearLowMountedWorkLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TrctrRarHighMountedWorkLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TrctrSideLowMountedWorkLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TrctrSdeHighMountedWorkLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TrctrFrntLowMountedWorkLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class TrctrFrntHghMountedWorkLightsCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementOEMOption2LightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementOEMOption1LightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementRightFacingWorkLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementLeftFacingWorkLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class LightingDataRqCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplmentRightForwardWorkLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementLeftForwardWorkLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ImplementRearWorkLightCmd : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class transdrivelineengaged_03 : std::uint8_t {
    DRIVELINE_DISENGAGED = 0,
    DRIVELINE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class trnstrqconverterlockupengaged_03 : std::uint8_t {
    TORQUE_CONVERTER_LOCKUP_DISENGAGED = 0,
    TORQUE_CONVERTER_LOCKUP_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class transshiftinprocess_03 : std::uint8_t {
    SHIFT_IS_NOT_IN_PROCESS = 0,
    SHIFT_IN_PROCESS = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class transtorquelockupprocess_03 : std::uint8_t {
    TRANSITION_IS_NOT_IN_PROCESS = 0,
    TRANSITION_IS_IN_PROCESS = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engmomentaryoverspeedenable_03 : std::uint8_t {
    MOMENTARY_ENGINE_OVERSPEED_IS_DISABLED = 0,
    MOMENTARY_ENGINE_OVERSPEED_IS_ENABLED = 1,
    RESERVED = 2,
    TAKE_NO_ACTION = 3,
  };

  enum class progressiveshiftdisable_03 : std::uint8_t {
    PROGRESSIVE_SHIFT_IS_NOT_DISABLED = 0,
    PROGRESSIVE_SHIFT_IS_DISABLED = 1,
    RESERVED = 2,
    TAKE_NO_ACTION = 3,
  };

  enum class momentaryengmaxpowerenable_03 : std::uint8_t {
    NOT_REQUESTING_MAXIMUM_POWER_AVAILABLE = 0,
    MOMENTARILY_REQUESTING_MAXIMUM_POWER_AVAILABLE = 1,
    FAULT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class OvrdCtrlModePriority_0_24 : std::uint8_t {
    HIGHEST_PRIORITY = 0,
    HIGH_PRIORITY = 1,
    MEDIUM_PRIORITY = 2,
    LOW_PRIORITY = 3,
  };

  enum class EngRqstdSpdCtrlCondns_0_24 : std::uint8_t {
    TRANSIENT_DISENGAGED_NON_LOCKUP = 0,
    STABILITY_DISENGAGED_NON_LOCKUP = 1,
    STABILITY_ENGAGE_IN_LOCKUP_COND1 = 2,
    STABILITY_ENGAGE_IN_LOCKUP_COND2 = 3,
  };

  enum class EngOvrdCtrlMode_0_24 : std::uint8_t {
    OVERRIDE_DISABLED = 0,
    SPEED_CONTROL = 1,
    TORQUE_CONTROL = 2,
    SPEED_TORQUE_LIMIT_CONTROL = 3,
  };

  enum class TSC1_CtrlPurpose_F_B : std::uint8_t {
    P1_ACCPEDAL_OPERSELECTION = 0,
    P2_CRUISE_CONTROL = 1,
    P3_PTO_GOVERNOR = 2,
    P4_ROAD_SPEED_GOVERNOR = 3,
    P5_ENGINE_PROTECTION = 4,
    RESERVED_1 = 5,
    RESERVED_2 = 6,
    RESERVED_3 = 7,
    RESERVED_4 = 8,
    RESERVED_5 = 9,
    RESERVED_6 = 10,
    RESERVED_7 = 11,
    RESERVED_8 = 12,
    RESERVED_9 = 13,
    RESERVED_10 = 14,
    RESERVED_11 = 15,
    RESERVED_12 = 16,
    RESERVED_13 = 17,
    RESERVED_14 = 18,
    RESERVED_15 = 19,
    RESERVED_16 = 20,
    RESERVED_17 = 21,
    RESERVED_18 = 22,
    RESERVED_19 = 23,
    RESERVED_20 = 24,
    RESERVED_21 = 25,
    RESERVED_22 = 26,
    RESERVED_23 = 27,
    RESERVED_24 = 28,
    RESERVED_25 = 29,
    RESERVED_26 = 30,
    P32_TEMPORARYPOWERTRAINCONTROL = 31,
  };

  enum class OvrdCtrlModePriority_F_B : std::uint8_t {
    HIGHEST_PRIORITY = 0,
    HIGH_PRIORITY = 1,
    MEDIUM_PRIORITY = 2,
    LOW_PRIORITY = 3,
  };

  enum class EngOvrdCtrlMode_F_B : std::uint8_t {
    OVERRIDE_DISABLED = 0,
    SPEED_CONTROL = 1,
    TORQUE_CONTROL = 2,
    SPEED_TORQUE_LIMIT_CONTROL = 3,
  };

  enum class TSC1_CtrlPurpose_0_B : std::uint8_t {
    P1_ACCPEDAL_OPERSELECTION = 0,
    P2_CRUISE_CONTROL = 1,
    P3_PTO_GOVERNOR = 2,
    P4_ROAD_SPEED_GOVERNOR = 3,
    P5_ENGINE_PROTECTION = 4,
    RESERVED_1 = 5,
    RESERVED_2 = 6,
    RESERVED_3 = 7,
    RESERVED_4 = 8,
    RESERVED_5 = 9,
    RESERVED_6 = 10,
    RESERVED_7 = 11,
    RESERVED_8 = 12,
    RESERVED_9 = 13,
    RESERVED_10 = 14,
    RESERVED_11 = 15,
    RESERVED_12 = 16,
    RESERVED_13 = 17,
    RESERVED_14 = 18,
    RESERVED_15 = 19,
    RESERVED_16 = 20,
    RESERVED_17 = 21,
    RESERVED_18 = 22,
    RESERVED_19 = 23,
    RESERVED_20 = 24,
    RESERVED_21 = 25,
    RESERVED_22 = 26,
    RESERVED_23 = 27,
    RESERVED_24 = 28,
    RESERVED_25 = 29,
    RESERVED_26 = 30,
    P32_TEMPORARYPOWERTRAINCONTROL = 31,
  };

  enum class OvrdCtrlModePriority_0_B : std::uint8_t {
    HIGHEST_PRIORITY = 0,
    HIGH_PRIORITY = 1,
    MEDIUM_PRIORITY = 2,
    LOW_PRIORITY = 3,
  };

  enum class PCC_Drvln_Diseng_Inhbt_Req : std::uint8_t {
    NO_INHIBIT_REQUEST = 0,
    DRVLINE_DISENG_INHIBIT_REQ = 1,
    RESERVED = 2,
    DONT_CARE_TAKE_NO_ACTION = 3,
  };

  enum class Tra_Idle_Gov_Fuel_Inhib_Support : std::uint8_t {
    FEATURE_NOT_SUPPORTED = 0,
    FEATURE_SUPPORTED = 1,
    RESERVED = 2,
    NOT_SUPPORTED = 3,
  };

  enum class TC1_Trans_Mode4_BobTail : std::uint8_t {
    DISABLE = 0,
    ENABLE = 1,
    RESERVED = 2,
    TAKE_NO_ACTION = 3,
  };

  enum class TC1_ECoast_Ctrl : std::uint8_t {
    TRANSMISSION_CONTROL_ENABLED = 0,
    ENGINE_CONTROL_ENABLED = 1,
    TAKE_NO_ACTION = 3,
  };

  enum class Clstr_Dead_Pedal_Txt_Alert : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Clstr_CAP_Txt_Alert : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Clstr_HC_Desorb_Txt_Alert : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Clstr_Parked_Regen_Txt_Alert : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Clstr_Regen_Fail_Txt_Alert : std::uint8_t {
    NO_TEXT = 0,
    TRY_AGAIN_LATER = 1,
    SERVICE_VEHICLE = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Status_Actuator1 : std::uint8_t {
    OFF = 0,
    ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Status_Actuator0 : std::uint8_t {
    OFF = 0,
    ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class PM_Regen_Status : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    SNA = 3,
  };

  enum class PM_Measurement_Status : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    SNA = 3,
  };

  enum class Cruise_Control_Resume_Command : std::uint8_t {
    CRUISE_CNTRL_RESUME_NOT_REQUEST = 0,
    CRUISE_CNTRL_RESUME_REQUESTED = 1,
    RESERVED = 2,
    DONT_CARE_TAKE_NO_ACTION = 3,
  };

  enum class Cruise_Control_Pause_Command : std::uint8_t {
    CRUISE_CONTROL_IS_ALLOWED = 0,
    CRUISE_CONTROL_IS_NOT_ALLOWED = 1,
    RESERVED = 2,
    DONT_CARE_TAKE_NO_ACTION = 3,
  };

  enum class Cruise_Control_Disable_Command : std::uint8_t {
    CRUISE_CONTROL_IS_ALLOWED = 0,
    CRUISE_CONTROL_IS_NOT_ALLOWED = 1,
    RESERVED = 2,
    DONT_CARE_TAKE_NO_ACTION = 3,
  };

  enum class Htr_OFF_Pwr_Out_Rng_Hi_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_ON_Pwr_Out_Rng_Lo_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_ON_Pwr_Out_Rng_Hi_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class CAN_MSG_Time_Out_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Boost_Volt_Out_Of_Rng_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Boost_Vol_Ip_Ovr_Volt_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Boost_Volt_Ip_Shrt_GND_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Boost_Volt_Ip_Shrt_Bat_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Hi_Sensor_Ip_Short_GND_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Hi_Sensor_Ip_Short_Bat_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Lo_Drive_Short_GND_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Lo_Drive_Over_Crnt_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Lo_Drive_Opn_Cir_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Lo_Drive_Short_Bat_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Hi_Drive_Short_Bat_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Hi_Drive_Over_Crnt_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Hi_Drive_Opn_Diag_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Htr_Hi_Drive_Short_GND_Di_Status : std::uint8_t {
    NOT_RUN = 0,
    PASS = 1,
    FAIL = 2,
    RUN_NO_PASS_OR_FAIL_DECISION = 3,
  };

  enum class Particulate_Sensor_Regen_State : std::uint8_t {
    REGENERATION_IS_NOT_ACTIVE = 0,
    REGNERATION_IS_ACTIVE_MANUAL = 1,
    REGENERATION_IS_ACTIVE_SENSOR = 2,
    SAE_RESERVED_1 = 3,
    SAE_RESERVED_2 = 4,
    SAE_RESERVED_3 = 5,
    SAE_RESERVED_4 = 6,
    SAE_RESERVED_5 = 7,
    SAE_RESERVED_6 = 8,
    SAE_RESERVED_7 = 9,
    SAE_RESERVED_8 = 10,
    SAE_RESERVED_9 = 11,
    SAE_RESERVED_10 = 12,
    SAE_RESERVED_11 = 13,
    SAE_RESERVED_12 = 14,
    NOT_AVAILABLE = 15,
  };

  enum class Particulate_Sensor_Regen_Status : std::uint8_t {
    REGEN_ATTEMPT_HAS_NOT_OCCURRED = 0,
    REGENERATION_WAS_SUCCESSFUL = 1,
    REGENERATION_FAILED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Particulate_Sensor_Heater_State : std::uint8_t {
    HTR_IS_IN_CLOSED_LOOP_CONTROL = 0,
    HTR_IS_BEING_HELD_SPEC_LEVEL = 1,
    HEATER_IS_IN_RAMP_MODE = 2,
    HEATER_IS_OFF = 3,
  };

  enum class Fire_App_Pump_Engagement : std::uint8_t {
    PUMP_NOT_ENGAGED = 0,
    PUMP_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE_OR_NOT_INSTALLED = 3,
  };

  enum class Cruise_Control_Resume_Switch_G : std::uint8_t {
    SWITCH_OFF = 0,
    SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Cruise_Control_Pause_Switch : std::uint8_t {
    SWITCH_OFF = 0,
    SWITCH_ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class DPF_Regen_Force_Switch_C : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DPF_Regen_Inhibit_Switch_C : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Veh_Lim_Spd_Gov_Inc_Switch_A : std::uint8_t {
    SWITCH_IN_THE_OFF_STATE = 0,
    SWITCH_IN_THE_ON_STATE = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Veh_Lim_Spd_Gov_Dec_Switch : std::uint8_t {
    SWITCH_IN_THE_OFF_STATE = 0,
    SWITCH_IN_THE_ON_STATE = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Fuel_Type_A : std::uint8_t {
    NOTAVAILABLE_NONE = 0,
    GASOLINE_PETROL_GAS = 1,
    METHANOL_METH = 2,
    ETHANOL_ETH = 3,
    DIESEL_DSL = 4,
    LIQUEFIEDPETROLEUMGAS_LPG_LPG = 5,
    COMPNATURALGAS_CNG_CNG = 6,
    PROPANE_PROP = 7,
    BATTERY_ELECTRIC_ELEC = 8,
    BI_FUELVEHGASOLINE_BI_GAS = 9,
    BI_FUELVEHMETHANOL_BI_METH = 10,
    BI_FUELVEHETHANOL_BI_ETH = 11,
    BI_FUELVEHLPG_BI_LPG = 12,
    BI_FUELVEHCNG_BI_CNG = 13,
    BI_FUELVEHPROPANE_BI_PROP = 14,
    BI_FUELVEHBATTERY_BI_ELEC = 15,
    BI_FUELVEHBATT_CMBENG_BI_MIX = 16,
    HYBRIDVEHGASENGINE_HYB_GAS = 17,
    HYBRIDVEHGASENGETHANOL_HYB_ETH = 18,
    HYBRIDVEHDIESELENGINE_HYB_DSL = 19,
    HYBRIDVEHBATTERY_HYB_ELEC = 20,
    HYBRIDVEHBATT_COMBENG_HYB_MIX = 21,
    HYBRIDVEHREGENMODE_HYB_REG = 22,
    ISO_SAE_RESERVED = 23,
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class Adaptive_Cruise_Control_Mode : std::uint8_t {
    OFF = 0,
    SPEEDCONTROLACTIVE = 1,
    DISTANCECONTROLACTIVE = 2,
    OVERTAKEMODE = 3,
    HOLDMODE = 4,
    FINISHMODE = 5,
    DISABLEORERRORCORRECTION = 6,
    NOTAVAILABLE_NOTVALID = 7,
  };

  enum class SCR_Warm_Up_Status : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Retarder_Inh_EBS_Switch2_Status : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Transfer_Case_Status_B : std::uint8_t {
    OFF = 0,
    ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Retarder_Inh_EBS_Switch_Status : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Change_Oil_Lamp_Status : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Electronic_Fan_Control_Status : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class FMI_SPN10_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN9_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN8_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN7_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN6_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN5_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN4_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN3_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN2_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN1_DM28 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashRedStopLamp_DM28 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp_DM28 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashMalfuncIndicatorLamp_DM28 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp_DM28 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class RedStopLampStatus_DM28 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatusDM28 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus_DM28 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class MalfuncIndicatorLampStatus_DM28 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class FMI_SPN1_DM25 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class NOx_Quality_Indicator : std::uint8_t {
    DATA_VALID = 0,
    RESERVED = 1,
  };

  enum class Engine_Operating_Mode : std::uint8_t {
    DISABLED = 0,
    PM_MODE = 1,
    STANDARD_NOX = 2,
    SCR_HIGH_EFFICIENCY = 3,
    SCR_HEATING = 4,
    DPF_HEATING = 5,
    DOC_HEATING = 6,
    ENGINE_BRAKING = 7,
    CRANKING = 8,
    ENGINE_TEST_FUNCTION = 9,
    RESERVED_1 = 10,
    RESERVED_2 = 11,
    RESERVED_3 = 12,
    RESERVED_4 = 13,
    RESERVED_5 = 14,
    RESERVED_6 = 15,
  };

  enum class Engine_Operating_State_A : std::uint8_t {
    ENGINESTOPPED = 0,
    PRESTART = 1,
    STARTING = 2,
    WARMUP = 3,
    RUNNING = 4,
    COOLDOWN = 5,
    ENGINESTOPPING = 6,
    POSTRUN = 7,
    SAE_ASSIGNMENT_1 = 8,
    SAE_ASSIGNMENT_2 = 9,
    SAE_ASSIGNMENT_3 = 10,
    SAE_ASSIGNMENT_4 = 11,
    SAE_ASSIGNMENT_5 = 12,
    SAE_ASSIGNMENT_6 = 13,
    RESERVED = 14,
    NOTAVAILABLE = 15,
  };

  enum class ACM_Test_Reset : std::uint8_t {
    NO_ACM_TEST_RESET_REQ_PRESENT = 0,
    FAULT_CODES_ARE_CLEARED_SUCCESS = 1,
    NOT_REALISED_1 = 2,
    NOT_REALISED_2 = 3,
  };

  enum class DPF_Reg_Release_Indicator : std::uint8_t {
    NO = 0,
    YES = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class FMI_SPN5_DM32 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN4_DM32 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN3_DM32 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN1_DM32 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN2_DM32 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class progressiveshiftdisable_0 : std::uint8_t {
    PROGRESSIVE_SHIFT_IS_NOT_DISABLED = 0,
    PROGRESSIVE_SHIFT_IS_DISABLED = 1,
    RESERVED = 2,
    TAKE_NO_ACTION = 3,
  };

  enum class engmomentaryoverspeedenable_0 : std::uint8_t {
    MOMENTARY_ENGINE_OVERSPEED_IS_DISABLED = 0,
    MOMENTARY_ENGINE_OVERSPEED_IS_ENABLED = 1,
    RESERVED = 2,
    TAKE_NO_ACTION = 3,
  };

  enum class transshiftinprocess_0 : std::uint8_t {
    SHIFT_IS_NOT_IN_PROCESS = 0,
    SHIFT_IN_PROCESS = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class transdrivelineengaged_0 : std::uint8_t {
    DRIVELINE_DISENGAGED = 0,
    DRIVELINE_ENGAGED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Cold_Start_Relay_Diag : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Cold_Start_Relay : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Engine_Crank_Inhibit_Flame_Start : std::uint8_t {
    NOSTARTENABLEDEVICE1INSTALLED = 0,
    GLOWPLUGSINSTALLED = 1,
    FUELLEDSTARTINSTALLED = 2,
    ETHERINJECTIONINSTALLED = 3,
    ELECTRICINLETAIRHEATERINSTALLED = 4,
    RESERVED_1 = 5,
    RESERVED_2 = 6,
    RESERVED_3 = 7,
    RESERVED_4 = 8,
    RESERVED_5 = 9,
    RESERVED_6 = 10,
    RESERVED_7 = 11,
    RESERVED_8 = 12,
    RESERVED_9 = 13,
    RESERVED_10 = 14,
    NOTAVAILABLE = 15,
  };

  enum class Cold_Start_Fuel_Solenoid : std::uint8_t {
    STARTENABLEOFF = 0,
    STARTENABLEON = 1,
    RESERVED = 2,
    NOTAVAILABLE = 3,
  };

  enum class DPF_Regen_Inhibit_Switch : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DPF_Regen_Force_Switch : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class TestIdentifier_DM8_E : std::uint8_t {
    COMPRESSION_TEST = 18,
    ACTUATOR_TEST = 52,
    RUN_UP_TEST = 54,
    TEST_OF_CYLINDER_CUT_OUT_TEST_1 = 55,
    TEST_OF_CYLINDER_CUT_OUT_TEST_2 = 56,
    TEST_OF_CYLINDER_CUT_OUT_TEST_3 = 57,
    TEST_OF_CYLINDER_CUT_OUT_TEST_4 = 64,
    TEST_OF_CYLINDER_CUT_OUT_TEST_5 = 65,
    TEST_OF_CYLINDER_CUT_OUT_TEST_6 = 66,
    DOSER_FLOW_TEST = 70,
    INJECTOR_LEAK_TEST = 71,
    DFSOV_LEAK_TEST = 72,
    HIGH_PRESSURE_SERVICE_TEST = 82,
  };

  enum class FMI_SPN10_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN9_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN8_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN7_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN6_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashMalfuncIndicatorLamp_DM6 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashRedStopLamp_DM6 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp_DM6 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp_DM6 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class RedStopLampStatus_DM6 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatusDM6 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus_DM6 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class MalfunIndicatorLampStatus_DM6 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class FMI_SPN5_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN4_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN3_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN2_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN1_DM6 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class PMFilterMonStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class NOxCnvrtctlystMonStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class NHMCCnvrtctlystMonStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class BstPressCtrlSystemMonStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class NOxCnvrtctlystMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTEDBYCONTROLLER = 0,
    TESTSUPPORTEDBYCONTROLLER = 1,
  };

  enum class PMFilterMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTEDBYCONTROLLER = 0,
    TESTSUPPORTEDBYCONTROLLER = 1,
  };

  enum class NHMCCnvrtctlystMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTEDBYCONTROLLER = 0,
    TESTSUPPORTEDBYCONTROLLER = 1,
  };

  enum class BstPressCtrlSystemMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTEDBYCONTROLLER = 0,
    TEST_SUPPORTED = 1,
  };

  enum class MisfireMonitoringSupport_DM5 : std::uint8_t {
    TEST_NOT_SUPPORTED = 0,
    TEST_SUPPORTED = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class OBDCompliance_DM5 : std::uint8_t {
    RESERVER_FOR_ASSIGNMENT_BY_SAE = 0,
    OBD_II_CALIFORNIA_ARB = 1,
    OBD_FEDERAL_EPA = 2,
    OBD_AND_OBD_II = 3,
    OBD_I = 4,
    NOT_INTENDED_TO_MEET_OBD_II_REQU = 5,
  };

  enum class SecondAirSystemMonStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class EvaporativeSystemMonStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class ACSystemRefrigerantMonStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class OxygenSensorHeaterMonStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class OxygenSensorMonStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class EGRSystemMonitoringStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class CatalystMonStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class ColdStartAidSystemMonStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class HeatedCatalystMonStatus_DM5 : std::uint8_t {
    TESTCOMPLETE = 0,
    TESTNOTCOMPLETE = 1,
  };

  enum class CatalystMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class ColdStartAidSystemMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class HeatedCatalystMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class EvaporativeSystemMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class SecondAirSystemMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class ACSystemRefrigerantMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class OxygenSensorMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class OxygenSensorHeaterMonSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class EGRSystemMonitoringSupp_DM5 : std::uint8_t {
    TESTNOTSUPPORTED = 0,
    TESTSUPPORTED = 1,
  };

  enum class MisfireMonitoringStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class ComprehensiveCompMonStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class FuelSystemMonitoringStatus_DM5 : std::uint8_t {
    TEST_COMPLETE = 0,
    TEST_NOT_COMPLETE = 1,
  };

  enum class CompreComponentMonSupp_DM5 : std::uint8_t {
    TEST_NOT_SUPPORTED = 0,
    TEST_SUPPORTED = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class FuelSystemMonitoringSupport_DM5 : std::uint8_t {
    TEST_NOT_SUPPORTED = 0,
    TEST_SUPPORTED = 1,
  };

  enum class FMI_SPN1_DM4 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class Engine_Torque_Mode_DM4 : std::uint8_t {
    LOW_IDLE_GOVERNOR_NO_REQUEST = 0,
    ACCELERATOR_PEDAL_OPERATOR_SELEC = 1,
    CRUISE_CONTROL = 2,
    PTO_GOVERNOR = 3,
    ROAD_SPEED_GOVERNOR = 4,
    ASR_CONTROL = 5,
    TRANSMISSION_CONTROL = 6,
    ABS_CONTROL = 7,
    TORQUE_LIMITING = 8,
    HIGH_SPEED_GOVERNOR = 9,
    BRAKING_SYSTEM = 10,
    REMOTE_ACCELERATOR = 11,
    SERVICE_PROCEDURE = 12,
    NOT_DEFINED = 13,
    OTHER = 14,
    NOT_AVAILABLE = 15,
  };

  enum class FMI_SPN10_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN9_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN8_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN7_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN6_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN5_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN4_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN3_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN2_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN1_DM23 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashRedStopLamp_DM23 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp_DM23 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashMalfuncIndicatorLamp_DM23 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp_DM23 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class RedStopLampStatus_DM23 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatusDM23 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus_DM23 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class MalfunIndicatorLampStatus_DM23 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class FMI_SPN10_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN9_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN8_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN7_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN6_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashRedStopLamp_DM12 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp_DM12 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashMalfuncIndicatorLamp_DM12 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp_DM12 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FMI_SPN5_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN4_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN3_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN2_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FMI_SPN1_DM12 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class RedStopLampStatus_DM12 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatusDM12 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus_DM12 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class MalfunIndicatorLampStatus_DM12 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ret_enbl_shift_sssist_switch_0 : std::uint8_t {
    RETARDER_SHIFT_ASSIST_DISABLED = 0,
    RETARDER_SHIFT_ASSIST_ENABLED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ret_enbl_brake_assist_awitch_0 : std::uint8_t {
    RETARDER_BRAKEASSISTDISABLED = 0,
    RETARDER_BRAKEASSISTENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class retarder_torque_mode_0 : std::uint8_t {
    NOREQUEST = 0,
    TORQUEREQUESTORRETDCONRTOLLING_1 = 1,
    TORQUEREQUESTORRETDCONRTOLLING_2 = 2,
    TORQUEREQUESTORRETDCONRTOLLING_3 = 3,
    TORQUEREQUESTORRETDCONRTOLLING_4 = 4,
    TORQUEREQUESTORRETDCONRTOLLING_5 = 5,
    TORQUEREQUESTORRETDCONRTOLLING_6 = 6,
    TORQUEREQUESTORRETDCONRTOLLING_7 = 7,
    TORQUEREQUESTORRETDCONRTOLLING_8 = 8,
    TORQUEREQUESTORRETDCONRTOLLING_9 = 9,
    TORQUEREQUESTORRETDCONRTOLLING_10 = 10,
    TORQUEREQUESTORRETDCONRTOLLING_11 = 11,
    TORQUEREQUESTORRETDCONRTOLLING_12 = 12,
    TORQUEREQUESTORRETDCONRTOLLING_13 = 13,
    TORQUEREQUESTORRETDCONRTOLLING_14 = 14,
    TORQUEREQUESTORRETDCONRTOLLING_15 = 15,
  };

  enum class AFT_Fuel_Enable_Control : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    RESERVEDFORSAEASSIGNMENT = 2,
    NOTAVAILABLE = 3,
  };

  enum class Regeneration_Status_A : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    RESERVEDFORSAEASSIGNMENT = 2,
    NOTAVAILABLE = 3,
  };

  enum class ControlByte_ACKM_RX : std::uint8_t {
    ACK = 0,
    NACK = 1,
    ACCESSDENIED = 2,
    CANNOTRESPOND = 3,
  };

  enum class HoldSignal : std::uint8_t {
    ALL_DEVICES = 0,
    DEVICES_WHOSE_BROADCAST_STATE_HA = 1,
    NOT_AVAILABLE = 15,
  };

  enum class SuspendSignal : std::uint8_t {
    INDEFINITESUSPENSIONOFALLBROADCA = 0,
    INDEFINITESUSPENSIONOFSOMEMESSAG = 1,
    TEMPORARYSUSPENSIONOFALLBROADCAS = 2,
    TEMPORARYSUSPENSIONOFSOMEMESSAGE = 3,
    RESUMINGNORMALBROADCASTPATTERN = 14,
    NOTAVAILABLE = 15,
  };

  enum class J1939Network3 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network2 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ISO9141 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1850 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ManufacturerSpecificPort : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class CurrentDataLink : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1587 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1922 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network1 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class Remote_Lock_Unlock_Req_21_4B : std::uint8_t {
    NO_REQUEST = 0,
    DOOR_LOCK_REQUESTED = 1,
    DOOR_UNLOCK_REQUESTED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class V2X_Vehicle_Request_21_4B : std::uint8_t {
    NOT_REQUESTED = 0,
    REQUESTED = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class LiftAxle1Position_2F : std::uint8_t {
    LIFTAXLEPOSDOWN = 0,
    LIFTAXLEPOSUP = 1,
    ERROR = 2,
    DESCRIPTION_FOR_THE_VALUE_0X3 = 3,
  };

  CanSignalLe<std::uint32_t> TripServiceBrakeApplications{0, 32, 1, 0, 0, 4211081215};  // appl
  CanSignalLe<std::uint32_t> TripServiceBrakeDistance{0, 32, 0.125, 0, 0, 526385151.875};  // km
  CanSignalLe<std::uint32_t> TripCompressionBrakeDistance{0, 32, 0.125, 0, 0, 526385151.875};  // km
  CanSignalLe<std::uint32_t> TotalCompressionBrakeDistance{0, 32, 0.125, 0, 0, 526385151.875};  // km
  CanSignalLe<std::uint16_t> TripDriveFuelEconomyGaseous{0, 16, 0.001953125, 0, 0, 125.498046875};  // km/kg
  CanSignalLe<std::uint32_t> TripCruiseFuelUsedGaseous{0, 32, 0.5, 0, 0, 2105540607.5};  // kg
  CanSignalLe<std::uint32_t> TripVehicleIdleFuelUsedGaseous{0, 32, 0.5, 0, 0, 2105540607.5};  // kg
  CanSignalLe<std::uint32_t> TripPTOGovNonmovingFuelUsed{0, 32, 0.5, 0, 0, 2105540607.5};  // kg
  CanSignalLe<std::uint32_t> TripPTOGovernorMovingFuelUsedGas{0, 32, 0.5, 0, 0, 2105540607.5};  // kg
  CanSignalLe<std::uint32_t> TripDriveFuelUsedGaseous{0, 32, 0.5, 0, 0, 2105540607.5};  // kg
  CanSignalLe<std::uint32_t> Longitude{0, 32, 1e-7, -210, -210, 211.1081215};  // Deg
  CanSignalLe<std::uint32_t> Latitude{0, 32, 1e-7, -210, -210, 211.1081215};  // Deg
  CanSignalLe<std::uint8_t> SupportVarTranRepRateforAcceSen{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> VerticalAccFigureofMeritExtRange{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> LongitudinalAccFigMeritExtRange{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> LateralAccFigureofMeritExtRange{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint16_t> VerticalAccelerationExtRange{0, 16, 0.01, -320, -320, 322.55};  // m/s�
  CanSignalLe<std::uint16_t> LongitudinalAccelerationExtRange{0, 16, 0.01, -320, -320, 322.55};  // m/s�
  CanSignalLe<std::uint16_t> LateralAccelerationExtendedRange{0, 16, 0.01, -320, -320, 322.55};  // m/s�
  CanSignalLe<std::uint8_t> EnginePrelubeOilLowPressureThre{0, 8, 4, 0, 0, 1000};  // kPa
  CanSignalLe<std::uint8_t> CrankAttemptCntonPresentStartAtt{0, 8, 1, 0, 0, 250};  // counts
  CanSignalLe<std::uint8_t> EngineIntakeManifoldPresCntrMode{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> EngineRotationDirection{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint16_t> EngineRatedSpeed{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint16_t> EngineRatedPower{0, 16, 0.5, 0, 0, 32127.5};  // kW
  CanSignalLe<std::uint32_t> TripFanOnTimeDuetoaManualSwitch{0, 32, 0.05, 0, 0, 210554060.75};  // hr
  CanSignalLe<std::uint32_t> TripFanOnTimeDuetotheEngineSys{0, 32, 0.05, 0, 0, 210554060.75};  // hr
  CanSignalLe<std::uint8_t> AxleLocation{0, 8, 1, 0, 0, 255};  // bit
  CanSignalLe<std::uint8_t> ADRCompliantHeaterInstallation{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> CabHeatingZone{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> EngineHeatingZone{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> CabVentilation{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> AuxiliaryHeaterWaterPumpStatus{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> AuxiliaryHeaterOutputPowerPerce{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> AuxiliaryHeaterInputAirTemp{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> Memorylevel{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> SuspensionControlRefusalInfo{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> SpeedDependantLevelControlStatus{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> AllowLevelControlDuringBrakingSt{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> SuspensionRemotecontrol2{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> SuspensionRemoteControl1{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> LiftAxle2Position{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> RearAxleinBumperRange{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> FrontAxleinBumperRange{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> DoorRelease{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> VehicleMotionInhibit{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> SecurityDevice{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> LevelControlMode{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> KneelingInformation{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> LiftingControlModeRearAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> LiftingControlModeFrontAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> LoweringControlModeRearAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> LoweringControlModeFrontAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> AboveNominalLevelRearAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> AboveNominalLevelFrontAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> BelowNominalLevelFrontAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> BelowNominalLevelRearAxle{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> NominalLevelRearAxle{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> RollAngleFigureofMerit{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> PitchAngleFigureofMerit{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> PitchRateFigureofMerit{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> PitchandRollCompensated{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> RollandPitchMeasurementLatency{0, 8, 0.5, 0, 0, 125};  // ms
  CanSignalLe<std::uint8_t> actual_eng_percent_torque_frac{0, 4, 0.125, 0, 0, 0.875};  // %
  CanSignalLe<std::uint8_t> Engine_Demand_Percent_Torque_A{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<Engine_Starter_Mode> Engine_Starter_Mode{0, 4};
  CanSignalLe<std::uint8_t> Src_Add_Cont_Dev_Eng_Control_A{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint16_t> EngineSpeed{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> TorqueEngineActualPercent{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> TorqueDriversEngineDemand{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<OtaTimerStatus_17> OtaTimerStatus_17{0, 2};
  CanSignalLe<std::uint8_t> PowerAvailable_5A{0, 8, 1, 0, 0, 250};  // kW/bit
  CanSignalLe<std::uint16_t> EnergyAvailable_5A{0, 16, 1, 0, 0, 64255};  // kWh/bit
  CanSignalLe<V2gStatusMode_5A> V2gStatusMode_5A{0, 4};
  CanSignalLe<V2gAllowedPermissions_5A> V2gAllowedPermissions_5A{0, 2};
  CanSignalLe<std::uint8_t> ChecksumValue_5A_4B{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> CounterValue_5A_4B{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Power_Request_5A_4B{0, 8, 1, 0, 0, 250};  // kW/bit
  CanSignalLe<std::uint16_t> Energy_Request_5A_4B{0, 16, 1, 0, 0, 64255};  // kWh/bit
  CanSignalLe<Grid_V2X_Request_5A_4B> Grid_V2X_Request_5A_4B{0, 4};
  CanSignalLe<std::uint8_t> Comm_Header_Byte_5A_4B{0, 8, 1, 0, 0, 255};
  CanSignalLe<Grid_V2X_Confirmation_5A_4B> Grid_V2X_Confirmation_5A_4B{0, 2};
  CanSignalLe<std::uint8_t> ChecksumValue_5A_4A{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> CounterValue_5A_4A{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> PowerRequest_5A_4A{0, 8, 1, 0, 0, 250};  // kW/bit
  CanSignalLe<std::uint16_t> EnergyRequest_5A_4A{0, 16, 1, 0, 0, 64255};  // kWh/bit
  CanSignalLe<GridV2xConfirmation_5A_4A> GridV2xConfirmation_5A_4A{0, 2};
  CanSignalLe<GridV2xRequest_5A_4A> GridV2xRequest_5A_4A{0, 4};
  CanSignalLe<std::uint8_t> CommHeaderByte_5A_4A{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> EngineDemandPercentTorq_4A_03{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> EngineStarterMode_4A_03{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> SourAddofContDevforEngCont_4A_03{0, 8, 1, 0, 0, 255};  // SA
  CanSignalLe<std::uint16_t> EngineSpeed_4A_03{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> ActualEnginePercentTorq_4A_03{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> DriversDemandEngPerTorq_4A_03{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> ActualEngPercTorqueFrac_4A_03{0, 8, 0.125, 0, 0, 0.875};  // %
  CanSignalLe<std::uint8_t> EngineTorqueMode_4A_03{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint32_t> TripDistance_4A_03{0, 32, 0.125, 0, 0, 526385151.9};  // km
  CanSignalLe<std::uint32_t> TotalVehicleDistance_4A_03{0, 32, 0.125, 0, 0, 526385151.9};  // km
  CanSignalLe<std::uint16_t> CargoAmbientTemperature_4A_03{0, 16, 1, -273, -273, 1734.96875};  // C
  CanSignalLe<std::uint8_t> FuelLevel2_4A_03{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> EngineFuelFilterDiffPress_4A_03{0, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint8_t> EngineOilFilterDiffPress_4A_03{0, 8, 0.5, 0, 0, 125};  // kPa
  CanSignalLe<std::uint8_t> WasherFluidLevel_4A_03{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> EngOilFilterDiffPressExt_4A_03{0, 8, 5, 0, 0, 1250};  // kPa
  CanSignalLe<std::uint8_t> FuelLevel1_4A_03{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> ECUTyp{0, 8, 1, 0, 0, 255};  // ASCII
  CanSignalLe<std::uint8_t> ECUParNum{0, 8, 1, 0, 0, 255};  // ASCII
  CanSignalLe<std::uint8_t> ECUSerNum{0, 8, 1, 0, 0, 255};  // ASCII
  CanSignalLe<std::uint8_t> ECULoc{0, 8, 1, 0, 0, 255};  // ASCII
  CanSignalLe<std::uint8_t> ECUManNam{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> ECUHarID{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint64_t> BCMCommData_4A_21{0, 64, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> CompressorConfiguration_21{0, 4, 1, 0, 0, 15};  // 16states/4bits
  CanSignalLe<std::uint8_t> AlternatorConfiguration_21{0, 4, 1, 0, 0, 15};  // 16states/4bits
  CanSignalLe<std::uint16_t> BatteryPackCapacity_5B{0, 16, 1, 0, 0, 64255};  // Ah
  CanSignalLe<std::uint16_t> BatteryPackStateOfCharge_5B{0, 16, 0.0025, 0, 0, 160.6375};  // %
  CanSignalLe<std::uint8_t> AirCompressorStatus_30{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint16_t> AlternatorSetpointVoltageCmd_11{0, 16, 0.001, 0, 0, 64.255};  // V/bit
  CanSignalLe<std::uint8_t> AverageCompressorDutyCycle_30{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> AverageVehicleSpeed_30{0, 16, 0.00390625, 0, 0, 250.996};  // km/h
  CanSignalLe<std::uint8_t> MaximumAirTankPrLim_30{0, 8, 8, 0, 0, 2000};  // kPa
  CanSignalLe<std::uint8_t> MinimumAirTankPrLim_30{0, 8, 8, 0, 0, 2000};  // kPa
  CanSignalLe<std::int8_t> ExternalChargeInhibitSt_30{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<std::uint8_t> ServiceBrake1AirPrRequest_11{0, 8, 8, 0, 0, 2000};  // kPa
  CanSignalLe<std::uint8_t> ServiceBrake2AirPrRequest_11{0, 8, 8, 0, 0, 2000};  // kPa
  CanSignalLe<std::uint8_t> ServiceBrakeCircuit1AirPr_21{0, 8, 8, 0, 0, 2000};  // kPa
  CanSignalLe<std::uint8_t> ServiceBrakeCircuit2AirPr_21{0, 8, 8, 0, 0, 2000};  // kPa
  CanSignalLe<std::uint8_t> Alternator1Status_1A{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint16_t> AlternatorSpeed_1A{0, 16, 0.5, 0, 0, 32127.5};  // rpm
  CanSignalLe<std::uint8_t> AlternatorTrqRampMaxSpeedCmd_1A{0, 8, 32, 0, 0, 8000};  // RPM PER BIT
  CanSignalLe<std::uint8_t> AlternatorTorqueRampTimeCmd_1A{0, 8, 0.1, 0, 0, 25};  // s/bit
  CanSignalLe<std::uint8_t> AlternatorExcitMaxCurentLimit_1A{0, 8, 0.25, 0, 0, 62.5};  // A/bit
  CanSignalLe<std::uint16_t> AlternatorSetpointVoltageCmd_1A{0, 16, 0.001, 0, 0, 64.255};  // V/bit
  CanSignalLe<std::uint8_t> StatusofSteeringAxle_5A{0, 4, 1, 0, 0, 15};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> MtrGrtr1Temp1_5A{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<OvrdCtrlModePriority_7> OvrdCtrlModePriority_7{0, 2};
  CanSignalLe<std::uint8_t> EngRqstdTrqLimit_7{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngRqstdSpdLimit_7{0, 16, 0.125, 0, 0, 8031.88};  // rpm
  CanSignalLe<EngRqstdSpdCtrlCondns_7> EngRqstdSpdCtrlCondns_7{0, 2};
  CanSignalLe<EngOvrdCtrlMode_7> EngOvrdCtrlMode_7{0, 2};
  CanSignalLe<AtleastOnePtoEngaged_3> AtleastOnePtoEngaged_3{0, 2};
  CanSignalLe<EngageMntcnstPtoEngFlyWheel_3> EngageMntcnstPtoEngFlyWheel_3{0, 2};
  CanSignalLe<EngagementStatusPtoEngFlyWheel_3> EngagementStatusPtoEngFlyWheel_3{0, 2};
  CanSignalLe<EnggMntTrnsFrcsOtputShaftPto_3> EnggMntTrnsFrcsOtputShaftPto_3{0, 2};
  CanSignalLe<EnggMntTrnsFrcsOtptShaftPto_3> EnggMntTrnsFrcsOtptShaftPto_3{0, 2};
  CanSignalLe<EnggMntTransOutputShaftPto_3> EnggMntTransOutputShaftPto_3{0, 2};
  CanSignalLe<EnggMntTransInputShaftPto2_3> EnggMntTransInputShaftPto2_3{0, 2};
  CanSignalLe<EnggMntTransInputShaftPto1_3> EnggMntTransInputShaftPto1_3{0, 2};
  CanSignalLe<EnggMntSttsPtoAccessoryDrive2_3> EnggMntSttsPtoAccessoryDrive2_3{0, 2};
  CanSignalLe<EnggMntSttsPtoAccessoryDrive1_3> EnggMntSttsPtoAccessoryDrive1_3{0, 2};
  CanSignalLe<EnggMntStsTransOutputShaftPto_3> EnggMntStsTransOutputShaftPto_3{0, 2};
  CanSignalLe<EnggMntStsTransInputShaftPto2_3> EnggMntStsTransInputShaftPto2_3{0, 2};
  CanSignalLe<EnggMntStsTransInputShaftPto1_3> EnggMntStsTransInputShaftPto1_3{0, 2};
  CanSignalLe<EnggMntcnsntPtoAccssoryDrive2_3> EnggMntcnsntPtoAccssoryDrive2_3{0, 2};
  CanSignalLe<EnggMntcnsntPtoAccssoryDrive1_3> EnggMntcnsntPtoAccssoryDrive1_3{0, 2};
  CanSignalLe<EnblSwtchTrnscseOutputShftPto_3> EnblSwtchTrnscseOutputShftPto_3{0, 2};
  CanSignalLe<EnbleSwitchTransOutputShftPto_3> EnbleSwitchTransOutputShftPto_3{0, 2};
  CanSignalLe<EnbleSwitchTransInputShftPto1_3> EnbleSwitchTransInputShftPto1_3{0, 2};
  CanSignalLe<EnbleSwitchPtoAccessoryDrive2_3> EnbleSwitchPtoAccessoryDrive2_3{0, 2};
  CanSignalLe<EnbleSwitchPtoAccessoryDrive1_3> EnbleSwitchPtoAccessoryDrive1_3{0, 2};
  CanSignalLe<EnableSwtchTransInputShftPto2_3> EnableSwtchTransInputShftPto2_3{0, 2};
  CanSignalLe<EnableSwitchPtoEngFlyWheel_3> EnableSwitchPtoEngFlyWheel_3{0, 2};
  /* vehicleid */
  CanSignalLe<std::uint8_t> BeltLock10Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BeltLock9Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BeltLock8Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BeltLock7Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> PsngrAirbagDeactiveSwitchSts_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BeltLock6Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BeltLock5Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BeltLock4Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BeltLock3Status_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> PassengerBeltLockStatus_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> DriverBeltlockStatus_53{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint16_t> EngineTotalAverageFuelEconomy_0{0, 16, 0.001953125, 0, 0, 125.498046875};  // km/L
  CanSignalLe<std::uint16_t> EngineTotalAverageFuelRate_0{0, 16, 0.05, 0, 0, 3212.75};  // L/H
  CanSignalLe<std::uint8_t> numberofreversegearratios_03{0, 8, 1, 0, 0, 125};
  CanSignalLe<std::uint8_t> numberofforwardgearratios_03{0, 8, 1, 0, 0, 125};
  CanSignalLe<std::uint16_t> transgearratio_03{0, 16, 0.001, 0, 0, 64.255};
  CanSignalLe<enblswtchtrnscseoutputshftpto_0> enblswtchtrnscseoutputshftpto_0{0, 2};
  CanSignalLe<enbleswitchtransoutputshftpto_0> enbleswitchtransoutputshftpto_0{0, 2};
  CanSignalLe<enableswtchtransinputshftpto2_0> enableswtchtransinputshftpto2_0{0, 2};
  CanSignalLe<enbleswitchtransinputshftpto1_0> enbleswitchtransinputshftpto1_0{0, 2};
  CanSignalLe<enableswitchptoengflywheel_0> enableswitchptoengflywheel_0{0, 2};
  CanSignalLe<enbleswitchptoaccessorydrive1_0> enbleswitchptoaccessorydrive1_0{0, 2};
  CanSignalLe<enbleswitchptoaccessorydrive2_0> enbleswitchptoaccessorydrive2_0{0, 2};
  CanSignalLe<enggmnttrnsfrcsotptshaftpto_0> enggmnttrnsfrcsotptshaftpto_0{0, 2};
  CanSignalLe<enggmnttransoutputshaftpto_0> enggmnttransoutputshaftpto_0{0, 2};
  CanSignalLe<enggmnttransinputshaftpto2_0> enggmnttransinputshaftpto2_0{0, 2};
  CanSignalLe<enggmnttransinputshaftpto1_0> enggmnttransinputshaftpto1_0{0, 2};
  CanSignalLe<engagemntcnstptoengflywheel_0> engagemntcnstptoengflywheel_0{0, 2};
  CanSignalLe<enggmntcnsntptoaccssorydrive1_0> enggmntcnsntptoaccssorydrive1_0{0, 2};
  CanSignalLe<enggmntcnsntptoaccssorydrive2_0> enggmntcnsntptoaccssorydrive2_0{0, 2};
  CanSignalLe<enggmnttrnsfrcsotputshaftpto_0> enggmnttrnsfrcsotputshaftpto_0{0, 2};
  CanSignalLe<enggmntststransoutputshaftpto_0> enggmntststransoutputshaftpto_0{0, 2};
  CanSignalLe<enggmntststransinputshaftpto2_0> enggmntststransinputshaftpto2_0{0, 2};
  CanSignalLe<enggmntststransinputshaftpto1_0> enggmntststransinputshaftpto1_0{0, 2};
  CanSignalLe<engagementstatusptoengflywheel_0> engagementstatusptoengflywheel_0{0, 2};
  CanSignalLe<enggmntsttsptoaccessorydrive1_0> enggmntsttsptoaccessorydrive1_0{0, 2};
  CanSignalLe<enggmntsttsptoaccessorydrive2_0> enggmntsttsptoaccessorydrive2_0{0, 2};
  CanSignalLe<atleastoneptoengaged_0> atleastoneptoengaged_0{0, 2};
  CanSignalLe<std::uint8_t> engrqstdtrqlimit_0{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<engovrdctrlmode_0> engovrdctrlmode_0{0, 2};
  CanSignalLe<RunningLightCmd> RunningLightCmd{0, 2};
  CanSignalLe<AltBeamHeadLightCmd> AltBeamHeadLightCmd{0, 2};
  CanSignalLe<LowBeamHeadLightCmd> LowBeamHeadLightCmd{0, 2};
  CanSignalLe<HighBeamHeadLightCmd> HighBeamHeadLightCmd{0, 2};
  CanSignalLe<TractorFrontFogLightsCmd> TractorFrontFogLightsCmd{0, 2};
  CanSignalLe<RotatingBeaconLightCmd> RotatingBeaconLightCmd{0, 2};
  CanSignalLe<BackUpLightAndAlarmHornCmd> BackUpLightAndAlarmHornCmd{0, 2};
  CanSignalLe<CenterStopLightCmd> CenterStopLightCmd{0, 2};
  CanSignalLe<Right_Stop_Light_Command> Right_Stop_Light_Command{0, 2};
  CanSignalLe<Left_Stop_Light_Command> Left_Stop_Light_Command{0, 2};
  CanSignalLe<ImplementClearanceLightCmd> ImplementClearanceLightCmd{0, 2};
  CanSignalLe<TractorClearanceLightCmd> TractorClearanceLightCmd{0, 2};
  CanSignalLe<ImplementMarkerLightCmd> ImplementMarkerLightCmd{0, 2};
  CanSignalLe<TractorMarkerLightCmd> TractorMarkerLightCmd{0, 2};
  CanSignalLe<RearFogLightCmd> RearFogLightCmd{0, 2};
  CanSignalLe<TrctrUndrsdeMountedWorkLightsCmd> TrctrUndrsdeMountedWorkLightsCmd{0, 2};
  CanSignalLe<TrctrRearLowMountedWorkLightsCmd> TrctrRearLowMountedWorkLightsCmd{0, 2};
  CanSignalLe<TrctrRarHighMountedWorkLightsCmd> TrctrRarHighMountedWorkLightsCmd{0, 2};
  CanSignalLe<TrctrSideLowMountedWorkLightsCmd> TrctrSideLowMountedWorkLightsCmd{0, 2};
  CanSignalLe<TrctrSdeHighMountedWorkLightsCmd> TrctrSdeHighMountedWorkLightsCmd{0, 2};
  CanSignalLe<TrctrFrntLowMountedWorkLightsCmd> TrctrFrntLowMountedWorkLightsCmd{0, 2};
  CanSignalLe<TrctrFrntHghMountedWorkLightsCmd> TrctrFrntHghMountedWorkLightsCmd{0, 2};
  CanSignalLe<ImplementOEMOption2LightCmd> ImplementOEMOption2LightCmd{0, 2};
  CanSignalLe<ImplementOEMOption1LightCmd> ImplementOEMOption1LightCmd{0, 2};
  CanSignalLe<ImplementRightFacingWorkLightCmd> ImplementRightFacingWorkLightCmd{0, 2};
  CanSignalLe<ImplementLeftFacingWorkLightCmd> ImplementLeftFacingWorkLightCmd{0, 2};
  CanSignalLe<LightingDataRqCmd> LightingDataRqCmd{0, 2};
  CanSignalLe<ImplmentRightForwardWorkLightCmd> ImplmentRightForwardWorkLightCmd{0, 2};
  CanSignalLe<ImplementLeftForwardWorkLightCmd> ImplementLeftForwardWorkLightCmd{0, 2};
  CanSignalLe<ImplementRearWorkLightCmd> ImplementRearWorkLightCmd{0, 2};
  CanSignalLe<std::uint16_t> frontaxlespeed_b{0, 16, 0.00390625, 0, 0, 250.996};  // km/h
  CanSignalLe<std::uint8_t> relativespdfrontaxleleftwheel_b{0, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
  CanSignalLe<std::uint8_t> relativespdfrontaxlerightwheel_b{0, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
  CanSignalLe<std::uint8_t> relativespdrearaxle1leftwheel_b{0, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
  CanSignalLe<std::uint8_t> relativespdrearaxle1rightwheel_b{0, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
  CanSignalLe<std::uint8_t> relativespdrearaxle2leftwheel_b{0, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
  CanSignalLe<std::uint8_t> relativespdrearaxle2rightwheel_b{0, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
  CanSignalLe<transdrivelineengaged_03> transdrivelineengaged_03{0, 2};
  CanSignalLe<trnstrqconverterlockupengaged_03> trnstrqconverterlockupengaged_03{0, 2};
  CanSignalLe<transshiftinprocess_03> transshiftinprocess_03{0, 2};
  CanSignalLe<transtorquelockupprocess_03> transtorquelockupprocess_03{0, 2};
  CanSignalLe<std::uint16_t> transoutputshaftspeed_03{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> percentclutchslip_03{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<engmomentaryoverspeedenable_03> engmomentaryoverspeedenable_03{0, 2};
  CanSignalLe<progressiveshiftdisable_03> progressiveshiftdisable_03{0, 2};
  CanSignalLe<momentaryengmaxpowerenable_03> momentaryengmaxpowerenable_03{0, 2};
  CanSignalLe<std::uint16_t> transinputshaftspeed_03{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> srcaddrssofcntrldvcfrtrnsctrl_03{0, 8, 1, 0, 0, 253};
  CanSignalLe<std::uint8_t> IlluminationBrightnessPercent_21{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<OvrdCtrlModePriority_0_24> OvrdCtrlModePriority_0_24{0, 2};
  CanSignalLe<std::uint8_t> EngRqstdTrqLimit_0_24{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngRqstdSpdLimit_0_24{0, 16, 0.125, 0, 0, 8031.88};  // rpm
  CanSignalLe<EngRqstdSpdCtrlCondns_0_24> EngRqstdSpdCtrlCondns_0_24{0, 2};
  CanSignalLe<EngOvrdCtrlMode_0_24> EngOvrdCtrlMode_0_24{0, 2};
  CanSignalLe<TSC1_CtrlPurpose_F_B> TSC1_CtrlPurpose_F_B{0, 5};
  CanSignalLe<OvrdCtrlModePriority_F_B> OvrdCtrlModePriority_F_B{0, 2};
  CanSignalLe<std::uint8_t> EngRqstdTrqLimit_F_B{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngRqstdSpdLimit_F_B{0, 16, 0.125, 0, 0, 8031.88};  // rpm
  CanSignalLe<EngOvrdCtrlMode_F_B> EngOvrdCtrlMode_F_B{0, 2};
  CanSignalLe<TSC1_CtrlPurpose_0_B> TSC1_CtrlPurpose_0_B{0, 5};
  CanSignalLe<OvrdCtrlModePriority_0_B> OvrdCtrlModePriority_0_B{0, 2};
  CanSignalLe<std::uint8_t> EngRqstdTrqLimit_0_B{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngRqstdSpdLimit_0_B{0, 16, 0.125, 0, 0, 8031.88};  // rpm
  CanSignalLe<std::uint32_t> trip_time_in_topgear{0, 32, 0.05, 0, 0, 210554060.75};  // hour
  CanSignalLe<std::uint32_t> trip_time_in_gear_down{0, 32, 0.05, 0, 0, 210554060.75};  // hour
  CanSignalLe<std::uint32_t> trip_time_in_derate_by_engine{0, 32, 0.05, 0, 0, 210554060.75};  // hour
  CanSignalLe<std::uint32_t> trip_time_in_vsl{0, 32, 0.05, 0, 0, 210554060.75};  // hour
  CanSignalLe<std::uint8_t> xbr_message_checksum{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> xbr_message_counter{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> xbr_urgency{0, 8, 0.4, 0, 0, 100};
  CanSignalLe<std::uint8_t> xbr_control_mode{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> xbr_priority{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> xbr_ebi_mode{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint16_t> external_acceleration_demand{0, 16, 0.00048828125, -15.687, -15.687, 5.687};  // m/s�
  CanSignalLe<std::uint8_t> road_departure_aebs_state_2A{0, 4, 1, 0, 0, 15};  // 16 states/4 bit
  CanSignalLe<std::uint8_t> timetocollisionrelevantobj_2A{0, 8, 0.05, 0, 0, 12.5};  // s/bit
  CanSignalLe<std::uint8_t> bendoff_probab_relvnt_object_2A{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> relevant_object_detect_aebs_2A{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> collision_warning_level_2A{0, 4, 1, 0, 0, 15};  // 16 states/4 bit
  CanSignalLe<std::uint8_t> advan_emer_braking_sys_state_2A{0, 4, 1, 0, 0, 15};  // 16 states/4 bit
  CanSignalLe<std::uint8_t> EcoRolPerSta_ERPS{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> BraPedSwi2_B2{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi9_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi8_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi7_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi6_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi5_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi4_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi3_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi2_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxStaFlyTorPoi1_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi9_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi8_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi7_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi6_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi5_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi4_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi3_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi2_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MinStaFlyTorPoi1_EngRCHiRes{0, 8, 4, -1000, -1000, 0};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi9_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi8_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi7_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi6_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi5_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi4_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi3_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi2_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> MaxEngBraFlyTorPoi1_EngRCHiRes{0, 8, 20, 0, 0, 5000};  // Nm
  CanSignalLe<std::uint8_t> EngSpePoi9_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi8_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi7_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi6_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi5_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi4_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi3_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi2_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<std::uint8_t> EngSpePoi1_EngRCHiRes{0, 8, 15, 0, 0, 3750};  // rpm
  CanSignalLe<PCC_Drvln_Diseng_Inhbt_Req> PCC_Drvln_Diseng_Inhbt_Req{0, 2};
  CanSignalLe<Tra_Idle_Gov_Fuel_Inhib_Support> Tra_Idle_Gov_Fuel_Inhib_Support{0, 2};
  CanSignalLe<std::uint16_t> Outlet_Corrected_NOx{0, 16, 0.05, -200, 0, 3012.75};  // ppm
  CanSignalLe<TC1_Trans_Mode4_BobTail> TC1_Trans_Mode4_BobTail{0, 2};
  CanSignalLe<TC1_ECoast_Ctrl> TC1_ECoast_Ctrl{0, 2};
  CanSignalLe<Clstr_Dead_Pedal_Txt_Alert> Clstr_Dead_Pedal_Txt_Alert{0, 2};
  CanSignalLe<Clstr_CAP_Txt_Alert> Clstr_CAP_Txt_Alert{0, 2};
  CanSignalLe<Clstr_HC_Desorb_Txt_Alert> Clstr_HC_Desorb_Txt_Alert{0, 2};
  CanSignalLe<Clstr_Parked_Regen_Txt_Alert> Clstr_Parked_Regen_Txt_Alert{0, 2};
  CanSignalLe<Clstr_Regen_Fail_Txt_Alert> Clstr_Regen_Fail_Txt_Alert{0, 2};
  CanSignalLe<std::uint8_t> Flame_Start_Cntrl_Sol_Status{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Glow_Plug_Relay_Enable_Status{0, 8, 1, 0, 0, 255};
  CanSignalLe<Status_Actuator1> Status_Actuator1{0, 2};
  CanSignalLe<Status_Actuator0> Status_Actuator0{0, 2};
  CanSignalLe<std::uint16_t> Fan_Control_Duty_Cycle{0, 16, 0.00001556, 0, 0, 1};
  CanSignalLe<std::uint16_t> Des_Exh_Man_Gauge_Press{0, 16, 0.015625, 0, 0, 1003.984375};  // kPa
  CanSignalLe<std::uint16_t> VGT_Span_Perc_EOL{0, 16, 0.1, 0, 0, 150};  // %
  CanSignalLe<PM_Regen_Status> PM_Regen_Status{0, 2};
  CanSignalLe<std::uint16_t> PM_Probe_Temperature{0, 16, 0.03125, -273.15, -50, 949.85};  // �C
  CanSignalLe<PM_Measurement_Status> PM_Measurement_Status{0, 2};
  CanSignalLe<std::uint8_t> EGR_Temp1_Pres_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> CAVD_Cyl6{0, 8, 0.1, -12.1, -12.1, 12.1};  // rpm/s
  CanSignalLe<std::uint8_t> CAVD_Cyl5{0, 8, 0.1, -12.1, -12.1, 12.1};  // rpm/s
  CanSignalLe<std::uint8_t> CAVD_Cyl4{0, 8, 0.1, -12.1, -12.1, 12.1};  // rpm/s
  CanSignalLe<std::uint8_t> CAVD_Cyl3{0, 8, 0.1, -12.1, -12.1, 12.1};  // rpm/s
  CanSignalLe<std::uint8_t> CAVD_Cyl2{0, 8, 0.1, -12.1, -12.1, 12.1};  // rpm/s
  CanSignalLe<std::uint8_t> CAVD_Cyl1{0, 8, 0.1, -12.1, -12.1, 12.1};  // rpm/s
  CanSignalLe<std::uint8_t> DEF_Pressure_Sensor_Voltage{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Exh_Man_T_Raw_V{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> TCOT_Raw_V{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Intake_Air_Temp_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Oil_Temp_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Intake_Manifold_Temp_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Coolant_Temperature_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Boost_Air_Temp_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> DPFOP_Sensor_Voltage{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> DPFDP_Sensor_Voltage{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Exh_Man_P_Raw_V{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Supply_Fuel_Pres_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Oil_Pres_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Fuel_Pressure_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Boost_Pressure_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Atmospheric_Pressure_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint16_t> Exh_Man_Temp{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint32_t> Trip_Fan_On_Time{0, 32, 0.05, 0, 0, 210554060.75};  // hours
  CanSignalLe<std::uint16_t> PM_Sensor_Current{0, 16, 0.00390625, 0, 0, 250.99609375};  // �A
  CanSignalLe<std::uint8_t> Accel_Pedal_1_Chan_2{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> Intake_Corrected_NOx{0, 16, 0.05, -200, 0, 3012.75};  // ppm
  CanSignalLe<Cruise_Control_Resume_Command> Cruise_Control_Resume_Command{0, 2};
  CanSignalLe<Cruise_Control_Pause_Command> Cruise_Control_Pause_Command{0, 2};
  CanSignalLe<Cruise_Control_Disable_Command> Cruise_Control_Disable_Command{0, 2};
  CanSignalLe<std::uint16_t> Heater_Resistance{0, 16, 0.0039063, 0, 0, 50};  // ohm
  CanSignalLe<Htr_OFF_Pwr_Out_Rng_Hi_Di_Status> Htr_OFF_Pwr_Out_Rng_Hi_Di_Status{0, 2};
  CanSignalLe<Htr_ON_Pwr_Out_Rng_Lo_Di_Status> Htr_ON_Pwr_Out_Rng_Lo_Di_Status{0, 2};
  CanSignalLe<Htr_ON_Pwr_Out_Rng_Hi_Di_Status> Htr_ON_Pwr_Out_Rng_Hi_Di_Status{0, 2};
  CanSignalLe<CAN_MSG_Time_Out_Di_Status> CAN_MSG_Time_Out_Di_Status{0, 2};
  CanSignalLe<Boost_Volt_Out_Of_Rng_Di_Status> Boost_Volt_Out_Of_Rng_Di_Status{0, 2};
  CanSignalLe<Boost_Vol_Ip_Ovr_Volt_Di_Status> Boost_Vol_Ip_Ovr_Volt_Di_Status{0, 2};
  CanSignalLe<Boost_Volt_Ip_Shrt_GND_Di_Status> Boost_Volt_Ip_Shrt_GND_Di_Status{0, 2};
  CanSignalLe<Boost_Volt_Ip_Shrt_Bat_Di_Status> Boost_Volt_Ip_Shrt_Bat_Di_Status{0, 2};
  CanSignalLe<Hi_Sensor_Ip_Short_GND_Di_Status> Hi_Sensor_Ip_Short_GND_Di_Status{0, 2};
  CanSignalLe<Hi_Sensor_Ip_Short_Bat_Di_Status> Hi_Sensor_Ip_Short_Bat_Di_Status{0, 2};
  CanSignalLe<Htr_Lo_Drive_Short_GND_Di_Status> Htr_Lo_Drive_Short_GND_Di_Status{0, 2};
  CanSignalLe<Htr_Lo_Drive_Over_Crnt_Di_Status> Htr_Lo_Drive_Over_Crnt_Di_Status{0, 2};
  CanSignalLe<Htr_Lo_Drive_Opn_Cir_Di_Status> Htr_Lo_Drive_Opn_Cir_Di_Status{0, 2};
  CanSignalLe<Htr_Lo_Drive_Short_Bat_Di_Status> Htr_Lo_Drive_Short_Bat_Di_Status{0, 2};
  CanSignalLe<Htr_Hi_Drive_Short_Bat_Di_Status> Htr_Hi_Drive_Short_Bat_Di_Status{0, 2};
  CanSignalLe<Htr_Hi_Drive_Over_Crnt_Di_Status> Htr_Hi_Drive_Over_Crnt_Di_Status{0, 2};
  CanSignalLe<Htr_Hi_Drive_Opn_Diag_Status> Htr_Hi_Drive_Opn_Diag_Status{0, 2};
  CanSignalLe<Htr_Hi_Drive_Short_GND_Di_Status> Htr_Hi_Drive_Short_GND_Di_Status{0, 2};
  CanSignalLe<std::uint8_t> Part_Sen_Max_Resistance{0, 8, 250, 0, 0, 62500};  // kOhm
  CanSignalLe<std::uint16_t> Part_Sensor_Temperature{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint8_t> Part_Sen_Regen_Failed_Count{0, 8, 1, 0, 0, 250};  // counts
  CanSignalLe<std::uint8_t> Part_Sensor_Power_Supply{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Part_Sensor_Detection_Status{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint16_t> Part_Sen_Time_Between_Regens{0, 16, 1, 0, 0, 385530};  // s
  CanSignalLe<std::uint16_t> Part_Sen_Active_Accum_Time{0, 16, 1, 0, 0, 385530};  // s
  CanSignalLe<Particulate_Sensor_Regen_State> Particulate_Sensor_Regen_State{0, 4};
  CanSignalLe<Particulate_Sensor_Regen_Status> Particulate_Sensor_Regen_Status{0, 2};
  CanSignalLe<Particulate_Sensor_Heater_State> Particulate_Sensor_Heater_State{0, 2};
  CanSignalLe<std::uint16_t> Particulate_Sensor_Resistance{0, 16, 1, 0, 0, 62500};  // kOhm
  CanSignalLe<Fire_App_Pump_Engagement> Fire_App_Pump_Engagement{0, 2};
  CanSignalLe<std::uint16_t> Vehicle_Speed_F{0, 16, 0.00390625, 0, 0, 250.996};  // km/h
  CanSignalLe<Cruise_Control_Resume_Switch_G> Cruise_Control_Resume_Switch_G{0, 2};
  CanSignalLe<Cruise_Control_Pause_Switch> Cruise_Control_Pause_Switch{0, 2};
  CanSignalLe<std::uint8_t> Req_ACC_Distance_Mode{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SeatBeltSwitch_21{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Veh_Limit_Speed_Gov_Enable{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Eng_Auto_Start_Enable_Switch{0, 2, 1, 0, 0, 3};
  CanSignalLe<DPF_Regen_Force_Switch_C> DPF_Regen_Force_Switch_C{0, 2};
  CanSignalLe<DPF_Regen_Inhibit_Switch_C> DPF_Regen_Inhibit_Switch_C{0, 2};
  CanSignalLe<Veh_Lim_Spd_Gov_Inc_Switch_A> Veh_Lim_Spd_Gov_Inc_Switch_A{0, 2};
  CanSignalLe<Veh_Lim_Spd_Gov_Dec_Switch> Veh_Lim_Spd_Gov_Dec_Switch{0, 2};
  CanSignalLe<std::uint8_t> Req_Percent_Fan_Speed_A{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Remote_Accel_Pedal_Position_D{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> EGRV_Position_Err{0, 16, 0.004, -125, -125, 132.02};  // %
  CanSignalLe<Fuel_Type_A> Fuel_Type_A{0, 8};
  CanSignalLe<std::uint16_t> TC2_Compressor_Out_Temperature{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> TC1_Compressor_Out_Temperature{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint32_t> Engine_State_A{0, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint8_t> Exhaust_Back_Press_CTL{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> TC1_Turbine_Outlet_Pressure{0, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint8_t> TC1_Turbine_Outlet_Press_Desired{0, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint8_t> Actual_Engine_Torque{0, 8, 30, 0, 0, 7500};  // Nm
  CanSignalLe<Adaptive_Cruise_Control_Mode> Adaptive_Cruise_Control_Mode{0, 3};
  CanSignalLe<std::uint16_t> AFT_O2_Inlet_Percent_Desired{0, 16, 0.0025, 0, 0, 160.6375};  // %
  CanSignalLe<std::uint8_t> Fuel_Temperature{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint16_t> Fuel_Pressure_2{0, 16, 0.1, 0, 0, 6425.5};  // kPa
  CanSignalLe<std::uint16_t> Cylinder_6_Ignition_Timing{0, 16, 0.0078125, -200, -200, 301.99};  // Deg
  CanSignalLe<std::uint16_t> Cylinder_5_Ignition_Timing{0, 16, 0.0078125, -200, -200, 301.99};  // Deg
  CanSignalLe<std::uint16_t> Cylinder_4_Ignition_Timing{0, 16, 0.0078125, -200, -200, 301.99};  // Deg
  CanSignalLe<std::uint16_t> Cylinder_3_Ignition_Timing{0, 16, 0.0078125, -200, -200, 301.99};  // Deg
  CanSignalLe<std::uint16_t> Cylinder_2_Ignition_Timing{0, 16, 0.0078125, -200, -200, 301.99};  // Deg
  CanSignalLe<std::uint16_t> Cylinder_1_Ignition_Timing{0, 16, 0.0078125, -200, -200, 301.99};  // Deg
  CanSignalLe<std::uint8_t> Eng_Intake_Manifold_Pressure{0, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint8_t> enginecoolantpumpoutlettemp_0{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> Engine_Coolant_Temperature_2{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint16_t> DEF_Pump_Motor_Speed{0, 16, 0.5, 0, 0, 32127.5};  // rpm
  CanSignalLe<std::uint8_t> Desired_Fan_Speed{0, 8, 14, 0, 0, 3500};  // rpm
  CanSignalLe<std::uint8_t> Desired_Boost_Pressure{0, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint16_t> Main_Fuel_Injection_Pulse_Width{0, 16, 0.58, 0, 0, 37268};  // microsec
  CanSignalLe<std::uint8_t> Lambda_Measured_Value{0, 8, 0.1, 0, 0, 250};
  CanSignalLe<std::uint8_t> Lambda_Temperature_Value{0, 8, 1, 0, 0, 250};  // �C
  CanSignalLe<SCR_Warm_Up_Status> SCR_Warm_Up_Status{0, 2};
  CanSignalLe<Retarder_Inh_EBS_Switch2_Status> Retarder_Inh_EBS_Switch2_Status{0, 2};
  CanSignalLe<Transfer_Case_Status_B> Transfer_Case_Status_B{0, 2};
  CanSignalLe<std::uint8_t> Accel_Ped_Pos_2_Volts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> Intake_Air_Temp_Volts_A{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> RPSAD_Counts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<std::uint8_t> APSAD_Counts{0, 8, 20, 0, 0, 5000};  // mV
  CanSignalLe<Retarder_Inh_EBS_Switch_Status> Retarder_Inh_EBS_Switch_Status{0, 2};
  CanSignalLe<Change_Oil_Lamp_Status> Change_Oil_Lamp_Status{0, 2};
  CanSignalLe<Electronic_Fan_Control_Status> Electronic_Fan_Control_Status{0, 2};
  CanSignalLe<std::uint8_t> Fuel_Temp_Volts_A{0, 8, 0.0196, 0, 0, 4.998};  // V
  CanSignalLe<std::uint8_t> Fuel_Press_2_Volts_A{0, 8, 0.0196, 0, 0, 4.998};  // V
  CanSignalLe<std::uint8_t> DPF_Outlet_Temp_Volts{0, 8, 0.0196, 0, 0, 4.998};  // V
  CanSignalLe<std::uint8_t> DPF_Delta_Pressure_Volts{0, 8, 0.0196, 0, 0, 4.998};  // V
  CanSignalLe<std::uint8_t> Doser_Fuel_Pressure_Volts{0, 8, 0.0196, 0, 0, 4.998};  // V
  CanSignalLe<std::uint8_t> DOC_Outlet_Temp_Volts_A{0, 8, 0.0196, 0, 0, 4.998};  // V
  CanSignalLe<std::uint8_t> DOC_Inlet_Temp_Volts_A{0, 8, 0.0196, 0, 0, 4.998};  // V
  CanSignalLe<std::uint16_t> Fuel_Pressure_Control_Cmd{0, 16, 0.0025, 0, 0, 160.6375};  // %
  CanSignalLe<std::uint16_t> Eng_Throttle_Act_1_Control_Cmd_A{0, 16, 0.0025, 0, 0, 160.6375};  // %
  CanSignalLe<std::uint16_t> Fuel_Volume_Control_Cmd{0, 16, 0.0025, 0, 0, 160.6375};  // %
  CanSignalLe<std::uint8_t> TC2_Wastegate_Control_A{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> TC1_Wastegate_Control{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> engintakevalveactuationsystem{0, 16, 1, -273, -273, 1734.96875};
  CanSignalLe<std::uint16_t> engcoolanttemphighresolution{0, 16, 0.03125, -273, -273, 1734.96875};
  CanSignalLe<std::uint16_t> CAC_Outlet_Temperature_A{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> Eng_Intake_Mainfold_Temperature{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> ReservedforSAEassignment_DM29{0, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint8_t> PermanentDTCs_DM29{0, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint8_t> PreviouslyMILOnDTCs_DM29{0, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint8_t> MILOnDTCs_DM29{0, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint8_t> AllPendingDTCs_DM29{0, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint8_t> PendingDTCs_DM29{0, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint8_t> CM_SPN10_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN10_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN10High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN10_DM28> FMI_SPN10_DM28{0, 5};
  CanSignalLe<std::uint16_t> SPN10_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN9_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN9_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN9High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN9_DM28> FMI_SPN9_DM28{0, 5};
  CanSignalLe<std::uint16_t> SPN9_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN8_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN8_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN8High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN8_DM28> FMI_SPN8_DM28{0, 5};
  CanSignalLe<std::uint16_t> SPN8_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN7_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN7_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN7High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN7_DM28> FMI_SPN7_DM28{0, 5};
  CanSignalLe<std::uint16_t> SPN7_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN6_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN6_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN6High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN6_DM28> FMI_SPN6_DM28{0, 5};
  CanSignalLe<std::uint16_t> SPN6_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> OC_SPN5_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN4_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN3_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN2_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> CM_SPN5_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN4_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN3_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN2_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SPN5High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint16_t> SPN5_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> SPN4High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint16_t> SPN4_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> SPN3High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint16_t> SPN3_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> SPN2High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint16_t> SPN2_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> SPN1High_DM28{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint16_t> SPN1_DM28{0, 16, 1, 0, 0, 65536};
  CanSignalLe<FMI_SPN5_DM28> FMI_SPN5_DM28{0, 5};
  CanSignalLe<FMI_SPN4_DM28> FMI_SPN4_DM28{0, 5};
  CanSignalLe<FMI_SPN3_DM28> FMI_SPN3_DM28{0, 5};
  CanSignalLe<FMI_SPN2_DM28> FMI_SPN2_DM28{0, 5};
  CanSignalLe<FMI_SPN1_DM28> FMI_SPN1_DM28{0, 5};
  CanSignalLe<FlashRedStopLamp_DM28> FlashRedStopLamp_DM28{0, 2};
  CanSignalLe<FlashProtectLamp_DM28> FlashProtectLamp_DM28{0, 2};
  CanSignalLe<FlashMalfuncIndicatorLamp_DM28> FlashMalfuncIndicatorLamp_DM28{0, 2};
  CanSignalLe<FlashAmberWarningLamp_DM28> FlashAmberWarningLamp_DM28{0, 2};
  CanSignalLe<RedStopLampStatus_DM28> RedStopLampStatus_DM28{0, 2};
  CanSignalLe<ProtectLampStatusDM28> ProtectLampStatusDM28{0, 2};
  CanSignalLe<AmberWarningLampStatus_DM28> AmberWarningLampStatus_DM28{0, 2};
  CanSignalLe<MalfuncIndicatorLampStatus_DM28> MalfuncIndicatorLampStatus_DM28{0, 2};
  CanSignalLe<std::uint8_t> OC_SPN1_DM28{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> CM_SPN1_DM28{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint32_t> SPNData_DM25{0, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint8_t> ExpandedFreezeFrameLength_DM25{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> CM_SPN1_DM25{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN1_DM25{0, 7, 1, 0, 0, 126};
  CanSignalLe<FMI_SPN1_DM25> FMI_SPN1_DM25{0, 5};
  CanSignalLe<std::uint16_t> SPN1_DM25{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> SPN1High_DM25{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SuppScaledTestResults_SPN5_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppScaledTestResults_SPN4_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppScaledTestResults_SPN3_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppScaledTestResults_SPN2_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppScaledTestResults_SPN1_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppDataStream_SPN5_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppDataStream_SPN4_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppDataStream_SPN3_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppDataStream_SPN2_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppDataStream_SPN1_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppExpFreezeFrame_SPN5_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppExpFreezeFrame_SPN4_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppExpFreezeFrame_SPN3_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppExpFreezeFrame_SPN2_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SuppExpFreezeFrame_SPN1_DM24{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> SPN5DataLength_DM24{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> SPN4DataLength_DM24{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> SPN3DataLength_DM24{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> SPN2DataLength_DM24{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> SPN1DataLength_DM24{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint16_t> SPN5_DM24{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN4_DM24{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN3_DM24{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN2_DM24{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN1_DM24{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> SPN5High_DM24{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN4High_DM24{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN3High_DM24{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN2High_DM24{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN1High_DM24{0, 3, 1, 0, 0, 7};
  CanSignalLe<NOx_Quality_Indicator> NOx_Quality_Indicator{0, 3};
  CanSignalLe<std::uint16_t> Turbine_Out_Exhaust_O2{0, 16, 0.003, 0, 0, 196.605};  // g/s
  CanSignalLe<std::uint16_t> Turbine_Out_Exhaust_NOx{0, 16, 0.05, -200, -200, 3012.75};  // ppm
  CanSignalLe<std::uint8_t> Turbine_Out_Exhaust_NO2{0, 8, 0.004, 0, 0, 1};  // ratio
  CanSignalLe<std::uint16_t> Turbine_Out_Exhaust_PM{0, 16, 0.03125, 0, 0, 2047.96875};  // g/hr
  CanSignalLe<std::uint16_t> Operator_Mode_Permit{0, 16, 1, 0, 0, 65535};
  CanSignalLe<Engine_Operating_Mode> Engine_Operating_Mode{0, 4};
  CanSignalLe<Engine_Operating_State_A> Engine_Operating_State_A{0, 4};
  CanSignalLe<std::uint16_t> Mass_Flow_Rate_Prop{0, 16, 0.125, -4096, -4096, 4095.875};  // g/s
  CanSignalLe<std::uint8_t> Drive_Cycle_Free_Running{0, 8, 1, 0, 0, 250};
  CanSignalLe<ACM_Test_Reset> ACM_Test_Reset{0, 2};
  CanSignalLe<DPF_Reg_Release_Indicator> DPF_Reg_Release_Indicator{0, 2};
  CanSignalLe<std::uint16_t> Requested_ILT_Posistion{0, 16, 0.00195313, 0, 0, 127.99837455};  // %
  CanSignalLe<std::uint16_t> Actual_ILT_Position{0, 16, 0.00195313, 0, 0, 127.99837455};  // %
  CanSignalLe<std::uint16_t> Actual_EGR_Position{0, 16, 0.00195313, 0, 0, 127.99837455};  // %
  CanSignalLe<std::uint16_t> Requested_EGR_Posistion{0, 16, 0.00195313, 0, 0, 127.99837455};  // %
  CanSignalLe<FMI_SPN5_DM32> FMI_SPN5_DM32{0, 5};
  CanSignalLe<FMI_SPN4_DM32> FMI_SPN4_DM32{0, 5};
  CanSignalLe<FMI_SPN3_DM32> FMI_SPN3_DM32{0, 5};
  CanSignalLe<std::uint16_t> DTC_Total_PrevActive_Time_5_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_Active_Time_5_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint8_t> DTC_Time_Until_Derate_5_DM32{0, 8, 0.25, 0, 0, 63.75};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_PrevActive_Time_4_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_Active_Time_4_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint8_t> DTC_Time_Until_Derate_4_DM32{0, 8, 0.25, 0, 0, 63.75};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_PrevActive_Time_3_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_Active_Time_3_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint8_t> DTC_Time_Until_Derate_3_DM32{0, 8, 0.25, 0, 0, 63.75};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_PrevActive_Time_2_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_Active_Time_2_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint8_t> DTC_Time_Until_Derate_2_DM32{0, 8, 0.25, 0, 0, 63.75};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_PrevActive_Time_1_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint16_t> DTC_Total_Active_Time_1_DM32{0, 16, 0.2, 0, 0, 13107};  // hr
  CanSignalLe<std::uint8_t> DTC_Time_Until_Derate_1_DM32{0, 8, 0.25, 0, 0, 63.75};  // hr
  CanSignalLe<std::uint16_t> SPN5_DM32{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN4_DM32{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN3_DM32{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN2_DM32{0, 16, 1, 0, 0, 65536};
  CanSignalLe<FMI_SPN1_DM32> FMI_SPN1_DM32{0, 5};
  CanSignalLe<std::uint16_t> SPN1_DM32{0, 16, 1, 0, 0, 65536};
  CanSignalLe<FMI_SPN2_DM32> FMI_SPN2_DM32{0, 5};
  CanSignalLe<std::uint8_t> SPN5High_DM32{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN4High_DM32{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN3High_DM32{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN2High_DM32{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN1High_DM32{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint16_t> Engine_Actual_Ignition_Timing{0, 16, 0.0078125, -200, -200, 301.99};  // Deg
  CanSignalLe<progressiveshiftdisable_0> progressiveshiftdisable_0{0, 2};
  CanSignalLe<engmomentaryoverspeedenable_0> engmomentaryoverspeedenable_0{0, 2};
  CanSignalLe<transshiftinprocess_0> transshiftinprocess_0{0, 2};
  CanSignalLe<std::uint16_t> transinputshaftspeed_0{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<transdrivelineengaged_0> transdrivelineengaged_0{0, 2};
  CanSignalLe<std::uint16_t> Trans_Output_Shaft_Speed{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint32_t> Operating_Mode_Status{0, 32, 1, 0, 0, 4294967295};
  CanSignalLe<Cold_Start_Relay_Diag> Cold_Start_Relay_Diag{0, 2};
  CanSignalLe<Cold_Start_Relay> Cold_Start_Relay{0, 2};
  CanSignalLe<std::uint8_t> Cold_Start_Fuel_Igniter{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<Engine_Crank_Inhibit_Flame_Start> Engine_Crank_Inhibit_Flame_Start{0, 4};
  CanSignalLe<Cold_Start_Fuel_Solenoid> Cold_Start_Fuel_Solenoid{0, 2};
  CanSignalLe<std::uint8_t> seat_belt_switch{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Estimated_Percent_Fan_Speed_A{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<DPF_Regen_Inhibit_Switch> DPF_Regen_Inhibit_Switch{0, 2};
  CanSignalLe<DPF_Regen_Force_Switch> DPF_Regen_Force_Switch{0, 2};
  CanSignalLe<std::uint16_t> TestLimitMinimum_DM8{0, 16, 1, 0, 0, 64255};
  CanSignalLe<std::uint16_t> TestLimitMaximum_DM8{0, 16, 1, 0, 0, 64255};
  CanSignalLe<std::uint16_t> TestValue_DM8{0, 16, 1, 0, 0, 64255};
  CanSignalLe<std::uint8_t> TestType_DM8{0, 8, 1, 0, 1, 64};
  CanSignalLe<TestIdentifier_DM8_E> TestIdentifier_DM8_E{0, 8};
  CanSignalLe<std::uint8_t> CM_SPN10_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN10_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN10High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN10_DM6> FMI_SPN10_DM6{0, 5};
  CanSignalLe<std::uint16_t> SPN10_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN9_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN9_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN9High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN9_DM6> FMI_SPN9_DM6{0, 5};
  CanSignalLe<std::uint16_t> SPN9_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN8_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN8_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN8High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN8_DM6> FMI_SPN8_DM6{0, 5};
  CanSignalLe<std::uint16_t> SPN8_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN7_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN7_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN7High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN7_DM6> FMI_SPN7_DM6{0, 5};
  CanSignalLe<std::uint16_t> SPN7_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN6_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN6_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN6High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN6_DM6> FMI_SPN6_DM6{0, 5};
  CanSignalLe<std::uint16_t> SPN6_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<FlashMalfuncIndicatorLamp_DM6> FlashMalfuncIndicatorLamp_DM6{0, 2};
  CanSignalLe<FlashRedStopLamp_DM6> FlashRedStopLamp_DM6{0, 2};
  CanSignalLe<FlashAmberWarningLamp_DM6> FlashAmberWarningLamp_DM6{0, 2};
  CanSignalLe<FlashProtectLamp_DM6> FlashProtectLamp_DM6{0, 2};
  CanSignalLe<std::uint16_t> SPN5_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN4_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN3_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN2_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN1_DM6{0, 16, 1, 0, 0, 65536};
  CanSignalLe<RedStopLampStatus_DM6> RedStopLampStatus_DM6{0, 2};
  CanSignalLe<ProtectLampStatusDM6> ProtectLampStatusDM6{0, 2};
  CanSignalLe<AmberWarningLampStatus_DM6> AmberWarningLampStatus_DM6{0, 2};
  CanSignalLe<MalfunIndicatorLampStatus_DM6> MalfunIndicatorLampStatus_DM6{0, 2};
  CanSignalLe<FMI_SPN5_DM6> FMI_SPN5_DM6{0, 5};
  CanSignalLe<FMI_SPN4_DM6> FMI_SPN4_DM6{0, 5};
  CanSignalLe<FMI_SPN3_DM6> FMI_SPN3_DM6{0, 5};
  CanSignalLe<FMI_SPN2_DM6> FMI_SPN2_DM6{0, 5};
  CanSignalLe<std::uint8_t> CM_SPN5_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN4_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN3_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN2_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN5_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN4_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN3_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN2_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN1_DM6{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> CM_SPN1_DM6{0, 1, 1, 0, 0, 1};
  CanSignalLe<FMI_SPN1_DM6> FMI_SPN1_DM6{0, 5};
  CanSignalLe<std::uint8_t> SPN5High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN4High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN3High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN2High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN1High_DM6{0, 3, 1, 0, 0, 7};
  CanSignalLe<PMFilterMonStatus_DM5> PMFilterMonStatus_DM5{0, 1};
  CanSignalLe<NOxCnvrtctlystMonStatus_DM5> NOxCnvrtctlystMonStatus_DM5{0, 1};
  CanSignalLe<NHMCCnvrtctlystMonStatus_DM5> NHMCCnvrtctlystMonStatus_DM5{0, 1};
  CanSignalLe<BstPressCtrlSystemMonStatus_DM5> BstPressCtrlSystemMonStatus_DM5{0, 1};
  CanSignalLe<NOxCnvrtctlystMonSupp_DM5> NOxCnvrtctlystMonSupp_DM5{0, 1};
  CanSignalLe<PMFilterMonSupp_DM5> PMFilterMonSupp_DM5{0, 1};
  CanSignalLe<NHMCCnvrtctlystMonSupp_DM5> NHMCCnvrtctlystMonSupp_DM5{0, 1};
  CanSignalLe<BstPressCtrlSystemMonSupp_DM5> BstPressCtrlSystemMonSupp_DM5{0, 1};
  CanSignalLe<MisfireMonitoringSupport_DM5> MisfireMonitoringSupport_DM5{0, 1};
  CanSignalLe<OBDCompliance_DM5> OBDCompliance_DM5{0, 8};
  CanSignalLe<std::uint8_t> PreviouslyActiveTroubleCodes_DM5{0, 8, 1, 0, 0, 240};
  CanSignalLe<std::uint8_t> ActiveTroubleCodes_DM5{0, 8, 1, 0, 0, 240};
  CanSignalLe<SecondAirSystemMonStatus_DM5> SecondAirSystemMonStatus_DM5{0, 1};
  CanSignalLe<EvaporativeSystemMonStatus_DM5> EvaporativeSystemMonStatus_DM5{0, 1};
  CanSignalLe<ACSystemRefrigerantMonStatus_DM5> ACSystemRefrigerantMonStatus_DM5{0, 1};
  CanSignalLe<OxygenSensorHeaterMonStatus_DM5> OxygenSensorHeaterMonStatus_DM5{0, 1};
  CanSignalLe<OxygenSensorMonStatus_DM5> OxygenSensorMonStatus_DM5{0, 1};
  CanSignalLe<EGRSystemMonitoringStatus_DM5> EGRSystemMonitoringStatus_DM5{0, 1};
  CanSignalLe<CatalystMonStatus_DM5> CatalystMonStatus_DM5{0, 1};
  CanSignalLe<ColdStartAidSystemMonStatus_DM5> ColdStartAidSystemMonStatus_DM5{0, 1};
  CanSignalLe<HeatedCatalystMonStatus_DM5> HeatedCatalystMonStatus_DM5{0, 1};
  CanSignalLe<CatalystMonSupp_DM5> CatalystMonSupp_DM5{0, 1};
  CanSignalLe<ColdStartAidSystemMonSupp_DM5> ColdStartAidSystemMonSupp_DM5{0, 1};
  CanSignalLe<HeatedCatalystMonSupp_DM5> HeatedCatalystMonSupp_DM5{0, 1};
  CanSignalLe<EvaporativeSystemMonSupp_DM5> EvaporativeSystemMonSupp_DM5{0, 1};
  CanSignalLe<SecondAirSystemMonSupp_DM5> SecondAirSystemMonSupp_DM5{0, 1};
  CanSignalLe<ACSystemRefrigerantMonSupp_DM5> ACSystemRefrigerantMonSupp_DM5{0, 1};
  CanSignalLe<OxygenSensorMonSupp_DM5> OxygenSensorMonSupp_DM5{0, 1};
  CanSignalLe<OxygenSensorHeaterMonSupp_DM5> OxygenSensorHeaterMonSupp_DM5{0, 1};
  CanSignalLe<EGRSystemMonitoringSupp_DM5> EGRSystemMonitoringSupp_DM5{0, 1};
  CanSignalLe<MisfireMonitoringStatus_DM5> MisfireMonitoringStatus_DM5{0, 1};
  CanSignalLe<ComprehensiveCompMonStatus_DM5> ComprehensiveCompMonStatus_DM5{0, 1};
  CanSignalLe<FuelSystemMonitoringStatus_DM5> FuelSystemMonitoringStatus_DM5{0, 1};
  CanSignalLe<CompreComponentMonSupp_DM5> CompreComponentMonSupp_DM5{0, 1};
  CanSignalLe<FuelSystemMonitoringSupport_DM5> FuelSystemMonitoringSupport_DM5{0, 1};
  CanSignalLe<std::uint16_t> SPN1_DM4{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> Engine_Speed_DM4{0, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> Intake_Manifold_Pressure_DM4{0, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint8_t> Engine_Load_DM4{0, 8, 1, 0, 0, 250};  // %
  CanSignalLe<std::uint16_t> Vehicle_Speed_DM4{0, 16, 0.00390625, 0, 0, 250.996};  // km/h
  CanSignalLe<std::uint8_t> OC_SPN1_DM4{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> CM_SPN1_DM4{0, 1, 1, 0, 0, 1};
  CanSignalLe<FMI_SPN1_DM4> FMI_SPN1_DM4{0, 5};
  CanSignalLe<std::uint8_t> FreezeFrameLength_DM4{0, 8, 1, 0, 0, 255};  // byte
  CanSignalLe<Engine_Torque_Mode_DM4> Engine_Torque_Mode_DM4{0, 4};
  CanSignalLe<std::uint8_t> Engine_Coolant_Temperature_DM4{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> SPN1High_DM4{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> CM_SPN10_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN10_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN10High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN10_DM23> FMI_SPN10_DM23{0, 5};
  CanSignalLe<std::uint16_t> SPN10_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN9_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN9_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN9High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN9_DM23> FMI_SPN9_DM23{0, 5};
  CanSignalLe<std::uint16_t> SPN9_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN8_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN8_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN8High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN8_DM23> FMI_SPN8_DM23{0, 5};
  CanSignalLe<std::uint16_t> SPN8_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN7_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN7_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN7High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN7_DM23> FMI_SPN7_DM23{0, 5};
  CanSignalLe<std::uint16_t> SPN7_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN6_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN6_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN6High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN6_DM23> FMI_SPN6_DM23{0, 5};
  CanSignalLe<std::uint16_t> SPN6_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<FMI_SPN5_DM23> FMI_SPN5_DM23{0, 5};
  CanSignalLe<FMI_SPN4_DM23> FMI_SPN4_DM23{0, 5};
  CanSignalLe<FMI_SPN3_DM23> FMI_SPN3_DM23{0, 5};
  CanSignalLe<FMI_SPN2_DM23> FMI_SPN2_DM23{0, 5};
  CanSignalLe<FMI_SPN1_DM23> FMI_SPN1_DM23{0, 5};
  CanSignalLe<FlashRedStopLamp_DM23> FlashRedStopLamp_DM23{0, 2};
  CanSignalLe<FlashProtectLamp_DM23> FlashProtectLamp_DM23{0, 2};
  CanSignalLe<FlashMalfuncIndicatorLamp_DM23> FlashMalfuncIndicatorLamp_DM23{0, 2};
  CanSignalLe<FlashAmberWarningLamp_DM23> FlashAmberWarningLamp_DM23{0, 2};
  CanSignalLe<std::uint16_t> SPN5_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN4_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN3_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN2_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN1_DM23{0, 16, 1, 0, 0, 65536};
  CanSignalLe<RedStopLampStatus_DM23> RedStopLampStatus_DM23{0, 2};
  CanSignalLe<ProtectLampStatusDM23> ProtectLampStatusDM23{0, 2};
  CanSignalLe<AmberWarningLampStatus_DM23> AmberWarningLampStatus_DM23{0, 2};
  CanSignalLe<MalfunIndicatorLampStatus_DM23> MalfunIndicatorLampStatus_DM23{0, 2};
  CanSignalLe<std::uint8_t> OC_SPN1_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> CM_SPN1_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN5_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN4_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN3_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN2_DM23{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN5_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN4_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN3_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN2_DM23{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN5High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN4High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN3High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN2High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN1High_DM23{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint16_t> TimeSinceDTCsCleared_DM21{0, 16, 1, 0, 0, 64255};  // min
  CanSignalLe<std::uint16_t> MinutesRunByEngMILActivated_DM21{0, 16, 1, 0, 0, 64255};  // min
  CanSignalLe<std::uint16_t> DistanceSinceDTCsCleared_DM21{0, 16, 1, 0, 0, 64255};  // km
  CanSignalLe<std::uint16_t> DistanceWhileMILisActivated_DM21{0, 16, 1, 0, 0, 64255};  // km
  CanSignalLe<std::uint8_t> CM_SPN10_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN10_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN10High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN10_DM12> FMI_SPN10_DM12{0, 5};
  CanSignalLe<std::uint16_t> SPN10_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN9_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN9_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN9High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN9_DM12> FMI_SPN9_DM12{0, 5};
  CanSignalLe<std::uint16_t> SPN9_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN8_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN8_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN8High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN8_DM12> FMI_SPN8_DM12{0, 5};
  CanSignalLe<std::uint16_t> SPN8_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN7_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN7_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN7High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN7_DM12> FMI_SPN7_DM12{0, 5};
  CanSignalLe<std::uint16_t> SPN7_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint8_t> CM_SPN6_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN6_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN6High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<FMI_SPN6_DM12> FMI_SPN6_DM12{0, 5};
  CanSignalLe<std::uint16_t> SPN6_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<FlashRedStopLamp_DM12> FlashRedStopLamp_DM12{0, 2};
  CanSignalLe<FlashProtectLamp_DM12> FlashProtectLamp_DM12{0, 2};
  CanSignalLe<FlashMalfuncIndicatorLamp_DM12> FlashMalfuncIndicatorLamp_DM12{0, 2};
  CanSignalLe<FlashAmberWarningLamp_DM12> FlashAmberWarningLamp_DM12{0, 2};
  CanSignalLe<FMI_SPN5_DM12> FMI_SPN5_DM12{0, 5};
  CanSignalLe<FMI_SPN4_DM12> FMI_SPN4_DM12{0, 5};
  CanSignalLe<FMI_SPN3_DM12> FMI_SPN3_DM12{0, 5};
  CanSignalLe<FMI_SPN2_DM12> FMI_SPN2_DM12{0, 5};
  CanSignalLe<FMI_SPN1_DM12> FMI_SPN1_DM12{0, 5};
  CanSignalLe<std::uint16_t> SPN5_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN4_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN3_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN2_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<std::uint16_t> SPN1_DM12{0, 16, 1, 0, 0, 65536};
  CanSignalLe<RedStopLampStatus_DM12> RedStopLampStatus_DM12{0, 2};
  CanSignalLe<ProtectLampStatusDM12> ProtectLampStatusDM12{0, 2};
  CanSignalLe<AmberWarningLampStatus_DM12> AmberWarningLampStatus_DM12{0, 2};
  CanSignalLe<MalfunIndicatorLampStatus_DM12> MalfunIndicatorLampStatus_DM12{0, 2};
  CanSignalLe<std::uint8_t> OC_SPN1_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> CM_SPN1_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN5_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN4_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN3_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> CM_SPN2_DM12{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OC_SPN5_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN4_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN3_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> OC_SPN2_DM12{0, 7, 1, 0, 0, 126};
  CanSignalLe<std::uint8_t> SPN5High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN4High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN3High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN2High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN1High_DM12{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint64_t> ManufAssignedTest16to64_DM10{0, 48, 1, 0, 0, 281474976710655};
  CanSignalLe<std::uint8_t> ManufAssignedTest10to15_DM10{0, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> TestNine_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestOne_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestTwo_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestThree_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestFour_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestFive_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestSix_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestSeven_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> TestEight_DM10{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint16_t> Desi_Abs_IMP_Turbo_Boost_Limit{0, 16, 0.1, 0, 0, 6425.5};  // kPa
  CanSignalLe<std::uint16_t> engfuel1injectorail1pressure_0{0, 16, 0.00390625, 0, 0, 250.996};  // MPa
  CanSignalLe<std::uint16_t> engfuelinjectionctlpressure_0{0, 16, 0.00390625, 0, 0, 250.996};  // MPa
  CanSignalLe<std::uint16_t> EngineFluidLevelPressure2{0, 16, 0.00390625, 0, 0, 250.996};
  CanSignalLe<std::uint8_t> EngineCoolantPressure_1{0, 8, 4, 0, 0, 1000};
  CanSignalLe<std::uint16_t> engine_exhaustflowrate{0, 16, 0.2, 0, 0, 12851};  // ppm
  CanSignalLe<std::uint32_t> ctrlrq{0, 32, 0.125, -650000, -650000, 16000};  // variable
  CanSignalLe<std::uint8_t> htrswstat{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint16_t> spdrpmrq_1{0, 16, 0.5, 0, 0, 32127.5};  // rpm
  CanSignalLe<std::uint16_t> spdrpmrq{0, 16, 0.5, 0, 0, 32127.5};  // rpm
  CanSignalLe<std::uint8_t> hvcurrph_eabc{0, 8, 1, 0, 0, 100};  // Arms
  CanSignalLe<std::uint8_t> hvcurrph_ehps{0, 8, 1, 0, 0, 100};  // Arms
  CanSignalLe<std::uint16_t> engturbocharger1intakepress{0, 16, 0.0078125, -250, -250, 251.99};  // kPa
  CanSignalLe<std::uint8_t> src_add_cont_dev_retd_control_0{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> intendedretarderpercenttorque_0{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<ret_enbl_shift_sssist_switch_0> ret_enbl_shift_sssist_switch_0{0, 2};
  CanSignalLe<ret_enbl_brake_assist_awitch_0> ret_enbl_brake_assist_awitch_0{0, 2};
  CanSignalLe<retarder_torque_mode_0> retarder_torque_mode_0{0, 4};
  CanSignalLe<std::uint8_t> retarderpercenttorque_0{0, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> urge_to_move_aborted{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint16_t> engdesiredabsintakemanifoldpress{0, 16, 0.1, 0, 0, 6425.5};  // kPa/bit
  CanSignalLe<std::uint16_t> engturbo4boostpress{0, 16, 0.125, 0, 0, 8191.875};  // kPa
  CanSignalLe<std::uint16_t> engturbo3boostpress{0, 16, 0.125, 0, 0, 8191.875};  // kPa
  CanSignalLe<std::uint16_t> engturbo2boostpress{0, 16, 0.125, 0, 0, 8191.875};  // kPa
  CanSignalLe<std::uint16_t> engturbo1boostpress{0, 16, 0.125, 0, 0, 8191.875};  // kPa
  CanSignalLe<std::uint8_t> drive_axle_location{0, 8, 1, 0, 0, 255};  // states/8 bit
  CanSignalLe<std::uint8_t> drive_axle_temperature{0, 8, 1, -40, -40, 210};  // deg C/bit,
  CanSignalLe<std::uint8_t> engchargeaircoolerthermostat_21{0, 8, 0.4, 0, 0, 100};
  CanSignalLe<std::uint8_t> engineintercoolertemp_21{0, 8, 1, -40, -40, 210};
  CanSignalLe<std::uint16_t> engineturbocharger1oiltemp_21{0, 16, 0.03125, -273, -273, 1734.96875};
  CanSignalLe<std::uint16_t> Engine_Oil_Temperature_21{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint8_t> Engine_Fuel_Temperature_21{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> Engine_Coolant_Temperature_21{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> transoillvlmeasurementstatus_21{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> transmissionoillevel1cnttimer_21{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> transmission1oilpressure_21{0, 8, 16, 0, 0, 4000};
  CanSignalLe<std::uint8_t> transmissionfilterdiffpress_21{0, 8, 2, 0, 0, 500};  // Kpa
  CanSignalLe<std::uint8_t> transmissionclutch1pressure_21{0, 8, 16, 0, 0, 4000};
  CanSignalLe<std::uint8_t> transmission_oil_level_21{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> transmission_oil_temperature1_21{0, 16, 0.03125, -273, -273, 1774.96875};  // deg C/bit
  CanSignalLe<std::uint8_t> Transmission_Oil_Level_HiLo_21{0, 8, 0.5, -62.5, -62.5, 62.5};  // L/bit
  CanSignalLe<std::uint8_t> hydraulic_brk_fluid_lvlswitch_21{0, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> upshift_indicator{0, 2, 1, 0, 0, 3};  // Status
  CanSignalLe<std::uint8_t> adjustable_road_speedlim_status{0, 2, 1, 0, 0, 3};  // Status
  CanSignalLe<std::uint8_t> veh_spdlimit_override_expistatus{0, 2, 1, 0, 0, 3};  // Status
  CanSignalLe<std::uint8_t> shift_synchronization_status{0, 4, 1, 0, 0, 15};  // Status
  CanSignalLe<std::uint8_t> vehicle_speedlimit_overridstatus{0, 2, 1, 0, 0, 3};  // Status
  CanSignalLe<std::uint8_t> driver_reward_lvl_update_approac{0, 8, 1, 0, 0, 255};  // Status
  CanSignalLe<std::uint8_t> driver_reward_level{0, 8, 1, 0, 0, 255};  // Status
  CanSignalLe<std::uint8_t> front_axle_group_engage_status{0, 2, 1, 0, 0, 3};  // 4 states/2 bit,
  CanSignalLe<std::uint8_t> hydraulic_brk_fluid_lvlswitch{0, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> speed_sign_type_units{0, 4, 1, 0, 0, 15};  // Kph/Mph
  CanSignalLe<std::uint8_t> lanetrackingstatusleftside_e8{0, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> lanetrackingstatusrightside_e8{0, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> forward_lane_imager_status{0, 4, 1, 0, 0, 15};  // 16 states/4 bit
  CanSignalLe<std::uint8_t> lane_departure_enable_status{0, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> advan_emer_braking_sys_state{0, 4, 1, 0, 0, 15};  // 16 states/4 bit
  CanSignalLe<std::uint8_t> traffic_speed_limit_sign_value{0, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> excessive_overspeed{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> speed_limit_exceeded{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> dynamometer_mode{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> acc_sensor_blocked{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> brake_overuse{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> audible_feedback_control{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> cms_braking_status{0, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> Rear_Right_Turn_Signal_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Left_Turn_Signal_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Right_High_Beam_Headlamp_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Left_High_Beam_Headlamp_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Right_Low_Beam_Headlamp_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Left_Low_Beam_Headlamp_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Air_Horn_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Electric_Horn_Command{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Turn_Signal_Switch{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Main_Light_Switch{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Front_Operator_Washer_Switch{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> Front_Nonoperator_Washer_Switch{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> Front_Operator_Wiper_Switch{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Front_Nonoperator_Wiper_Switch{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint16_t> Net_Battery_Current{0, 16, 0.05, -1600, -1600, 1612.75};  // A/bit,
  CanSignalLe<std::uint8_t> Transmission_Neutral_Switch{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint32_t> ParameterGroupNumber{0, 24, 1, 0, 0, 16777215};
  CanSignalLe<std::uint8_t> TrailerVdcActive_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> YcEngineControlActive_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> YcBrakeControlActive_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> RopEngineControlActive_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> RopBrakeControlActive_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> VdcFullyOperational_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> VdcBrakeLightRequest_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Electronic_Stability_Control_3E{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint32_t> Trip_Distance_HR_0{0, 32, 5, 0, 0, 21055406075};  // meters
  CanSignalLe<std::uint32_t> Total_Vehicle_Distance_HR_0{0, 32, 5, 0, 0, 21055406075};  // meters
  CanSignalLe<std::uint8_t> Lane_Departure_Right{0, 2, 2, 0, 0, 6};  // kPa
  CanSignalLe<std::uint8_t> Lane_Departure_Left{0, 2, 2, 0, 0, 6};  // kPa
  CanSignalLe<std::uint8_t> Lane_Departure_Imm_RightSide{0, 2, 1, 0, 0, 3};  // states/2 bit
  CanSignalLe<std::uint8_t> Lane_Departure_Imm_LeftSide{0, 2, 1, 0, 0, 3};  // states/2 bit
  CanSignalLe<std::uint8_t> tractormnttrailerabswarning_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> halt_brake_switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> railroad_mode_switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> src_add_ctrl_dev_brake_ctrl_B{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> abs_fully_operational_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> tractionctrloverrideswitch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_hill_holder_switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_off_road_switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> abs_off_road_switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> brake_pedal_position_B{0, 8, 0.4, 0, 0, 100};
  CanSignalLe<std::uint8_t> ebs_brake_switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> antilock_braking_abs_active_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_brake_control_active_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_engine_control_active_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Trailer_ABS{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> ATC_ASR_Info_Signal{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> ABS_EBS_Amber_Warning_Signal{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EBS_Red_Warning_Signal{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Engine_Retarder_Selection_B{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Remote_Accel_Enable_Switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Aux_Engine_Shutdown_Switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Engine_Derate_Switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Accelerator_Interlock_Switch_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint16_t> SCR_Oper_Indu_Act_Trav_Dist{0, 16, 1, 0, 0, 64255};  // km
  CanSignalLe<std::uint8_t> Catalyst_Conv_Efficiency{0, 8, 0.4, 0, 0, 100};
  CanSignalLe<std::uint8_t> Eng_Aux_Cooler_Supply2{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Eng_Aux_Cooler_Supply1{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<AFT_Fuel_Enable_Control> AFT_Fuel_Enable_Control{0, 2};
  CanSignalLe<Regeneration_Status_A> Regeneration_Status_A{0, 2};
  CanSignalLe<std::uint16_t> Fuel_Pressure_Control{0, 16, 0.0025, 0, 0, 160.6375};  // %
  CanSignalLe<std::uint16_t> Fuel_Rate_B{0, 16, 0.05, 0, 0, 3212.75};  // L/h
  CanSignalLe<std::uint16_t> Fuel_Pressure_1_A{0, 16, 0.1, 0, 0, 6553.5};  // kPa
  CanSignalLe<std::uint32_t> trip_distance{0, 32, 0.125, 0, 0, 526385151.9};  // km
  CanSignalLe<std::uint32_t> Total_Vehicle_Distance{0, 32, 0.125, 0, 0, 526385151.9};  // km
  CanSignalLe<std::uint8_t> ForwardCollisionWarning{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> ACCDistanceAlertSignal{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> ACCSystemShutoffWarning{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> ACCTargetDetected{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint16_t> RoadCurvature{0, 16, 0.0078125, -250, -250, 251.992};  // 1/km
  CanSignalLe<std::uint8_t> ACCSetDistanceMode{0, 3, 1, 0, 0, 7};  // bit
  CanSignalLe<std::uint8_t> ACCMode{0, 3, 1, 0, 0, 7};  // bit
  CanSignalLe<std::uint8_t> ACCSetSpeed{0, 8, 1, 0, 0, 250};  // km/h
  CanSignalLe<std::uint8_t> DistanceToTheForwardVehicle{0, 8, 1, 0, 0, 250};  // m
  CanSignalLe<std::uint8_t> SpeedOfTheForwardVehicle{0, 8, 1, 0, 0, 250};  // km/h
  CanSignalLe<std::uint32_t> ParameterGroupNumber_ACKM_RX{0, 24, 1, 0, 0, 16777215};
  CanSignalLe<std::uint8_t> AddressAccessDenied_ACKM_RX{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> AddressAcknowledged_ACKM__RX{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> AddressBusy_ACKM_RX{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> AddressNegativeAck_ACKM_RX{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> GroupFunctionValue_ACKM_RX{0, 8, 1, 0, 0, 255};
  CanSignalLe<ControlByte_ACKM_RX> ControlByte_ACKM_RX{0, 8};
  CanSignalLe<std::uint16_t> Altitude{0, 16, 0.125, -2500, -2500, 5531.88};  // m
  CanSignalLe<std::uint16_t> HorizontalDilutionofPrecision{0, 16, 0.002, 0, 0, 99};
  CanSignalLe<std::uint16_t> NavigationBasedVehicleSpeed{0, 16, 0.00390625, 0, 0, 255.99609375};  // km/h
  CanSignalLe<std::uint16_t> CompassBearing{0, 16, 0.0078125, 0, 0, 501.99};  // deg
  CanSignalLe<std::uint8_t> BCM_UDS_TX_4A_placeholder{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint16_t> SuspendDuration{0, 16, 1, 0, 0, 64255};  // sec
  CanSignalLe<HoldSignal> HoldSignal{0, 4};
  CanSignalLe<SuspendSignal> SuspendSignal{0, 4};
  CanSignalLe<J1939Network3> J1939Network3{0, 2};
  CanSignalLe<J1939Network2> J1939Network2{0, 2};
  CanSignalLe<ISO9141> ISO9141{0, 2};
  CanSignalLe<SAE_J1850> SAE_J1850{0, 2};
  CanSignalLe<ManufacturerSpecificPort> ManufacturerSpecificPort{0, 2};
  CanSignalLe<CurrentDataLink> CurrentDataLink{0, 2};
  CanSignalLe<SAE_J1587> SAE_J1587{0, 2};
  CanSignalLe<SAE_J1922> SAE_J1922{0, 2};
  CanSignalLe<J1939Network1> J1939Network1{0, 2};
  CanSignalLe<std::uint8_t> BCM_UDS_TX_4B_placeholder{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_Inhibit{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_Rqst{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_ECU_Addr{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> OnBoard_Programming_Progress{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> OBP_Key_Switch_Request{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Tgt_Not_Ready{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Pwr_Supply_Insuff{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Park_Brake{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Key_Switch_Pos{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Engine_Speed{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Overall_Status{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Programming_Status{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Target_ECU_Address{0, 8, 1, 0, 0, 255};
  CanSignalLe<Remote_Lock_Unlock_Req_21_4B> Remote_Lock_Unlock_Req_21_4B{0, 2};
  CanSignalLe<V2X_Vehicle_Request_21_4B> V2X_Vehicle_Request_21_4B{0, 2};
  CanSignalLe<std::uint8_t> Message_Checksum_Value{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Message_Counter_Value{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint16_t> Est_Remaining_Programming_time{0, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint8_t> Programming_Type{0, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Crank_Inhibit_Request{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Comm_Header_Byte{0, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint32_t> TripFanOnTimeDueToAC_00{0, 32, 0.05, 0, 0, 214748364.75};  // Hr
  CanSignalLe<std::uint8_t> AuxHeaterOutputCoolantTemp_19{0, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> AuxiliaryHeaterMode_19{0, 4, 1, 0, 0, 15};  // bit
  CanSignalLe<LiftAxle1Position_2F> LiftAxle1Position_2F{0, 2};
  CanSignalLe<std::uint16_t> RollAngle_03{0, 16, 0.002, -64, -64, 64.51};  // Deg
  CanSignalLe<std::uint16_t> PitchRate_03{0, 16, 0.002, -64, -64, 64.51};  // Deg/s
  CanSignalLe<std::uint16_t> PitchAngle_03{0, 16, 0.002, -64, -64, 64.51};  // Deg
  CanSignalLe<std::uint8_t> CrankAtmpCntonPresentStartAtt_00{0, 8, 1, 0, 0, 255};  // count
  CanSignalLe<std::uint16_t> CargoWeight_21{0, 16, 2, 0, 0, 128510};  // kg
};

struct EEC4_00 : public CanMessage<0x9CFEBE00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct MHVACSTAT1_D9 : public CanMessage<0x98FF20D9, 8> {
  CanSignalLe<std::uint16_t> tmpract{32, 12, 0.1, -40, -40, 100};  // degC
  CanSignalLe<std::uint16_t> tmprset{44, 12, 0.1, -40, -40, 100};
};

struct MVS_00 : public CanMessage<0x98FDE500, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct EFLP2_00 : public CanMessage<0x98FEDB00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct EEC17_00 : public CanMessage<0x98FC4B00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM5_3D : public CanMessage<0x98FECE3D, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM5_27 : public CanMessage<0x98FECE27, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM5_17 : public CanMessage<0x98FECE17, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM5_00 : public CanMessage<0x98FECE00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM21_00 : public CanMessage<0x98C1FF00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct AT1OGC2_00 : public CanMessage<0x8CFD0E00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct AT1IGC2_00 : public CanMessage<0x8CFD1000, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct A1SCRSI1_00 : public CanMessage<0x98FD6E00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct RESET_17 : public CanMessage<0x9CDEFF17, 8> {
  enum class ServiceCompIdentification_17 : std::uint8_t {
    SERVICE_CHECK_FOR_ENTIRE_VEHICLE = 0,
    BRAKE_LINING_LEFT_FRONT_AXLE = 1,
    BRAKE_LINING_RIGHT_FRONT_AXLE = 2,
    BRAKE_LINING_LEFT_REAR_AXLE = 3,
    BRAKE_LINING_RIGHT_REAR_AXLE = 4,
    CLUTCH_LINING = 5,
    NOT_DEFINED_1 = 6,
    NOT_DEFINED_2 = 7,
    NOT_DEFINED_3 = 8,
    NOT_DEFINED_4 = 9,
    NOT_DEFINED_5 = 10,
    BRAKE_LINING_LEFT_REAR_AXLE_2 = 11,
    BRAKE_LINING_RIGHT_REAR_AXLE_2 = 12,
    BRAKE_LINING_LEFT_REAR_AXLE_3 = 13,
    BRAKE_LINING_RIGHT_REAR_AXLE_3 = 14,
    BRAKE_LINING_GENERAL = 15,
    REG_GEN_CHECK_FOR_ENTIRE_VEH = 16,
    BRAKE_SYSTEM_SPECIAL_CHECK = 17,
    IN_BETWEEN_CHECK = 18,
    CHECK_TRIP_RECORDER = 19,
    CHECK_EXHAUST_GAS = 20,
    CHECK_VEHICLE_SPEED_LIMITER = 21,
    NOT_DEFINED_6 = 22,
    NOT_DEFINED_7 = 23,
    NOT_DEFINED_8 = 24,
    NOT_DEFINED_9 = 25,
    NOT_DEFINED_10 = 26,
    NOT_DEFINED_11 = 27,
    NOT_DEFINED_12 = 28,
    NOT_DEFINED_13 = 29,
    ENGINE_COOLANT_CHANGE = 30,
    ENGINE_COOLANT_FILTER_CHANGE = 31,
    ENGINE_OIL_ENGINE_1 = 32,
    ENGINE_OIL_ENGINE_2 = 33,
    NOT_DEFINED_14 = 34,
    STEERING_OIL = 35,
    NOT_DEFINED_15 = 36,
    TRANS_OIL_TRANSMISSION_1 = 37,
    TRANS_OIL_TRANSMISSION_2 = 38,
    TRANS_OIL_FILTER_TRANSMISSION_1 = 39,
    INTERMEDIATE_TRANSMISSION_OIL = 40,
    NOT_DEFINED_16 = 41,
    FRONT_AXLE_OIL = 42,
    REAR_AXLE_OIL = 43,
    NOT_DEFINED_17 = 44,
    NOT_DEFINED_18 = 45,
    NOT_DEFINED_19 = 46,
    NOT_DEFINED_20 = 47,
    TIRES = 48,
    ENGINE_AIR_FILTER = 49,
    ENGINE_OIL_FILTER = 50,
    ENGINE_FUEL_FILTER = 51,
    NOT_DEFINED_21 = 52,
    NOT_DEFINED_22 = 53,
    NOT_DEFINED_23 = 54,
    NOT_DEFINED_24 = 55,
    NOT_DEFINED_25 = 56,
    NOT_DEFINED_26 = 57,
    NOT_DEFINED_27 = 58,
    NOT_DEFINED_28 = 59,
    NOT_DEFINED_29 = 60,
    TACHOGRAPH = 61,
    DRIVER_CARD_1 = 62,
    DRIVER_CARD_2 = 63,
    NOT_DEFINED_30 = 64,
    NOT_DEFINED_31 = 65,
    NOT_DEFINED_32 = 66,
    NOT_DEFINED_33 = 67,
    NOT_DEFINED_34 = 68,
    NOT_DEFINED_35 = 69,
    NOT_DEFINED_36 = 70,
    NOT_DEFINED_37 = 71,
    NOT_DEFINED_38 = 72,
    NOT_DEFINED_39 = 73,
    NOT_DEFINED_40 = 74,
    NOT_DEFINED_41 = 75,
    NOT_DEFINED_42 = 76,
    NOT_DEFINED_43 = 77,
    NOT_DEFINED_44 = 78,
    NOT_DEFINED_45 = 79,
    NOT_DEFINED_46 = 80,
    NOT_DEFINED_47 = 81,
    NOT_DEFINED_48 = 82,
    NOT_DEFINED_49 = 83,
    NOT_DEFINED_50 = 84,
    NOT_DEFINED_51 = 85,
    NOT_DEFINED_52 = 86,
    NOT_DEFINED_53 = 87,
    NOT_DEFINED_54 = 88,
    NOT_DEFINED_55 = 89,
    NOT_DEFINED_56 = 90,
    NOT_DEFINED_57 = 91,
    NOT_DEFINED_58 = 92,
    NOT_DEFINED_59 = 93,
    NOT_DEFINED_60 = 94,
    NOT_DEFINED_61 = 95,
    NOT_DEFINED_62 = 96,
    NOT_DEFINED_63 = 97,
    NOT_DEFINED_64 = 98,
    NOT_DEFINED_65 = 99,
    NOT_DEFINED_66 = 100,
    NOT_DEFINED_67 = 101,
    NOT_DEFINED_68 = 102,
    NOT_DEFINED_69 = 103,
    NOT_DEFINED_70 = 104,
    NOT_DEFINED_71 = 105,
    NOT_DEFINED_72 = 106,
    NOT_DEFINED_73 = 107,
    NOT_DEFINED_74 = 108,
    NOT_DEFINED_75 = 109,
    NOT_DEFINED_76 = 110,
    NOT_DEFINED_77 = 111,
    NOT_DEFINED_78 = 112,
    NOT_DEFINED_79 = 113,
    NOT_DEFINED_80 = 114,
    NOT_DEFINED_81 = 115,
    NOT_DEFINED_82 = 116,
    NOT_DEFINED_83 = 117,
    NOT_DEFINED_84 = 118,
    NOT_DEFINED_85 = 119,
    NOT_DEFINED_86 = 120,
    NOT_DEFINED_87 = 121,
    NOT_DEFINED_88 = 122,
    NOT_DEFINED_89 = 123,
    NOT_DEFINED_90 = 124,
    NOT_DEFINED_91 = 125,
    NOT_DEFINED_92 = 126,
    NOT_DEFINED_93 = 127,
    NOT_DEFINED_94 = 128,
    NOT_DEFINED_95 = 129,
    NOT_DEFINED_96 = 130,
    NOT_DEFINED_97 = 131,
    NOT_DEFINED_98 = 132,
    NOT_DEFINED_99 = 133,
    NOT_DEFINED_100 = 134,
    NOT_DEFINED_101 = 135,
    NOT_DEFINED_102 = 136,
    NOT_DEFINED_103 = 137,
    NOT_DEFINED_104 = 138,
    NOT_DEFINED_105 = 139,
    NOT_DEFINED_106 = 140,
    NOT_DEFINED_107 = 141,
    NOT_DEFINED_108 = 142,
    NOT_DEFINED_109 = 143,
    NOT_DEFINED_110 = 144,
    NOT_DEFINED_111 = 145,
    NOT_DEFINED_112 = 146,
    NOT_DEFINED_113 = 147,
    NOT_DEFINED_114 = 148,
    NOT_DEFINED_115 = 149,
    NOT_DEFINED_116 = 150,
    NOT_DEFINED_117 = 151,
    NOT_DEFINED_118 = 152,
    NOT_DEFINED_119 = 153,
    NOT_DEFINED_120 = 154,
    NOT_DEFINED_121 = 155,
    NOT_DEFINED_122 = 156,
    NOT_DEFINED_123 = 157,
    NOT_DEFINED_124 = 158,
    NOT_DEFINED_125 = 159,
    NOT_DEFINED_126 = 160,
    NOT_DEFINED_127 = 161,
    NOT_DEFINED_128 = 162,
    NOT_DEFINED_129 = 163,
    NOT_DEFINED_130 = 164,
    NOT_DEFINED_131 = 165,
    NOT_DEFINED_132 = 166,
    NOT_DEFINED_133 = 167,
    NOT_DEFINED_134 = 168,
    NOT_DEFINED_135 = 169,
    NOT_DEFINED_136 = 170,
    NOT_DEFINED_137 = 171,
    NOT_DEFINED_138 = 172,
    NOT_DEFINED_139 = 173,
    NOT_DEFINED_140 = 174,
    NOT_DEFINED_141 = 175,
    NOT_DEFINED_142 = 176,
    NOT_DEFINED_143 = 177,
    NOT_DEFINED_144 = 178,
    NOT_DEFINED_145 = 179,
    NOT_DEFINED_146 = 180,
    NOT_DEFINED_147 = 181,
    NOT_DEFINED_148 = 182,
    NOT_DEFINED_149 = 183,
    NOT_DEFINED_150 = 184,
    NOT_DEFINED_151 = 185,
    NOT_DEFINED_152 = 186,
    NOT_DEFINED_153 = 187,
    NOT_DEFINED_154 = 188,
    NOT_DEFINED_155 = 189,
    NOT_DEFINED_156 = 190,
    NOT_DEFINED_157 = 191,
    NOT_DEFINED_158 = 192,
    NOT_DEFINED_159 = 193,
    NOT_DEFINED_160 = 194,
    NOT_DEFINED_161 = 195,
    NOT_DEFINED_162 = 196,
    NOT_DEFINED_163 = 197,
    NOT_DEFINED_164 = 198,
    NOT_DEFINED_165 = 199,
    NOT_DEFINED_166 = 200,
    NOT_DEFINED_167 = 201,
    NOT_DEFINED_168 = 202,
    NOT_DEFINED_169 = 203,
    NOT_DEFINED_170 = 204,
    NOT_DEFINED_171 = 205,
    NOT_DEFINED_172 = 206,
    NOT_DEFINED_173 = 207,
    NOT_DEFINED_174 = 208,
    NOT_DEFINED_175 = 209,
    NOT_DEFINED_176 = 210,
    NOT_DEFINED_177 = 211,
    NOT_DEFINED_178 = 212,
    NOT_DEFINED_179 = 213,
    NOT_DEFINED_180 = 214,
    NOT_DEFINED_181 = 215,
    NOT_DEFINED_182 = 216,
    NOT_DEFINED_183 = 217,
    NOT_DEFINED_184 = 218,
    NOT_DEFINED_185 = 219,
    NOT_DEFINED_186 = 220,
    NOT_DEFINED_187 = 221,
    NOT_DEFINED_188 = 222,
    NOT_DEFINED_189 = 223,
    NOT_DEFINED_190 = 224,
    NOT_DEFINED_191 = 225,
    NOT_DEFINED_192 = 226,
    NOT_DEFINED_193 = 227,
    NOT_DEFINED_194 = 228,
    NOT_DEFINED_195 = 229,
    NOT_DEFINED_196 = 230,
    NOT_DEFINED_197 = 231,
    NOT_DEFINED_198 = 232,
    NOT_DEFINED_199 = 233,
    NOT_DEFINED_200 = 234,
    NOT_DEFINED_201 = 235,
    NOT_DEFINED_202 = 236,
    NOT_DEFINED_203 = 237,
    NOT_DEFINED_204 = 238,
    NOT_DEFINED_205 = 239,
    MANUFACTURER_SPECIFIC_1 = 240,
    MANUFACTURER_SPECIFIC_2 = 241,
    MANUFACTURER_SPECIFIC_3 = 242,
    MANUFACTURER_SPECIFIC_4 = 243,
    MANUFACTURER_SPECIFIC_5 = 244,
    MANUFACTURER_SPECIFIC_6 = 245,
    MANUFACTURER_SPECIFIC_7 = 246,
    MANUFACTURER_SPECIFIC_8 = 247,
    MANUFACTURER_SPECIFIC_9 = 248,
    MANUFACTURER_SPECIFIC_10 = 249,
    RESERVED_1 = 250,
    RESERVED_2 = 251,
    RESET_ALL_COMPONENTS = 252,
    NO_ACTION_TO_BE_TAKEN = 253,
    ERROR = 254,
    COMPONENT_ID_NOT_AVAILABLE = 255,
  };

  CanSignalLe<ServiceCompIdentification_17> ServiceCompIdentification_17{8, 8};
};

struct SERV_00 : public CanMessage<0x98FEC000, 8> {
  CanSignalLe<std::uint16_t> ServiceDelayOpTimeBased_00{48, 16, 1, -32127, -32127, 32128};  // Hour
  CanSignalLe<std::uint16_t> ServiceDistance_00{8, 16, 5, -160635, -160635, 160640};  // km/bit
};

struct MI1_00 : public CanMessage<0x98FB4D00, 8> {
  CanSignalLe<std::uint8_t> FuelFilterLifeRemain_00{32, 8, 0.4, 0, 0, 100};  // %
};

struct AT1P1I_00 : public CanMessage<0x98FCD500, 8> {
  CanSignalLe<std::uint8_t> DefFilterLife_00{32, 8, 0.4, 0, 0, 100};  // %
};

struct ASC5_2F : public CanMessage<0x8CFE572F, 8> {
  enum class ElecShockAbsorberCtlMode_2F : std::uint8_t {
    NORMOPDAMPERPASSIVE = 0,
    NORMOPDAMPERACTIVE = 1,
    ERROR = 2,
    DESCRIPTION_FOR_THE_VALUE_0X3 = 3,
  };

  CanSignalLe<ElecShockAbsorberCtlMode_2F> ElecShockAbsorberCtlMode_2F{26, 2};
};

struct LCMD_21 : public CanMessage<0x8CFE4121, 8> {
  enum class RightTurnSignalLightsCmd_21 : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class LeftTurnSignalLightsCmd_21 : std::uint8_t {
    DEACTIVATE = 0,
    ACTIVATE = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  CanSignalLe<RightTurnSignalLightsCmd_21> RightTurnSignalLightsCmd_21{12, 2};
  CanSignalLe<LeftTurnSignalLightsCmd_21> LeftTurnSignalLightsCmd_21{14, 2};
};

struct DM7_00 : public CanMessage<0x98E34A00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM6_00 : public CanMessage<0x98FECF00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM56_00 : public CanMessage<0x98FCC700, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM34_00 : public CanMessage<0x98A04A00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM33_00 : public CanMessage<0x98A14A00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM30_3D : public CanMessage<0x98A44A3D, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM30_00 : public CanMessage<0x98A44A00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM29_00 : public CanMessage<0x989E4A00, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM28_00 : public CanMessage<0x98FD8000, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM25_00 : public CanMessage<0x98FDB700, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM24_00 : public CanMessage<0x98FDB600, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM23_00 : public CanMessage<0x98FDB500, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct DM12_00 : public CanMessage<0x98FED400, 255> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTSNA_00 : public CanMessage<0x98FB1700, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTENA_00 : public CanMessage<0x98FB1600, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEEA_00 : public CanMessage<0x98FB1500, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTVMA_00 : public CanMessage<0x98FB1400, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEHA_00 : public CanMessage<0x98FB1300, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTFCA_00 : public CanMessage<0x98FB1200, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTSNS_00 : public CanMessage<0x98FB1100, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTENS_00 : public CanMessage<0x98FB1000, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEES_00 : public CanMessage<0x98FB0F00, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTVMS_00 : public CanMessage<0x98FB0E00, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEHS_00 : public CanMessage<0x98FB0D00, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTFCS_00 : public CanMessage<0x98FB0C00, 34> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTSNV_00 : public CanMessage<0x98FB0B00, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTENV_00 : public CanMessage<0x98FB0A00, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEEV_00 : public CanMessage<0x98FB0900, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTVMV_00 : public CanMessage<0x98FB0800, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEHV_00 : public CanMessage<0x98FB0700, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTFCV_00 : public CanMessage<0x98FB0600, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEEEA_00 : public CanMessage<0x98FB0500, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTVMEA_00 : public CanMessage<0x98FB0400, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTEHEA_00 : public CanMessage<0x98FB0300, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct NTFCEA_00 : public CanMessage<0x98FB0200, 68> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct AT2IGC2_00 : public CanMessage<0x8CFD0C00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct ASI2_00 : public CanMessage<0x9CFBE800, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct ATNXMS1_00 : public CanMessage<0x98FAFA00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct ATNXMS2_00 : public CanMessage<0x98FAF900, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct AT1HI3_00 : public CanMessage<0x98FAF700, 32> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct A1SCRDSR3_00 : public CanMessage<0x8CF0FB00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct AT2IG1_00 : public CanMessage<0x98F01000, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct GHGTTL_00 : public CanMessage<0x98FB0100, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct GHGTTA_00 : public CanMessage<0x98FB0000, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct GHGTTS_00 : public CanMessage<0x98FAFF00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct GHGTA_00 : public CanMessage<0x98FAFE00, 28> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct GHGTS_00 : public CanMessage<0x98FAFD00, 28> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct GHGTL_00 : public CanMessage<0x98FAFC00, 52> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct IT6_00 : public CanMessage<0x9CFE8700, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct IT2_00 : public CanMessage<0x9CFE8300, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct IT1_00 : public CanMessage<0x9CFE8200, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct IC2_00 : public CanMessage<0x98FDD000, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct TFAC_00 : public CanMessage<0x90F01A00, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct TCW_00 : public CanMessage<0x98FE9600, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct EEC15_00 : public CanMessage<0x98FDC300, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct EEC14_00 : public CanMessage<0x98FDC200, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct EPSI_00 : public CanMessage<0x98FCD000, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct EEC21_00 : public CanMessage<0x98FBD600, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct EEC2_27 : public CanMessage<0x8CF00327, 8> {
  CanSignalLe<std::uint64_t> CarbRawData{0, 64, 1, 0, 0, 65535};
};

struct BSC_VDC2 : public CanMessage<0x98F0090B, 8> {
  CanSignalLe<std::uint8_t> LongitudinalAcc_0B{56, 8, 0.1, -12.5, -12.5, 12.5};  // m/s�
  CanSignalLe<std::uint16_t> SteeringWheelAngle_0B{0, 16, 0.0009765625, -31.374, -31.374, 31.374};  // rad
  CanSignalLe<std::uint16_t> YawRate{24, 16, 0.0001220703125, -3.92, -3.92, 3.92};  // Rad/s
};

struct TD : public CanMessage<0x98FEE621, 8> {
  CanSignalLe<std::uint8_t> local_minute_offset{48, 8, 1, -125, -125, 125};  // min/bit
  CanSignalLe<std::uint8_t> local_hour_offset{56, 8, 1, -125, -125, 125};  // hr/bit
  CanSignalLe<std::uint8_t> Year{40, 8, 1, 1985, 1985, 2235};  // years
  CanSignalLe<std::uint8_t> Seconds{0, 8, 0.25, 0, 0, 62.5};  // s
  CanSignalLe<std::uint8_t> Month{24, 8, 1, 0, 0, 250};  // month
  CanSignalLe<std::uint8_t> Minutes{8, 8, 1, 0, 0, 250};  // min
  CanSignalLe<std::uint8_t> Hours{16, 8, 1, 0, 0, 250};  // hr
  CanSignalLe<std::uint8_t> Day{32, 8, 0.25, 0, 0, 62.5};  // days
};

struct E_VEP2 : public CanMessage<0x98FE8D00, 8> {
  CanSignalLe<std::uint16_t> battery_potential_powerInput2{0, 16, 0.05, 0, 0, 3212.75};  // V
};

struct BFD : public CanMessage<0x98FF3121, 8> {
  CanSignalLe<std::uint8_t> Wheelchair_Lift_Door_status{44, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> StopArm_status{36, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> StopArmLamp_status{32, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Right_Red_Lamp_status{14, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Right_Amber_Lamp_status{6, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Left_Red_Lamp_status{12, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Left_Amber_Lamp_status{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Right_Red_Lamp_status{10, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Right_Amber_Lamp_status{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Left_Red_Lamp_status{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Left_Amber_Lamp_status{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Emergency_Door_Status{34, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> CrossingGate_Status{40, 4, 1, 0, 0, 15};
};

struct B2V_ELECENERGY : public CanMessage<0x98FF23F3, 8> {
  CanSignalLe<std::uint32_t> b2v_totdischgenergy{24, 24, 0.1, 0, 0, 1677721.5};  // Kwh
  CanSignalLe<std::uint32_t> b2v_totchgenergy{0, 24, 0.1, 0, 0, 1677721.5};  // Kwh
};

struct AIR3_30 : public CanMessage<0x98FAC330, 8> {
  CanSignalLe<std::uint8_t> AirDryerCartridgeLifeRemain_30{0, 8, 0.4, 0, 0, 100};  // %
};

struct PROPB73_21 : public CanMessage<0x98FF7321, 1> {
  CanSignalLe<std::uint8_t> StarterPrognosticsTxStatus_21{4, 4, 1, 0, 0, 15};  // state
  CanSignalLe<std::uint8_t> AirSystemLeakTxStatus_21{0, 4, 1, 0, 0, 15};  // state
};

struct PROPB72_21 : public CanMessage<0x98FF7221, 8> {
  CanSignalLe<std::uint16_t> SecondaryAirPressureDelta_21{48, 16, 0.1, 0, 0, 6425.5};  // kPa
  CanSignalLe<std::uint16_t> PrimaryAirPressureDelta_21{32, 16, 0.1, 0, 0, 6425.5};  // kPa
  CanSignalLe<std::uint16_t> EngineOffDuration_21{0, 16, 1, 0, 0, 64255};  // min
  CanSignalLe<std::uint16_t> AmbientAirTempDelta_21{16, 16, 0.03125, -273, -273, 1734.96875};  // C
};

struct PROPB71_21 : public CanMessage<0x98FF7121, 8> {
  enum class CrankStartStatus_21 : std::uint8_t {
    UNSUCCESSFUL_CRANK = 0,
    SUCCESSFUL_CRANK = 1,
    RESERVED_1 = 2,
    RESERVED_2 = 3,
    RESERVED_3 = 4,
    RESERVED_4 = 5,
    RESERVED_5 = 6,
    RESERVED_6 = 7,
    RESERVED_7 = 8,
    RESERVED_8 = 9,
    RESERVED_9 = 10,
    RESERVED_10 = 11,
    RESERVED_11 = 12,
    INVALID_DATA = 13,
    ERROR = 14,
    NOT_AVAILABLE = 15,
  };

  CanSignalLe<std::uint16_t> EngineOilTempInCrank_21{16, 16, 0.03125, -273, -273, 1734.96875};  // C
  CanSignalLe<CrankStartStatus_21> CrankStartStatus_21{0, 4};
  CanSignalLe<std::uint16_t> CrankDuration_21{48, 16, 1, 0, 0, 64255};  // ms
  CanSignalLe<std::uint8_t> BarometricPressureInCrank_21{8, 8, 0.5, 0, 0, 125};  // kPa
  CanSignalLe<std::uint16_t> AverageCrankingSpeed_21{32, 16, 0.125, 0, 0, 8031.875};  // rpm
};

struct PROPB43_27 : public CanMessage<0x98FF4327, 1> {
  CanSignalLe<std::uint8_t> FuelWaterSeperatorTxStatus_27{4, 4, 1, 0, 0, 15};  // state
  CanSignalLe<std::uint8_t> EngineAirFilterTxStatus_27{0, 4, 1, 0, 0, 15};  // state
};

struct PROPB00_D7 : public CanMessage<0x98FF00D7, 1> {
  CanSignalLe<std::uint8_t> WMSTransmitterStatus_D7{0, 4, 1, 0, 0, 15};  // state
};

struct CVW_0B : public CanMessage<0x98FE700B, 8> {
  CanSignalLe<std::uint16_t> Comb_Veh_Weight_0B{16, 16, 10, 0, 0, 642550};  // kg
};

struct PROPB63_4A : public CanMessage<0x98FF634A, 2> {
  CanSignalLe<std::uint8_t> StarterPrognosticsRxStatus_4A{12, 4, 1, 0, 0, 15};  // state
  CanSignalLe<std::uint8_t> AirSystemLeakRxStatus_4A{8, 4, 1, 0, 0, 15};  // state
  CanSignalLe<std::uint8_t> EngineAirFilterRxStatus_4A{0, 4, 1, 0, 0, 15};  // state
  CanSignalLe<std::uint8_t> FuelWaterSeperatorRxStatus_4A{4, 4, 1, 0, 0, 15};  // state
};

struct PROPB62_4A : public CanMessage<0x98FF624A, 1> {
  CanSignalLe<std::uint8_t> WMSReceiverStatus_4A{0, 4, 1, 0, 0, 15};  // state
};

struct PROPB27_21 : public CanMessage<0x98FF2721, 8> {
  enum class RemoteLockUnlockStatus_21 : std::uint8_t {
    NOT_REQUESTED = 0,
    DOOR_POD_CMD_REQ_SENT = 1,
    INTERLOCKS_NOT_MET = 2,
    NO_COMM_FROM_DOOR_POD = 3,
    RESERVED_1 = 4,
    RESERVED_2 = 5,
    RESERVED_3 = 6,
    RESERVED_4 = 7,
    RESERVED_5 = 8,
    RESERVED_6 = 9,
    RESERVED_7 = 10,
    RESERVED_8 = 11,
    RESERVED_9 = 12,
    RESERVED_10 = 13,
    RESERVED_11 = 14,
    NOT_AVAILABLE = 15,
  };

  CanSignalLe<RemoteLockUnlockStatus_21> RemoteLockUnlockStatus_21{0, 4};
};

struct AC_FE_FE : public CanMessage<0x98EEFEFE, 8> {
  CanSignalLe<std::uint8_t> ArbitrationAddressCapable_FE_FE{63, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> IndustryGroup_FE_FE{60, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> VehicleSystem_FE_FE{49, 7, 1, 0, 0, 127};
  CanSignalLe<std::uint8_t> VehicleSystemInstance_FE_FE{56, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Function_FE_FE{40, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> FunctionInstance_FE_FE{35, 5, 1, 0, 0, 31};
  CanSignalLe<std::uint16_t> ManufacturerCode_FE_FE{21, 11, 1, 0, 0, 2047};
  CanSignalLe<std::uint8_t> ECUInstance_FE_FE{32, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint32_t> IdentityNumber_FE_FE{0, 21, 1, 0, 0, 2097151};
};

struct DM14_03_4A : public CanMessage<0x98D9034A, 8> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct DM18_03_4A : public CanMessage<0x98D4034A, 1024> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct DM16_03_4A : public CanMessage<0x98D7034A, 1024> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct PropBBCMTX_21_4A : public CanMessage<0x8CFF224A, 8> {
  CanSignalLe<std::uint64_t> BCMCommData_21_4A{0, 64, 1, 0, 0, 0};
};

struct PropB_2A_21 : public CanMessage<0x98FF2A21, 8> {
  CanSignalLe<std::uint8_t> BcmAlarmRequest_21{8, 6, 1, 0, 0, 63};
};

struct B2V_ELECENERGY_F3 : public CanMessage<0x98FF96F3, 8> {
  CanSignalLe<std::uint32_t> B2v_TotDischgEnergy_F3{24, 24, 0.1, 0, 0, 1677721.5};  // Kwh
  CanSignalLe<std::uint32_t> B2v_TotChgEnergy_F3{0, 24, 0.1, 0, 0, 1677721.5};  // Kwh
};

struct CN_53 : public CanMessage<0x80F02B53, 8> {
  CanSignalLe<std::uint8_t> Crash_Checksum_53{60, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Crash_Counter_53{56, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Crash_Type_53{0, 5, 1, 0, 0, 31};
};

struct VI_5A : public CanMessage<0x98FEEC5A, 256> {
  /* vehicleid */
};

struct PCC_RQST_TX : public CanMessage<0x98EAFF11, 3> {
  CanSignalLe<std::uint32_t> ParameterGroupNumber{0, 24, 1, 0, 0, 16777215};
};

struct PropB_30_VCU_5A : public CanMessage<0x98FF305A, 8> {
  CanSignalLe<std::uint16_t> hves_time_to_charge_5a{0, 12, 1, 0, 0, 4094};  // mins
};

struct HRLFC_0 : public CanMessage<0x98FD0900, 8> {
  CanSignalLe<std::uint32_t> highresolutionengtripfuel_0{0, 32, 0.001, 0, 0, 4211081.215};  // l
  CanSignalLe<std::uint32_t> highresolutionengtotalfuelused_0{32, 32, 0.001, 0, 0, 4211081.215};  // l
};

struct DLCC1 : public CanMessage<0x98FD0700, 8> {
  enum class CmprssnBrkEnblSwtchIndctrLampCmd : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engoilpresslowlampcmd : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engcoolanttemphighlampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engcoolantlevellowlampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engidlemanagementactivelampcmd_0 : std::uint8_t {
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engairfilterrestrictionlampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engfuelfilterrestrictedlampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engctrlmodule1rdyforuselampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engctrlmodule2rdyforuselampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engctrlmodule3rdyforuselampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engspeedhighlampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class engspeedveryhighlampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class vhclaccelratelimitlampcmd_0 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> EngineInterCoolerTempHighCmd_0{48, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EngineFuelTempHighLampCmd_0{46, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EngineOilTempHighLampCmd_0{44, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EngineStopStartEnabledLampCmd_0{42, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EngStopStartAutoStartFailedCmd_0{40, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EngStopStartAutomStopActiveCmd_0{38, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EngIdleMgmtPendEventLampCmd_0{36, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engine_brake_active_lamp_cmd_0{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> obd_malfunction_indic_lamp_cmd{6, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engine_redstop_lamp_cmd{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engine_amberwarning_lamp_cmd{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engine_protect_lamp_command{0, 2, 1, 0, 0, 3};
  CanSignalLe<CmprssnBrkEnblSwtchIndctrLampCmd> CmprssnBrkEnblSwtchIndctrLampCmd{10, 2};
  CanSignalLe<engoilpresslowlampcmd> engoilpresslowlampcmd{12, 2};
  CanSignalLe<engcoolanttemphighlampcmd_0> engcoolanttemphighlampcmd_0{14, 2};
  CanSignalLe<engcoolantlevellowlampcmd_0> engcoolantlevellowlampcmd_0{16, 2};
  CanSignalLe<engidlemanagementactivelampcmd_0> engidlemanagementactivelampcmd_0{18, 2};
  CanSignalLe<engairfilterrestrictionlampcmd_0> engairfilterrestrictionlampcmd_0{20, 2};
  CanSignalLe<engfuelfilterrestrictedlampcmd_0> engfuelfilterrestrictedlampcmd_0{22, 2};
  CanSignalLe<engctrlmodule1rdyforuselampcmd_0> engctrlmodule1rdyforuselampcmd_0{24, 2};
  CanSignalLe<engctrlmodule2rdyforuselampcmd_0> engctrlmodule2rdyforuselampcmd_0{26, 2};
  CanSignalLe<engctrlmodule3rdyforuselampcmd_0> engctrlmodule3rdyforuselampcmd_0{28, 2};
  CanSignalLe<engspeedhighlampcmd_0> engspeedhighlampcmd_0{30, 2};
  CanSignalLe<engspeedveryhighlampcmd_0> engspeedveryhighlampcmd_0{32, 2};
  CanSignalLe<vhclaccelratelimitlampcmd_0> vhclaccelratelimitlampcmd_0{34, 2};
};

struct HVESSD6_VCU_5A : public CanMessage<0x8CF0955A, 8> {
  CanSignalLe<std::uint8_t> hvesstemp{56, 8, 1, -40, -40, 210};
};

struct MOCICNTRLPILOTSTAT : public CanMessage<0x98FF1480, 8> {
  CanSignalLe<std::uint8_t> controlpilot_mode{42, 3, 1, 0, 0, 7};
};

struct PCC_DM13 : public CanMessage<0x98DF11FE, 8> {
  enum class SuspendSignal : std::uint8_t {
    INDEFINITESUSPENSIONOFALLBROADCA = 0,
    INDEFINITESUSPENSIONOFSOMEMESSAG = 1,
    TEMPORARYSUSPENSIONOFALLBROADCAS = 2,
    TEMPORARYSUSPENSIONOFSOMEMESSAGE = 3,
    RESUMINGNORMALBROADCASTPATTERN = 14,
    NOTAVAILABLE = 15,
  };

  enum class SAE_J1922 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1850 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1587 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ManufacturerSpecificPort : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network3 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network2 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network1 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ISO9141 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class HoldSignal : std::uint8_t {
    ALL_DEVICES = 0,
    DEVICES_WHOSE_BROADCAST_STATE_HA = 1,
    NOT_AVAILABLE = 15,
  };

  enum class CurrentDataLink : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  CanSignalLe<SuspendSignal> SuspendSignal{24, 4};
  CanSignalLe<std::uint16_t> SuspendDuration{32, 16, 1, 0, 0, 64255};  // sec
  CanSignalLe<SAE_J1922> SAE_J1922{2, 2};
  CanSignalLe<SAE_J1850> SAE_J1850{10, 2};
  CanSignalLe<SAE_J1587> SAE_J1587{4, 2};
  CanSignalLe<ManufacturerSpecificPort> ManufacturerSpecificPort{8, 2};
  CanSignalLe<J1939Network3> J1939Network3{22, 2};
  CanSignalLe<J1939Network2> J1939Network2{14, 2};
  CanSignalLe<J1939Network1> J1939Network1{0, 2};
  CanSignalLe<ISO9141> ISO9141{12, 2};
  CanSignalLe<HoldSignal> HoldSignal{28, 4};
  CanSignalLe<CurrentDataLink> CurrentDataLink{6, 2};
};

struct HSI1_VCU : public CanMessage<0x98FED65A, 8> {
  CanSignalLe<std::uint8_t> hybsysoffbrdchrgrconnstat{6, 2, 1, 0, 0, 3};
};

struct FCG : public CanMessage<0x9CFEAF00, 8> {
  CanSignalLe<std::uint32_t> total_fuel_used_gaseous{32, 32, 0.5, 0, 0, 2105540607.5};  // kg/bit
};

struct HSS1_VCU : public CanMessage<0x98FCC25A, 8> {
  CanSignalLe<std::uint8_t> HybridSystemRegenBrakeInd_5a{20, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> HybridSystemOverheatIndic_5a{2, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> HybridSystemStopIndic_5a{4, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> HybridSystemInitializationInd_5a{22, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> HybridSystemWarningIndic_5a{0, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> strdengysrclvl{32, 8, 0.4, 0, 0, 100};  // %
};

struct PropB_VCU_00_ClntLvl : public CanMessage<0x98FF005A, 8> {
  CanSignalLe<std::uint8_t> clntloopclvllow{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> clntloopblvllow{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> clntloopalvllow{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> clntloopclvl{24, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> clntloopblvl{16, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> clntloopalvl{8, 8, 0.4, 0, 0, 100};  // %
};

struct MTPIMSTAT : public CanMessage<0x88FF00EF, 8> {
  CanSignalLe<std::uint32_t> mtrspd{40, 18, 1, -130000, -130000, 20000};  // rpm
};

struct MTPIMSWVERS : public CanMessage<0x98FF06EF, 8> {
  CanSignalLe<std::uint16_t> swverBranch{16, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint32_t> swverBuild{32, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint8_t> swverminor{8, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> swvermajor{0, 8, 1, 0, 0, 255};
};

struct MDCDCSTAT : public CanMessage<0x98FFD6B3, 8> {
  CanSignalLe<std::uint16_t> CoolTmpr{48, 16, 0.1, -40, -40, 110};  // degC
};

struct MHVACSTAT1 : public CanMessage<0x98FF2019, 8> {
  CanSignalLe<std::uint16_t> tmpract{32, 12, 0.1, -40, -40, 100};  // degC
  CanSignalLe<std::uint16_t> tmprset{44, 12, 0.1, -40, -40, 100};
};

struct MTPIMTRQINFO : public CanMessage<0x98FF03EF, 8> {
  CanSignalLe<std::uint16_t> trqavailregen{32, 16, 1, 0, 0, 65000};  // Nm
  CanSignalLe<std::uint16_t> trqavailmtr{16, 16, 1, 0, 0, 65000};  // Nm
  CanSignalLe<std::uint16_t> trqest{0, 16, 2, -65000, -65000, 10000};  // Nm
};

struct MG1IMT_VCU : public CanMessage<0x8C02FF5A, 8> {
  CanSignalLe<std::uint8_t> mtrgrtr1temp1{0, 8, 1, -40, -40, 210};  // �C
};

struct MBTMSSTAT : public CanMessage<0x98FF0ACA, 8> {
  CanSignalLe<std::uint16_t> btmspumpflowest{48, 16, 0.5, 0, 0, 150};  // lpm
  CanSignalLe<std::uint8_t> btmspumpspd{40, 8, 32, 0, 0, 8000};  // rpm
  CanSignalLe<std::uint8_t> btmsmode{0, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> BtmsCoolTmprOut{24, 8, 1, -40, -40, 210};  // degC
};

struct MBTMSSTAT2 : public CanMessage<0x98FF0BCA, 8> {
  CanSignalLe<std::uint8_t> btmsheatpwr{32, 8, 0.1, 0, 0, 25};  // kW
  CanSignalLe<std::uint8_t> btmschillpwr{24, 8, 0.1, 0, 0, 25};  // kW
};

struct MTPIMMTRROTATE2 : public CanMessage<0x98FF0BEF, 8> {
  CanSignalLe<std::uint64_t> MtrRotTotMtr{0, 48, 1, 0, 0, 65535};
};

struct B2V_ST2 : public CanMessage<0x98FF12F3, 8> {
  CanSignalLe<std::uint16_t> b2v_st2_maxlmt_dischgcurr{48, 16, 0.1, 0, 0, 6553.5};  // A
  CanSignalLe<std::uint16_t> b2v_st2_maxlmt_chgcurr{32, 16, 0.1, 0, 0, 6553.5};  // A
  CanSignalLe<std::uint16_t> b2v_st2_current{16, 16, 0.1, -1000, -1000, 5553.5};
  CanSignalLe<std::uint8_t> b2v_st2_soh{8, 8, 0.4, 0, 0, 102};  // pct
  CanSignalLe<std::uint8_t> b2v_st2_soc{0, 8, 0.4, 0, 0, 102};  // pct
};

struct B2V_ST4 : public CanMessage<0x98FF14F3, 8> {
  CanSignalLe<std::uint8_t> b2v_st4_avg_temp{16, 8, 1, -50, -50, 205};  // degC
  CanSignalLe<std::uint8_t> b2v_st4_min_temp{8, 8, 1, -50, -50, 205};  // degC
  CanSignalLe<std::uint8_t> b2v_st4_max_temp{0, 8, 1, -50, -50, 205};  // degC
};

struct MHVACSTAT2 : public CanMessage<0x98FF3019, 8> {
  CanSignalLe<std::uint8_t> hvpwract{16, 8, 0.1, 0, 0, 25.3};  // kW
  CanSignalLe<std::uint8_t> instancestat{0, 4, 1, 0, 0, 15};
};

struct PropB_C8 : public CanMessage<0x98FFC882, 8> {
  CanSignalLe<std::uint64_t> fms_Identification{0, 64, 1, 0, 0, 65535};
};

struct PropB_CB : public CanMessage<0x98FFCB82, 8> {
  CanSignalLe<std::uint16_t> fms_health{48, 16, 1, 0, 0, 65535};
};

struct PropB_C9 : public CanMessage<0x98FFC982, 8> {
  CanSignalLe<std::uint8_t> eng_airfilter6_diff_pressure{56, 8, 0.05, 0, 0, 12.5};  // KPa/bit
  CanSignalLe<std::uint8_t> eng_airfilter5_diff_pressure{48, 8, 0.05, 0, 0, 12.5};  // KPa/bit
  CanSignalLe<std::uint16_t> lube_dp_adv_res{32, 16, 0.02, 0, 0, 1250};  // KPa/bit
  CanSignalLe<std::uint16_t> stage2fueldpadvres{16, 16, 0.01, 0, 0, 500};  // KPa/bit
  CanSignalLe<std::uint16_t> stage1fueldpadvres{0, 16, 0.01, 0, 0, 500};  // KPa/bit
};

struct HVESSS1_VCU : public CanMessage<0x8CF0965A, 8> {
  CanSignalLe<std::uint8_t> hvessintchrgrstat{32, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> hvessopstat{44, 4, 1, 0, 0, 15};
};

struct HSS2_VCU : public CanMessage<0x98FCB75A, 8> {
  CanSignalLe<std::uint8_t> hybridsystemenginestop_5a{16, 8, 0.2, 0, 0, 51};  // hr
  CanSignalLe<std::uint16_t> distance_to_empty{32, 16, 0.03125, 0, 0, 2007.96875};  // km
};

struct MG1IS1 : public CanMessage<0x8CFB745A, 8> {
  CanSignalLe<std::uint8_t> mot_gen_status_5a{0, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint16_t> propulsion_torque_percent{12, 12, 0.0625, -125, -125, 125.9375};  // %
};

struct BC_DD_82 : public CanMessage<0x98FEFC82, 8> {
  CanSignalLe<std::uint8_t> enginefuelfilterdiffpressure_82{16, 8, 2, 0, 0, 500};  // kPa
};

struct E_IC1_82 : public CanMessage<0x98FEF682, 8> {
  CanSignalLe<std::uint8_t> eng_airfilter1_dif_pressure_82{32, 8, 0.05, 0, 0, 12.5};  // Kpa
  CanSignalLe<std::uint8_t> enginecoolantfilterdiffpress_82{56, 8, 0.5, 0, 0, 125};  // KPa
};

struct E_A1SCRAI2 : public CanMessage<0x90F03300, 8> {
  CanSignalLe<std::uint16_t> aft1_scr_intermediate_nh3{0, 16, 0.1, -200, -200, 6225.5};  // ppm
};

struct E_A1SCREGT2 : public CanMessage<0x98FCC500, 8> {
  CanSignalLe<std::uint16_t> aft1_scr_intrmediate_temperature{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
};

struct E_EOM : public CanMessage<0x9CFD0800, 8> {
  CanSignalLe<std::uint16_t> engine_oil_temperature3{48, 16, 0.03125, -273, -273, 1734.96875};
  CanSignalLe<std::uint16_t> engine_oil_relative_dielectric{32, 16, 0.0001220703125, 0, 0, 7.844};
  CanSignalLe<std::uint16_t> engine_oil_density{16, 16, 0.00003052, 0, 0, 1.961};  // g/cc
  CanSignalLe<std::uint16_t> engine_oil_viscosity{0, 16, 0.015625, 0, 0, 1003.984375};  // Cp
};

struct E_TCI2 : public CanMessage<0x98FE9A00, 8> {
  CanSignalLe<std::uint16_t> engine_turbocharger1_intaketemp{0, 16, 0.03125, -273, -273, 1734.96875};
};

struct E_EFS_82 : public CanMessage<0x98FE6A82, 8> {
  CanSignalLe<std::uint8_t> eng_fuelfilterpressuresuction_82{16, 8, 2, 0, 0, 500};  // KPa
};

struct E_EFS : public CanMessage<0x98FE6A00, 8> {
  CanSignalLe<std::uint8_t> engine_oilfilteroutletpressure{32, 8, 4, 0, 0, 1000};  // kPa
};

struct E_A1SCRDSI1 : public CanMessage<0x8CF02300, 8> {
  CanSignalLe<std::uint16_t> aft1_diesel_exhaust_fluid_act{24, 16, 0.1, 0, 0, 6425.5};  // g/bit
};

struct IC2 : public CanMessage<0x98FDD082, 8> {
  CanSignalLe<std::uint8_t> engine_air_filter4_diff_pressure{16, 8, 0.05, 0, 0, 12.5};  // kPa
  CanSignalLe<std::uint8_t> engine_air_filter3_diff_pressure{8, 8, 0.05, 0, 0, 12.5};  // kPa
  CanSignalLe<std::uint8_t> engine_air_filter2_diff_pressure{0, 8, 0.05, 0, 0, 12.5};  // kPa
};

struct EFP : public CanMessage<0x98FCE48C, 8> {
  CanSignalLe<std::uint16_t> engine_fuel1_temperature1{48, 16, 0.03125, -273, -273, 1734.96875};  // �C/bit
  CanSignalLe<std::uint16_t> engine_fuel_dielectricity{32, 16, 0.0001220703125, 0, 0, 7.844};
  CanSignalLe<std::uint16_t> engine_fuel_density{16, 16, 0.00003052, 0, 0, 1.961};  // g/cc
  CanSignalLe<std::uint16_t> engine_fuel_dynamic_viscosity{0, 16, 0.015625, 0, 0, 1003.984375};  // Cp
};

struct E_EI1 : public CanMessage<0x9CFE9200, 8> {
  CanSignalLe<std::uint16_t> EngFuelSys1GasMassFlowRate_00{32, 16, 0.05, 0, 0, 3212.75};  // kg/h
  CanSignalLe<std::uint8_t> engine_oil_filter_intake_press{0, 8, 4, 0, 0, 1000};  // kPa
  CanSignalLe<std::uint16_t> engine_exhaust_pressure_1{8, 16, 0.0078125, -250, -250, 251.99};  // kPa
};

struct E_AAI : public CanMessage<0x9CFE8C00, 8> {
  CanSignalLe<std::uint8_t> auxiliary_temperature_1{0, 8, 1, -40, -40, 210};  // deg C
  CanSignalLe<std::uint8_t> auxiliary_pressure_1{16, 8, 16, 0, 0, 4000};  // kPa
};

struct E_TC : public CanMessage<0x98FEDD00, 8> {
  CanSignalLe<std::uint16_t> engine_turbocharger_1_speed{8, 16, 4, 0, 0, 257020};
};

struct ST_ECMP_TEL : public CanMessage<0x98FFDD4A, 256> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct DM27 : public CanMessage<0x98FD82FE, 256> {
  enum class FailureModeIdentifier2 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashMalfuncIndicatorLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashRedStopLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class RedStopLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> SPNConversionMethod2{47, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OccurrenceCount2{40, 7, 1, 0, 0, 126};
  CanSignalLe<FailureModeIdentifier2> FailureModeIdentifier2{35, 5};
  CanSignalLe<std::uint32_t> SuspectParameterNumber2{16, 19, 1, 0, 0, 65536};
  CanSignalLe<FlashMalfuncIndicatorLamp2> FlashMalfuncIndicatorLamp2{14, 2};
  CanSignalLe<FlashRedStopLamp2> FlashRedStopLamp2{12, 2};
  CanSignalLe<FlashAmberWarningLamp2> FlashAmberWarningLamp2{10, 2};
  CanSignalLe<FlashProtectLamp2> FlashProtectLamp2{8, 2};
  CanSignalLe<std::uint8_t> MalfunctionIndicatorLampStatus2{6, 2, 1, 0, 0, 3};
  CanSignalLe<RedStopLampStatus2> RedStopLampStatus2{4, 2};
  CanSignalLe<AmberWarningLampStatus2> AmberWarningLampStatus2{2, 2};
  CanSignalLe<ProtectLampStatus2> ProtectLampStatus2{0, 2};
};

struct BC_EBC1_1_0 : public CanMessage<0x98F00100, 8> {
  CanSignalLe<std::uint8_t> tractormnttrailerabswarning_0{62, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> halt_brake_switch_0{58, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> railroad_mode_switch_0{56, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> src_add_ctrl_dev_brake_ctrl_0{48, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> abs_fully_operational_0{40, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> tractionctrloverrideswitch_0{22, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_hill_holder_switch_0{20, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_off_road_switch_0{18, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> abs_off_road_switch_0{16, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> brake_pedal_position_0{8, 8, 0.4, 0, 0, 100};
  CanSignalLe<std::uint8_t> ebs_brake_switch_0{6, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> antilock_braking_abs_active_0{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_brake_control_active_0{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_engine_control_active_0{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> ABS_EBS_Amber_Warning_0{44, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Trailer_ABS_0{60, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> ATC_ASR_Info_Signal_0{46, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EBS_Red_Warning_Signal_0{42, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Engine_Retarder_Selection_B_0{32, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Remote_Accel_Enable_Switch_B_0{30, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Aux_Engine_Shutdown_Switch_B_0{28, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Engine_Derate_Switch_B_0{26, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Accelerator_Interlock_Switch_B_0{24, 2, 1, 0, 0, 3};
};

struct PropB_29_BCM : public CanMessage<0x98FF2921, 8> {
  CanSignalLe<std::uint8_t> check_ac{9, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint16_t> distancetoemptydef{48, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint8_t> engineoiltemperature{13, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> frontaxleengaged{16, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint16_t> distancetoemptyfuel{32, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint8_t> diagnosticmodecmd{29, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> ptob_engaged{26, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> ptoa_engaged{25, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> alarmrequest{18, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> airfilterrestriction{17, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> powerdividerlock{15, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> lowcoolantlevel{14, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> oilpressure{12, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> retarderovertemp{11, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> retarderactive{10, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> lowoillevel{5, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> lowwasherfluid{3, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> servicebrake{4, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> changeoilfilter{6, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> highcoolanttemperature{7, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> servicefuelfilter{8, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> waterinfuel{2, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> brakefluidlow_na{1, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> brakefluidlow_export{0, 1, 1, 0, 0, 1};
};

struct DM13_Global : public CanMessage<0x98DFFFFE, 8> {
  enum class HoldSignal : std::uint8_t {
    ALL_DEVICES = 0,
    DEVICES_WHOSE_BROADCAST_STATE_HA = 1,
    NOT_AVAILABLE = 15,
  };

  enum class SuspendSignal : std::uint8_t {
    INDEFINITESUSPENSIONOFALLBROADCA = 0,
    INDEFINITESUSPENSIONOFSOMEMESSAG = 1,
    TEMPORARYSUSPENSIONOFALLBROADCAS = 2,
    TEMPORARYSUSPENSIONOFSOMEMESSAGE = 3,
    RESUMINGNORMALBROADCASTPATTERN = 14,
    NOTAVAILABLE = 15,
  };

  enum class J1939Network3 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network2 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ISO9141 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1850 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ManufacturerSpecificPort : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class CurrentDataLink : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1587 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1922 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network1 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  CanSignalLe<std::uint16_t> SuspendDuration{32, 16, 1, 0, 0, 64255};  // sec
  CanSignalLe<HoldSignal> HoldSignal{28, 4};
  CanSignalLe<SuspendSignal> SuspendSignal{24, 4};
  CanSignalLe<J1939Network3> J1939Network3{22, 2};
  CanSignalLe<J1939Network2> J1939Network2{14, 2};
  CanSignalLe<ISO9141> ISO9141{12, 2};
  CanSignalLe<SAE_J1850> SAE_J1850{10, 2};
  CanSignalLe<ManufacturerSpecificPort> ManufacturerSpecificPort{8, 2};
  CanSignalLe<CurrentDataLink> CurrentDataLink{6, 2};
  CanSignalLe<SAE_J1587> SAE_J1587{4, 2};
  CanSignalLe<SAE_J1922> SAE_J1922{2, 2};
  CanSignalLe<J1939Network1> J1939Network1{0, 2};
};

struct BFC : public CanMessage<0x98FF3021, 8> {
  CanSignalLe<std::uint8_t> Wheelchair_Lift_Door_cmd{44, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> CrossingGate_Cmd{40, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> StopArm_cmd{36, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> StopArmLamp_cmd{32, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Right_Red_Lamp_cmd{14, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Left_Red_Lamp_cmd{12, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Right_Red_Lamp_cmd{10, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Left_Red_Lamp_cmd{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Right_Amber_Lamp_cmd{6, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Left_Amber_Lamp_cmd{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Right_Amber_Lamp_cmd{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Front_Left_Amber_Lamp_cmd{0, 2, 1, 0, 0, 3};
};

struct VCCD : public CanMessage<0x98FF3321, 8> {
  CanSignalLe<std::uint8_t> Rear_Right_Turn_Signal_Status{46, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Rear_Left_Turn_Signal_Status{42, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Right_High_Beam_Headlamp_Status{38, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Left_High_Beam_Headlamp_Status{36, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Right_Low_Beam_Headlamp_Status{34, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Left_Low_Beam_Headlamp_Status{32, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Aggregate_Door_Switch_Status{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Air_Horn_Status{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Electric_Horn_Status{0, 2, 1, 0, 0, 3};
};

struct ETC5_21 : public CanMessage<0x9CFEC321, 8> {
  CanSignalLe<std::uint8_t> Transmission_Neutral_Switch_21{10, 2, 1, 0, 0, 3};
};

struct TF1 : public CanMessage<0x98FEF803, 8> {
  CanSignalLe<std::uint8_t> transoillvlmeasurementstatus_03{60, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> transmissionoillevel1cnttimer_03{56, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> transmission1oilpressure_03{24, 8, 16, 0, 0, 4000};
  CanSignalLe<std::uint8_t> transmissionfilterdiffpress_03{16, 8, 2, 0, 0, 500};  // Kpa
  CanSignalLe<std::uint8_t> transmissionclutch1pressure_03{0, 8, 16, 0, 0, 4000};
  CanSignalLe<std::uint8_t> transmission_oil_level{8, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> transmission_oil_temperature1{32, 16, 0.03125, -273, -273, 1774.96875};  // deg C/bit
  CanSignalLe<std::uint8_t> Transmission_Oil_Level_HiLo{48, 8, 0.5, -62.5, -62.5, 62.5};  // L/bit
};

struct BCM_UDS_RX_4A : public CanMessage<0x98DA4A21, 1024> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct UDS_RX_FA : public CanMessage<0x98DAFAFE, 1024> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct UDS_TX_FA : public CanMessage<0x98DAFEFA, 1024> {
  CanSignalLe<std::uint8_t> BCM_UDS_TX_4A_placeholder{0, 8, 1, 0, 0, 255};
};

struct UDS_RX_F9 : public CanMessage<0x98DAF9FE, 1024> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct UDS_TX_F9 : public CanMessage<0x98DAFEF9, 1024> {
  CanSignalLe<std::uint8_t> BCM_UDS_TX_4A_placeholder{0, 8, 1, 0, 0, 255};
};

struct UDS_TX_4A : public CanMessage<0x98DAFE4A, 1024> {
  CanSignalLe<std::uint8_t> BCM_UDS_TX_4A_placeholder{0, 8, 1, 0, 0, 255};
};

struct E_CCVS1_21 : public CanMessage<0x98FEF121, 8> {
  enum class ParkBrakeReleaseInhibitRqst_21 : std::uint8_t {
    CRISECTRLACTIVATORNOTINPOSCOAST = 0,
    CRUISECTRLACTIVATORINPOSCOAST = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlPauseSwitch_21 : std::uint8_t {
    CRISECTRLACTIVATORNOTINPOSCOAST = 0,
    CRUISECTRLACTIVATORINPOSCOAST = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Engine_Test_Mode_Switch_A_21 : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Engine_Idle_Decrement_Switch_21 : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Engine_Idle_Increment_Switch_21 : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlState_21 : std::uint8_t {
    OFF_DISABLED = 0,
    HOLD = 1,
    ACCELERATE = 2,
    DECELERATE = 3,
    RESUME = 4,
    SET = 5,
    ACCELERATOROVERRIDE = 6,
    NOTAVAILABLE = 7,
  };

  enum class PTOState_21 : std::uint8_t {
    OFF_DISABLED = 0,
    HOLD = 1,
    REMOTEHOLD = 2,
    STANDBY = 3,
    REMOTESTANDBY = 4,
    SET = 5,
    DECELERATE_COAST = 6,
    RESUME = 7,
    ACCELERATE = 8,
    ACCELERATOROVERRIDE = 9,
    PREPROGRAMMED_SET_SPEED_1 = 10,
    PREPROGRAMMED_SET_SPEED_2 = 11,
    PREPROGRAMMED_SET_SPEED_3 = 12,
    PREPROGRAMMED_SET_SPEED_4 = 13,
    PREPROGRAMMED_SET_SPEED_5 = 14,
    PREPROGRAMMED_SET_SPEED_6 = 15,
    PREPROGRAMMED_SET_SPEED_7 = 16,
    PREPROGRAMMED_SET_SPEED_8 = 17,
    PTO_SET_SPEED_MEMORY_1 = 18,
    PTO_SET_SPEED_MEMORY_2 = 19,
    NOTDEFINED = 20,
    NOTAVAILABLE = 31,
  };

  enum class CruiseControlAccelerateSwitch_21 : std::uint8_t {
    CRSCTRLACTVTRNOTINPOSACCELERATE = 0,
    CRSCTRLACTIVATORINPOSACCELERATE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlResumeSwitch_21 : std::uint8_t {
    CRSECTRLACTIVATORNOTINPOSRESUME = 0,
    CRUISECTRLACTIVATORINPOSRESUME = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlCoastSwitch_21 : std::uint8_t {
    CRISECTRLACTIVATORNOTINPOSCOAST = 0,
    CRUISECTRLACTIVATORINPOSCOAST = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlSetSwitch_21 : std::uint8_t {
    CRUISECTRLACTIVATORNOTINPOSSET = 0,
    CRUISECTRLACTIVATORINPOSSET = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class ClutchSwitch_21 : std::uint8_t {
    CLUTCHPEDALRELEASED = 0,
    CLUTCHPEDALDEPRESSED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class brakeswitch_21 : std::uint8_t {
    BRAKEPEDALRELEASED = 0,
    BRAKEPEDALDEPRESSED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlEnableSwitch_21 : std::uint8_t {
    CRUISECTRLDISABLED = 0,
    CRUISECTRLENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlActive_21 : std::uint8_t {
    CRUISECTRLSWITCHEDOFF = 0,
    CRUISECTRLSWITCHEDON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class ParkingBrakeSwitch_21 : std::uint8_t {
    PARKINGBRAKENOTSET = 0,
    PARKINGBRAKESET = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class TwoSpeedAxleSwitch_21 : std::uint8_t {
    LOWSPEEDRANGE = 0,
    HIGHSPEEDRANGE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<ParkBrakeReleaseInhibitRqst_21> ParkBrakeReleaseInhibitRqst_21{6, 2};
  CanSignalLe<CruiseControlPauseSwitch_21> CruiseControlPauseSwitch_21{4, 2};
  CanSignalLe<std::uint8_t> EngShutdownOverrideSwitchA_21{62, 2, 1, 0, 0, 3};
  CanSignalLe<Engine_Test_Mode_Switch_A_21> Engine_Test_Mode_Switch_A_21{60, 2};
  CanSignalLe<Engine_Idle_Decrement_Switch_21> Engine_Idle_Decrement_Switch_21{58, 2};
  CanSignalLe<Engine_Idle_Increment_Switch_21> Engine_Idle_Increment_Switch_21{56, 2};
  CanSignalLe<CruiseControlState_21> CruiseControlState_21{53, 3};
  CanSignalLe<PTOState_21> PTOState_21{48, 5};
  CanSignalLe<std::uint8_t> CruiseControlSetSpeed_21{40, 8, 1, 0, 0, 250};  // km/h
  CanSignalLe<CruiseControlAccelerateSwitch_21> CruiseControlAccelerateSwitch_21{38, 2};
  CanSignalLe<CruiseControlResumeSwitch_21> CruiseControlResumeSwitch_21{36, 2};
  CanSignalLe<CruiseControlCoastSwitch_21> CruiseControlCoastSwitch_21{34, 2};
  CanSignalLe<CruiseControlSetSwitch_21> CruiseControlSetSwitch_21{32, 2};
  CanSignalLe<ClutchSwitch_21> ClutchSwitch_21{30, 2};
  CanSignalLe<brakeswitch_21> brakeswitch_21{28, 2};
  CanSignalLe<CruiseControlEnableSwitch_21> CruiseControlEnableSwitch_21{26, 2};
  CanSignalLe<CruiseControlActive_21> CruiseControlActive_21{24, 2};
  CanSignalLe<std::uint16_t> WheelBasedVehicleSpeed_21{8, 16, 0.00390625, 0, 0, 250.996};  // km/h
  CanSignalLe<ParkingBrakeSwitch_21> ParkingBrakeSwitch_21{2, 2};
  CanSignalLe<TwoSpeedAxleSwitch_21> TwoSpeedAxleSwitch_21{0, 2};
};

struct E_VEP1_21 : public CanMessage<0x98FEF721, 8> {
  CanSignalLe<std::uint8_t> sli_battery1_net_current_21{0, 8, 1, -125, -125, 125};  // A
  CanSignalLe<std::uint16_t> charging_system_potential_21{16, 16, 1, 0, 0, 3212.75};
  CanSignalLe<std::uint8_t> alternator_current_21{8, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint16_t> Keyswitch_Battery_Potential_21{48, 16, 0.05, 0, 0, 3212.75};  // V
  CanSignalLe<std::uint16_t> Battery_Potential_21{32, 16, 0.05, 0, 0, 3212.75};  // V
};

struct BC_EBC1_1_B : public CanMessage<0x98F0010B, 8> {
  CanSignalLe<std::uint8_t> tractormnttrailerabswarning_B_B{62, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> halt_brake_switch_B_B{58, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> railroad_mode_switch_B_B{56, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> src_add_ctrl_dev_brake_ctrl_B_B{48, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> abs_fully_operational_B_B{40, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> tractionctrloverrideswitch_B_B{22, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_hill_holder_switch_B_B{20, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_off_road_switch_B_B{18, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> abs_off_road_switch_B_B{16, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> brake_pedal_position_B_B{8, 8, 0.4, 0, 0, 100};
  CanSignalLe<std::uint8_t> ebs_brake_switch_B_B{6, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> antilock_braking_abs_active_B_B{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_brake_control_active_B_B{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> asr_engine_control_active_B_B{0, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Trailer_ABS_B{60, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> ATC_ASR_Info_Signal_B{46, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> ABS_EBS_Amber_Warning_Signal_B{44, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> EBS_Red_Warning_Signal_B{42, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Engine_Retarder_Selection_B_B{32, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Remote_Accel_Enable_Switch_B_B{30, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Aux_Engine_Shutdown_Switch_B_B{28, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Engine_Derate_Switch_B_B{26, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Accelerator_Interlock_Switch_B_B{24, 2, 1, 0, 0, 3};
};

struct BRAKES : public CanMessage<0x98FEFA21, 8> {
  CanSignalLe<std::uint8_t> parkingbrakeredwarningsignal_21{26, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> parkingbrakeactuator_21{24, 2, 1, 0, 0, 3};  // 4 states/2 bit
  CanSignalLe<std::uint8_t> brakeapplicationpressure_21{0, 8, 4, 0, 0, 1000};  // kPa/bit
  CanSignalLe<std::uint8_t> Brake_Sec_Pressure{16, 8, 4, 0, 0, 1000};  // kpa/bit
  CanSignalLe<std::uint8_t> Brake_Prim_Pressure{8, 8, 4, 0, 0, 1000};  // kpa/bit
};

struct VDSC1 : public CanMessage<0x98FE4F0B, 8> {
  CanSignalLe<std::uint8_t> trailer_vdc_active{16, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> yc_brake_control_active{14, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> yc_engine_control_active{12, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> rop_brake_control_active{10, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> rop_engine_control_active{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> vdcbrakelightrequest{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> vdcfullyoperational{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Electronic_Stability_Control{0, 2, 1, 0, 0, 3};
};

struct Tel_DM3 : public CanMessage<0x98FECC4A, 1> {
  CanSignalLe<std::int8_t> DM3_Signal{0, 1, 1, 0, 0, 0};
};

struct Tel_DM11 : public CanMessage<0x98FED34A, 1> {
  CanSignalLe<std::int8_t> DM11_Signal{0, 1, 1, 0, 0, 0};
};

struct Tel_RQST_RX : public CanMessage<0x98EA4AFE, 3> {
  CanSignalLe<std::uint32_t> ParameterGroupNumber_RQST_RX{0, 24, 1, 0, 0, 16777215};
};

struct CI : public CanMessage<0x98FEEBFE, 46> {
  CanSignalLe<std::uint16_t> UnitNumber{36, 11, 1, 0, 0, 2047};
  CanSignalLe<std::uint16_t> SerialNumber{27, 9, 1, 0, 0, 511};
  CanSignalLe<std::uint32_t> Model{7, 20, 1, 0, 0, 1048575};
  CanSignalLe<std::uint8_t> Make{0, 6, 1, 0, 0, 63};
};

struct ETC6 : public CanMessage<0x9CFEAB03, 8> {
  CanSignalLe<std::uint8_t> Highest_Poss_Gear{8, 8, 1, -125, -125, 125};
};

struct TC : public CanMessage<0x98FEF433, 8> {
  CanSignalLe<std::uint8_t> extended_tire_pressure_support{38, 2, 1, 0, 0, 3};  // 4states/2bit
  CanSignalLe<std::uint8_t> tirepressure_threshold_detection{61, 3, 1, 0, 0, 7};  // states/3 bit
  CanSignalLe<std::uint16_t> tire_air_leakage_rate{40, 16, 0.1, 0, 0, 6425.5};  // Pa/s per bit
  CanSignalLe<std::uint8_t> tire_sensor_electrical_fault{36, 2, 1, 0, 0, 3};  // states/2 bit
  CanSignalLe<std::uint8_t> tire_status{34, 2, 1, 0, 0, 3};  // states/2 bit
  CanSignalLe<std::uint8_t> tire_sensor_enable_status{32, 2, 1, 0, 0, 3};  // states/2 bit
  CanSignalLe<std::uint16_t> Tire_Temperature{16, 16, 0.03125, -273, -273, 1734.96875};  // C/bit
  CanSignalLe<std::uint8_t> Tire_Pressure{8, 8, 4, 0, 0, 1000};  // kPa
  CanSignalLe<std::uint8_t> Tire_Location{0, 8, 1, 0, 0, 255};  // states/8 bit
};

struct Tel_PGN_Request_4A : public CanMessage<0x98EAFE4A, 3> {
  CanSignalLe<std::uint32_t> ParameterGroupNumber{0, 24, 1, 0, 0, 16777215};
};

struct CIM : public CanMessage<0x98D0FF37, 8> {
  CanSignalLe<std::uint8_t> Illumination_Brightness_Percent{0, 8, 0.4, 0, 0, 100};  // %
};

struct EII : public CanMessage<0x98FDC5FE, 512> {
  CanSignalLe<std::uint8_t> ECU_Type{32, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Serial_Number{24, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Part_Number{16, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Manufacturer_Name{8, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Location{0, 6, 1, 0, 0, 63};
};

struct Tel_EII : public CanMessage<0x98FDC54A, 512> {
  CanSignalLe<std::uint8_t> ECU_Manufacturer_Name{32, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Type{24, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Location{16, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Part_Number{8, 6, 1, 0, 0, 63};
  CanSignalLe<std::uint8_t> ECU_Serial_Number{0, 6, 1, 0, 0, 63};
};

struct BCM_UDS_TX_4A : public CanMessage<0x98DA214A, 1024> {
  CanSignalLe<std::uint8_t> BCM_UDS_TX_4A_placeholder{0, 8, 1, 0, 0, 255};
};

struct UDS_RX_4A : public CanMessage<0x98DA4AFE, 1024> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct DM20 : public CanMessage<0x98C2FFFE, 512> {
  CanSignalLe<std::uint16_t> ApplSysMonitorNumerator_DM20{56, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint16_t> ApplSysMonitorDenominator_DM20{72, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint32_t> SPNofApplSysMonitor_DM20{32, 24, 1, 0, 0, 524287};
  CanSignalLe<std::uint16_t> OBDMonitoringCondEncount_DM20{16, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint16_t> IgnitionCycleCounter_DM20{0, 16, 1, 0, 0, 65535};
};

struct OnBoard_Program1_RX_4B : public CanMessage<0x98684B21, 8> {
  CanSignalLe<std::uint8_t> OnBoard_Programming_Progress{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> OBP_Key_Switch_Request{28, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Tgt_Not_Ready{26, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Pwr_Supply_Insuff{24, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Park_Brake{22, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Key_Switch_Pos{20, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Engine_Speed{18, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Overall_Status{16, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Programming_Status{8, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Target_ECU_Address{0, 8, 1, 0, 0, 255};
};

struct DM19 : public CanMessage<0x98D3FFFE, 512> {
  CanSignalLe<std::uint32_t> CalibrationID4{128, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint32_t> CalibrationID3{96, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint32_t> CalibrationID2{64, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint32_t> CalibrationID1{32, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint32_t> CalibrationVerificationNumber{0, 32, 1, 0, 0, 4294967295};
};

struct DM2 : public CanMessage<0x98FECBFE, 256> {
  enum class FailureModeIdentifier2 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashMalfuncIndicatorLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashRedStopLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class RedStopLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> SPNConversionMethod2{47, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OccurrenceCount2{40, 7, 1, 0, 0, 126};
  CanSignalLe<FailureModeIdentifier2> FailureModeIdentifier2{35, 5};
  CanSignalLe<std::uint32_t> SuspectParameterNumber2{16, 19, 1, 0, 0, 65536};
  CanSignalLe<FlashMalfuncIndicatorLamp2> FlashMalfuncIndicatorLamp2{14, 2};
  CanSignalLe<FlashRedStopLamp2> FlashRedStopLamp2{12, 2};
  CanSignalLe<FlashAmberWarningLamp2> FlashAmberWarningLamp2{10, 2};
  CanSignalLe<FlashProtectLamp2> FlashProtectLamp2{8, 2};
  CanSignalLe<std::uint8_t> MalfunctionIndicatorLampStatus2{6, 2, 1, 0, 0, 3};
  CanSignalLe<RedStopLampStatus2> RedStopLampStatus2{4, 2};
  CanSignalLe<AmberWarningLampStatus2> AmberWarningLampStatus2{2, 2};
  CanSignalLe<ProtectLampStatus2> ProtectLampStatus2{0, 2};
};

struct Tel_SOFT : public CanMessage<0x98FEDA4A, 256> {
  CanSignalLe<std::uint8_t> SoftwareIdentification{8, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> NumberOfSoftwareIdFields{0, 8, 1, 0, 0, 255};
};

struct Tel_CI : public CanMessage<0x98FEEB4A, 512> {
  CanSignalLe<std::uint16_t> UnitNumber{56, 11, 1, 0, 0, 2047};
  CanSignalLe<std::uint16_t> SerialNumber{40, 9, 1, 0, 0, 511};
  CanSignalLe<std::uint32_t> Model{16, 20, 1, 0, 0, 1048575};
  CanSignalLe<std::uint8_t> Make{0, 6, 1, 0, 0, 63};
};

struct Tel_DM2 : public CanMessage<0x98FECB4A, 256> {
  enum class FailureModeIdentifier2 : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashMalfuncIndicatorLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashRedStopLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp2 : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class RedStopLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatus2 : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> SPNConversionMethod2{47, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OccurrenceCount2{40, 7, 1, 0, 0, 126};
  CanSignalLe<FailureModeIdentifier2> FailureModeIdentifier2{35, 5};
  CanSignalLe<std::uint32_t> SuspectParameterNumber2{16, 19, 1, 0, 0, 65536};
  CanSignalLe<FlashMalfuncIndicatorLamp2> FlashMalfuncIndicatorLamp2{14, 2};
  CanSignalLe<FlashRedStopLamp2> FlashRedStopLamp2{12, 2};
  CanSignalLe<FlashAmberWarningLamp2> FlashAmberWarningLamp2{10, 2};
  CanSignalLe<FlashProtectLamp2> FlashProtectLamp2{8, 2};
  CanSignalLe<std::uint8_t> MalfunctionIndicatorLampStatus2{6, 2, 1, 0, 0, 3};
  CanSignalLe<RedStopLampStatus2> RedStopLampStatus2{4, 2};
  CanSignalLe<AmberWarningLampStatus2> AmberWarningLampStatus2{2, 2};
  CanSignalLe<ProtectLampStatus2> ProtectLampStatus2{0, 2};
};

struct OnBoard_Program2_RX : public CanMessage<0x98FB8121, 8> {
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_Inhibit{10, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_Rqst{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_ECU_Addr{0, 8, 1, 0, 0, 255};
};

struct OnBoard_Program1_TX_4A : public CanMessage<0x9868214A, 8> {
  CanSignalLe<std::uint8_t> OnBoard_Programming_Progress{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> OBP_Key_Switch_Request{28, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Tgt_Not_Ready{26, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Pwr_Supply_Insuff{24, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Park_Brake{22, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Key_Switch_Pos{20, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Engine_Speed{18, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Overall_Status{16, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Programming_Status{8, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Target_ECU_Address{0, 8, 1, 0, 0, 255};
};

struct ST_ECMP_RX : public CanMessage<0x98FFDD00, 256> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct E_ESLFI : public CanMessage<0x9CFEB700, 10> {
  CanSignalLe<std::uint32_t> Total_Engine_Cruise_Time{48, 32, 0.05, 0, 0, 210554060.75};  // hr
};

struct PropB_Vehicle_State_BCM : public CanMessage<0x98FFF821, 8> {
  enum class VehicleOperationalMode_21 : std::uint8_t {
    NORMAL = 0,
    PERFORMANCEPLUS = 1,
    ECONOMY = 2,
    OFFROAD = 3,
    BALANCED_PERFORMANCE = 4,
    RESERVED_1 = 5,
    RESERVED_2 = 6,
    RESERVED_3 = 7,
    RESERVED_4 = 8,
    RESERVED_5 = 9,
    RESERVED_6 = 10,
    RESERVED_7 = 11,
    RESERVED_8 = 12,
    RESERVED_9 = 13,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  CanSignalLe<VehicleOperationalMode_21> VehicleOperationalMode_21{8, 4};
  CanSignalLe<std::uint8_t> Vehicle_State_MsgCounter{57, 7, 1, 0, 0, 127};
  CanSignalLe<std::uint8_t> MsgCtr_OddParity_Bit{56, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> Crank_GridState{36, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Ign_GridState{34, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Acc_GridState{32, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Load_Control_States{6, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Vehicle_AutoTheft_Status{4, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Vehicle_AutoSS_Status{2, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Vehicle_SleepMode{0, 2, 1, 0, 0, 3};
};

struct Prop_A_Cluster_TX_4A : public CanMessage<0x98EF174A, 200> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct Proprietary_A_TX_4A : public CanMessage<0x98EF214A, 8> {
  enum class RemoteLockUnlockReq_21_4A : std::uint8_t {
    NO_REQUEST = 0,
    DOOR_LOCK_REQUESTED = 1,
    DOOR_UNLOCK_REQUESTED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class V2xVehicleRequest_21_4A : std::uint8_t {
    NOT_REQUESTED = 0,
    REQUESTED = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<RemoteLockUnlockReq_21_4A> RemoteLockUnlockReq_21_4A{32, 2};
  CanSignalLe<V2xVehicleRequest_21_4A> V2xVehicleRequest_21_4A{34, 2};
  CanSignalLe<std::uint8_t> Message_Checksum_Value{48, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Message_Counter_Value{44, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint16_t> Est_Remaining_Programming_time{16, 16, 1, 0, 0, 65535};
  CanSignalLe<std::uint8_t> Programming_Type{12, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> Crank_Inhibit_Request{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Comm_Header_Byte{0, 8, 1, 0, 0, 255};
};

struct Prop_A_Cluster_RX_4A : public CanMessage<0x98EF4A17, 200> {
  CanSignalLe<std::uint8_t> Freeform_Text_Messaging{0, 8, 1, 0, 0, 255};
};

struct OnBoard_Program2_TX_4A : public CanMessage<0x98FB814A, 8> {
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_Inhibit{10, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_Rqst{8, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Initiate_ECU_Addr{0, 8, 1, 0, 0, 255};
};

struct OnBoard_Program1_RX_4A : public CanMessage<0x98684A21, 8> {
  CanSignalLe<std::uint8_t> OnBoard_Programming_Progress{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> OBP_Key_Switch_Request{28, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Tgt_Not_Ready{26, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Pwr_Supply_Insuff{24, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Park_Brake{22, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Key_Switch_Pos{20, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Engine_Speed{18, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OBP_Interlock_Overall_Status{16, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> OnBoard_Programming_Status{8, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> OnBoard_Prgm_Target_ECU_Address{0, 8, 1, 0, 0, 255};
};

struct SOFT : public CanMessage<0x98FEDAFE, 256> {
  CanSignalLe<std::uint8_t> SoftwareIdentification{8, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> NumberOfSoftwareIdFields{0, 8, 1, 0, 0, 255};
};

struct Tel_DM1 : public CanMessage<0x98FECA4A, 160> {
  enum class FailureModeIdentifier : std::uint8_t {
    ABOVE_NORMAL_MOST_SEVERE = 0,
    BELOW_NORMAL_MOST_SEVERE = 1,
    DATA_ERRATIC = 2,
    VOLTAGE_ABOVE_NORMAL = 3,
    VOLTAGE_BELOW_NORMAL = 4,
    CURRENT_BELOW_NORMAL = 5,
    CURRENT_ABOVE_NORMAL = 6,
    MECHANICAL_SYSTEM_NOT_RESPONDING = 7,
    ABNORMAL_FREQUENCY = 8,
    ABNORMAL_UPDATE_RATE = 9,
    ABNORMAL_RATE_OF_CHANGE = 10,
    ROOT_CAUSE_NOT_KNOWN = 11,
    BAD_INTELLIGENT_DEVICE = 12,
    OUT_OF_CALIBRATION = 13,
    SPECIAL_INTSTRUCTIONS = 14,
    ABOVE_NORMAL_LEAST_SEVERE = 15,
    ABOVE_NORMAL_MODERATELY_SEVERE = 16,
    BELOW_NORMAL_LEAST_SEVERE = 17,
    BELOW_NORMAL_MODERATELS_SEVERE = 18,
    RECEIVED_NETWORK_DATA_IN_ERROR = 19,
    NOT_AVAILABLE = 31,
  };

  enum class FlashMalfuncIndicatorLamp : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashRedStopLamp : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashAmberWarningLamp : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class FlashProtectLamp : std::uint8_t {
    SLOW_FLASH = 0,
    FAST_FLASH = 1,
    RESERVED = 2,
    UNAVAILABLE_DO_NOT_FLASH = 3,
  };

  enum class RedStopLampState : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class AmberWarningLampStatus : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  enum class ProtectLampStatus : std::uint8_t {
    LAMP_OFF = 0,
    LAMP_ON = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> SPNConversionMethod{47, 1, 1, 0, 0, 1};
  CanSignalLe<std::uint8_t> OccurrenceCount{40, 7, 1, 0, 0, 126};
  CanSignalLe<FailureModeIdentifier> FailureModeIdentifier{35, 5};
  CanSignalLe<std::uint32_t> SuspectParameterNumber{16, 19, 1, 0, 0, 65536};
  CanSignalLe<FlashMalfuncIndicatorLamp> FlashMalfuncIndicatorLamp{14, 2};
  CanSignalLe<FlashRedStopLamp> FlashRedStopLamp{12, 2};
  CanSignalLe<FlashAmberWarningLamp> FlashAmberWarningLamp{10, 2};
  CanSignalLe<FlashProtectLamp> FlashProtectLamp{8, 2};
  CanSignalLe<std::uint8_t> MalfunctionIndicatorLampStatus{6, 2, 1, 0, 0, 3};
  CanSignalLe<RedStopLampState> RedStopLampState{4, 2};
  CanSignalLe<AmberWarningLampStatus> AmberWarningLampStatus{2, 2};
  CanSignalLe<ProtectLampStatus> ProtectLampStatus{0, 2};
};

struct Tel_DM13 : public CanMessage<0x98DF4AFE, 8> {
  enum class HoldSignal : std::uint8_t {
    ALL_DEVICES = 0,
    DEVICES_WHOSE_BROADCAST_STATE_HA = 1,
    NOT_AVAILABLE = 15,
  };

  enum class SuspendSignal : std::uint8_t {
    INDEFINITESUSPENSIONOFALLBROADCA = 0,
    INDEFINITESUSPENSIONOFSOMEMESSAG = 1,
    TEMPORARYSUSPENSIONOFALLBROADCAS = 2,
    TEMPORARYSUSPENSIONOFSOMEMESSAGE = 3,
    RESUMINGNORMALBROADCASTPATTERN = 14,
    NOTAVAILABLE = 15,
  };

  enum class J1939Network3 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network2 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ISO9141 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1850 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class ManufacturerSpecificPort : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class CurrentDataLink : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1587 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class SAE_J1922 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  enum class J1939Network1 : std::uint8_t {
    STOP_BROADCAST = 0,
    START_BROADCAST = 1,
    RESERVED = 2,
    DONT_CARE = 3,
  };

  CanSignalLe<std::uint16_t> SuspendDuration{32, 16, 1, 0, 0, 64255};  // sec
  CanSignalLe<HoldSignal> HoldSignal{28, 4};
  CanSignalLe<SuspendSignal> SuspendSignal{24, 4};
  CanSignalLe<J1939Network3> J1939Network3{22, 2};
  CanSignalLe<J1939Network2> J1939Network2{14, 2};
  CanSignalLe<ISO9141> ISO9141{12, 2};
  CanSignalLe<SAE_J1850> SAE_J1850{10, 2};
  CanSignalLe<ManufacturerSpecificPort> ManufacturerSpecificPort{8, 2};
  CanSignalLe<CurrentDataLink> CurrentDataLink{6, 2};
  CanSignalLe<SAE_J1587> SAE_J1587{4, 2};
  CanSignalLe<SAE_J1922> SAE_J1922{2, 2};
  CanSignalLe<J1939Network1> J1939Network1{0, 2};
};

struct E_AT1T1I_1 : public CanMessage<0x98FE5600, 8> {
  enum class SCR_Oper_Induc_Severity : std::uint8_t {
    NO_INDUCEMENT_ACTIVE = 0,
    INDUCEMENT_LEVEL_1_1 = 1,
    INDUCEMENT_LEVEL_1_2 = 2,
    INDUCEMENT_LEVEL_2 = 3,
    INDUCEMENT_LEVEL_3 = 4,
    INDUCEMENT_LEVEL_4 = 5,
    RESERVED = 6,
    NOT_AVAILABLE_NOT_SUPPORTED = 7,
  };

  enum class SCR_Oper_Induc_Act : std::uint8_t {
    OFF = 0,
    ON_SOLID = 1,
    RESERVED_FOR_SAE_ASSIGNMENT_1 = 2,
    RESERVED_FOR_SAE_ASSIGNMENT_2 = 3,
    ON_FAST_BLINK_1_HZ = 4,
    RESERVED_FOR_SAE_ASSIGNMENT_3 = 5,
    RESERVED_FOR_SAE_ASSIGNMENT_4 = 6,
    NOT_AVAILABLE = 7,
  };

  CanSignalLe<std::uint8_t> Aft1DsllExhaustFluidTnk1Heater_0{56, 5, 1, 0, 0, 31};
  CanSignalLe<std::uint8_t> Aft1DslExhaustFluidTnk1TmpFmi_0{40, 5, 1, 0, 0, 31};
  CanSignalLe<std::uint8_t> aft1deftankheater_0{48, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> aft1deftanklevel_0{16, 16, 0.1, 0, 0, 6425.5};  //  mm/bit
  CanSignalLe<std::uint8_t> aft1deftanklevelpreliminaryfmi_0{32, 5, 1, 0, 0, 31};
  CanSignalLe<SCR_Oper_Induc_Severity> SCR_Oper_Induc_Severity{45, 3};
  CanSignalLe<SCR_Oper_Induc_Act> SCR_Oper_Induc_Act{37, 3};
  CanSignalLe<std::uint8_t> DEF_Tank_Temperature{8, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> DEF_Tank_Level_B{0, 8, 0.4, 0, 0, 100};  // %
};

struct E_OII : public CanMessage<0x98FC2A00, 8> {
  CanSignalLe<std::uint8_t> oi_torque_derate_l2{24, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint8_t> oi_torque_derate_l1{0, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint16_t> OI_Time_Trq_Derate_L2{32, 16, 1, 0, 0, 64255};  // min
  CanSignalLe<std::uint16_t> OI_Time_Trq_Derate_L1{8, 16, 1, 0, 0, 64255};  // min
};

struct E_EEC6 : public CanMessage<0x90FDA300, 8> {
  CanSignalLe<std::uint8_t> VGT_posn_des{16, 8, 0.4, 0, 0, 100};  // %
};

struct E_AT1AC1 : public CanMessage<0x98FD9F00, 8> {
  enum class AFT1_Purge_Air_Actuator : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    RESERVED = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<AFT1_Purge_Air_Actuator> AFT1_Purge_Air_Actuator{60, 2};
};

struct E_AT1TI_1_2 : public CanMessage<0x98FD7900, 32> {
  CanSignalLe<std::uint32_t> Trip_No_Of_Active_Regens_B{96, 32, 1, 0, 0, 4294967295};  // Counts
  CanSignalLe<std::uint32_t> Trip_Act_Regen_Time_B{32, 32, 1, 0, 0, 4294967295};  // s
  CanSignalLe<std::uint32_t> Trip_Fuel_Used{0, 32, 0.5, 0, 0, 2105540607.5};  // L
};

struct E_A1SCREGT : public CanMessage<0x94FD3E00, 8> {
  CanSignalLe<std::uint16_t> Catalyst_Outlet_Gas_Temp{24, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> Catalyst_Intake_Gas_Temp{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
};

struct E_A1DEFI : public CanMessage<0x98FD9B00, 8> {
  CanSignalLe<std::uint8_t> Aft1DieselExhaustFluidTemp2_0{0, 8, 1, 0, -40, 210};  // degc
  CanSignalLe<std::uint8_t> DEF_Concentration{8, 8, 0.25, 0, 0, 62.5};  // %
};

struct E_EI2 : public CanMessage<0x98FC9600, 8> {
  CanSignalLe<std::uint16_t> engineoilfilterintakepresshigh{16, 16, 0.1, 0, 0, 6425.5};  // kPa
  CanSignalLe<std::uint16_t> Exhaust_Back_Pressure_ExtRange{0, 16, 0.125, 0, 0, 8031.875};  // kPa
};

struct E_TTI2 : public CanMessage<0x9CFEB000, 20> {
  CanSignalLe<std::uint32_t> Trip_Engine_Idle_Time{96, 32, 0.05, 0, 0, 210554060.75};  // hr
  CanSignalLe<std::uint32_t> Trip_Engine_Run_Time{64, 32, 0.05, 0, 0, 210554060.75};  // hr
};

struct E_LTFI : public CanMessage<0x9CFEB900, 22> {
  CanSignalLe<std::uint16_t> TripDriveFuelEconomy_4A_00{160, 16, 0.001953125, 0, 0, 125.498046875};  // km/L
  CanSignalLe<std::uint32_t> TripCruiseFuelUsed_4A_00{128, 32, 0.5, 0, 0, 2105540607.5};  // L
  CanSignalLe<std::uint32_t> trip_drive_fuel_used{0, 32, 0.5, 0, 0, 2105540607.5};  // L/bit
  CanSignalLe<std::uint32_t> Trip_Vehicle_Idle_Fuel_Used{96, 32, 0.5, 0, 0, 2105540607.5};  // L
  CanSignalLe<std::uint32_t> Trip_PTO_Gov_NonMove_Fuel_Used{64, 32, 0.5, 0, 0, 2105540607.5};  // L
  CanSignalLe<std::uint32_t> Trip_PTO_Gov_Move_Fuel_Used{32, 32, 0.5, 0, 0, 2105540607.5};  // L
};

struct E_AT1OG1 : public CanMessage<0x98F00F00, 8> {
  CanSignalLe<std::uint16_t> aft1_outlet_percent_oxygen1{16, 16, 0.000514, -12, -12, 21};  // ppm
  CanSignalLe<std::uint16_t> NOx_Out_Concentration{0, 16, 0.05, -200, -200, 3012.75};  // ppm
};

struct E_OI : public CanMessage<0x98FEFF00, 8> {
  enum class Water_In_Fuel : std::uint8_t {
    NO = 0,
    YES = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> OilFilterLife_00{40, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<Water_In_Fuel> Water_In_Fuel{0, 2};
};

struct E_PTO : public CanMessage<0x98FEF000, 8> {
  enum class Remote_PTO_Speed_Cntrl_Switch_A : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PTO_Gov_Pre_Spd_Cntrl_Switch_A : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PTO_Enable_Switch : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<std::uint16_t> power_take_off_set_speed{24, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> power_takeoff_oil_temperature{0, 8, 1, -40, -40, 210};
  CanSignalLe<std::uint8_t> engine_pto_gov_disable_switch{62, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> auxiliary_input_ignore_switch{60, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> remoteptogovpreprgmspeedswitch2{58, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> operatorengineptogovmemorySwitch{56, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engineptogovaccelerateswitch{54, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engineptogovernorresumeswitch{52, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engineptogovcoastdecelswitch{50, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> engine_pto_governor_set_switch{48, 2, 1, 0, 0, 3};
  CanSignalLe<Remote_PTO_Speed_Cntrl_Switch_A> Remote_PTO_Speed_Cntrl_Switch_A{44, 2};
  CanSignalLe<PTO_Gov_Pre_Spd_Cntrl_Switch_A> PTO_Gov_Pre_Spd_Cntrl_Switch_A{42, 2};
  CanSignalLe<PTO_Enable_Switch> PTO_Enable_Switch{40, 2};
  CanSignalLe<std::uint16_t> Power_Take_Off_Speed_0{8, 16, 0.125, 0, 0, 8031.875};  // rpm
};

struct E_HOURS : public CanMessage<0x98FEE500, 8> {
  CanSignalLe<std::uint32_t> Engine_Total_Revolutions{32, 32, 1000, 0, 0, 4211081215000};  // rpm
  CanSignalLe<std::uint32_t> Total_Engine_Hours_A{0, 32, 0.05, 0, 0, 210554060.75};  // hr
};

struct E_EEC7 : public CanMessage<0x98FD9400, 8> {
  CanSignalLe<std::uint16_t> CC_Oil_Separator_Speed{32, 16, 1, 0, 0, 64255};  // rpm
  CanSignalLe<std::uint16_t> EGR_Position_A{0, 16, 0.0025, 0, 0, 160.6375};  // %
};

struct E_DPFC1 : public CanMessage<0x98FD7C00, 8> {
  enum class regeninhibitduetotresher : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class filterconditionnotmetactiveregen : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class hydrocarbondoserpurgingenable : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class regenforcedstatus : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class regeninitialconfiguration : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class regeninhibitduetotemporarysystem : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class regeninhibitedduetosystemtimeout : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class regen_availability_status : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class High_Exhaust_Temp_Lamp : std::uint8_t {
    OFF = 0,
    ON_SOLID = 1,
    RESERVED_FOR_SAE_ASSIGNMENT_1 = 2,
    RESERVED_FOR_SAE_ASSIGNMENT_2 = 3,
    ON_FAST_BLINK_1HZ = 4,
    RESERVED_FOR_SAE_ASSIGNMENT_3 = 5,
    RESERVED_FOR_SAE_ASSIGNMENT_4 = 6,
    NOT_AVAILABLE = 7,
  };

  enum class Reg_Inh_Eng_Not_Warm_Up : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inh_Sys_Fault_Active : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inh_Low_Exhaust_Gas_Temp : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inh_Prk_Brk_Not_Set : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inh_Veh_Speed_Too_High : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inhibit_Out_of_Neutral : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inhibit_Accel_Off_Idle : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Regen_Inhibit_PTO_Active : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inh_Serv_Brake_Active : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inhibit_Clutch_Diseng : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Reg_Inhibit_Inhibit_Switch : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Regen_Inhibit_Status : std::uint8_t {
    NOT_INHIBITED = 0,
    INHIBITED = 1,
    RESERVED_FOR_SAE_ASSIGNMENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class DPF_Status_B : std::uint8_t {
    REGENERATION_NOT_NEEDED = 0,
    REGEN_NEEDED_LOWEST_LEVEL = 1,
    REGEN_NEEDED_MODERATE_LEVEL = 2,
    REGEN_NEEDED_HIGHEST_LEVEL = 3,
    RESERVED_FOR_SAE_ASSIGNMENT_1 = 4,
    RESERVED_FOR_SAE_ASSIGNMENT_2 = 5,
    RESERVED_FOR_SAE_ASSIGNMENT_3 = 6,
    NOT_AVAILABLE = 7,
  };

  enum class Regen_Status : std::uint8_t {
    NOT_ACTIVE = 0,
    ACTIVE = 1,
    REGENNEEDED_AUTOREGENIMMINENT = 2,
    NOT_AVAILABLE = 3,
  };

  enum class DPF_Lamp_Command : std::uint8_t {
    OFF = 0,
    ON_SOLID = 1,
    RESERVED_FOR_SAE_ASSIGNMENT_1 = 2,
    RESERVED_FOR_SAE_ASSIGNMENT_2 = 3,
    ON_FAST_BLINK_1HZ = 4,
    RESERVED_FOR_SAE_ASSIGNMENT_3 = 5,
    RESERVED_FOR_SAE_ASSIGNMENT_4 = 6,
    NOT_AVAILABLE = 7,
  };

  CanSignalLe<regeninhibitduetotresher> regeninhibitduetotresher{62, 2};
  CanSignalLe<filterconditionnotmetactiveregen> filterconditionnotmetactiveregen{60, 2};
  CanSignalLe<std::uint8_t> regennhibitduelowexhaustpressure{34, 2, 1, 0, 0, 3};
  CanSignalLe<hydrocarbondoserpurgingenable> hydrocarbondoserpurgingenable{56, 2};
  CanSignalLe<regenforcedstatus> regenforcedstatus{53, 3};
  CanSignalLe<regeninitialconfiguration> regeninitialconfiguration{48, 2};
  CanSignalLe<std::uint8_t> regeninhibitduevehiclespeedbelow{46, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> regeninhibitduetopermanentsystem{42, 2, 1, 0, 0, 3};
  CanSignalLe<regeninhibitduetotemporarysystem> regeninhibitduetotemporarysystem{40, 2};
  CanSignalLe<regeninhibitedduetosystemtimeout> regeninhibitedduetosystemtimeout{38, 2};
  CanSignalLe<std::uint8_t> passive_regeneration_status{8, 2, 1, 0, 0, 3};
  CanSignalLe<regen_availability_status> regen_availability_status{3, 2};
  CanSignalLe<High_Exhaust_Temp_Lamp> High_Exhaust_Temp_Lamp{50, 3};
  CanSignalLe<Reg_Inh_Eng_Not_Warm_Up> Reg_Inh_Eng_Not_Warm_Up{44, 2};
  CanSignalLe<Reg_Inh_Sys_Fault_Active> Reg_Inh_Sys_Fault_Active{36, 2};
  CanSignalLe<Reg_Inh_Low_Exhaust_Gas_Temp> Reg_Inh_Low_Exhaust_Gas_Temp{58, 2};
  CanSignalLe<Reg_Inh_Prk_Brk_Not_Set> Reg_Inh_Prk_Brk_Not_Set{32, 2};
  CanSignalLe<Reg_Inh_Veh_Speed_Too_High> Reg_Inh_Veh_Speed_Too_High{30, 2};
  CanSignalLe<Reg_Inhibit_Out_of_Neutral> Reg_Inhibit_Out_of_Neutral{28, 2};
  CanSignalLe<Reg_Inhibit_Accel_Off_Idle> Reg_Inhibit_Accel_Off_Idle{26, 2};
  CanSignalLe<Regen_Inhibit_PTO_Active> Regen_Inhibit_PTO_Active{24, 2};
  CanSignalLe<Reg_Inh_Serv_Brake_Active> Reg_Inh_Serv_Brake_Active{22, 2};
  CanSignalLe<Reg_Inhibit_Clutch_Diseng> Reg_Inhibit_Clutch_Diseng{20, 2};
  CanSignalLe<Reg_Inhibit_Inhibit_Switch> Reg_Inhibit_Inhibit_Switch{18, 2};
  CanSignalLe<Regen_Inhibit_Status> Regen_Inhibit_Status{16, 2};
  CanSignalLe<DPF_Status_B> DPF_Status_B{12, 3};
  CanSignalLe<Regen_Status> Regen_Status{10, 2};
  CanSignalLe<DPF_Lamp_Command> DPF_Lamp_Command{0, 3};
};

struct E_LFC : public CanMessage<0x98FEE900, 8> {
  CanSignalLe<std::uint32_t> Total_Engine_Fuel_Used{32, 32, 0.5, 0, 0, 2105540607.5};  // L
  CanSignalLe<std::uint32_t> Engine_Trip_Fuel{0, 32, 0.5, 0, 0, 2105540607.5};  // L
};

struct E_VEP1 : public CanMessage<0x98FEF700, 8> {
  CanSignalLe<std::uint16_t> charging_system_potential_0{16, 16, 1, 0, 0, 3212.75};
  CanSignalLe<std::uint8_t> sli_battery1_net_current_0{0, 8, 1, -125, -125, 125};  // A
  CanSignalLe<std::uint8_t> alternator_current_0{8, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint16_t> Keyswitch_Battery_Potential{48, 16, 0.05, 0, 0, 3212.75};  // V
  CanSignalLe<std::uint16_t> Battery_Potential{32, 16, 0.05, 0, 0, 3212.75};  // V
};

struct E_LFE1 : public CanMessage<0x98FEF200, 8> {
  CanSignalLe<std::uint8_t> engine_throttle_valve2_position{56, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> engine_throttle_valve1_position1{48, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint16_t> Average_Fuel_Economy{32, 16, 0.001953125, 0, 0, 125.498};  // km/L
  CanSignalLe<std::uint16_t> Instantaneous_Fuel_Economy{16, 16, 0.001953125, 0, 0, 125.498};  // km/L
  CanSignalLe<std::uint16_t> Engine_Fuel_Rate_B{0, 16, 0.05, 0, 0, 3212.75};  // L/h
};

struct DM1 : public CanMessage<0x98FECAFE, 160> {
  CanSignalLe<std::uint8_t> SPN2High_DM1{69, 3, 1, 0, 0, 7};
  CanSignalLe<std::uint8_t> SPN1High_DM1{37, 3, 1, 0, 0, 7};
};

struct E_IO : public CanMessage<0x98FEDC00, 8> {
  CanSignalLe<std::uint32_t> Total_Idle_Hours{32, 32, 0.05, 0, 0, 210554060.75};  // hr
  CanSignalLe<std::uint32_t> Total_Idle_Fuel_Used{0, 32, 0.5, 0, 0, 2105540607.5};  // L
};

struct E_IC1 : public CanMessage<0x98FEF600, 8> {
  CanSignalLe<std::uint8_t> engine_airfilter_1_diff_pressure{32, 8, 0.05, 0, 0, 12.5};  // kPa
  CanSignalLe<std::uint8_t> enginecoolantfilterdiffpress{56, 8, 0.5, 0, 0, 125};  // KPa
  CanSignalLe<std::uint16_t> Exhaust_Gas_Temperature{40, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint8_t> engine_intake_air_pressure{24, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint8_t> Intake_Manifold_Temperature{16, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> Intake_Manifold_Pressure{8, 8, 2, 0, 0, 500};  // kPa
  CanSignalLe<std::uint8_t> DPF_Inlet_Pressure{0, 8, 0.5, 0, 0, 125};  // kPa
};

struct E_AT1HI : public CanMessage<0x98FD9800, 40> {
  CanSignalLe<std::uint32_t> Avg_Time_Bet_Act_DPF_Reg{256, 32, 1, 0, 0, 4211081215};  // s
  CanSignalLe<std::uint32_t> Tot_No_Of_Act_Reg_Man_Reqs{224, 32, 1, 0, 0, 4211081215};
  CanSignalLe<std::uint32_t> Tot_No_Of_Act_Reg_Inh_Reqs{192, 32, 1, 0, 0, 4211081215};
  CanSignalLe<std::uint32_t> Total_No_Of_Active_Regens{96, 32, 1, 0, 0, 4294967295};  // counts
  CanSignalLe<std::uint32_t> Total_Disabled_Time{64, 32, 1, 0, 0, 4211081215};  // s
  CanSignalLe<std::uint32_t> Total_Regen_Time{32, 32, 1, 0, 0, 4211081215};  // s
  CanSignalLe<std::uint32_t> Total_Fuel_Used{0, 32, 0.5, 0, 0, 2105540607.5};  // L
};

struct E_DM26 : public CanMessage<0x98FDB800, 8> {
  enum class NHMCCnvrtctlystMonComplete_DM26 : std::uint8_t {
    COMPLETETHISCYCLE = 0,
    NOTCOMPLETETHISCYCLE = 1,
  };

  enum class NOxCnvrtctlystMonComplete_DM26 : std::uint8_t {
    COMPLETETHISCYCLE = 0,
    NOTCOMPLETETHISCYCLE = 1,
  };

  enum class PMFilterMonComplete_DM26 : std::uint8_t {
    COMPLETERFORCYCLE = 0,
    NOTCOMPLETEFORCYCLE = 1,
  };

  enum class BstPressCtrlSystemMonComp_DM26 : std::uint8_t {
    COMPLETEFORTHISMONITORCYCLE = 0,
    NOTCOMPLETETHISMONITORCYCLE = 1,
  };

  enum class ColdStartAidSystemMonComp_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class EGRSystemMonitoringComplete_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class OxygenSensorHeaterMonComp_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class OxygenSensorMonComplete_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class ACSystemRefrigerantMonComp_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class SecondAirSystemMonComplete_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class EvaporativeSystemMonComp_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class HeatedCatalystMonComplete_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class CatalystMonComplete_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class NHMCCnvrtctlystMonEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class NOxCnvrtctlystMonEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class PMFilterMonEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class BstPressCtrlSysMonEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class ColdStartAidSysMonEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class EGRSystemMonitoringEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class OxygenSensorHtrMonEnabled_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class OxygenSensorMonEnabled_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class ACSystemRefrigMonEnabled_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class SecondAirSystemMonEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class EvaporativeSystemMonEnabled_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class HeatedCatalystMonEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class CatalystMonEnabled_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class ComprehensiveComptMonComp_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class FuelSystemMonitoringComp_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class MisfireMonitoringComplete_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  enum class ComprehensiveCompMonEnabled_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class FuelSystemMonitoringEnabled_DM26 : std::uint8_t {
    COMPLETECYCLEORNOTSUPPORTED = 0,
    NOTCOMPLETECYCLE = 1,
  };

  enum class MisfireMonitoringEnabled_DM26 : std::uint8_t {
    DISABLEDFORRESTOFCYCLE = 0,
    ENABLEDFORCYCLE = 1,
  };

  CanSignalLe<NHMCCnvrtctlystMonComplete_DM26> NHMCCnvrtctlystMonComplete_DM26{60, 1};
  CanSignalLe<NOxCnvrtctlystMonComplete_DM26> NOxCnvrtctlystMonComplete_DM26{59, 1};
  CanSignalLe<PMFilterMonComplete_DM26> PMFilterMonComplete_DM26{58, 1};
  CanSignalLe<BstPressCtrlSystemMonComp_DM26> BstPressCtrlSystemMonComp_DM26{57, 1};
  CanSignalLe<ColdStartAidSystemMonComp_DM26> ColdStartAidSystemMonComp_DM26{56, 1};
  CanSignalLe<EGRSystemMonitoringComplete_DM26> EGRSystemMonitoringComplete_DM26{55, 1};
  CanSignalLe<OxygenSensorHeaterMonComp_DM26> OxygenSensorHeaterMonComp_DM26{54, 1};
  CanSignalLe<OxygenSensorMonComplete_DM26> OxygenSensorMonComplete_DM26{53, 1};
  CanSignalLe<ACSystemRefrigerantMonComp_DM26> ACSystemRefrigerantMonComp_DM26{52, 1};
  CanSignalLe<SecondAirSystemMonComplete_DM26> SecondAirSystemMonComplete_DM26{51, 1};
  CanSignalLe<EvaporativeSystemMonComp_DM26> EvaporativeSystemMonComp_DM26{50, 1};
  CanSignalLe<HeatedCatalystMonComplete_DM26> HeatedCatalystMonComplete_DM26{49, 1};
  CanSignalLe<CatalystMonComplete_DM26> CatalystMonComplete_DM26{48, 1};
  CanSignalLe<NHMCCnvrtctlystMonEnabled_DM26> NHMCCnvrtctlystMonEnabled_DM26{44, 1};
  CanSignalLe<NOxCnvrtctlystMonEnabled_DM26> NOxCnvrtctlystMonEnabled_DM26{43, 1};
  CanSignalLe<PMFilterMonEnabled_DM26> PMFilterMonEnabled_DM26{42, 1};
  CanSignalLe<BstPressCtrlSysMonEnabled_DM26> BstPressCtrlSysMonEnabled_DM26{41, 1};
  CanSignalLe<ColdStartAidSysMonEnabled_DM26> ColdStartAidSysMonEnabled_DM26{40, 1};
  CanSignalLe<EGRSystemMonitoringEnabled_DM26> EGRSystemMonitoringEnabled_DM26{39, 1};
  CanSignalLe<OxygenSensorHtrMonEnabled_DM26> OxygenSensorHtrMonEnabled_DM26{38, 1};
  CanSignalLe<OxygenSensorMonEnabled_DM26> OxygenSensorMonEnabled_DM26{37, 1};
  CanSignalLe<ACSystemRefrigMonEnabled_DM26> ACSystemRefrigMonEnabled_DM26{36, 1};
  CanSignalLe<SecondAirSystemMonEnabled_DM26> SecondAirSystemMonEnabled_DM26{35, 1};
  CanSignalLe<EvaporativeSystemMonEnabled_DM26> EvaporativeSystemMonEnabled_DM26{34, 1};
  CanSignalLe<HeatedCatalystMonEnabled_DM26> HeatedCatalystMonEnabled_DM26{33, 1};
  CanSignalLe<CatalystMonEnabled_DM26> CatalystMonEnabled_DM26{32, 1};
  CanSignalLe<ComprehensiveComptMonComp_DM26> ComprehensiveComptMonComp_DM26{30, 1};
  CanSignalLe<FuelSystemMonitoringComp_DM26> FuelSystemMonitoringComp_DM26{29, 1};
  CanSignalLe<MisfireMonitoringComplete_DM26> MisfireMonitoringComplete_DM26{28, 1};
  CanSignalLe<ComprehensiveCompMonEnabled_DM26> ComprehensiveCompMonEnabled_DM26{26, 1};
  CanSignalLe<FuelSystemMonitoringEnabled_DM26> FuelSystemMonitoringEnabled_DM26{25, 1};
  CanSignalLe<MisfireMonitoringEnabled_DM26> MisfireMonitoringEnabled_DM26{24, 1};
  CanSignalLe<std::uint8_t> NumOfWarmUpsSinceDTCsClear_DM26{16, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint16_t> TimeSinceEngineStart_DM26{0, 16, 1, 0, 0, 64255};  // s
};

struct E_VH : public CanMessage<0x98FEE700, 8> {
  CanSignalLe<std::uint32_t> total_vehicle_hours_0{0, 32, 0.05, 0, 0, 210554060.75};  // hr
  CanSignalLe<std::uint32_t> Total_Power_Take_Off_Hours{32, 32, 0.05, 0, 0, 210554060.75};  // hr
};

struct VDHR : public CanMessage<0x98FEC117, 8> {
  CanSignalLe<std::uint32_t> Trip_Distance_HR{32, 32, 5, 0, 0, 21055406075};  // meters
  CanSignalLe<std::uint32_t> Total_Vehicle_Distance_HR{0, 32, 5, 0, 0, 21055406075};  // meters
};

struct E_SHUTDN : public CanMessage<0x98FEE400, 8> {
  enum class EWPS_Configuration : std::uint8_t {
    DISABLEDINCALIB = 0,
    ENABLEDINCALIB = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EWPS_State : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EWPS_Timer_Override : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EWPS_Approaching_Shutdown : std::uint8_t {
    NOTAPPROACHING = 0,
    APPROACHING = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EWPS_Has_Shutdown_Engine : std::uint8_t {
    NO = 0,
    YES = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Wait_To_Start_Lamp_A : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class IST_Timer_Function : std::uint8_t {
    DISABLEDINCALIB = 0,
    ENABLEDINCALIB = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class IST_Timer_State : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class IST_Timer_Override : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class IST_Drive_Alert_Mode : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class IST_Has_Shutdown_Engine : std::uint8_t {
    NO = 0,
    YES = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> ac_highpressure_fan_switch{16, 2, 1, 0, 0, 3};
  CanSignalLe<EWPS_Configuration> EWPS_Configuration{46, 2};
  CanSignalLe<EWPS_State> EWPS_State{38, 2};
  CanSignalLe<EWPS_Timer_Override> EWPS_Timer_Override{36, 2};
  CanSignalLe<EWPS_Approaching_Shutdown> EWPS_Approaching_Shutdown{34, 2};
  CanSignalLe<EWPS_Has_Shutdown_Engine> EWPS_Has_Shutdown_Engine{32, 2};
  CanSignalLe<Wait_To_Start_Lamp_A> Wait_To_Start_Lamp_A{24, 2};
  CanSignalLe<IST_Timer_Function> IST_Timer_Function{14, 2};
  CanSignalLe<IST_Timer_State> IST_Timer_State{6, 2};
  CanSignalLe<IST_Timer_Override> IST_Timer_Override{4, 2};
  CanSignalLe<IST_Drive_Alert_Mode> IST_Drive_Alert_Mode{2, 2};
  CanSignalLe<IST_Has_Shutdown_Engine> IST_Has_Shutdown_Engine{0, 2};
};

struct E_LFI : public CanMessage<0x9CFEB300, 8> {
  CanSignalLe<std::uint32_t> Total_PTO_Fuel_Used{0, 32, 0.5, 0, 0, 2105540607.5};  // L
};

struct E_FD1 : public CanMessage<0x98FEBD00, 8> {
  enum class Engine_Fan_State : std::uint8_t {
    FANOFF = 0,
    ENGSYSTEMGENERAL = 1,
    EXCESSIVEENGAIRTEMP = 2,
    EXCESSIVEENGOILTEMP = 3,
    EXCESSIVEENGCOOLANTTEMP = 4,
    EXCESSIVETRANSOILTEMP = 5,
    EXCESSIVEHYDOILTEMP = 6,
    DEFAULTOPERATION = 7,
    AFTERTREATMENTREGENACTIVE = 8,
    MANUALCTRL = 9,
    TRANSRETARDER = 10,
    A_CSYSTEM = 11,
    TIMER = 12,
    ENGBRAKE = 13,
    OTHER = 14,
    NOTAVAILABLE = 15,
  };

  CanSignalLe<std::uint16_t> Engine_Fan_Speed{16, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<Engine_Fan_State> Engine_Fan_State{8, 4};
  CanSignalLe<std::uint8_t> Estimated_Percent_Fan_Speed{0, 8, 0.4, 0, 0, 100};  // %
};

struct E_ET2 : public CanMessage<0x98FEA400, 8> {
  CanSignalLe<std::uint16_t> engine_exhaustgas_diffpress{32, 16, 0.0078125, -250, -250, 251.99};  // kPA
  CanSignalLe<std::uint16_t> EGR_Temperature{48, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> Engine_Oil_Temperature_2{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
};

struct E_ET1 : public CanMessage<0x98FEEE00, 8> {
  CanSignalLe<std::uint8_t> engchargeaircoolerthermostat_0{56, 8, 0.4, 0, 0, 100};
  CanSignalLe<std::uint8_t> engineintercoolertempe_0{48, 8, 1, -40, -40, 210};
  CanSignalLe<std::uint16_t> engineturbocharger1oiltemp_0{32, 16, 0.03125, -273, -273, 1734.96875};
  CanSignalLe<std::uint16_t> Engine_Oil_Temperature{16, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint8_t> Engine_Fuel_Temperature{8, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint8_t> Engine_Coolant_Temperature{0, 8, 1, -40, -40, 210};  // �C
};

struct E_EH : public CanMessage<0x9CFEB100, 8> {
  CanSignalLe<std::uint32_t> Total_ECM_Run_Time{32, 32, 0.05, 0, 0, 210554060.75};  // hr
  CanSignalLe<std::uint32_t> Total_Vehicle_Distance_ECM_A{0, 32, 0.125, 0, 0, 526385151.9};  // km
};

struct E_EGF1 : public CanMessage<0x8CF00A00, 8> {
  CanSignalLe<std::uint16_t> Eng_exhaust_gas_massflow_rate{0, 16, 0.05, 0, 0, 3212.75};  // kg/h
  CanSignalLe<std::uint16_t> Mass_Air_Flow_A{16, 16, 0.05, 0, 0, 3212.75};  // kg/h
};

struct BC_DD : public CanMessage<0x98FEFC21, 8> {
  CanSignalLe<std::uint16_t> cargo_ambient_temperature_21{32, 16, 1, -273, -273, 1734.96875};  // C/bit
  CanSignalLe<std::uint8_t> fuellevel2_21{48, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> engine_fuel_filter_diff_press_21{16, 8, 2, 0, 0, 500};  // KPa
  CanSignalLe<std::uint8_t> engine_oil_filter_diff_press1_21{24, 8, 0.5, 0, 0, 125};  // KPa
  CanSignalLe<std::uint8_t> washer_fluid_level_21{0, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> engine_oil_filter_diff_press{56, 8, 5, 0, 0, 1250};  // KPa
  CanSignalLe<std::uint8_t> Fuel_Level{8, 8, 0.4, 0, 0, 100};  // %
};

struct E_A1DOC : public CanMessage<0x98FD2000, 8> {
  CanSignalLe<std::uint16_t> DOC_Outlet_Temperature_A{16, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> DOC_Inlet_Temperature_C{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
};

struct E_AT1S : public CanMessage<0x98FD7B00, 8> {
  CanSignalLe<std::uint16_t> DPF_Soot_Load_Regen_Thresh_A{48, 16, 0.0025, 0, 0, 160.6375};  // %
  CanSignalLe<std::uint32_t> DPF_Time_Since_Last_Act_Regen_A{16, 32, 1, 0, 0, 4211081215};  // s
  CanSignalLe<std::uint8_t> DPF_Ash_Load{8, 8, 1, 0, 0, 250};  // %
  CanSignalLe<std::uint8_t> DPF_Soot_Load{0, 8, 1, 0, 0, 250};  // %
};

struct E_AT1GP : public CanMessage<0x98FD8C00, 8> {
  CanSignalLe<std::uint16_t> DPF_Outlet_Pressure_2{16, 16, 0.1, 0, 0, 6425.5};
  CanSignalLe<std::uint16_t> DPF_Inlet_Pressure_1{0, 16, 0.1, 0, 0, 6425.5};  // kPa
};

struct E_EOI : public CanMessage<0x8CFD9200, 8> {
  enum class engine_controlld_shutdwn_request : std::uint8_t {
    ENGINESTOPPED = 0,
    PRESTART = 1,
    STARTING = 2,
    WARMUP = 3,
    RUNNING = 4,
    COOLDOWN = 5,
    ENGINESTOPPING = 6,
    POSTRUN = 7,
    SAE_ASSIGNMENT_1 = 8,
    SAE_ASSIGNMENT_2 = 9,
    SAE_ASSIGNMENT_3 = 10,
    SAE_ASSIGNMENT_4 = 11,
    SAE_ASSIGNMENT_5 = 12,
    SAE_ASSIGNMENT_6 = 13,
    RESERVED = 14,
    NOTAVAILABLE = 15,
  };

  enum class engine_derate_request : std::uint8_t {
    ENGINESTOPPED = 0,
    PRESTART = 1,
    STARTING = 2,
    WARMUP = 3,
    RUNNING = 4,
    COOLDOWN = 5,
    ENGINESTOPPING = 6,
    POSTRUN = 7,
    SAE_ASSIGNMENT_1 = 8,
    SAE_ASSIGNMENT_2 = 9,
    SAE_ASSIGNMENT_3 = 10,
    SAE_ASSIGNMENT_4 = 11,
    SAE_ASSIGNMENT_5 = 12,
    SAE_ASSIGNMENT_6 = 13,
    RESERVED = 14,
    NOTAVAILABLE = 15,
  };

  enum class Engine_Operating_State : std::uint8_t {
    ENGINESTOPPED = 0,
    PRESTART = 1,
    STARTING = 2,
    WARMUP = 3,
    RUNNING = 4,
    COOLDOWN = 5,
    ENGINESTOPPING = 6,
    POSTRUN = 7,
    SAE_ASSIGNMENT_1 = 8,
    SAE_ASSIGNMENT_2 = 9,
    SAE_ASSIGNMENT_3 = 10,
    SAE_ASSIGNMENT_4 = 11,
    SAE_ASSIGNMENT_5 = 12,
    SAE_ASSIGNMENT_6 = 13,
    RESERVED = 14,
    NOTAVAILABLE = 15,
  };

  CanSignalLe<engine_controlld_shutdwn_request> engine_controlld_shutdwn_request{42, 2};
  CanSignalLe<engine_derate_request> engine_derate_request{56, 8};
  CanSignalLe<Engine_Operating_State> Engine_Operating_State{0, 4};
};

struct E_AT1IG1 : public CanMessage<0x98F00E00, 8> {
  enum class O2_Heater_A : std::uint8_t {
    AUTOMATIC = 0,
    PREHEAT_2 = 1,
    PREHEAT_1 = 2,
    HEATER_OFF = 3,
  };

  CanSignalLe<O2_Heater_A> O2_Heater_A{45, 2};
  CanSignalLe<std::uint16_t> AFT_O2_Inlet_Percent{16, 16, 0.000514, -12, -12, 21};  // %
  CanSignalLe<std::uint16_t> NOx_Inlet_Concentration_D{0, 16, 0.05, -200, -200, 3012.75};  // ppm
};

struct E_EEC9 : public CanMessage<0x90FCFD00, 8> {
  CanSignalLe<std::uint16_t> Fuel_Rail_Pressure_Desired{32, 16, 0.00390625, 0, 0, 250.996};  // MPa
};

struct E_EFLP1 : public CanMessage<0x98FEEF00, 8> {
  CanSignalLe<std::uint8_t> eng_extended_crankcase_pressure{8, 8, 0.05, 0, 0, 12.5};  // kPa
  CanSignalLe<std::uint8_t> Engine_Coolant_Level{56, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Engine_Coolant_Pressure{48, 8, 2, 0, 0, 510};  // kPa
  CanSignalLe<std::uint16_t> Engine_Crankcase_Pressure{32, 16, 0.0078125, -250, -250, 251.99};  // kPa
  CanSignalLe<std::uint8_t> Engine_Oil_Pressure{24, 8, 4, 0, 0, 1000};  // kPa
  CanSignalLe<std::uint8_t> Engine_Oil_Level{16, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Fuel_Delivery_Pressure{0, 8, 4, 0, 0, 1000};  // kPa
};

struct E_EEC5 : public CanMessage<0x98FDD500, 8> {
  enum class VGT_Ctrl_Mode : std::uint8_t {
    OPEN_LOOP = 0,
    CLOSED_LOOP = 1,
    RESERVED = 2,
    NA = 3,
  };

  enum class Fuel_Control_System_Status : std::uint8_t {
    OPEN_LOOP = 0,
    CLOSED_LOOP = 1,
    RESERVED = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> VGT_Actuator_Pos{56, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<VGT_Ctrl_Mode> VGT_Ctrl_Mode{52, 2};
  CanSignalLe<Fuel_Control_System_Status> Fuel_Control_System_Status{50, 2};
  CanSignalLe<std::uint16_t> EGR_Control{32, 16, 0.0025, 0, 0, 160.6375};  // %
};

struct E_EEC2 : public CanMessage<0x8CF00300, 8> {
  enum class Road_Speed_Limit_Status : std::uint8_t {
    ACTIVE = 0,
    NOTACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Accel_Ped_Kickdown_Switch : std::uint8_t {
    KICKDOWNPASSIVE = 0,
    KICKDOWNACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Accel_Pedal_Low_Idle_Switch : std::uint8_t {
    ACCLPEDAL1NOTINLOWIDLECONDITION = 0,
    ACCELPEDAL1INLOWIDLECONDITION = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<std::uint8_t> estimatedpumpingpercentTorque{56, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> scr_thermal_management_active{46, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> dpf_thermal_management_active{44, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> momentaryengmaxpowerenable{42, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> acceleratorpedal2lowIdleswitch{6, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> accelerator_pedal_position_2{32, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> veh_accele_rate_limit_status{40, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> Actual_Max_Avg_Eng_Perc_Torque{48, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Remote_Accel_Pedal_Position{24, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<std::uint8_t> Engine_Load{16, 8, 1, 0, 0, 250};  // %
  CanSignalLe<std::uint8_t> Accelerator_Pedal_Position_1{8, 8, 0.4, 0, 0, 100};  // %
  CanSignalLe<Road_Speed_Limit_Status> Road_Speed_Limit_Status{4, 2};
  CanSignalLe<Accel_Ped_Kickdown_Switch> Accel_Ped_Kickdown_Switch{2, 2};
  CanSignalLe<Accel_Pedal_Low_Idle_Switch> Accel_Pedal_Low_Idle_Switch{0, 2};
};

struct E_CCSS : public CanMessage<0x98FEED00, 8> {
  CanSignalLe<std::uint8_t> Cruise_Cntrl_Low_Set_Lim_Speed{16, 8, 1, 0, 0, 250};  // km/h
  CanSignalLe<std::uint8_t> Cruise_Cntrl_High_Set_Lim_Speed{8, 8, 1, 0, 0, 250};  // km/h
  CanSignalLe<std::uint8_t> Maximum_Vehicle_Speed_Limit{0, 8, 1, 0, 0, 250};  // km/h
};

struct E_AMB : public CanMessage<0x98FEF500, 8> {
  CanSignalLe<std::uint16_t> road_surface_temperature{48, 16, 0.03125, -273, -273, 1734.96875};  //  �C
  CanSignalLe<std::uint16_t> cab_interior_temperature{8, 16, 0.03125, -273, -273, 1734.96875};  //  �C
  CanSignalLe<std::uint8_t> Air_Intake_Temperature{40, 8, 1, -40, -40, 210};  // �C
  CanSignalLe<std::uint16_t> Ambient_Air_Temperature{24, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint8_t> Barometric_Pressure{0, 8, 0.5, 0, 0, 125};  // kPa
};

struct E_AT1IG2 : public CanMessage<0x98FDB400, 8> {
  CanSignalLe<std::uint16_t> aft1_exhaust_temperature1{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> DPF_Inlet_Temperature{16, 16, 0.03125, -273, -273, 1734.96875};  // �C
};

struct E_AT1IMG : public CanMessage<0x98FDB200, 8> {
  CanSignalLe<std::uint16_t> AFT_DPF_Diff_Pressure{32, 16, 0.1, 0, 0, 6425.5};  // kPa
  CanSignalLe<std::uint16_t> aft1dpfintermediatetemperature_0{16, 16, 0.03125, -273, 0, 1734.96875};  // �C/bit
  CanSignalLe<std::uint16_t> Exhaust_Gas_Temp_2{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
};

struct E_AT1OG2 : public CanMessage<0x98FDB300, 8> {
  CanSignalLe<std::uint16_t> DPF_Outlet_Temperature{16, 16, 0.03125, -273, -273, 1734.96875};  // �C
  CanSignalLe<std::uint16_t> Exhaust_Gas_Temp_3{0, 16, 0.03125, -273, -273, 1734.96875};  // �C
};

struct E_VI : public CanMessage<0x98FEEC00, 256> {
  /* vehicleid */
};

struct ADASIS_V2 : public CanMessage<0x98F0F611, 8> {
  CanSignalLe<std::uint32_t> eHorizonData_H{32, 32, 1, 0, 0, 4294967295};
  CanSignalLe<std::uint32_t> eHorizonData_L{0, 32, 1, 0, 0, 4294967295};
};

struct E_NGCCC3 : public CanMessage<0x98FFC200, 32> {
  CanSignalLe<std::uint8_t> NGCCGenericSPN{254, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> TopGearNumber{248, 6, 1, 0, 0, 31};  // bit
  CanSignalLe<std::uint16_t> GearRatio_18{236, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_17{224, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_16{212, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_15{200, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_14{188, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_13{176, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_12{164, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_11{152, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_10{140, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_09{128, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_08{116, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_07{104, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_06{92, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_05{80, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_04{68, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_03{56, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_02{44, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> GearRatio_01{32, 12, 0.01, 0, 0, 40.95};  // ratio
  CanSignalLe<std::uint16_t> TireRadius{16, 16, 0.01, 0, 0, 655};  // rev/mile
  CanSignalLe<std::uint16_t> RearAxleRatioHigh{0, 16, 0.001, 0, 0, 65};  // ratio
};

struct BC_VW : public CanMessage<0x98FEEA21, 8> {
  CanSignalLe<std::uint16_t> TrailerWeight_21{24, 16, 2, 0, 0, 128510};  // kg
  CanSignalLe<std::uint16_t> AxleWeight{8, 16, 0.5, 0, 0, 32127.5};  // kg
};

struct ETC7 : public CanMessage<0x98FE4A03, 8> {
  CanSignalLe<std::uint8_t> activeshiftconsoleindicator{10, 2, 1, 0, 0, 3};  // 8 states/3 bit
  CanSignalLe<std::uint8_t> trans_air_supply_press_indicator{48, 3, 1, 0, 0, 7};  // bit
  CanSignalLe<std::uint8_t> TransmissionMode3Indicator{18, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> transmissionmode8indicator{38, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> transmissionmode7indicator{36, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> transmissionmode5indicator{32, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> transmissionmode6indicator{34, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> Transmission_Warning_Indicator{42, 2, 1, 0, 0, 3};
  CanSignalLe<std::uint8_t> TransmissionMode1Indicator{22, 2, 1, 0, 0, 3};  // bit
};

struct PCC_RQST_RX : public CanMessage<0x98EA11FE, 3> {
  CanSignalLe<std::uint32_t> ParameterGroupNumber_RQST_RX{0, 24, 1, 0, 0, 16777215};
};

struct PCC_NGCC2 : public CanMessage<0x8CFFC411, 8> {
  CanSignalLe<std::uint32_t> NGCCFaultLatitude{40, 24, 0.000025, -200, -200, 219};  // deg
  CanSignalLe<std::uint32_t> NGCCFaultLongitude{16, 24, 0.000025, -200, -200, 219};  // deg
  CanSignalLe<std::uint16_t> NGCCFaultNumber{0, 16, 1, 0, 0, 65530};
};

struct E_EC3 : public CanMessage<0x98FCE700, 8> {
  CanSignalLe<std::uint8_t> EngineFrictionTorquePoint7{48, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> EngineFrictionTorquePoint6{40, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> EngineFrictionTorquePoint5{32, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> EngineFrictionTorquePoint4{24, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> EngineFrictionTorquePoint3{16, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> EngineFrictionTorquePoint2{8, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> EngineFrictionTorqueAtIdlePoint1{0, 8, 1, -125, -125, 125};  // %
};

struct PCC_NGCC1 : public CanMessage<0x8CFFC111, 8> {
  CanSignalLe<std::uint8_t> NGCCOptimizationGearNumber{50, 6, 1, 0, 0, 31};  // bit
  CanSignalLe<std::uint8_t> NeutralCoastStatus{48, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint16_t> NGCCPresentSetSpeedOffset{32, 16, 0.00390625, -125, -125, 125.996};  // km/h
  CanSignalLe<std::uint16_t> NGCCAveragePercentGrade{16, 16, 0.0005, -16, -16, 16.5};  // %
  CanSignalLe<std::uint16_t> NGCCFutureSetSpeedOffset{0, 16, 0.00390625, -125, -125, 125.996};  // km/h
};

struct E_NGCCC : public CanMessage<0x98FFC000, 8> {
  CanSignalLe<std::uint8_t> NeutralCoastPP{26, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> TransmissionType{24, 2, 1, 0, 0, 3};  // bit
  CanSignalLe<std::uint8_t> AccelDecelTimeOffset{16, 8, 0.1, 0, 0, 25};  // seconds
  CanSignalLe<std::uint8_t> FutureDistance{8, 8, 5, 0, 0, 1250};  // meters
  CanSignalLe<std::uint8_t> SpeedTradeoffFactor{0, 8, 0.005, 0, 0, 1.25};
};

struct PCC_CVW : public CanMessage<0x98FE7011, 8> {
  CanSignalLe<std::uint16_t> Comb_Veh_Weight{16, 16, 10, 0, 0, 642550};  // kg
};

struct PCC_PCC1 : public CanMessage<0x8CF0D311, 8> {
  CanSignalLe<std::uint8_t> RoadwayNextGradeDistance{48, 8, 25, 0, 0, 6250};  // meters
  CanSignalLe<std::uint8_t> PCCNextGrade{32, 8, 0.125, -16, -16, 15.25};  // %
  CanSignalLe<std::uint8_t> PCCPresentGrade{24, 8, 0.125, -16, -16, 15.25};  // %
  CanSignalLe<std::uint8_t> PCCState{16, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint16_t> PCCSetSpeedOffset{0, 16, 0.00390625, -125, -125, 125.996};  // km/h
};

struct E_CM2 : public CanMessage<0x98851100, 8> {
  CanSignalLe<std::uint8_t> PCCMaximumNegativeOffset{40, 8, 0.125, -31.25, -31.25, 0};  // km/h
  CanSignalLe<std::uint8_t> PCCMaximumPositiveOffset{32, 8, 0.125, 0, 0, 31.25};  // km/h
};

struct E_CCVS3 : public CanMessage<0x98FCDC00, 8> {
  enum class PCCSetSpeedOffsetStatus : std::uint8_t {
    CC_DOES_NOT_INCLUDE_PCC_OFFSET = 0,
    CC_INCLUDES_PCC_OFFSET = 1,
    RESERVED = 2,
    NA = 3,
  };

  enum class Adaptive_Cruise_Ctrl_Ready_Sts : std::uint8_t {
    OFF_CC_NOT_ACCEPT_COMMANDS = 0,
    ON_CC_ACCEPTS_COMMANDS = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  CanSignalLe<std::uint16_t> Cruise_Ctrl_Set_Speed_HR{32, 16, 0.00390625, 0, 0, 250.996};  // km/h
  CanSignalLe<PCCSetSpeedOffsetStatus> PCCSetSpeedOffsetStatus{5, 2};
  CanSignalLe<Adaptive_Cruise_Ctrl_Ready_Sts> Adaptive_Cruise_Ctrl_Ready_Sts{0, 2};
};

struct TCU_ETC2 : public CanMessage<0x98F00503, 8> {
  CanSignalLe<std::uint16_t> Transmission_Requested_Range{32, 16, 1, 0, 0, 255};  // gear value
  CanSignalLe<std::uint16_t> Transmission_Current_Range{48, 16, 1, 0, 0, 255};  // gear value
  CanSignalLe<std::uint8_t> TransmissionCurrentGear{24, 8, 1, -125, -125, 125};  // gear value
  CanSignalLe<std::uint16_t> TransmissionActualGearRatio{8, 16, 0.001, 0, 0, 64.255};
  CanSignalLe<std::uint8_t> Transmission_Selected_Gear_A{0, 8, 1, -125, -125, 125};  // gear value
};

struct E_ERC1 : public CanMessage<0x98F0000F, 8> {
  enum class Ret_Enbl_Shift_Assist_Switch : std::uint8_t {
    RETARDER_SHIFT_ASSIST_DISABLED = 0,
    RETARDER_SHIFT_ASSIST_ENABLED = 1,
    ERROR = 2,
    NOT_AVAILABLE = 3,
  };

  enum class Ret_Enbl_Brake_Assist_Switch : std::uint8_t {
    RETARDER_BRAKEASSISTDISABLED = 0,
    RETARDER_BRAKEASSISTENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Retarder_Torque_Mode : std::uint8_t {
    NOREQUEST = 0,
    TORQUEREQUESTORRETDCONRTOLLING_1 = 1,
    TORQUEREQUESTORRETDCONRTOLLING_2 = 2,
    TORQUEREQUESTORRETDCONRTOLLING_3 = 3,
    TORQUEREQUESTORRETDCONRTOLLING_4 = 4,
    TORQUEREQUESTORRETDCONRTOLLING_5 = 5,
    TORQUEREQUESTORRETDCONRTOLLING_6 = 6,
    TORQUEREQUESTORRETDCONRTOLLING_7 = 7,
    TORQUEREQUESTORRETDCONRTOLLING_8 = 8,
    TORQUEREQUESTORRETDCONRTOLLING_9 = 9,
    TORQUEREQUESTORRETDCONRTOLLING_10 = 10,
    TORQUEREQUESTORRETDCONRTOLLING_11 = 11,
    TORQUEREQUESTORRETDCONRTOLLING_12 = 12,
    TORQUEREQUESTORRETDCONRTOLLING_13 = 13,
    TORQUEREQUESTORRETDCONRTOLLING_14 = 14,
    TORQUEREQUESTORRETDCONRTOLLING_15 = 15,
  };

  CanSignalLe<std::uint8_t> Act_Max_Ava_Retarder_Perc_Torque{56, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> Drivers_Dmnd_Retd_Percent_Torque{40, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> Src_Add_Cont_Dev_Retd_Control{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Intended_Retarder_Percent_Torque{16, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> RetarderPercentTorque{8, 8, 1, -125, -125, 125};  // %
  CanSignalLe<Ret_Enbl_Shift_Assist_Switch> Ret_Enbl_Shift_Assist_Switch{6, 2};
  CanSignalLe<Ret_Enbl_Brake_Assist_Switch> Ret_Enbl_Brake_Assist_Switch{4, 2};
  CanSignalLe<Retarder_Torque_Mode> Retarder_Torque_Mode{0, 4};
};

struct E_EEC3 : public CanMessage<0x98FEDF00, 8> {
  CanSignalLe<std::uint16_t> Exhaust_Gas_Mass_Flow{40, 16, 0.2, 0, 0, 12851};  // kg/h
  CanSignalLe<std::uint8_t> EstEngParLossPercentTorque{32, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> Eng_Dsired_Oper_Speed_Assm_Adj{24, 8, 1, 0, 0, 250};
  CanSignalLe<std::uint16_t> Engine_Desired_Operating_Spd{8, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> TorqueNorminalFrictionPercent{0, 8, 1, -125, -125, 125};  // %
};

struct E_EEC1 : public CanMessage<0x8CF00400, 8> {
  enum class Engine_Starter_Mode : std::uint8_t {
    STARTNOTRQED = 0,
    STARTERACTIVEGEARNOTENGAGED = 1,
    STARTERACTIVEGEARENGAGED = 2,
    STARTFINISHED = 3,
    STARTERINHIBITEDDUEENGALREADYRUN = 4,
    STARTERINHIDUEENGNOTRDYSTART = 5,
    STARTERINHIDUEDRIVELINEENGAGED = 6,
    STARTERINHIDUEACTIVEIMMOBILIZER = 7,
    STARTERINHIDUESTARTEROVER_TEMP = 8,
    STRTINHIDUEINTAKEAIRSHUTVALVEACT = 9,
    STARTERINHIDUEACTIVESCRINDUC = 10,
    RESERVED = 11,
    SARTERINHIREASONUNKNOWN = 12,
    ERROR_1 = 13,
    ERROR_2 = 14,
    NOTAVAILABLE = 15,
  };

  enum class Engine_Torque_Mode : std::uint8_t {
    LOW_IDLE_GOVERNOR_NO_REQUEST = 0,
    ACCELERATOR_PEDAL_OPERATOR = 1,
    CRUISE_CONTROL = 2,
    PTO_GOVERNOR = 3,
    ROAD_SPEED_GOVERNOR = 4,
    ASR_CONTROL = 5,
    TRANSMISSION_CONTROL = 6,
    ABS_CONTROL = 7,
    TORQUE_LIMITING = 8,
    HIGH_SPEED_GOVERNOR = 9,
    BRAKING_SYSTEM = 10,
    REMOTE_ACCELERATOR = 11,
    SERVICE_PROCEDURE = 12,
    NOT_DEFINED = 13,
    OTHER = 14,
    NOT_AVAILABLE = 15,
  };

  CanSignalLe<std::uint8_t> actual_eng_percent_torque_frac{4, 4, 0.125, 0, 0, 0.875};  // %
  CanSignalLe<std::uint8_t> Engine_Demand_Percent_Torque_A{56, 8, 1, -125, -125, 125};  // %
  CanSignalLe<Engine_Starter_Mode> Engine_Starter_Mode{48, 4};
  CanSignalLe<std::uint8_t> Src_Add_Cont_Dev_Eng_Control_A{40, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint16_t> EngineSpeed{24, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> TorqueEngineActualPercent{16, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> TorqueDriversEngineDemand{8, 8, 1, -125, -125, 125};  // %
  CanSignalLe<Engine_Torque_Mode> Engine_Torque_Mode{0, 4};
};

struct E_EC1 : public CanMessage<0x98FEE300, 40> {
  CanSignalLe<std::uint8_t> Eng_Default_Idle_Tq_Lim{312, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> Engine_Default_Torque_Limit{256, 16, 1, 0, 0, 64255};  // Nm
  CanSignalLe<std::uint16_t> EngineMomentOfInertia{240, 16, 0.004, 0, 0, 257.02};  // kg-m�
  CanSignalLe<std::uint8_t> Eng_Req_Trq_Cntrl_Rg_Up_Lim{216, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> Eng_Req_Trq_Cntrl_Rg_Low_Lim{208, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint8_t> Eng_Req_Spd_Cntrl_Rg_Up_Lim{200, 8, 10, 0, 0, 2500};  // rpm
  CanSignalLe<std::uint8_t> Eng_Req_Spd_Cntrl_Rg_Low_Lim{192, 8, 10, 0, 0, 2500};  // rpm
  CanSignalLe<std::uint8_t> Eng_Max_Mom_Over_Time_Limit{184, 8, 0.1, 0, 0, 25};  // s
  CanSignalLe<std::uint16_t> Max_Mom_Eng_Ov_Spd_At_Point_7_A{168, 16, 0.125, 0, 0, 8031.88};  // rpm
  CanSignalLe<std::uint16_t> ReferenceEngineTorque{152, 16, 1, 0, 0, 64255};  // Nm
  CanSignalLe<std::uint16_t> Eng_Gain_Of_The_Endspeed_Gov{136, 16, 0.00078125, 0, 0, 50.2};  // %/rpm
  CanSignalLe<std::uint16_t> EngineSpeedAtHighIdlePoint6{120, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> PercentTorqueAtPoint5{112, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngineSpeedAtPoint5{96, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> PercentTorqueAtPoint4{88, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngineSpeedAtPoint4{72, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> PercentTorqueAtPoint3{64, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngineSpeedAtPoint3{48, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> PercentTorqueAtPoint2{40, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngineSpeedAtPoint2{24, 16, 0.125, 0, 0, 8031.875};  // rpm
  CanSignalLe<std::uint8_t> PercentTorqueAtPoint1{16, 8, 1, -125, -125, 125};  // %
  CanSignalLe<std::uint16_t> EngineSpeedAtPoint1{0, 16, 0.125, 0, 0, 8031.875};  // rpm
};

struct E_CCVS1 : public CanMessage<0x98FEF100, 8> {
  enum class parkbrakereleaseinhibitrequest : std::uint8_t {
    CRISECTRLACTIVATORNOTINPOSCOAST = 0,
    CRUISECTRLACTIVATORINPOSCOAST = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class cruisecontrolpauseswitch : std::uint8_t {
    CRISECTRLACTIVATORNOTINPOSCOAST = 0,
    CRUISECTRLACTIVATORINPOSCOAST = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EngShutdownOverrideSwitch_A : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Engine_Test_Mode_Switch_A : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Engine_Idle_Decrement_Switch : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Engine_Idle_Increment_Switch : std::uint8_t {
    OFF = 0,
    ON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlState : std::uint8_t {
    OFF_DISABLED = 0,
    HOLD = 1,
    ACCELERATE = 2,
    DECELERATE = 3,
    RESUME = 4,
    SET = 5,
    ACCELERATOROVERRIDE = 6,
    NOTAVAILABLE = 7,
  };

  enum class PTOState : std::uint8_t {
    OFF_DISABLED = 0,
    HOLD = 1,
    REMOTEHOLD = 2,
    STANDBY = 3,
    REMOTESTANDBY = 4,
    SET = 5,
    DECELERATE_COAST = 6,
    RESUME = 7,
    ACCELERATE = 8,
    ACCELERATOROVERRIDE = 9,
    PREPROGRAMMED_SET_SPEED_1 = 10,
    PREPROGRAMMED_SET_SPEED_2 = 11,
    PREPROGRAMMED_SET_SPEED_3 = 12,
    PREPROGRAMMED_SET_SPEED_4 = 13,
    PREPROGRAMMED_SET_SPEED_5 = 14,
    PREPROGRAMMED_SET_SPEED_6 = 15,
    PREPROGRAMMED_SET_SPEED_7 = 16,
    PREPROGRAMMED_SET_SPEED_8 = 17,
    PTO_SET_SPEED_MEMORY_1 = 18,
    PTO_SET_SPEED_MEMORY_2 = 19,
    NOTDEFINED = 20,
    NOTAVAILABLE = 31,
  };

  enum class CruiseControlAccelerateSwitch : std::uint8_t {
    CRSCTRLACTVTRNOTINPOSACCELERATE = 0,
    CRSCTRLACTIVATORINPOSACCELERATE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlResumeSwitch : std::uint8_t {
    CRSECTRLACTIVATORNOTINPOSRESUME = 0,
    CRUISECTRLACTIVATORINPOSRESUME = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlCoastSwitch : std::uint8_t {
    CRISECTRLACTIVATORNOTINPOSCOAST = 0,
    CRUISECTRLACTIVATORINPOSCOAST = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlSetSwitch : std::uint8_t {
    CRUISECTRLACTIVATORNOTINPOSSET = 0,
    CRUISECTRLACTIVATORINPOSSET = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class ClutchSwitch : std::uint8_t {
    CLUTCHPEDALRELEASED = 0,
    CLUTCHPEDALDEPRESSED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class BrakeSwitch : std::uint8_t {
    BRAKEPEDALRELEASED = 0,
    BRAKEPEDALDEPRESSED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlEnableSwitch : std::uint8_t {
    CRUISECTRLDISABLED = 0,
    CRUISECTRLENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class CruiseControlActive : std::uint8_t {
    CRUISECTRLSWITCHEDOFF = 0,
    CRUISECTRLSWITCHEDON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class ParkingBrakeSwitch : std::uint8_t {
    PARKINGBRAKENOTSET = 0,
    PARKINGBRAKESET = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class TwoSpeedAxleSwitch : std::uint8_t {
    LOWSPEEDRANGE = 0,
    HIGHSPEEDRANGE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<parkbrakereleaseinhibitrequest> parkbrakereleaseinhibitrequest{6, 2};
  CanSignalLe<cruisecontrolpauseswitch> cruisecontrolpauseswitch{4, 2};
  CanSignalLe<EngShutdownOverrideSwitch_A> EngShutdownOverrideSwitch_A{62, 2};
  CanSignalLe<Engine_Test_Mode_Switch_A> Engine_Test_Mode_Switch_A{60, 2};
  CanSignalLe<Engine_Idle_Decrement_Switch> Engine_Idle_Decrement_Switch{58, 2};
  CanSignalLe<Engine_Idle_Increment_Switch> Engine_Idle_Increment_Switch{56, 2};
  CanSignalLe<CruiseControlState> CruiseControlState{53, 3};
  CanSignalLe<PTOState> PTOState{48, 5};
  CanSignalLe<std::uint8_t> CruiseControlSetSpeed{40, 8, 1, 0, 0, 250};  // km/h
  CanSignalLe<CruiseControlAccelerateSwitch> CruiseControlAccelerateSwitch{38, 2};
  CanSignalLe<CruiseControlResumeSwitch> CruiseControlResumeSwitch{36, 2};
  CanSignalLe<CruiseControlCoastSwitch> CruiseControlCoastSwitch{34, 2};
  CanSignalLe<CruiseControlSetSwitch> CruiseControlSetSwitch{32, 2};
  CanSignalLe<ClutchSwitch> ClutchSwitch{30, 2};
  CanSignalLe<BrakeSwitch> BrakeSwitch{28, 2};
  CanSignalLe<CruiseControlEnableSwitch> CruiseControlEnableSwitch{26, 2};
  CanSignalLe<CruiseControlActive> CruiseControlActive{24, 2};
  CanSignalLe<std::uint16_t> WheelBasedVehicleSpeed{8, 16, 0.00390625, 0, 0, 250.996};  // km/h
  CanSignalLe<ParkingBrakeSwitch> ParkingBrakeSwitch{2, 2};
  CanSignalLe<TwoSpeedAxleSwitch> TwoSpeedAxleSwitch{0, 2};
};

#undef CanSignalLe
#undef CanSignalBe
#undef CanMessage

using canID_0xC0000000 = VECTOR__INDEPENDENT_SIG_MSG;
using canID_0x9CFEBE00 = EEC4_00;
using canID_0x98FF20D9 = MHVACSTAT1_D9;
using canID_0x98FDE500 = MVS_00;
using canID_0x98FEDB00 = EFLP2_00;
using canID_0x98FC4B00 = EEC17_00;
using canID_0x98FECE3D = DM5_3D;
using canID_0x98FECE27 = DM5_27;
using canID_0x98FECE17 = DM5_17;
using canID_0x98FECE00 = DM5_00;
using canID_0x98C1FF00 = DM21_00;
using canID_0x8CFD0E00 = AT1OGC2_00;
using canID_0x8CFD1000 = AT1IGC2_00;
using canID_0x98FD6E00 = A1SCRSI1_00;
using canID_0x9CDEFF17 = RESET_17;
using canID_0x98FEC000 = SERV_00;
using canID_0x98FB4D00 = MI1_00;
using canID_0x98FCD500 = AT1P1I_00;
using canID_0x8CFE572F = ASC5_2F;
using canID_0x8CFE4121 = LCMD_21;
using canID_0x98E34A00 = DM7_00;
using canID_0x98FECF00 = DM6_00;
using canID_0x98FCC700 = DM56_00;
using canID_0x98A04A00 = DM34_00;
using canID_0x98A14A00 = DM33_00;
using canID_0x98A44A3D = DM30_3D;
using canID_0x98A44A00 = DM30_00;
using canID_0x989E4A00 = DM29_00;
using canID_0x98FD8000 = DM28_00;
using canID_0x98FDB700 = DM25_00;
using canID_0x98FDB600 = DM24_00;
using canID_0x98FDB500 = DM23_00;
using canID_0x98FED400 = DM12_00;
using canID_0x98FB1700 = NTSNA_00;
using canID_0x98FB1600 = NTENA_00;
using canID_0x98FB1500 = NTEEA_00;
using canID_0x98FB1400 = NTVMA_00;
using canID_0x98FB1300 = NTEHA_00;
using canID_0x98FB1200 = NTFCA_00;
using canID_0x98FB1100 = NTSNS_00;
using canID_0x98FB1000 = NTENS_00;
using canID_0x98FB0F00 = NTEES_00;
using canID_0x98FB0E00 = NTVMS_00;
using canID_0x98FB0D00 = NTEHS_00;
using canID_0x98FB0C00 = NTFCS_00;
using canID_0x98FB0B00 = NTSNV_00;
using canID_0x98FB0A00 = NTENV_00;
using canID_0x98FB0900 = NTEEV_00;
using canID_0x98FB0800 = NTVMV_00;
using canID_0x98FB0700 = NTEHV_00;
using canID_0x98FB0600 = NTFCV_00;
using canID_0x98FB0500 = NTEEEA_00;
using canID_0x98FB0400 = NTVMEA_00;
using canID_0x98FB0300 = NTEHEA_00;
using canID_0x98FB0200 = NTFCEA_00;
using canID_0x8CFD0C00 = AT2IGC2_00;
using canID_0x9CFBE800 = ASI2_00;
using canID_0x98FAFA00 = ATNXMS1_00;
using canID_0x98FAF900 = ATNXMS2_00;
using canID_0x98FAF700 = AT1HI3_00;
using canID_0x8CF0FB00 = A1SCRDSR3_00;
using canID_0x98F01000 = AT2IG1_00;
using canID_0x98FB0100 = GHGTTL_00;
using canID_0x98FB0000 = GHGTTA_00;
using canID_0x98FAFF00 = GHGTTS_00;
using canID_0x98FAFE00 = GHGTA_00;
using canID_0x98FAFD00 = GHGTS_00;
using canID_0x98FAFC00 = GHGTL_00;
using canID_0x9CFE8700 = IT6_00;
using canID_0x9CFE8300 = IT2_00;
using canID_0x9CFE8200 = IT1_00;
using canID_0x98FDD000 = IC2_00;
using canID_0x90F01A00 = TFAC_00;
using canID_0x98FE9600 = TCW_00;
using canID_0x98FDC300 = EEC15_00;
using canID_0x98FDC200 = EEC14_00;
using canID_0x98FCD000 = EPSI_00;
using canID_0x98FBD600 = EEC21_00;
using canID_0x8CF00327 = EEC2_27;
using canID_0x98F0090B = BSC_VDC2;
using canID_0x98FEE621 = TD;
using canID_0x98FE8D00 = E_VEP2;
using canID_0x98FF3121 = BFD;
using canID_0x98FF23F3 = B2V_ELECENERGY;
using canID_0x98FAC330 = AIR3_30;
using canID_0x98FF7321 = PROPB73_21;
using canID_0x98FF7221 = PROPB72_21;
using canID_0x98FF7121 = PROPB71_21;
using canID_0x98FF4327 = PROPB43_27;
using canID_0x98FF00D7 = PROPB00_D7;
using canID_0x98FE700B = CVW_0B;
using canID_0x98FF634A = PROPB63_4A;
using canID_0x98FF624A = PROPB62_4A;
using canID_0x98FF2721 = PROPB27_21;
using canID_0x98EEFEFE = AC_FE_FE;
using canID_0x98D9034A = DM14_03_4A;
using canID_0x98D4034A = DM18_03_4A;
using canID_0x98D7034A = DM16_03_4A;
using canID_0x8CFF224A = PropBBCMTX_21_4A;
using canID_0x98FF2A21 = PropB_2A_21;
using canID_0x98FF96F3 = B2V_ELECENERGY_F3;
using canID_0x80F02B53 = CN_53;
using canID_0x98FEEC5A = VI_5A;
using canID_0x98EAFF11 = PCC_RQST_TX;
using canID_0x98FF305A = PropB_30_VCU_5A;
using canID_0x98FD0900 = HRLFC_0;
using canID_0x98FD0700 = DLCC1;
using canID_0x8CF0955A = HVESSD6_VCU_5A;
using canID_0x98FF1480 = MOCICNTRLPILOTSTAT;
using canID_0x98DF11FE = PCC_DM13;
using canID_0x98FED65A = HSI1_VCU;
using canID_0x9CFEAF00 = FCG;
using canID_0x98FCC25A = HSS1_VCU;
using canID_0x98FF005A = PropB_VCU_00_ClntLvl;
using canID_0x88FF00EF = MTPIMSTAT;
using canID_0x98FF06EF = MTPIMSWVERS;
using canID_0x98FFD6B3 = MDCDCSTAT;
using canID_0x98FF2019 = MHVACSTAT1;
using canID_0x98FF03EF = MTPIMTRQINFO;
using canID_0x8C02FF5A = MG1IMT_VCU;
using canID_0x98FF0ACA = MBTMSSTAT;
using canID_0x98FF0BCA = MBTMSSTAT2;
using canID_0x98FF0BEF = MTPIMMTRROTATE2;
using canID_0x98FF12F3 = B2V_ST2;
using canID_0x98FF14F3 = B2V_ST4;
using canID_0x98FF3019 = MHVACSTAT2;
using canID_0x98FFC882 = PropB_C8;
using canID_0x98FFCB82 = PropB_CB;
using canID_0x98FFC982 = PropB_C9;
using canID_0x8CF0965A = HVESSS1_VCU;
using canID_0x98FCB75A = HSS2_VCU;
using canID_0x8CFB745A = MG1IS1;
using canID_0x98FEFC82 = BC_DD_82;
using canID_0x98FEF682 = E_IC1_82;
using canID_0x90F03300 = E_A1SCRAI2;
using canID_0x98FCC500 = E_A1SCREGT2;
using canID_0x9CFD0800 = E_EOM;
using canID_0x98FE9A00 = E_TCI2;
using canID_0x98FE6A82 = E_EFS_82;
using canID_0x98FE6A00 = E_EFS;
using canID_0x8CF02300 = E_A1SCRDSI1;
using canID_0x98FDD082 = IC2;
using canID_0x98FCE48C = EFP;
using canID_0x9CFE9200 = E_EI1;
using canID_0x9CFE8C00 = E_AAI;
using canID_0x98FEDD00 = E_TC;
using canID_0x98FFDD4A = ST_ECMP_TEL;
using canID_0x98FD82FE = DM27;
using canID_0x98F00100 = BC_EBC1_1_0;
using canID_0x98FF2921 = PropB_29_BCM;
using canID_0x98DFFFFE = DM13_Global;
using canID_0x98FF3021 = BFC;
using canID_0x98FF3321 = VCCD;
using canID_0x9CFEC321 = ETC5_21;
using canID_0x98FEF803 = TF1;
using canID_0x98DA4A21 = BCM_UDS_RX_4A;
using canID_0x98DAFAFE = UDS_RX_FA;
using canID_0x98DAFEFA = UDS_TX_FA;
using canID_0x98DAF9FE = UDS_RX_F9;
using canID_0x98DAFEF9 = UDS_TX_F9;
using canID_0x98DAFE4A = UDS_TX_4A;
using canID_0x98FEF121 = E_CCVS1_21;
using canID_0x98FEF721 = E_VEP1_21;
using canID_0x98F0010B = BC_EBC1_1_B;
using canID_0x98FEFA21 = BRAKES;
using canID_0x98FE4F0B = VDSC1;
using canID_0x98FECC4A = Tel_DM3;
using canID_0x98FED34A = Tel_DM11;
using canID_0x98EA4AFE = Tel_RQST_RX;
using canID_0x98FEEBFE = CI;
using canID_0x9CFEAB03 = ETC6;
using canID_0x98FEF433 = TC;
using canID_0x98EAFE4A = Tel_PGN_Request_4A;
using canID_0x98D0FF37 = CIM;
using canID_0x98FDC5FE = EII;
using canID_0x98FDC54A = Tel_EII;
using canID_0x98DA214A = BCM_UDS_TX_4A;
using canID_0x98DA4AFE = UDS_RX_4A;
using canID_0x98C2FFFE = DM20;
using canID_0x98684B21 = OnBoard_Program1_RX_4B;
using canID_0x98D3FFFE = DM19;
using canID_0x98FECBFE = DM2;
using canID_0x98FEDA4A = Tel_SOFT;
using canID_0x98FEEB4A = Tel_CI;
using canID_0x98FECB4A = Tel_DM2;
using canID_0x98FB8121 = OnBoard_Program2_RX;
using canID_0x9868214A = OnBoard_Program1_TX_4A;
using canID_0x98FFDD00 = ST_ECMP_RX;
using canID_0x9CFEB700 = E_ESLFI;
using canID_0x98FFF821 = PropB_Vehicle_State_BCM;
using canID_0x98EF174A = Prop_A_Cluster_TX_4A;
using canID_0x98EF214A = Proprietary_A_TX_4A;
using canID_0x98EF4A17 = Prop_A_Cluster_RX_4A;
using canID_0x98FB814A = OnBoard_Program2_TX_4A;
using canID_0x98684A21 = OnBoard_Program1_RX_4A;
using canID_0x98FEDAFE = SOFT;
using canID_0x98FECA4A = Tel_DM1;
using canID_0x98DF4AFE = Tel_DM13;
using canID_0x98FE5600 = E_AT1T1I_1;
using canID_0x98FC2A00 = E_OII;
using canID_0x90FDA300 = E_EEC6;
using canID_0x98FD9F00 = E_AT1AC1;
using canID_0x98FD7900 = E_AT1TI_1_2;
using canID_0x94FD3E00 = E_A1SCREGT;
using canID_0x98FD9B00 = E_A1DEFI;
using canID_0x98FC9600 = E_EI2;
using canID_0x9CFEB000 = E_TTI2;
using canID_0x9CFEB900 = E_LTFI;
using canID_0x98F00F00 = E_AT1OG1;
using canID_0x98FEFF00 = E_OI;
using canID_0x98FEF000 = E_PTO;
using canID_0x98FEE500 = E_HOURS;
using canID_0x98FD9400 = E_EEC7;
using canID_0x98FD7C00 = E_DPFC1;
using canID_0x98FEE900 = E_LFC;
using canID_0x98FEF700 = E_VEP1;
using canID_0x98FEF200 = E_LFE1;
using canID_0x98FECAFE = DM1;
using canID_0x98FEDC00 = E_IO;
using canID_0x98FEF600 = E_IC1;
using canID_0x98FD9800 = E_AT1HI;
using canID_0x98FDB800 = E_DM26;
using canID_0x98FEE700 = E_VH;
using canID_0x98FEC117 = VDHR;
using canID_0x98FEE400 = E_SHUTDN;
using canID_0x9CFEB300 = E_LFI;
using canID_0x98FEBD00 = E_FD1;
using canID_0x98FEA400 = E_ET2;
using canID_0x98FEEE00 = E_ET1;
using canID_0x9CFEB100 = E_EH;
using canID_0x8CF00A00 = E_EGF1;
using canID_0x98FEFC21 = BC_DD;
using canID_0x98FD2000 = E_A1DOC;
using canID_0x98FD7B00 = E_AT1S;
using canID_0x98FD8C00 = E_AT1GP;
using canID_0x8CFD9200 = E_EOI;
using canID_0x98F00E00 = E_AT1IG1;
using canID_0x90FCFD00 = E_EEC9;
using canID_0x98FEEF00 = E_EFLP1;
using canID_0x98FDD500 = E_EEC5;
using canID_0x8CF00300 = E_EEC2;
using canID_0x98FEED00 = E_CCSS;
using canID_0x98FEF500 = E_AMB;
using canID_0x98FDB400 = E_AT1IG2;
using canID_0x98FDB200 = E_AT1IMG;
using canID_0x98FDB300 = E_AT1OG2;
using canID_0x98FEEC00 = E_VI;
using canID_0x98F0F611 = ADASIS_V2;
using canID_0x98FFC200 = E_NGCCC3;
using canID_0x98FEEA21 = BC_VW;
using canID_0x98FE4A03 = ETC7;
using canID_0x98EA11FE = PCC_RQST_RX;
using canID_0x8CFFC411 = PCC_NGCC2;
using canID_0x98FCE700 = E_EC3;
using canID_0x8CFFC111 = PCC_NGCC1;
using canID_0x98FFC000 = E_NGCCC;
using canID_0x98FE7011 = PCC_CVW;
using canID_0x8CF0D311 = PCC_PCC1;
using canID_0x98851100 = E_CM2;
using canID_0x98FCDC00 = E_CCVS3;
using canID_0x98F00503 = TCU_ETC2;
using canID_0x98F0000F = E_ERC1;
using canID_0x98FEDF00 = E_EEC3;
using canID_0x8CF00400 = E_EEC1;
using canID_0x98FEE300 = E_EC1;
using canID_0x98FEF100 = E_CCVS1;

