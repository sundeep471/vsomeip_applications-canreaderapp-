#define CanMessage ::util::CanMessage
#define CanSignalBe ::util::CanSignalBe
#define CanSignalLe ::util::CanSignalLe

struct TimeDate_ICL : public CanMessage<0x98FEE617, 8> {
  enum class Seconds : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class Minutes : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class Hours : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class Month : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class Day : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class Year : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<Seconds> Seconds{0, 8};
  CanSignalLe<Minutes> Minutes{8, 8};
  CanSignalLe<Hours> Hours{16, 8};
  CanSignalLe<Month> Month{24, 8};
  CanSignalLe<Day> Day{32, 8};
  CanSignalLe<Year> Year{40, 8};
};

struct ADASP1_RTC : public CanMessage<0x98FF554A, 8> {
  enum class ProfileLong_Offset_0x05 : std::uint16_t {
    INVALID = 8191,
  };

  enum class Metadata_MapProvider_0x06 : std::uint8_t {
    UNKNOWN = 0,
    NAVTEQ = 1,
    TELEATLAS = 2,
    ZENRIN = 3,
    NOTAVAILABLE = 7,
  };

  enum class Pos_Offset_0x01 : std::uint16_t {
    INVALID = 8191,
  };

  enum class ProfileShort_Offset_0x04 : std::uint16_t {
    INVALID = 8191,
  };

  enum class Stub_Offset_0x03 : std::uint16_t {
    INVALID = 8191,
  };

  enum class ADAS_MsgType : std::uint8_t {
    POSITION = 1,
    STUB = 3,
    PROFILE_SHORT = 4,
    PROFILE_LONG = 5,
    METADATA = 6,
  };

  enum class Pos_PathIndex_0x01 : std::uint8_t {
    UNKNOWN = 0,
    POSITION_NOT_IN_DIGITZED_AREA = 1,
    POSITION_NOT_ON_ROAD = 2,
    SYSTEM_NOT_CALIBRATED = 3,
    MESSAGE_IS_REFERRING_TO_THE_CURRENT_STREET_SEGMENT = 4,
    STUB_INDICATION = 5,
    CONTINUATION_STUB = 6,
  };

  enum class Stub_PathIndex_0x03 : std::uint8_t {
    INVALID_VALUE_1 = 0,
    INVALID_VALUE_2 = 1,
    INVALID_VALUE_3 = 2,
    INVALID_VALUE_4 = 3,
    INVALID_VALUE_5 = 4,
    BRANCH_INDICATION = 5,
    CONTINUATION_STUB = 6,
    INVALID_VALUE_6 = 7,
  };

  enum class ProfileLong_Update_0x05 : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  enum class Pos_PositionAge_0x01 : std::uint16_t {
    MS = 510,
    NOTAVAILABLE = 511,
  };

  enum class ProfileShort_Update_0x04 : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  enum class ProfileLong_Retransmission_0x05 : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  enum class ProfileShort_Retransmission_0x04 : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  enum class Stub_Retransmission_0x03 : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  enum class ProfileLong_ControlPoint_0x05 : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  enum class ProfileShort_ControlPoint_0x04 : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  enum class Pos_PositionProbability_0x01 : std::uint8_t {
    UNKNOWN = 0,
    NOTAVAILABLE = 31,
  };

  enum class ProfileLong_ProfileType_0x05 : std::uint8_t {
    N_A = 0,
    LONGITUDE = 1,
    LATITUDE = 2,
    TRAFFIC_SIGN = 8,
    TRUCK_SPEED_LIMITS = 9,
    ZM_RESERVED_SYSTEM_SPECIFIC = 16,
    ISA_RULE_BOOK_RESERVED_SYSTEM_SPECIFIC = 17,
    RESERVED_SYSTEM_SPECIFIC_1 = 18,
    RESERVED_SYSTEM_SPECIFIC_2 = 19,
    RESERVED_SYSTEM_SPECIFIC_3 = 20,
    RESERVED_SYSTEM_SPECIFIC_4 = 21,
    RESERVED_SYSTEM_SPECIFIC_5 = 22,
    RESERVED_SYSTEM_SPECIFIC_6 = 23,
    RESERVED_SYSTEM_SPECIFIC_7 = 24,
    RESERVED_SYSTEM_SPECIFIC_8 = 25,
    RESERVED_SYSTEM_SPECIFIC_9 = 26,
    RESERVED_SYSTEM_SPECIFIC_10 = 27,
    RESERVED_SYSTEM_SPECIFIC_11 = 28,
    RESERVED_SYSTEM_SPECIFIC_12 = 29,
    RESERVED_SYSTEM_SPECIFIC_13 = 30,
    RESERVED_SYSTEM_SPECIFIC_14 = 31,
  };

  enum class ProfileShort_ProfileType_0x04 : std::uint8_t {
    NOTAVAILABLE = 0,
    CURVATURE = 1,
    ROUTE_NUMBER_TYPES = 2,
    SLOPE_STEP = 3,
    SLOPE_LINEAR = 4,
    ROAD_ACCESSIBILITY = 5,
    ROAD_CONDITION = 6,
    VARIALBLE_SPEED_SIGN_POSITION = 7,
    HEADING_CHANGE = 8,
    RESERVED_SYSTEM_SPECIFIC_1 = 16,
    RESERVED_SYSTEM_SPECIFIC_2 = 17,
    RESERVED_SYSTEM_SPECIFIC_3 = 18,
    RESERVED_SYSTEM_SPECIFIC_4 = 19,
    RESERVED_SYSTEM_SPECIFIC_5 = 20,
    RESERVED_SYSTEM_SPECIFIC_6 = 21,
    RESERVED_SYSTEM_SPECIFIC_7 = 22,
    RESERVED_SYSTEM_SPECIFIC_8 = 23,
    RESERVED_SYSTEM_SPECIFIC_9 = 24,
    RESERVED_SYSTEM_SPECIFIC_10 = 25,
    RESERVED_SYSTEM_SPECIFIC_11 = 26,
    RESERVED_SYSTEM_SPECIFIC_12 = 27,
    RESERVED_SYSTEM_SPECIFIC_13 = 28,
    RESERVED_SYSTEM_SPECIFIC_14 = 29,
    RESERVED_SYSTEM_SPECIFIC_15 = 30,
    RESERVED_SYSTEM_SPECIFIC_16 = 31,
  };

  enum class Stub_SubPathIndex_0x03 : std::uint8_t {
    INVALID_VALUE_1 = 0,
    INVALID_VALUE_2 = 1,
    INVALID_VALUE_3 = 2,
    INVALID_VALUE_4 = 3,
    INVALID_VALUE_5 = 4,
    BRANCH_INDICATION = 5,
    CONTINUATION_STUB = 6,
    INVALID_VALUE_6 = 7,
  };

  enum class Stub_FunctionalRoadClass : std::uint8_t {
    UNKNOWN = 0,
    HIGHEST_PRIORITY = 1,
    PRIORITY_2 = 2,
    PRIORITY_3 = 3,
    PRIORITY_4 = 4,
    PRIORITY_5 = 5,
    LOWEST_PRIORITY = 6,
    NOTAVAILABLE = 7,
  };

  enum class ProfileShort_Distance1_0x04 : std::uint16_t {
    INVALID = 1023,
  };

  enum class Metadata_RegionCode_0x06 : std::uint16_t {
    UNKNOWN = 0,
    NOTAVAILABLE = 32767,
  };

  enum class Metadata_DrivingSide_0x06 : std::uint8_t {
    DRIVINGSIDELEFT = 0,
    DRIVINGSIDERIGHT = 1,
  };

  enum class ProfileShort_AccuracyClass_0x04 : std::uint8_t {
    HIGHEST_ACCURACY = 0,
    MID_ACCURACY = 1,
    LOWEST_ACCURACY = 2,
    NOTAVAILABLE = 3,
  };

  enum class Stub_RelativeProbability_0x03 : std::uint8_t {
    TURN_IS_NOT_ALLOWED = 0,
    NOT_AVAILABLE = 31,
  };

  enum class Pos_Speed_0x01 : std::uint16_t {
    M_S_1 = 0,
    X1_TO_0X39_12_6M_S_TO_0_2M_S = 1,
    STANDING_STILL = 64,
    X41_TO_0X1FD_0_2M_S_TO_89_0M_S = 65,
    M_S_2 = 510,
    NOTAVAILABLE = 511,
  };

  enum class Pos_CurrentLane_0x01 : std::uint8_t {
    UNKNOWN = 0,
    EMERGENCY_LANE = 1,
    SINGLE_LANE_ROAD = 2,
    LEFT_MOST_LANE = 3,
    RIGHT_MOST_LANE = 4,
    MIDDLE_LANE_ON_3_LANE_ROAD = 5,
    NOTAVAILABLE = 7,
  };

  enum class ProfileShort_Value0_0x04 : std::uint16_t {
    NOTAVAILABLE = 1023,
  };

  enum class Stub_FormOfWay_0x03 : std::uint8_t {
    UNKNOWN = 0,
    FREEWAY_OR_CONTROLLED_ACCESS_ROAD_THAT_IS_NOT_A_SLIP_ROAD_RAMP = 1,
    MULTIPLE_CARRIAGEWAY_OR_MULTIPLY_DIGITIZED_ROAD = 2,
    SINGLE_CARRIAGEWAY_DEFAULT = 3,
    ROUNDABOUT_CIRCLE = 4,
    TRAFFIC_SQUARE_SPECIAL_TRAFFIC_FIGURE = 5,
    PARALLEL_ROAD = 8,
    SLIP_ROAD_RAMP_ON_A_FREEWAY_OR_CONTROLLED_ACCESS_ROAD = 9,
    SLIP_ROAD_RAMP_NOT_ON_A_FREEWAY_OR_CONTROLLED_ACCESS_ROAD = 10,
    SERVICE_ROAD_OR_FRONTAGE_ROAD = 11,
    ENTRANCE_TO_OR_EXIT_OF_A_CAR_PARK = 12,
    PEDESTRIAN_ZONE = 13,
  };

  enum class Stub_ComplexIntersection_0x03 : std::uint8_t {
    NEW_PATH_IS_NOT_PART_OF_COMPLEX_INTERSECTION = 0,
    NEW_PATH_IS_PART_OF_COMPLEX_INTERSECTION = 1,
    UNKNOWN = 2,
    NOTAVAILABLE = 3,
  };

  enum class Pos_PositionConfidence_0x01 : std::uint8_t {
    HIGHEST_CONFIDENCE = 0,
    CONFIDENCE_1 = 1,
    CONFIDENCE_2 = 2,
    CONFIDENCE_3 = 3,
    CONFIDENCE_4 = 4,
    CONFIDENCE_5 = 5,
    LOWEST_CONFIDECE = 6,
    NOTAVAILABLE = 7,
  };

  enum class Stub_PartOfCalculatedRoute_0x03 : std::uint8_t {
    NEW_PATH_IS_NOT_PART_OF_CALCULATED_ROUTE = 0,
    NEW_PATH_IS_PART_OF_CALCULATED_ROUTE = 1,
    UNKNOWN = 2,
    NOTAVAILABLE = 3,
  };

  enum class ProfileShort_Value1_0x04 : std::uint16_t {
    NOTAVAILABLE = 1023,
  };

  enum class Metadata_MapVersionYear_0x06 : std::uint8_t {
    NOTAVAILABLE = 63,
  };

  enum class Metadata_MapVersionQtr_0x06 : std::uint8_t {
    Q1 = 0,
    Q2 = 1,
    Q3 = 2,
    Q4 = 3,
  };

  enum class Stub_TurnAngle_0x03 : std::uint8_t {
    UNKNOWN = 254,
    NOT_AVAILABLE = 255,
  };

  enum class Stub_NumberOfLanes_0x03 : std::uint8_t {
    NOT_AVAILABLE = 7,
  };

  enum class Metadata_SpeedUnits_0x06 : std::uint8_t {
    KM_H = 0,
    MPH = 1,
  };

  enum class Stub_NumberOfOppositeLanes_0x03 : std::uint8_t {
    NOT_AVAILABLE = 3,
  };

  enum class Stub_RightOfWay_0x03 : std::uint8_t {
    PARENT_PATH_HAS_RIGHT_OF_WAY = 0,
    SUB_PATH_HAS_RIGHT_OF_WAY = 1,
    UNKNOWN = 2,
    NOTAVAILABLE = 3,
  };

  enum class Pos_RelativeHeading_0x01 : std::uint8_t {
    UNKNOWN = 254,
    NOTAVAILABLE = 255,
  };

  enum class Stub_LastStubAtOffset : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
  };

  CanSignalBe<std::uint16_t> Metadata_CountryCode_0x06{8, 10, 1, 0, 0, 1023};
  CanSignalBe<ProfileLong_Offset_0x05> ProfileLong_Offset_0x05{8, 13};
  CanSignalBe<Metadata_MapProvider_0x06> Metadata_MapProvider_0x06{2, 3};
  CanSignalBe<Pos_Offset_0x01> Pos_Offset_0x01{8, 13};
  CanSignalBe<ProfileShort_Offset_0x04> ProfileShort_Offset_0x04{8, 13};
  CanSignalBe<Stub_Offset_0x03> Stub_Offset_0x03{8, 13};
  CanSignalBe<ADAS_MsgType> ADAS_MsgType{5, 3};
  CanSignalBe<std::uint16_t> Metadata_HardwareVersion_0x06{24, 9, 1, 0, 0, 511};
  CanSignalBe<std::uint8_t> Metadata_ProtocolSubVer_0x06{17, 3, 1, 0, 0, 7};
  CanSignalBe<std::uint8_t> ProfileLong_PathIndex_0x05{16, 6, 1, 0, 8, 63};
  CanSignalBe<std::uint8_t> Metadata_ProtocolVerMajor_0x06{20, 2, 1, 0, 0, 3};
  CanSignalBe<Pos_PathIndex_0x01> Pos_PathIndex_0x01{16, 6};
  CanSignalBe<std::uint8_t> ProfileShort_PathIndex_0x04{16, 6, 1, 0, 8, 63};
  CanSignalBe<Stub_PathIndex_0x03> Stub_PathIndex_0x03{16, 6};
  CanSignalBe<std::uint8_t> ProfileLong_CyclicCounter_0x05{22, 2, 1, 0, 0, 3};
  CanSignalBe<std::uint8_t> Metadata_CyclicCounter_0x06{22, 2, 1, 0, 0, 3};
  CanSignalBe<std::uint8_t> Pos_CyclicCounter_0x01{22, 2, 1, 0, 0, 3};
  CanSignalBe<std::uint8_t> ProfileShort_CyclicCounter_0x04{22, 2, 1, 0, 0, 3};
  CanSignalBe<std::uint8_t> Stub_CyclicCounter_0x03{22, 2, 1, 0, 0, 3};
  CanSignalBe<ProfileLong_Update_0x05> ProfileLong_Update_0x05{24, 1};
  CanSignalBe<Pos_PositionAge_0x01> Pos_PositionAge_0x01{32, 9};
  CanSignalBe<ProfileShort_Update_0x04> ProfileShort_Update_0x04{24, 1};
  CanSignalBe<std::uint8_t> Stub_Update_0x03{24, 1, 1, 0, 0, 1};
  CanSignalBe<ProfileLong_Retransmission_0x05> ProfileLong_Retransmission_0x05{25, 1};
  CanSignalBe<ProfileShort_Retransmission_0x04> ProfileShort_Retransmission_0x04{25, 1};
  CanSignalBe<Stub_Retransmission_0x03> Stub_Retransmission_0x03{25, 1};
  CanSignalBe<ProfileLong_ControlPoint_0x05> ProfileLong_ControlPoint_0x05{26, 1};
  CanSignalBe<ProfileShort_ControlPoint_0x04> ProfileShort_ControlPoint_0x04{26, 1};
  CanSignalBe<Pos_PositionProbability_0x01> Pos_PositionProbability_0x01{25, 5};
  CanSignalBe<ProfileLong_ProfileType_0x05> ProfileLong_ProfileType_0x05{27, 5};
  CanSignalBe<std::uint8_t> Pos_PositionIndex_0x01{30, 2, 1, 0, 0, 3};
  CanSignalBe<ProfileShort_ProfileType_0x04> ProfileShort_ProfileType_0x04{27, 5};
  CanSignalBe<Stub_SubPathIndex_0x03> Stub_SubPathIndex_0x03{26, 6};
  CanSignalBe<Stub_FunctionalRoadClass> Stub_FunctionalRoadClass{32, 3};
  CanSignalBe<ProfileShort_Distance1_0x04> ProfileShort_Distance1_0x04{44, 10};
  CanSignalBe<Metadata_RegionCode_0x06> Metadata_RegionCode_0x06{40, 15};
  CanSignalBe<std::uint32_t> ProfileLong_Value_0x05{56, 32, 1, 0, 0, 4294967293};
  CanSignalBe<Metadata_DrivingSide_0x06> Metadata_DrivingSide_0x06{39, 1};
  CanSignalBe<ProfileShort_AccuracyClass_0x04> ProfileShort_AccuracyClass_0x04{38, 2};
  CanSignalBe<Stub_RelativeProbability_0x03> Stub_RelativeProbability_0x03{35, 5};
  CanSignalBe<Pos_Speed_0x01> Pos_Speed_0x01{48, 9};
  CanSignalBe<Pos_CurrentLane_0x01> Pos_CurrentLane_0x01{41, 3};
  CanSignalBe<ProfileShort_Value0_0x04> ProfileShort_Value0_0x04{50, 10};
  CanSignalBe<Stub_FormOfWay_0x03> Stub_FormOfWay_0x03{40, 4};
  CanSignalBe<Stub_ComplexIntersection_0x03> Stub_ComplexIntersection_0x03{44, 2};
  CanSignalBe<Pos_PositionConfidence_0x01> Pos_PositionConfidence_0x01{44, 3};
  CanSignalBe<Stub_PartOfCalculatedRoute_0x03> Stub_PartOfCalculatedRoute_0x03{46, 2};
  CanSignalBe<ProfileShort_Value1_0x04> ProfileShort_Value1_0x04{56, 10};
  CanSignalBe<std::uint8_t> Segment_EffectiveSpeedLimitType{48, 3, 1, 0, 0, 5};
  CanSignalBe<Metadata_MapVersionYear_0x06> Metadata_MapVersionYear_0x06{48, 6};
  CanSignalBe<std::uint8_t> Segment_EffectiveSpeedLimit{51, 5, 1, 0, 0, 29};
  CanSignalBe<Metadata_MapVersionQtr_0x06> Metadata_MapVersionQtr_0x06{54, 2};
  CanSignalBe<Stub_TurnAngle_0x03> Stub_TurnAngle_0x03{48, 8};
  CanSignalBe<Stub_NumberOfLanes_0x03> Stub_NumberOfLanes_0x03{56, 3};
  CanSignalBe<std::uint8_t> Metadata_ProtocolVerMinor_0x06{56, 4, 1, 0, 0, 15};
  CanSignalBe<Metadata_SpeedUnits_0x06> Metadata_SpeedUnits_0x06{60, 1};
  CanSignalBe<Stub_NumberOfOppositeLanes_0x03> Stub_NumberOfOppositeLanes_0x03{59, 2};
  CanSignalBe<Stub_RightOfWay_0x03> Stub_RightOfWay_0x03{61, 2};
  CanSignalBe<Pos_RelativeHeading_0x01> Pos_RelativeHeading_0x01{56, 8};
  CanSignalBe<Stub_LastStubAtOffset> Stub_LastStubAtOffset{63, 1};
};

struct GPM1_K : public CanMessage<0x98FF6027, 8> {
  enum class CalculatedGTWStatus : std::uint8_t {
    NOESTIMATIONAVAILABLE = 0,
    LOWQUALITY = 1,
    NORMALQUALITY = 2,
    HIGHQUALITY = 3,
    NOT_USED_1 = 4,
    NOT_USED_2 = 5,
    NOT_USED_3 = 6,
    NOT_USED_4 = 7,
    NOT_USED_5 = 8,
    NOT_USED_6 = 9,
    NOT_USED_7 = 10,
    NOT_USED_8 = 11,
    NOT_USED_9 = 12,
    NOT_USED_10 = 13,
    ERROR = 14,
    NOT_AVAILABLE = 15,
  };

  enum class CalculatedGTW : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<CalculatedGTWStatus> CalculatedGTWStatus{44, 4};
  CanSignalLe<CalculatedGTW> CalculatedGTW{48, 16};
};

struct GPM2_K : public CanMessage<0x8CFF6327, 8> {
  enum class Charge61 : std::uint8_t {
    GENERATORNOTCHARGING = 0,
    GENERATORCHARGING = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class TrailerDetectionAndABSStatus : std::uint8_t {
    NOTRAILERDETECTED = 0,
    TRAILERDETECTEDNOABS = 1,
    TRAILERABSDETECTED = 2,
    TRAILEREBSDETECTED = 3,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class CoastingActiveSignal : std::uint8_t {
    FALSE = 0,
    TRUE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EngineStartInhibitByOTAStatus : std::uint8_t {
    NOTINHIBITED = 0,
    INHIBITED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<Charge61> Charge61{2, 2};
  CanSignalLe<TrailerDetectionAndABSStatus> TrailerDetectionAndABSStatus{16, 3};
  CanSignalLe<CoastingActiveSignal> CoastingActiveSignal{58, 2};
  CanSignalLe<EngineStartInhibitByOTAStatus> EngineStartInhibitByOTAStatus{62, 2};
};

struct BrakeLiningWearInformation2_K : public CanMessage<0x98FF4327, 8> {
  enum class AxleNumber : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingLeftWheel_1 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingLeftWheel_2 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingLeftWheel_3 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingLeftWheel_4 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingLeftWheel_5 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingRightWheel_1 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingRightWheel_2 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingRightWheel_3 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingRightWheel_4 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningRemainingRightWheel_5 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class BrakeLiningDistanceRemainingLeftWheel_1 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingLeftWheel_2 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingLeftWheel_3 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingLeftWheel_4 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingLeftWheel_5 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingRightWheel_1 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingRightWheel_2 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingRightWheel_3 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingRightWheel_4 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class BrakeLiningDistanceRemainingRightWheel_5 : std::uint16_t {
    BRAKELININGSNEW = 64256,
    CHANGEBRAKELININGS = 64272,
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  CanSignalLe<AxleNumber> AxleNumber{0, 8};
  CanSignalLe<BrakeLiningRemainingLeftWheel_1> BrakeLiningRemainingLeftWheel_1{8, 8};
  CanSignalLe<BrakeLiningRemainingLeftWheel_2> BrakeLiningRemainingLeftWheel_2{8, 8};
  CanSignalLe<BrakeLiningRemainingLeftWheel_3> BrakeLiningRemainingLeftWheel_3{8, 8};
  CanSignalLe<BrakeLiningRemainingLeftWheel_4> BrakeLiningRemainingLeftWheel_4{8, 8};
  CanSignalLe<BrakeLiningRemainingLeftWheel_5> BrakeLiningRemainingLeftWheel_5{8, 8};
  CanSignalLe<BrakeLiningRemainingRightWheel_1> BrakeLiningRemainingRightWheel_1{16, 8};
  CanSignalLe<BrakeLiningRemainingRightWheel_2> BrakeLiningRemainingRightWheel_2{16, 8};
  CanSignalLe<BrakeLiningRemainingRightWheel_3> BrakeLiningRemainingRightWheel_3{16, 8};
  CanSignalLe<BrakeLiningRemainingRightWheel_4> BrakeLiningRemainingRightWheel_4{16, 8};
  CanSignalLe<BrakeLiningRemainingRightWheel_5> BrakeLiningRemainingRightWheel_5{16, 8};
  CanSignalLe<BrakeLiningDistanceRemainingLeftWheel_1> BrakeLiningDistanceRemainingLeftWheel_1{32, 16};
  CanSignalLe<BrakeLiningDistanceRemainingLeftWheel_2> BrakeLiningDistanceRemainingLeftWheel_2{32, 16};
  CanSignalLe<BrakeLiningDistanceRemainingLeftWheel_3> BrakeLiningDistanceRemainingLeftWheel_3{32, 16};
  CanSignalLe<BrakeLiningDistanceRemainingLeftWheel_4> BrakeLiningDistanceRemainingLeftWheel_4{32, 16};
  CanSignalLe<BrakeLiningDistanceRemainingLeftWheel_5> BrakeLiningDistanceRemainingLeftWheel_5{32, 16};
  CanSignalLe<BrakeLiningDistanceRemainingRightWheel_1> BrakeLiningDistanceRemainingRightWheel_1{48, 16};
  CanSignalLe<BrakeLiningDistanceRemainingRightWheel_2> BrakeLiningDistanceRemainingRightWheel_2{48, 16};
  CanSignalLe<BrakeLiningDistanceRemainingRightWheel_3> BrakeLiningDistanceRemainingRightWheel_3{48, 16};
  CanSignalLe<BrakeLiningDistanceRemainingRightWheel_4> BrakeLiningDistanceRemainingRightWheel_4{48, 16};
  CanSignalLe<BrakeLiningDistanceRemainingRightWheel_5> BrakeLiningDistanceRemainingRightWheel_5{48, 16};
};

struct RGE23_TT_C8 : public CanMessage<0x98FE5EC8, 8> {
  enum class IdentificationDataIndex : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<std::uint8_t> TyreOrWheelIdentification{0, 8, 1, 0, 0, 0};
  CanSignalLe<std::int16_t> TyreTemperature{8, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint16_t> AirLeakageDetection{24, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyrePressureThresholdDetection{40, 3, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyreModulePowerSupply{43, 2, 1, 0, 0, 0};
  CanSignalLe<IdentificationDataIndex> IdentificationDataIndex{48, 8};
};

struct RGE23_TT_C0 : public CanMessage<0x98FE5EC0, 8> {
  enum class IdentificationDataIndex : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<std::uint8_t> TyreOrWheelIdentification{0, 8, 1, 0, 0, 0};
  CanSignalLe<std::int16_t> TyreTemperature{8, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint16_t> AirLeakageDetection{24, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyrePressureThresholdDetection{40, 3, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyreModulePowerSupply{43, 2, 1, 0, 0, 0};
  CanSignalLe<IdentificationDataIndex> IdentificationDataIndex{48, 8};
};

struct RGE23_TT_B8 : public CanMessage<0x98FE5EB8, 8> {
  enum class IdentificationDataIndex : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<std::uint8_t> TyreOrWheelIdentification{0, 8, 1, 0, 0, 0};
  CanSignalLe<std::int16_t> TyreTemperature{8, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint16_t> AirLeakageDetection{24, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyrePressureThresholdDetection{40, 3, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyreModulePowerSupply{43, 2, 1, 0, 0, 0};
  CanSignalLe<IdentificationDataIndex> IdentificationDataIndex{48, 8};
};

struct RGE23_TT_B0 : public CanMessage<0x98FE5EB0, 8> {
  enum class IdentificationDataIndex : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<std::uint8_t> TyreOrWheelIdentification{0, 8, 1, 0, 0, 0};
  CanSignalLe<std::int16_t> TyreTemperature{8, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint16_t> AirLeakageDetection{24, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyrePressureThresholdDetection{40, 3, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyreModulePowerSupply{43, 2, 1, 0, 0, 0};
  CanSignalLe<IdentificationDataIndex> IdentificationDataIndex{48, 8};
};

struct RGE23_TT_A8 : public CanMessage<0x98FE5EA8, 8> {
  enum class IdentificationDataIndex : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<std::uint8_t> TyreOrWheelIdentification{0, 8, 1, 0, 0, 0};
  CanSignalLe<std::int16_t> TyreTemperature{8, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint16_t> AirLeakageDetection{24, 16, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyrePressureThresholdDetection{40, 3, 1, 0, 0, 0};
  CanSignalLe<std::uint8_t> TyreModulePowerSupply{43, 2, 1, 0, 0, 0};
  CanSignalLe<IdentificationDataIndex> IdentificationDataIndex{48, 8};
};

struct VehicleWeight_ICL : public CanMessage<0x98FEEA17, 8> {
  enum class AxleLocation : std::uint8_t {
    NOTAVAILABLE = 255,
  };

  enum class AxleWeight : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class TrailerWeight : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class CargoWeight : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<AxleLocation> AxleLocation{0, 8};
  CanSignalLe<AxleWeight> AxleWeight{8, 16};
  CanSignalLe<TrailerWeight> TrailerWeight{24, 16};
  CanSignalLe<CargoWeight> CargoWeight{40, 16};
};

struct ImplicitSpeedLimit_RTC : public CanMessage<0x99FF4F4A, 8> {
  enum class motorwayMapSpeedLim : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class DCMapSpeedLim : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class SCMapSpeedLim : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class ruralMapSpeedLim : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class urbanMapSpeedLim : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class playstreetMapSpeedLim : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<motorwayMapSpeedLim> motorwayMapSpeedLim{0, 8};
  CanSignalLe<DCMapSpeedLim> DCMapSpeedLim{8, 8};
  CanSignalLe<SCMapSpeedLim> SCMapSpeedLim{16, 8};
  CanSignalLe<ruralMapSpeedLim> ruralMapSpeedLim{24, 8};
  CanSignalLe<urbanMapSpeedLim> urbanMapSpeedLim{32, 8};
  CanSignalLe<playstreetMapSpeedLim> playstreetMapSpeedLim{40, 8};
};

struct RTCInformationProprietary2_RTC : public CanMessage<0x99FF474A, 8> {
  enum class UpdatePrecondition : std::uint8_t {
    ALLPRECONDITIONFULLFILLED = 0,
    BATTERYTOOLOW = 1,
    PBRAKESTATUS = 2,
    ENGINERUNNING = 3,
    GEARBOXSTATUS = 4,
    TAKENOACTION = 255,
  };

  CanSignalLe<UpdatePrecondition> UpdatePrecondition{8, 8};
};

struct CI3_CMS : public CanMessage<0x99FF7347, 8> {
  enum class Battery24VCurrent : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  CanSignalLe<Battery24VCurrent> Battery24VCurrent{24, 16};
};

struct CruiseControlVehSpeed_K : public CanMessage<0x98FEF127, 8> {
  enum class ParkingBrakeSwitch : std::uint8_t {
    PARKINGBRAKENOTSET = 0,
    PARKINGBRAKESET = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class WheelBasedVehicleSpeed : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class CruiseCtrlActive : std::uint8_t {
    CRUISECONTROLSWITCHEDOFF = 0,
    CRUISECONTROLSWITCHEDON = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class BrakeSwitch : std::uint8_t {
    BRAKEPEDALRELEASED = 0,
    BRAKEPEDALDEPRESSED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class ClutchSwitch : std::uint8_t {
    CLUTCHPEDALRELEASED = 0,
    CLUTCHPEDALDEPRESSED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
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
    ACCELERATEOVERRIDE = 9,
    PREPROGRAMMEDSETSPEED1 = 10,
    PREPROGRAMMEDSETSPEED2 = 11,
    PREPROGRAMMEDSETSPEED3 = 12,
    PREPROGRAMMEDSETSPEED4 = 13,
    PREPROGRAMMEDSETSPEED5 = 14,
    PREPROGRAMMEDSETSPEED6 = 15,
    PREPROGRAMMEDSETSPEED7 = 16,
    PREPROGRAMMEDSETSPEED8 = 17,
    NOTAVAILABLE = 31,
  };

  CanSignalLe<ParkingBrakeSwitch> ParkingBrakeSwitch{2, 2};
  CanSignalLe<WheelBasedVehicleSpeed> WheelBasedVehicleSpeed{8, 16};
  CanSignalLe<CruiseCtrlActive> CruiseCtrlActive{24, 2};
  CanSignalLe<BrakeSwitch> BrakeSwitch{28, 2};
  CanSignalLe<ClutchSwitch> ClutchSwitch{30, 2};
  CanSignalLe<PTOState> PTOState{48, 5};
};

struct PBS1_EPB : public CanMessage<0x99FF4150, 8> {
  enum class ParkingBrakeReleaseInhibitStatus : std::uint8_t {
    NORELEASEINHIBITACTIVE = 0,
    AUTORELEASEINHIBITED = 1,
    AUTOANDMANUALRELEASEINHIBITED = 2,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class ParkingBrakeStatus : std::uint8_t {
    PARKINGBRAKENOTAPPLIED = 0,
    PARKINGBRAKEAPPLIED = 1,
    PARKINGBRAKERELEASING = 2,
    PARKINGBRAKEAPPLYING = 3,
    PARKINGBRAKEAPPLIEDANDCHARGINGTRAILER = 4,
    SECONDARYBRAKINGACTIVE = 5,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class ParkingBrakeReleaseInhibitReason : std::uint8_t {
    NOACTIVEINHIBITREASON = 0,
    NOPEDALPRESSED = 1,
    INSUFFICIENTAIRPRESSURE = 2,
    CHARGINGTRAILER = 3,
    TRAILERTESTACTIVE = 4,
    EXTERNALINHIBITREASON = 12,
    NOTAVAILABLE = 15,
  };

  CanSignalLe<ParkingBrakeReleaseInhibitStatus> ParkingBrakeReleaseInhibitStatus{10, 3};
  CanSignalLe<ParkingBrakeStatus> ParkingBrakeStatus{16, 3};
  CanSignalLe<ParkingBrakeReleaseInhibitReason> ParkingBrakeReleaseInhibitReason{24, 4};
};

struct PBC1_RTC : public CanMessage<0x8DFF404A, 8> {
  enum class ParkingBrakeReleaseInhibit : std::uint8_t {
    NOINHIBITACTIVE = 0,
    INHIBITAUTORELEASE = 1,
    INHIBITAUTOANDMANUALRELEASE = 2,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class ParkingBrakeReleaseInhibitRequestSource : std::uint8_t {
    NOACTIVEREQUESTSOURCE = 0,
    VCBBATTERYCHARGINGCABLEATTACHED = 1,
    VCBPANTOGRAPHCHARGINGACTIVE = 2,
    OTA_SW_UPDATE_ACTIVE = 3,
    TAKENOACTION = 15,
  };

  CanSignalLe<ParkingBrakeReleaseInhibit> ParkingBrakeReleaseInhibit{16, 3};
  CanSignalLe<ParkingBrakeReleaseInhibitRequestSource> ParkingBrakeReleaseInhibitRequestSource{24, 4};
  CanSignalLe<std::uint8_t> XBRMessageCounter{56, 4, 1, 0, 0, 15};
  CanSignalLe<std::uint8_t> XBRMessageChecksum{60, 4, 1, 0, 0, 15};
};

struct ExternalControlMessage_RTC : public CanMessage<0x8CFFFC4A, 8> {
  enum class EngineStartAllowed : std::uint8_t {
    ENGINESTARTNOTALLOWED = 0,
    ENGINESTARTALLOWED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<EngineStartAllowed> EngineStartAllowed{20, 2};
};

struct DLN9_K : public CanMessage<0x98FF8227, 8> {
  enum class PropulsionState : std::uint8_t {
    NOT_READY = 0,
    READY = 1,
    STARTING = 2,
    STOPPING = 3,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class ElectricDriveModeRequest : std::uint8_t {
    NOELECTRICDRIVEMODEREQUESTED = 0,
    ELECTRICDRIVEMODEREQUESTED = 1,
    BATTERYCHARGEMODEREQUESTED = 2,
    PRESERVECHARGEMODEREQUESTED = 3,
    TAKENOACTION = 7,
  };

  CanSignalLe<PropulsionState> PropulsionState{24, 3};
  CanSignalLe<ElectricDriveModeRequest> ElectricDriveModeRequest{43, 3};
};

struct ETC2_T : public CanMessage<0x98F00503, 8> {
  enum class SelectedGear : std::uint8_t {
    PARK = 251,
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class CurrentGear : std::uint8_t {
    PARK = 251,
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<SelectedGear> SelectedGear{0, 8};
  CanSignalLe<CurrentGear> CurrentGear{24, 8};
};

struct VehiclePosition_1000_RTC : public CanMessage<0x98FEF34A, 8> {
  enum class Latitude : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Longitude : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  CanSignalLe<Latitude> Latitude{0, 32};
  CanSignalLe<Longitude> Longitude{32, 32};
};

struct VehiDirSpeed_1000_RTC : public CanMessage<0x98FEE84A, 8> {
  enum class CompassBearing : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class NavigationBasedVehicleSpeed : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class Pitch : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class Altitude : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  CanSignalLe<CompassBearing> CompassBearing{0, 16};
  CanSignalLe<NavigationBasedVehicleSpeed> NavigationBasedVehicleSpeed{16, 16};
  CanSignalLe<Pitch> Pitch{32, 16};
  CanSignalLe<Altitude> Altitude{48, 16};
};

struct TI_T : public CanMessage<0x98FF6E03, 8> {
  enum class EcoRollActive : std::uint8_t {
    DISABLED = 0,
    ENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class GearboxInReverse : std::uint8_t {
    GEARBOXNOTINREVERSE = 0,
    GEARBOXINREVERSE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<EcoRollActive> EcoRollActive{6, 2};
  CanSignalLe<GearboxInReverse> GearboxInReverse{50, 2};
};

struct SecondaryWakeUp_HMS : public CanMessage<0x8CFF04EF, 8> {
  enum class WakeUp_RTC : std::uint8_t {
    NOWAKEUP = 0,
    WAKEUP = 1,
    TAKENOACTION = 3,
  };

  CanSignalLe<WakeUp_RTC> WakeUp_RTC{22, 2};
};

struct TelltaleStatus_ICL : public CanMessage<0x98FD7D17, 8> {
  enum class TelltaleBlockID : std::uint8_t {
    DONTCARE = 15,
  };

  enum class TelltaleStatus1 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus2 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus3 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus4 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus5 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus6 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus7 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus8 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus9 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus10 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus11 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus12 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus13 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus14 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  enum class TelltaleStatus15 : std::uint8_t {
    OFF = 0,
    CONDITION_RED = 1,
    CONDITION_YELLOW = 2,
    CONDITION_INFO = 3,
    NOTAVALIABLE = 7,
  };

  CanSignalLe<TelltaleBlockID> TelltaleBlockID{0, 4};
  CanSignalLe<TelltaleStatus1> TelltaleStatus1{4, 3};
  CanSignalLe<TelltaleStatus2> TelltaleStatus2{8, 3};
  CanSignalLe<TelltaleStatus3> TelltaleStatus3{12, 3};
  CanSignalLe<TelltaleStatus4> TelltaleStatus4{16, 3};
  CanSignalLe<TelltaleStatus5> TelltaleStatus5{20, 3};
  CanSignalLe<TelltaleStatus6> TelltaleStatus6{24, 3};
  CanSignalLe<TelltaleStatus7> TelltaleStatus7{28, 3};
  CanSignalLe<TelltaleStatus8> TelltaleStatus8{32, 3};
  CanSignalLe<TelltaleStatus9> TelltaleStatus9{36, 3};
  CanSignalLe<TelltaleStatus10> TelltaleStatus10{40, 3};
  CanSignalLe<TelltaleStatus11> TelltaleStatus11{44, 3};
  CanSignalLe<TelltaleStatus12> TelltaleStatus12{48, 3};
  CanSignalLe<TelltaleStatus13> TelltaleStatus13{52, 3};
  CanSignalLe<TelltaleStatus14> TelltaleStatus14{56, 3};
  CanSignalLe<TelltaleStatus15> TelltaleStatus15{60, 3};
};

struct GPM3_K : public CanMessage<0x98FF6B27, 8> {
  enum class LowEngineCoolantWaterLevel : std::uint8_t {
    NOTLOWENGINECOOLANTWATERLEVEL = 0,
    LOWENGINECOOLANTWATERLEVEL = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class VehicleSpeedImplausible : std::uint8_t {
    SPEEDPLAUSIBLE = 0,
    SPEEDIMPLAUSIBE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SeatBeltReminder : std::uint8_t {
    NOLEVEL = 0,
    STLEVEL = 1,
    NDLEVEL = 2,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class TotalPropulsionStateReadyHours : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  CanSignalLe<LowEngineCoolantWaterLevel> LowEngineCoolantWaterLevel{0, 2};
  CanSignalLe<VehicleSpeedImplausible> VehicleSpeedImplausible{2, 2};
  CanSignalLe<SeatBeltReminder> SeatBeltReminder{27, 3};
  CanSignalLe<TotalPropulsionStateReadyHours> TotalPropulsionStateReadyHours{32, 32};
};

struct ServiceInformationProp_K : public CanMessage<0x98FFAE27, 8> {
  enum class ServiceDistanceProp : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class ServiceTime : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class ServiceDistanceWarning : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    NOTAVAILABLE = 3,
  };

  enum class OperationalTimeWarning : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<ServiceDistanceProp> ServiceDistanceProp{8, 16};
  CanSignalLe<ServiceTime> ServiceTime{24, 16};
  CanSignalLe<ServiceDistanceWarning> ServiceDistanceWarning{40, 2};
  CanSignalLe<OperationalTimeWarning> OperationalTimeWarning{42, 2};
};

struct AlarmStatusProp_ALM : public CanMessage<0x98FFBE1D, 8> {
  enum class AlarmOnOffStatus : std::uint8_t {
    ALARMUNSET = 0,
    ALARMSET = 1,
    ALARMSETWITHERROR = 13,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class AlarmModeStatus : std::uint8_t {
    UNARMEDMODE = 0,
    STANDARDMODE = 1,
    PERIMETERMODE = 2,
    TRANSPORTMODE = 3,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class AlarmSystemActivationReason : std::uint8_t {
    NOACTIVATION = 0,
    MAINPOWERSUPPLY = 2,
    IGNITION = 4,
    PANICALARM = 10,
    DRIVERDOOR = 16,
    PASSENGERDOOR = 17,
    DOORBEHINDDRIVER = 18,
    DOORBEHINDPASSENGER = 19,
    CARGODOOR = 24,
    STORAGEBOXDRIVERSIDEUPPER = 33,
    STORAGEBOXDRIVERSIDELOWER = 34,
    STORAGEBOXPASSENGERSIDEUPPER = 35,
    STORAGEBOXPASSENGERSIDELOWER = 36,
    RESANALOGUE = 48,
    FRONTGRILLE = 64,
    ROOFHATCH = 68,
    EXTRA1 = 80,
    EXTRA2 = 81,
    EXTRA3 = 82,
    SIREN = 96,
    BATTERYBACKUP = 98,
    MOTION = 112,
    INCLINATION = 114,
    FTATHEFTDETECTED = 128,
    FTASENSORDAMAGED = 129,
    SWITCH_A1 = 161,
    SWITCH_A2 = 162,
    SWITCH_A3 = 163,
    SWITCH_A4 = 164,
    SWITCH_A5 = 165,
    SWITCH_A6 = 166,
    SWITCH_A7 = 167,
    SWITCH_A8 = 168,
    SWITCH_A9 = 169,
    SWITCH_A10 = 170,
    SWITCH_B1 = 177,
    SWITCH_B2 = 178,
    SWITCH_E1 = 225,
    SWITCH_E2 = 226,
    SWITCH_E3 = 227,
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class SensorIgnition : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PanicAlarmStatus : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorDriverDoor : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorPassengerDoor : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorDoorBehindDriver : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorDoorBehindPassenger : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorCargoDoor : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorStorageBoxDriverSideUpper : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorStorageBoxDriverSideLower : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorStorageBoxPassengerSideUpper : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorStorageBoxPassengerSideLower : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorResAnalogue : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorFrontGrille : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorRoofHatch : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorExtra1 : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorExtra2 : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorExtra3 : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorSiren : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorFTAStatus : std::uint8_t {
    NOTDAMAGED = 0,
    DAMAGED = 1,
    ERROR = 2,
    NA = 3,
  };

  enum class SensorMotion : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorInclination : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SensorFTATheft : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<AlarmOnOffStatus> AlarmOnOffStatus{0, 4};
  CanSignalLe<AlarmModeStatus> AlarmModeStatus{4, 4};
  CanSignalLe<AlarmSystemActivationReason> AlarmSystemActivationReason{8, 8};
  CanSignalLe<SensorIgnition> SensorIgnition{16, 2};
  CanSignalLe<PanicAlarmStatus> PanicAlarmStatus{18, 2};
  CanSignalLe<SensorDriverDoor> SensorDriverDoor{20, 2};
  CanSignalLe<SensorPassengerDoor> SensorPassengerDoor{22, 2};
  CanSignalLe<SensorDoorBehindDriver> SensorDoorBehindDriver{24, 2};
  CanSignalLe<SensorDoorBehindPassenger> SensorDoorBehindPassenger{26, 2};
  CanSignalLe<SensorCargoDoor> SensorCargoDoor{28, 2};
  CanSignalLe<SensorStorageBoxDriverSideUpper> SensorStorageBoxDriverSideUpper{30, 2};
  CanSignalLe<SensorStorageBoxDriverSideLower> SensorStorageBoxDriverSideLower{32, 2};
  CanSignalLe<SensorStorageBoxPassengerSideUpper> SensorStorageBoxPassengerSideUpper{34, 2};
  CanSignalLe<SensorStorageBoxPassengerSideLower> SensorStorageBoxPassengerSideLower{36, 2};
  CanSignalLe<SensorResAnalogue> SensorResAnalogue{38, 2};
  CanSignalLe<SensorFrontGrille> SensorFrontGrille{40, 2};
  CanSignalLe<SensorRoofHatch> SensorRoofHatch{42, 2};
  CanSignalLe<SensorExtra1> SensorExtra1{44, 2};
  CanSignalLe<SensorExtra2> SensorExtra2{46, 2};
  CanSignalLe<SensorExtra3> SensorExtra3{48, 2};
  CanSignalLe<SensorSiren> SensorSiren{50, 2};
  CanSignalLe<SensorFTAStatus> SensorFTAStatus{52, 2};
  CanSignalLe<SensorMotion> SensorMotion{54, 2};
  CanSignalLe<SensorInclination> SensorInclination{56, 2};
  CanSignalLe<SensorFTATheft> SensorFTATheft{62, 2};
};

struct PowertrainControl2_K : public CanMessage<0x8CFF7427, 8> {
  enum class AcceleratorPedalPosition : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<AcceleratorPedalPosition> AcceleratorPedalPosition{24, 8};
};

struct AccumulatedTripDataParams_K : public CanMessage<0x99FF2327, 8> {
  enum class AccumulatedTripDataParams : std::uint8_t {
    TIME_TOTALACCUMULATED = 0,
    TIME_IDLE = 1,
    TIME_PTO_IN_IDLE = 2,
    TIME_PTO_IN_DRIVE = 3,
    TIME_FORCEDELECTRICMODE = 5,
    TIME_FORCEDCHARGINGMODE = 6,
    TIME_IDLSTOPSTARTDEACTIVATED = 8,
    DISTANCE_TOTALACCUMULATED = 16,
    DISTANCE_PTO_IN_DRIVE = 19,
    DISTANCE_FORCEDELECTRICMODE = 21,
    DISTANCE_FORCEDCHARGINGMODE = 22,
    DISTANCE_IDLSTOPSTARTDEACTIVATED = 24,
    FUELLIQUID_TOTALACCUMULATED = 32,
    FUELLIQUID_IDLE = 33,
    FUELLIQUID_PTO_IN_IDLE = 34,
    FUELLIQUID_PTO_IN_DRIVE = 35,
    FUELGASEOUS_TOTALACCUMULATED = 48,
    FUELGASEOUS_IDLE = 49,
    FUELGASEOUS_PTO_IN_IDLE = 50,
    FUELGASEOUS_PTO_IN_DRIVE = 51,
    ENERGYUSED_TOTALACCUMULATED = 64,
    ENERGYUSED_IDLE = 65,
    ENERGYUSED_PTO_IN_IDLE = 66,
    ENERGYUSED_PTO_IN_DRIVE = 67,
    TIMEINPAUSE_TOTALACCUMULATED = 80,
    TIMEINPAUSE_IDLE = 81,
    TIMEINPAUSE_PTO_IN_IDLE = 82,
    TIMEINPAUSE_PTO_IN_DRIVE = 83,
    DISTANCEINPAUSE_TOTALACCUMULATED = 96,
    DISTANCEINPAUSE_PTO_IN_DRIVE = 99,
    TAKENOACTION = 255,
  };

  enum class Distance_ForcedChargingMode_0x16 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Distance_ForcedElectricMode_0x15 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Distance_IdlStopStartDeactivated_0x18 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Distance_PTO_In_Drive_0x13 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Distance_TotalAccumulated_0x10 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class DistanceInPause_PTO_In_Drive_0x63 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class DistanceInPause_TotalAccumulated_0x60 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class EnergyUsed_Idle_0x41 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class EnergyUsed_PTO_In_Drive_0x43 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class EnergyUsed_PTO_In_Idle_0x42 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class EnergyUsed_TotalAccumulated_0x40 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelGaseous_Idle_0x31 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelGaseous_PTO_In_Drive_0x33 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelGaseous_PTO_In_Idle_0x32 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelGaseous_TotalAccumulated_0x30 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelLiquid_Idle_0x21 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelLiquid_PTO_In_Drive_0x23 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelLiquid_PTO_In_Idle_0x22 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class FuelLiquid_TotalAccumulated_0x20 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Time_ForcedChargingMode_0x06 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Time_ForcedElectricMode_0x05 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Time_Idle_0x01 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Time_IdlStopStartDeactivated_0x08 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Time_PTO_In_Drive_0x03 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Time_PTO_In_Idle_0x02 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class Time_TotalAccumulated_0x00 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class TimeInPause_Idle_0x51 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class TimeInPause_PTO_In_Drive_0x53 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class TimeInPause_PTO_In_Idle_0x52 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  enum class TimeInPause_TotalAccumulated_0x50 : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  CanSignalLe<AccumulatedTripDataParams> AccumulatedTripDataParams{0, 8};
  CanSignalLe<Distance_ForcedChargingMode_0x16> Distance_ForcedChargingMode_0x16{8, 32};
  CanSignalLe<Distance_ForcedElectricMode_0x15> Distance_ForcedElectricMode_0x15{8, 32};
  CanSignalLe<Distance_IdlStopStartDeactivated_0x18> Distance_IdlStopStartDeactivated_0x18{8, 32};
  CanSignalLe<Distance_PTO_In_Drive_0x13> Distance_PTO_In_Drive_0x13{8, 32};
  CanSignalLe<Distance_TotalAccumulated_0x10> Distance_TotalAccumulated_0x10{8, 32};
  CanSignalLe<DistanceInPause_PTO_In_Drive_0x63> DistanceInPause_PTO_In_Drive_0x63{8, 32};
  CanSignalLe<DistanceInPause_TotalAccumulated_0x60> DistanceInPause_TotalAccumulated_0x60{8, 32};
  CanSignalLe<EnergyUsed_Idle_0x41> EnergyUsed_Idle_0x41{8, 32};
  CanSignalLe<EnergyUsed_PTO_In_Drive_0x43> EnergyUsed_PTO_In_Drive_0x43{8, 32};
  CanSignalLe<EnergyUsed_PTO_In_Idle_0x42> EnergyUsed_PTO_In_Idle_0x42{8, 32};
  CanSignalLe<EnergyUsed_TotalAccumulated_0x40> EnergyUsed_TotalAccumulated_0x40{8, 32};
  CanSignalLe<FuelGaseous_Idle_0x31> FuelGaseous_Idle_0x31{8, 32};
  CanSignalLe<FuelGaseous_PTO_In_Drive_0x33> FuelGaseous_PTO_In_Drive_0x33{8, 32};
  CanSignalLe<FuelGaseous_PTO_In_Idle_0x32> FuelGaseous_PTO_In_Idle_0x32{8, 32};
  CanSignalLe<FuelGaseous_TotalAccumulated_0x30> FuelGaseous_TotalAccumulated_0x30{8, 32};
  CanSignalLe<FuelLiquid_Idle_0x21> FuelLiquid_Idle_0x21{8, 32};
  CanSignalLe<FuelLiquid_PTO_In_Drive_0x23> FuelLiquid_PTO_In_Drive_0x23{8, 32};
  CanSignalLe<FuelLiquid_PTO_In_Idle_0x22> FuelLiquid_PTO_In_Idle_0x22{8, 32};
  CanSignalLe<FuelLiquid_TotalAccumulated_0x20> FuelLiquid_TotalAccumulated_0x20{8, 32};
  CanSignalLe<Time_ForcedChargingMode_0x06> Time_ForcedChargingMode_0x06{8, 32};
  CanSignalLe<Time_ForcedElectricMode_0x05> Time_ForcedElectricMode_0x05{8, 32};
  CanSignalLe<Time_Idle_0x01> Time_Idle_0x01{8, 32};
  CanSignalLe<Time_IdlStopStartDeactivated_0x08> Time_IdlStopStartDeactivated_0x08{8, 32};
  CanSignalLe<Time_PTO_In_Drive_0x03> Time_PTO_In_Drive_0x03{8, 32};
  CanSignalLe<Time_PTO_In_Idle_0x02> Time_PTO_In_Idle_0x02{8, 32};
  CanSignalLe<Time_TotalAccumulated_0x00> Time_TotalAccumulated_0x00{8, 32};
  CanSignalLe<TimeInPause_Idle_0x51> TimeInPause_Idle_0x51{8, 32};
  CanSignalLe<TimeInPause_PTO_In_Drive_0x53> TimeInPause_PTO_In_Drive_0x53{8, 32};
  CanSignalLe<TimeInPause_PTO_In_Idle_0x52> TimeInPause_PTO_In_Idle_0x52{8, 32};
  CanSignalLe<TimeInPause_TotalAccumulated_0x50> TimeInPause_TotalAccumulated_0x50{8, 32};
};

struct AmbientConditions_K : public CanMessage<0x98FEF527, 8> {
  enum class AmbientAirTemperature : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<AmbientAirTemperature> AmbientAirTemperature{24, 16};
};

struct GPM10_K : public CanMessage<0x99FF3C27, 8> {
  enum class AuxiliaryAmbientAirTemperature : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<AuxiliaryAmbientAirTemperature> AuxiliaryAmbientAirTemperature{16, 16};
};

struct DI_OnChange_K_FF : public CanMessage<0x98FE6B27, 9> {
  CanSignalLe<std::int8_t> Driver1Identifier{0, 8, 1, 0, 0, 0};  // ASCII
  CanSignalLe<std::int8_t> Delimiter1{8, 8, 1, 0, 0, 0};  // ASCII '*'
  CanSignalLe<std::int8_t> Driver2Identifier{16, 8, 1, 0, 0, 0};  // ASCII
  CanSignalLe<std::int8_t> Delimiter2{24, 8, 1, 0, 0, 0};  // ASCII '*'
};

struct TP_CM_K_FF : public CanMessage<0x98ECFF27, 8> {
  enum class ControlByteTP_CM : std::uint8_t {
    DESTINATION_SPECIFIC_REQUEST_TO_SEND_RTS = 16,
    DESTINATION_SPECIFIC_CLEAR_TO_SEND_CTS = 17,
    END_OF_MESSAGE_ACKNOWLEDGE_END = 19,
    BROADCASTANNOUNCEMESSAGE_BAM = 32,
    CONNECTIONABORT_ABO = 255,
  };

  CanSignalLe<ControlByteTP_CM> ControlByteTP_CM{0, 8};
  CanSignalLe<std::uint16_t> TPCM_BAM_TotalMessageSize{8, 16, 1, 0, 0, 64255};  // byte
  CanSignalLe<std::uint8_t> TPCM_BAM_TotalNumberOfPackets{24, 8, 1, 0, 0, 255};  // counts
  CanSignalLe<std::uint32_t> TPCM_BAM_PGNumber{40, 24, 1, 0, 0, 16777215};  // PGN
};

struct TP_DT_K_FF : public CanMessage<0x98EBFF27, 8> {
  CanSignalLe<std::uint8_t> SequenceNumber{0, 8, 1, 0, 0, 255};  // count
  CanSignalLe<std::uint8_t> Byte1{8, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte2{16, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte3{24, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte4{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte5{40, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte6{48, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte7{56, 8, 1, 0, 0, 255};
};

struct PTOInformationProp_K : public CanMessage<0x98FF9027, 8> {
  enum class PTO_AL1 : std::uint8_t {
    NOTENGAGED = 0,
    ENGAGED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PTO_AL2 : std::uint8_t {
    NOTENGAGED = 0,
    ENGAGED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PTO_EG3 : std::uint8_t {
    NOTENGAGED = 0,
    ENGAGED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PTO_Engaged : std::uint8_t {
    PTONOTENGAGED = 0,
    PTOENGAGED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<PTO_AL1> PTO_AL1{4, 2};
  CanSignalLe<PTO_AL2> PTO_AL2{6, 2};
  CanSignalLe<PTO_EG3> PTO_EG3{20, 2};
  CanSignalLe<PTO_Engaged> PTO_Engaged{48, 2};
};

struct DriverEvaluationData_K : public CanMessage<0x9CFF0627, 8> {
  enum class DriverEvaluationCategory1 : std::uint8_t {
    NOEVENT = 0,
    HILL = 1,
    WEAR = 2,
    ANTICIPATION = 3,
    GEARSELECTION = 4,
    HYBRIDBRAKE = 5,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class DriverEvaluationCategory2 : std::uint8_t {
    NOEVENT = 0,
    HILL = 1,
    WEAR = 2,
    ANTICIPATION = 3,
    GEARSELECTION = 4,
    HYBRIDBRAKE = 5,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class DriverEvaluationCategory3 : std::uint8_t {
    NOEVENT = 0,
    HILL = 1,
    WEAR = 2,
    ANTICIPATION = 3,
    GEARSELECTION = 4,
    HYBRIDBRAKE = 5,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class DriverEvaluationCategory4 : std::uint8_t {
    NOEVENT = 0,
    HILL = 1,
    WEAR = 2,
    ANTICIPATION = 3,
    GEARSELECTION = 4,
    HYBRIDBRAKE = 5,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class EvaluationActiveCategory1 : std::uint8_t {
    EVALUATIONNOTACTIVE = 0,
    EVALUATIONACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EvaluationActiveCategory2 : std::uint8_t {
    EVALUATIONNOTACTIVE = 0,
    EVALUATIONACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EvaluationActiveCategory3 : std::uint8_t {
    EVALUATIONNOTACTIVE = 0,
    EVALUATIONACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EvaluationActiveCategory4 : std::uint8_t {
    EVALUATIONNOTACTIVE = 0,
    EVALUATIONACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<DriverEvaluationCategory1> DriverEvaluationCategory1{8, 4};
  CanSignalLe<DriverEvaluationCategory2> DriverEvaluationCategory2{12, 4};
  CanSignalLe<DriverEvaluationCategory3> DriverEvaluationCategory3{16, 4};
  CanSignalLe<DriverEvaluationCategory4> DriverEvaluationCategory4{20, 4};
  CanSignalLe<EvaluationActiveCategory1> EvaluationActiveCategory1{56, 2};
  CanSignalLe<EvaluationActiveCategory2> EvaluationActiveCategory2{58, 2};
  CanSignalLe<EvaluationActiveCategory3> EvaluationActiveCategory3{60, 2};
  CanSignalLe<EvaluationActiveCategory4> EvaluationActiveCategory4{62, 2};
};

struct DriverEvaluationEvents_K : public CanMessage<0x9CFF1327, 8> {
  enum class InstEventWeight_Cat1 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class InstEventWeight_Cat2 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class InstEventWeight_Cat3 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class InstEventWeight_Cat4 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class InstEventGrade_Cat1 : std::uint8_t {
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class InstEventGrade_Cat2 : std::uint8_t {
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class InstEventGrade_Cat3 : std::uint8_t {
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class InstEventGrade_Cat4 : std::uint8_t {
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class DriverEvaluationVersionNumber : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<InstEventWeight_Cat1> InstEventWeight_Cat1{0, 8};
  CanSignalLe<InstEventWeight_Cat2> InstEventWeight_Cat2{8, 8};
  CanSignalLe<InstEventWeight_Cat3> InstEventWeight_Cat3{16, 8};
  CanSignalLe<InstEventWeight_Cat4> InstEventWeight_Cat4{24, 8};
  CanSignalLe<InstEventGrade_Cat1> InstEventGrade_Cat1{32, 4};
  CanSignalLe<InstEventGrade_Cat2> InstEventGrade_Cat2{36, 4};
  CanSignalLe<InstEventGrade_Cat3> InstEventGrade_Cat3{40, 4};
  CanSignalLe<InstEventGrade_Cat4> InstEventGrade_Cat4{44, 4};
  CanSignalLe<DriverEvaluationVersionNumber> DriverEvaluationVersionNumber{48, 8};
};

struct TP_CM_K_RTC : public CanMessage<0x98EC4A27, 8> {
  enum class ControlByteTP_CM : std::uint8_t {
    DESTINATION_SPECIFIC_REQUEST_TO_SEND_RTS = 16,
    DESTINATION_SPECIFIC_CLEAR_TO_SEND_CTS = 17,
    END_OF_MESSAGE_ACKNOWLEDGE_END = 19,
    BROADCASTANNOUNCEMESSAGE_BAM = 32,
    CONNECTIONABORT_ABO = 255,
  };

  CanSignalLe<ControlByteTP_CM> ControlByteTP_CM{0, 8};
  CanSignalLe<std::uint16_t> TPCM_RTS_TotalMessageSize{8, 16, 1, 0, 0, 64255};  // byte
  CanSignalLe<std::uint8_t> TPCM_BAM_TotalNumberOfPackets{24, 8, 1, 0, 0, 255};  // counts
  CanSignalLe<std::uint32_t> TPCM_BAM_PGNumber{40, 24, 1, 0, 0, 16777215};  // PGN
};

struct TP_DT_K_RTC : public CanMessage<0x98EB4A27, 8> {
  CanSignalLe<std::uint8_t> SequenceNumber{0, 8, 1, 0, 0, 255};  // count
  CanSignalLe<std::uint8_t> Byte1{8, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte2{16, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte3{24, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte4{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte5{40, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte6{48, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte7{56, 8, 1, 0, 0, 255};
};

struct VehicleId_K_RTC : public CanMessage<0x98FEEC27, 9> {
  CanSignalLe<std::int8_t> VehicleIdentification{0, 8, 1, 0, 0, 0};  // ASCII
  CanSignalLe<std::int8_t> Delimiter{8, 8, 1, 0, 0, 0};  // ASCII '*'
};

struct CUVInformation_V : public CanMessage<0x98FFB11E, 8> {
  enum class HeadLampFailure : std::uint8_t {
    NOHEADLAMPFAILURE = 0,
    HEADLAMPFAILURE = 1,
    TAKENOACTION = 3,
  };

  enum class HazardWarningSignalStatus : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class TrailerDetection : std::uint8_t {
    TRAILERNOTDETECTED = 0,
    TRAILERDETECTED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<HeadLampFailure> HeadLampFailure{18, 2};
  CanSignalLe<HazardWarningSignalStatus> HazardWarningSignalStatus{38, 2};
  CanSignalLe<TrailerDetection> TrailerDetection{54, 2};
};

struct ICLInformationProprietary_ICL : public CanMessage<0x98FFD517, 8> {
  enum class PerformanceMode : std::uint8_t {
    STANDARDMODE = 0,
    ECONOMYMODE = 1,
    POWERMODE = 2,
    OFFROADMODE = 3,
    INDIVIDUALMODE1 = 12,
    INDIVIDUALMODE2 = 13,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  CanSignalLe<PerformanceMode> PerformanceMode{42, 4};
};

struct ReductantDTEInformation_E : public CanMessage<0x99FF8400, 8> {
  enum class TotalReductantUsed : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  CanSignalLe<TotalReductantUsed> TotalReductantUsed{32, 32};
};

struct TCO1_TCO : public CanMessage<0x8CFE6CEE, 8> {
  enum class Driver1WorkingState : std::uint8_t {
    REST = 0,
    AVAILABILITY = 1,
    WORK = 2,
    DRIVE = 3,
    RESERVED = 4,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class Driver2WorkingState : std::uint8_t {
    REST = 0,
    AVAILABILITY = 1,
    WORK = 2,
    DRIVE = 3,
    RESERVED = 4,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class VehicleMotion : std::uint8_t {
    VEHICLENOTINMOTION = 0,
    VEHICLEINMOTION = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Driver1TimeRelatedStates : std::uint8_t {
    NORMAL_NOLIMITSREACHED = 0,
    LIMIT1_15MINBEFORE4_5H = 1,
    LIMIT2_4_5HREACHED = 2,
    LIMIT3_15MINBEFORE9H = 3,
    LIMIT4_9HREACHED = 4,
    LIMIT5_15MINBEFORE16H = 5,
    LIMIT6_16HREACHED = 6,
    WEEKLYDRIVINGTIMEPREWARNINGACTIVE = 7,
    WEEKLYDRIVINGTIMEWARNINGACTIVE = 8,
    WEEKSDRIVINGTIMEPREWARNINGACTIVE = 9,
    WEEKSDRIVINGTIMEWARNINGACTIVE = 10,
    DRIVER1CARDEXPIRYWARNINGACTIVE = 11,
    NEXTMANDATORYDRIVER1CARDDOWNLOADWARNINGACTIVE = 12,
    OTHER = 13,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class DriverCardDriver1 : std::uint8_t {
    DRIVERCARDNOTPRESENT = 0,
    DRIVERCARDPRESENT = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Overspeed : std::uint8_t {
    NOOVERSPEED = 0,
    OVERSPEED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Driver2TimeRelatedStates : std::uint8_t {
    NORMAL_NOLIMITSREACHED = 0,
    LIMIT1_15MINBEFORE4_5H = 1,
    LIMIT2_4_5HREACHED = 2,
    LIMIT3_15MINBEFORE9H = 3,
    LIMIT4_9HREACHED = 4,
    LIMIT5_15MINBEFORE16H = 5,
    LIMIT6_16HREACHED = 6,
    RESERVED = 7,
    OTHER = 13,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class DriverCardDriver2 : std::uint8_t {
    DRIVERCARDNOTPRESENT = 0,
    DRIVERCARDPRESENT = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SystemEvent : std::uint8_t {
    NOSYSTEMEVENT = 0,
    SYSTEMEVENT = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class HandlingInformation : std::uint8_t {
    NOHANDLINGINFORMATION = 0,
    HANDLINGINFORMATION = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class SystemPerformance : std::uint8_t {
    NOSYSTEMPERFORMANCE = 0,
    SYSTEMPERFORMANCE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DirectionIndicator : std::uint8_t {
    FORWARD = 0,
    REVERSE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class TCOVehSpeed : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<Driver1WorkingState> Driver1WorkingState{0, 3};
  CanSignalLe<Driver2WorkingState> Driver2WorkingState{3, 3};
  CanSignalLe<VehicleMotion> VehicleMotion{6, 2};
  CanSignalLe<Driver1TimeRelatedStates> Driver1TimeRelatedStates{8, 4};
  CanSignalLe<DriverCardDriver1> DriverCardDriver1{12, 2};
  CanSignalLe<Overspeed> Overspeed{14, 2};
  CanSignalLe<Driver2TimeRelatedStates> Driver2TimeRelatedStates{16, 4};
  CanSignalLe<DriverCardDriver2> DriverCardDriver2{20, 2};
  CanSignalLe<SystemEvent> SystemEvent{24, 2};
  CanSignalLe<HandlingInformation> HandlingInformation{26, 2};
  CanSignalLe<SystemPerformance> SystemPerformance{28, 2};
  CanSignalLe<DirectionIndicator> DirectionIndicator{30, 2};
  CanSignalLe<TCOVehSpeed> TCOVehSpeed{48, 16};
};

struct HiResVehicleDist_TCO : public CanMessage<0x98FEC1EE, 8> {
  enum class HighResolTotalVehicleDistance : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4278190080,
  };

  CanSignalLe<HighResolTotalVehicleDistance> HighResolTotalVehicleDistance{0, 32};
};

struct BSS2_A : public CanMessage<0x98FF400B, 8> {
  enum class BrakeLiningWearWarning : std::uint8_t {
    LININGWEARWARNINGNOTACTIVE = 0,
    LININGWEARWARNINGACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<BrakeLiningWearWarning> BrakeLiningWearWarning{10, 2};
};

struct EngineInfoProprietary2_E : public CanMessage<0x98FF5F00, 8> {
  enum class UreaLevel : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class TorqueLimit : std::uint8_t {
    NOTORQUELIMIT = 0,
    ACTIVETORQUELIMITDUETOEMISSION = 1,
    PENDINGTORQUELIMITDUETOEMISSION = 2,
    ACTIVETORQUELIMITDUETOHIGHGBXTEMP = 3,
    ACTIVETORQUELIMITDUETOHIGHEXHAUSTTEMP = 4,
    ACTIVETORQUELIMITGENERAL = 5,
    DONTCARE_TAKENOACTION = 7,
  };

  enum class EngineAirFilterStatus : std::uint8_t {
    AIRFILTEROK = 0,
    AIRFILTERREDUCEDFUNCTIONALITY = 1,
    AIRFILTERCLOGGED = 2,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<UreaLevel> UreaLevel{0, 8};
  CanSignalLe<TorqueLimit> TorqueLimit{11, 3};
  CanSignalLe<EngineAirFilterStatus> EngineAirFilterStatus{48, 3};
};

struct EEC2_E : public CanMessage<0x8CF00300, 8> {
  enum class AcceleratorPedalPosition : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class PercentLoadAtCurrentSpeed : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<AcceleratorPedalPosition> AcceleratorPedalPosition{8, 8};
  CanSignalLe<PercentLoadAtCurrentSpeed> PercentLoadAtCurrentSpeed{16, 8};
};

struct HRLFC_E : public CanMessage<0x98FD0900, 8> {
  enum class HighResEngineTotalFuelUsed : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4294967295,
  };

  CanSignalLe<HighResEngineTotalFuelUsed> HighResEngineTotalFuelUsed{32, 32};
};

struct OBDInformation2_E : public CanMessage<0x99FF4600, 8> {
  enum class DegradationInformation : std::uint32_t {
    TAKENOACTION = 4294967295,
  };

  CanSignalLe<DegradationInformation> DegradationInformation{0, 32};
};

struct PowertrainInformation3_E : public CanMessage<0x98FF7C00, 8> {
  enum class LowEngineOilPressure : std::uint8_t {
    NOTLOWENGINEOILPRESSURE = 0,
    LOWENGINEOILPRESSURE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class HighEngineCoolantTemp : std::uint8_t {
    NOTHIGHENGINECOOLANTTEMP = 0,
    HIGHENGINECOOLANTTEMP = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DPFRegenerationState : std::uint8_t {
    ACTIVATIONNOTNEEDEDALERT = 0,
    MANUALACTIVATIONINHIBITED = 1,
    MANUALREGENERATIONINITIALIZATION = 2,
    MANUALREGENERATIONACTIVE = 3,
    MANUALREGENERATIONCOMPLETED = 4,
    MANUALREGENERATIONABORTED = 5,
    AUTOMATICREGENERATIONINHIBITED = 6,
    AUTOMATICREGENERATIONINITIALIZATION = 7,
    AUTOMATICREGENERATIONACTIVE = 8,
    AUTOMATICREGENERATIONCOMPLETED = 9,
    AUTOMATICREGENERATIONABORTED = 10,
    DONTCARE = 15,
  };

  CanSignalLe<LowEngineOilPressure> LowEngineOilPressure{20, 2};
  CanSignalLe<HighEngineCoolantTemp> HighEngineCoolantTemp{24, 2};
  CanSignalLe<DPFRegenerationState> DPFRegenerationState{44, 4};
};

struct EEC1_E : public CanMessage<0x8CF00400, 8> {
  enum class ActualEngine_PercTorque : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class EngineSpeed : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<ActualEngine_PercTorque> ActualEngine_PercTorque{16, 8};
  CanSignalLe<EngineSpeed> EngineSpeed{24, 16};
};

struct FuelConsumption_E : public CanMessage<0x98FEE900, 8> {
  enum class TotalFuelUsed : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4278190080,
  };

  CanSignalLe<TotalFuelUsed> TotalFuelUsed{32, 32};
};

struct FuelEconomy_E : public CanMessage<0x98FEF200, 8> {
  enum class FuelRate : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class InstantaneousFuelEconomy : std::uint16_t {
    INFINITE_E_G_DURING_ENGINE_MOTORING_NOTE_SCANIA_DEVIATION_FROM_J1939 = 64255,
    INFINITE_E_G_DURING_ENGINE_MOTORING = 64256,
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<FuelRate> FuelRate{0, 16};
  CanSignalLe<InstantaneousFuelEconomy> InstantaneousFuelEconomy{16, 16};
};

struct DM1_E : public CanMessage<0x98FECA00, 8> {
  enum class RedStopLampStatus : std::uint8_t {
    LAMPOFF = 0,
    LAMPON = 1,
    TAKENOACTION = 3,
  };

  CanSignalLe<RedStopLampStatus> RedStopLampStatus{4, 2};
};

struct GPM1_E : public CanMessage<0x98FF6000, 8> {
  enum class CalculatedGTW : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<CalculatedGTW> CalculatedGTW{48, 16};
};

struct DPFC1_E : public CanMessage<0x98FD7C00, 8> {
  enum class DpfStatus : std::uint8_t {
    REGENERATION_NOT_NEEDED = 0,
    REGENERATION_NEEDED_LOWEST_LEVEL = 1,
    REGENERATION_NEEDED_MODERATE_LEVEL = 2,
    REGENERATION_NEEDED_HIGHEST_LEVEL = 3,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<DpfStatus> DpfStatus{12, 3};
};

struct PowertrainInformation1_E : public CanMessage<0x98FF7A00, 8> {
  enum class ManualRegenState : std::uint8_t {
    NO_ONGOING_MANUAL_REGENERATION = 0,
    MANUAL_ACTIVATION_INHIBITED = 1,
    MANUAL_REGENERATION_ONGOING = 2,
    MANUAL_REGENERATION_COMPLETE = 3,
    MANUAL_REGENERATION_ABORTED = 4,
    NOT_AVAILABLE = 7,
  };

  enum class RegenNeedLevel : std::uint8_t {
    REGENERATION_NOT_NEEDED = 0,
    REGENERATION_NEED_LEVEL_LOW = 1,
    REGENERATION_NEEDED_LEVEL_MODERATE = 2,
    REGENERATION_NEEDED_LEVEL_SEMI_HIGH = 3,
    REGENERATION_NEEDED_LEVEL_HIGHEST = 4,
    NOT_AVAILABLE = 7,
  };

  CanSignalLe<ManualRegenState> ManualRegenState{37, 3};
  CanSignalLe<RegenNeedLevel> RegenNeedLevel{60, 3};
};

struct EBC2_A : public CanMessage<0x98FEBF0B, 8> {
  enum class FrontAxleSpeed : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  CanSignalLe<FrontAxleSpeed> FrontAxleSpeed{0, 16};
  CanSignalLe<std::uint8_t> RelSpeedRearAxle_1LeftWheel{32, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
  CanSignalLe<std::uint8_t> RelSpeedRearAxle_1RightWheel{40, 8, 0.0625, -7.8125, -7.8125, 7.8125};  // km/h
};

struct TransmissionProprietary3_T : public CanMessage<0x98FFA303, 8> {
  enum class LowClutchFluidLevel : std::uint8_t {
    NOLOWCLUTCHFLUIDLEVEL = 0,
    LOWCLUTCHFLUIDLEVEL = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class ClutchOverloadInformation : std::uint8_t {
    CLUTCHNOTOVERLOADED = 0,
    CLUTCHHIGHENERGY = 1,
    CLUTCHHIGHTEMPERATURE = 2,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<LowClutchFluidLevel> LowClutchFluidLevel{10, 2};
  CanSignalLe<ClutchOverloadInformation> ClutchOverloadInformation{12, 3};
};

struct EBC5_A : public CanMessage<0x98FDC40B, 8> {
  enum class BrakeTemperatureWarning : std::uint8_t {
    NOTACTIVE = 0,
    ACTIVE = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<BrakeTemperatureWarning> BrakeTemperatureWarning{0, 2};
};

struct DashDisplay_CMS : public CanMessage<0x98FEFC47, 8> {
  enum class FuelLevel : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<FuelLevel> FuelLevel{8, 8};
};

struct OBDInformation_E : public CanMessage<0x98FF7B00, 8> {
  enum class TorqueLimTimeHoursOrMinutes : std::uint8_t {
    TIME_IN_HOURS = 0,
    TIME_IN_MINUTES = 1,
    DONT_CARE = 3,
  };

  enum class SpeedLimTimeHoursOrMinutes : std::uint8_t {
    TIME_IN_HOURS = 0,
    TIME_IN_MINUTES = 1,
    DONT_CARE = 3,
  };

  enum class TimeToTorqueLimit : std::uint8_t {
    NOPENDINGTORQUELIMIT_PARAMETER_SPECIFIC_INDICATOR = 251,
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class TimeToSpeedLimit : std::uint8_t {
    NOPENDINGSPEEDLIMIT_PARAMETER_SPECIFIC_INDICATOR = 251,
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<TorqueLimTimeHoursOrMinutes> TorqueLimTimeHoursOrMinutes{12, 2};
  CanSignalLe<SpeedLimTimeHoursOrMinutes> SpeedLimTimeHoursOrMinutes{14, 2};
  CanSignalLe<TimeToTorqueLimit> TimeToTorqueLimit{16, 8};
  CanSignalLe<TimeToSpeedLimit> TimeToSpeedLimit{24, 8};
};

struct DoorControl1_BCI : public CanMessage<0x98FE4EE6, 8> {
  enum class PositionOfDoors : std::uint8_t {
    ATLEAST1DOORISOPEN = 0,
    CLOSINGLASTDOOR = 1,
    ALLDOORSCLOSED = 2,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  CanSignalLe<PositionOfDoors> PositionOfDoors{0, 4};
};

struct GPM1_ICL : public CanMessage<0x98FF6017, 8> {
  enum class ChassiNo : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4278190080,
  };

  CanSignalLe<ChassiNo> ChassiNo{0, 32};
};

struct Acknowledgement_TCO_FF : public CanMessage<0x98E8FFEE, 8> {
  enum class ControlByte_ACK : std::uint8_t {
    POSITIVEACKNOWLEDGEMENT_ACK = 0,
    NEGATIVEACKNOWLEDGEMENT_NACK = 1,
    ACCESSDENIED_NACK_AD = 2,
    PGNSUPPORTEDBUTECUCANNOTRESPONDNOW_NACK_BUSY = 3,
    DONTT_CARE_TAKE_NO_ACTION = 255,
  };

  CanSignalLe<ControlByte_ACK> ControlByte_ACK{0, 8};
  CanSignalLe<std::uint32_t> ACKM_ACK_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
  CanSignalLe<std::uint32_t> ACKM_NACK_AD_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
  CanSignalLe<std::uint32_t> ACKM_NACK_Busy_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
  CanSignalLe<std::uint32_t> ACKM_NACK_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
};

struct RequestPGN_RTC_TCO : public CanMessage<0x98EAEE4A, 3> {
  CanSignalLe<std::uint32_t> PGN{0, 24, 1, 0, 0, 16449535};
};

struct RequestPGN_RTC_K : public CanMessage<0x98EA274A, 3> {
  CanSignalLe<std::uint32_t> PGN{0, 24, 1, 0, 0, 16449535};
};

struct Acknowledgement_K_FF : public CanMessage<0x98E8FF27, 8> {
  enum class ControlByte_ACK : std::uint8_t {
    POSITIVEACKNOWLEDGEMENT_ACK = 0,
    NEGATIVEACKNOWLEDGEMENT_NACK = 1,
    ACCESSDENIED_NACK_AD = 2,
    PGNSUPPORTEDBUTECUCANNOTRESPONDNOW_NACK_BUSY = 3,
    DONTT_CARE_TAKE_NO_ACTION = 255,
  };

  enum class ACKM_ACK_GroupFunctionValue : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class ACKM_NACK_AD_GroupFunctionValue : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class ACKM_NACK_Bsy_GroupFunctionValue : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class ACKM_NACK_GroupFunctionValue : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<ControlByte_ACK> ControlByte_ACK{0, 8};
  CanSignalLe<ACKM_ACK_GroupFunctionValue> ACKM_ACK_GroupFunctionValue{8, 8};
  CanSignalLe<ACKM_NACK_AD_GroupFunctionValue> ACKM_NACK_AD_GroupFunctionValue{8, 8};
  CanSignalLe<ACKM_NACK_Bsy_GroupFunctionValue> ACKM_NACK_Bsy_GroupFunctionValue{8, 8};
  CanSignalLe<ACKM_NACK_GroupFunctionValue> ACKM_NACK_GroupFunctionValue{8, 8};
  CanSignalLe<std::uint8_t> ACKM_ACK_Address{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> ACKM_NACK_AD_Address{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> ACKM_NACK_Address{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> ACKM_NACK_Busy_Address{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint32_t> ACKM_ACK_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
  CanSignalLe<std::uint32_t> ACKM_NACK_AD_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
  CanSignalLe<std::uint32_t> ACKM_NACK_Busy_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
  CanSignalLe<std::uint32_t> ACKM_NACK_PGNumber{40, 24, 1, 0, 0, 16449535};  // PGN
};

struct HybridInfoProp_HMS : public CanMessage<0x98FF52EF, 8> {
  enum class VCBBatteryChargeMux : std::uint8_t {
    DISTANCE_TO_EMPTY_IN_KILOMETERS = 0,
    DISTANCE_TO_EMPTY_IN_MILES = 1,
    TIME_TO_EMPTY = 2,
    TIME_TO_FULLY_CHARGED = 3,
  };

  enum class VCBBatteryDTE_KM_resolution : std::uint8_t {
    DISTANCE_IN_0_1_KILOMETER_STEPS = 0,
    DISTANCE_IN_KILOMETERS = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class VCBBatteryDTE_MI_resolution : std::uint8_t {
    DISTANCE_IN_0_1_MILE_STEPS = 0,
    DISTANCE_IN_MILES = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class VCBBatteryTTE_resolution : std::uint8_t {
    TIME_IN_MINUTES = 0,
    TIME_IN_5_MINUTE_STEPS = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class VCBBatteryDTE_KM : std::uint16_t {
    ERROR = 4064,
    NOTAVAILABLE = 4095,
  };

  enum class VCBBatteryDTE_MI : std::uint16_t {
    ERROR = 4064,
    NOTAVAILABLE = 4095,
  };

  enum class VCBBatteryTTE : std::uint16_t {
    ERROR = 4064,
    NOTAVAILABLE = 4095,
  };

  CanSignalLe<VCBBatteryChargeMux> VCBBatteryChargeMux{10, 2};
  CanSignalLe<VCBBatteryDTE_KM_resolution> VCBBatteryDTE_KM_resolution{12, 2};
  CanSignalLe<VCBBatteryDTE_MI_resolution> VCBBatteryDTE_MI_resolution{12, 2};
  CanSignalLe<VCBBatteryTTE_resolution> VCBBatteryTTE_resolution{12, 2};
  CanSignalLe<VCBBatteryDTE_KM> VCBBatteryDTE_KM{32, 12};
  CanSignalLe<VCBBatteryDTE_MI> VCBBatteryDTE_MI{32, 12};
  CanSignalLe<VCBBatteryTTE> VCBBatteryTTE{32, 12};
};

struct EI_E : public CanMessage<0x9CFE9200, 8> {
  enum class EngMassFlow : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  CanSignalLe<EngMassFlow> EngMassFlow{32, 16};
};

struct GFC_E : public CanMessage<0x9CFEAF00, 8> {
  enum class TotalFuelUsed : std::uint32_t {
    ERRORINDICATOR = 4261412864,
    NOTAVAILABLE = 4278190080,
  };

  CanSignalLe<TotalFuelUsed> TotalFuelUsed{32, 32};
};

struct DI_TCO_RTC : public CanMessage<0x98FE6BEE, 9> {
  CanSignalLe<std::int8_t> Driver1Identifier{0, 8, 1, 0, 0, 0};  // ASCII
  CanSignalLe<std::int8_t> Delimiter1{8, 8, 1, 0, 0, 0};  // ASCII '*'
  CanSignalLe<std::int8_t> Delimiter2{24, 8, 1, 0, 0, 0};  // ASCII '*'
};

struct TP_CM_TCO_RTC : public CanMessage<0x98EC4AEE, 8> {
  enum class ControlByteTP_CM : std::uint8_t {
    DESTINATION_SPECIFIC_REQUEST_TO_SEND_RTS = 16,
    DESTINATION_SPECIFIC_CLEAR_TO_SEND_CTS = 17,
    END_OF_MESSAGE_ACKNOWLEDGE_END = 19,
    BROADCASTANNOUNCEMESSAGE_BAM = 32,
    CONNECTIONABORT_ABO = 255,
  };

  CanSignalLe<ControlByteTP_CM> ControlByteTP_CM{0, 8};
  CanSignalLe<std::uint16_t> TPCM_BAM_TotalMessageSize{8, 16, 1, 0, 0, 64255};  // byte
  CanSignalLe<std::uint8_t> TPCM_BAM_TotalNumberOfPackets{24, 8, 1, 0, 0, 255};  // counts
  CanSignalLe<std::uint32_t> TPCM_BAM_PGNumber{40, 24, 1, 0, 0, 16777215};  // PGN
};

struct TP_DT_TCO_RTC : public CanMessage<0x98EB4AEE, 8> {
  CanSignalLe<std::uint8_t> SequenceNumber{0, 8, 1, 0, 0, 255};  // count
  CanSignalLe<std::uint8_t> Byte1{8, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte2{16, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte3{24, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte4{32, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte5{40, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte6{48, 8, 1, 0, 0, 255};
  CanSignalLe<std::uint8_t> Byte7{56, 8, 1, 0, 0, 255};
};

struct DistanceToEmpty_CMS : public CanMessage<0x99FF8347, 8> {
  enum class DTEUnit : std::uint8_t {
    KILOMETER = 0,
    MILES = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class FuelDTE_KM : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class FuelDTE_MI : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class FuelTTE : std::uint16_t {
    ERROR = 4064,
    NOTAVAILABLE = 4095,
  };

  enum class ReductantTTE : std::uint16_t {
    ERROR = 4064,
    NOTAVAILABLE = 4095,
  };

  enum class ReductantDTE_KM : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  enum class ReductantDTE_MI : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65535,
  };

  CanSignalLe<DTEUnit> DTEUnit{0, 2};
  CanSignalLe<FuelDTE_KM> FuelDTE_KM{8, 16};
  CanSignalLe<FuelDTE_MI> FuelDTE_MI{8, 16};
  CanSignalLe<FuelTTE> FuelTTE{24, 12};
  CanSignalLe<ReductantTTE> ReductantTTE{36, 12};
  CanSignalLe<ReductantDTE_KM> ReductantDTE_KM{48, 16};
  CanSignalLe<ReductantDTE_MI> ReductantDTE_MI{48, 16};
};

struct TireConditionProprietary2_TM : public CanMessage<0x99FFA633, 8> {
  enum class TirePressThresholdDetection : std::uint8_t {
    EXTREMEOVERPRESSURE = 0,
    OVERPRESSURE = 1,
    NOWARNINGPRESSURE = 2,
    UNDERPRESSURE = 3,
    EXTREMEUNDERPRESSURE = 4,
    NOTDEFINED = 5,
    ERRORINDICATOR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<TirePressThresholdDetection> TirePressThresholdDetection{61, 3};
};

struct TireConditionProprietary2_TPM_AF : public CanMessage<0x99FFA6AF, 8> {
  enum class TirePressThresholdDetection : std::uint8_t {
    EXTREMEOVERPRESSURE = 0,
    OVERPRESSURE = 1,
    NOWARNINGPRESSURE = 2,
    UNDERPRESSURE = 3,
    EXTREMEUNDERPRESSURE = 4,
    NOTDEFINED = 5,
    ERRORINDICATOR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<TirePressThresholdDetection> TirePressThresholdDetection{61, 3};
};

struct TireConditionProprietary2_TPM_B7 : public CanMessage<0x99FFA6B7, 8> {
  enum class TirePressThresholdDetection : std::uint8_t {
    EXTREMEOVERPRESSURE = 0,
    OVERPRESSURE = 1,
    NOWARNINGPRESSURE = 2,
    UNDERPRESSURE = 3,
    EXTREMEUNDERPRESSURE = 4,
    NOTDEFINED = 5,
    ERRORINDICATOR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<TirePressThresholdDetection> TirePressThresholdDetection{61, 3};
};

struct TireConditionProprietary2_TPM_BF : public CanMessage<0x99FFA6BF, 8> {
  enum class TirePressThresholdDetection : std::uint8_t {
    EXTREMEOVERPRESSURE = 0,
    OVERPRESSURE = 1,
    NOWARNINGPRESSURE = 2,
    UNDERPRESSURE = 3,
    EXTREMEUNDERPRESSURE = 4,
    NOTDEFINED = 5,
    ERRORINDICATOR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<TirePressThresholdDetection> TirePressThresholdDetection{61, 3};
};

struct TireConditionProprietary2_TPM_C7 : public CanMessage<0x99FFA6C7, 8> {
  enum class TirePressThresholdDetection : std::uint8_t {
    EXTREMEOVERPRESSURE = 0,
    OVERPRESSURE = 1,
    NOWARNINGPRESSURE = 2,
    UNDERPRESSURE = 3,
    EXTREMEUNDERPRESSURE = 4,
    NOTDEFINED = 5,
    ERRORINDICATOR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<TirePressThresholdDetection> TirePressThresholdDetection{61, 3};
};

struct TireConditionProprietary2_TPM_CF : public CanMessage<0x99FFA6CF, 8> {
  enum class TirePressThresholdDetection : std::uint8_t {
    EXTREMEOVERPRESSURE = 0,
    OVERPRESSURE = 1,
    NOWARNINGPRESSURE = 2,
    UNDERPRESSURE = 3,
    EXTREMEUNDERPRESSURE = 4,
    NOTDEFINED = 5,
    ERRORINDICATOR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<TirePressThresholdDetection> TirePressThresholdDetection{61, 3};
};

struct ZM_PolicyResponse_K : public CanMessage<0x99FF2E27, 8> {
  enum class ZM_PolicyID : std::uint8_t {
    TAKENOACTION = 255,
  };

  enum class ZM_FullfillStatus : std::uint8_t {
    POLICYNOTFULLFILLED = 0,
    POLICYFULLFILLED = 1,
    TAKENOACTION = 3,
  };

  enum class ZM_AcceptStatus : std::uint8_t {
    OK = 1,
    NOTPRIORITIZED = 2,
    NOTOK_ABORTEDBYUSER = 8,
    NOTOK_ABORTEDBYVEHICLE = 9,
    NOTOK_ABORTEDBYFUNCREASON = 10,
    NOTOK_ABORTEDBYFUNCREASONSPECIFIC1 = 11,
    NOTOK_ABORTEDBYFUNCREASONSPECIFIC2 = 12,
    NOTOK_ABORTEDBYFUNCREASONSPECIFIC3 = 13,
    NOTOK_ABORTEDBYFUNCREASONSPECIFIC4 = 14,
    NOTOK_ABORTEDBYFUNCREASONSPECIFIC5 = 15,
    NOTOK_ABORTEDBYBODYWORK = 16,
    NOTOK_NOTCONFIGURED = 17,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC2 = 18,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC3 = 19,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC4 = 20,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC5 = 21,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC6 = 22,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC7 = 23,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC8 = 24,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC9 = 25,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC10 = 26,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC11 = 27,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC12 = 28,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC13 = 29,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC14 = 30,
    NOTOK_ABORTEDBYBODYWORKSPECIFIC15 = 31,
    NOTOK_NOTSUPPORTED = 58,
    NOTOK_POLICYOVERFLOW = 59,
    NOTAVAILABLE = 63,
  };

  enum class ZM_ActiveState : std::uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    PENDING = 2,
    TAKENOACTION = 7,
  };

  enum class ZM_PolicyType : std::uint8_t {
    SPEEDLIMIT = 0,
    NOISE = 1,
    EMISSION = 2,
    BODYWORKSPECIFIC = 3,
    TAKENOACTION = 31,
  };

  CanSignalBe<ZM_PolicyID> ZM_PolicyID{0, 8};
  CanSignalBe<ZM_FullfillStatus> ZM_FullfillStatus{8, 2};
  CanSignalBe<ZM_AcceptStatus> ZM_AcceptStatus{10, 6};
  CanSignalBe<ZM_ActiveState> ZM_ActiveState{16, 3};
  CanSignalBe<ZM_PolicyType> ZM_PolicyType{19, 5};
};

struct ZM_Status_RTC : public CanMessage<0x99FF3F4A, 8> {
  enum class ZM_ServiceStatus : std::uint8_t {
    ZM_INACTIVE = 0,
    ZM_ACTIVE = 1,
    TAKENOACTION = 3,
  };

  enum class ZM_DisplayStatus : std::uint8_t {
    NOPOLICY = 0,
    MULTIPLEPOLICIES = 1,
    NOISEPOLICY = 2,
    NOISEPOLICYVIOLATED = 3,
    EMISSIONPOLICY = 4,
    EMISSIONPOLICYVIOLATED = 5,
    BODYBUILDERPOLICY = 6,
    BODYBUILDERPOLICYVIOLATED = 7,
    TAKENOACTION = 63,
  };

  enum class DisplayInformation : std::uint8_t {
    NOTIFICATIONSOFF = 0,
    NOTIFICATIONSON = 1,
    TAKENOACTION = 15,
  };

  enum class SpeedPolicyState : std::uint8_t {
    NOACTIVEPOLICIES = 0,
    ATLEASTONEPOLICYACTIVE = 1,
    ATLEASTONEPOLICYVIOLATED = 2,
    POLICIESINACTIVATEDBYUSER = 3,
    TAKENOACTION = 15,
  };

  enum class EmissionPolicyState : std::uint8_t {
    NOACTIVEPOLICIES = 0,
    ATLEASTONEPOLICYACTIVE = 1,
    ATLEASTONEPOLICYVIOLATED = 2,
    POLICIESINACTIVATEDBYUSER = 3,
    TAKENOACTION = 15,
  };

  enum class NoisePolicyState : std::uint8_t {
    NOACTIVEPOLICIES = 0,
    ATLEASTONEPOLICYACTIVE = 1,
    ATLEASTONEPOLICYVIOLATED = 2,
    POLICIESINACTIVATEDBYUSER = 3,
    TAKENOACTION = 15,
  };

  enum class BodyBuilderPolicyState : std::uint8_t {
    NOACTIVEPOLICIES = 0,
    ATLEASTONEPOLICYACTIVE = 1,
    ATLEASTONEPOLICYVIOLATED = 2,
    POLICIESINACTIVATEDBYUSER = 3,
    TAKENOACTION = 15,
  };

  CanSignalLe<ZM_ServiceStatus> ZM_ServiceStatus{0, 2};
  CanSignalLe<ZM_DisplayStatus> ZM_DisplayStatus{2, 6};
  CanSignalLe<DisplayInformation> DisplayInformation{8, 4};
  CanSignalLe<SpeedPolicyState> SpeedPolicyState{16, 4};
  CanSignalLe<EmissionPolicyState> EmissionPolicyState{20, 4};
  CanSignalLe<NoisePolicyState> NoisePolicyState{24, 4};
  CanSignalLe<BodyBuilderPolicyState> BodyBuilderPolicyState{28, 4};
};

struct SecondaryWakeUp_ALM : public CanMessage<0x8CFF041D, 8> {
  enum class WakeUp_RTC : std::uint8_t {
    NOWAKEUP = 0,
    WAKEUP = 1,
    TAKENOACTION = 3,
  };

  CanSignalLe<WakeUp_RTC> WakeUp_RTC{22, 2};
};

struct SecondaryWakeUp_BCI : public CanMessage<0x8CFF04E6, 8> {
  enum class WakeUp_RTC : std::uint8_t {
    NOWAKEUP = 0,
    WAKEUP = 1,
    TAKENOACTION = 3,
  };

  CanSignalLe<WakeUp_RTC> WakeUp_RTC{22, 2};
};

struct StayAlive_RTC : public CanMessage<0x99FF7D4A, 8> {
  enum class StayAliveRequest : std::uint8_t {
    STAYALIVENOTREQUESTED = 0,
    STAYALIVEREQUESTED = 1,
    TAKENOACTION = 3,
  };

  enum class TargetSystem_Byte1 : std::uint8_t {
    COO_BIN_00000001 = 1,
    EMS_BIN_00000010 = 2,
    AMS_BIN_00000100 = 4,
    TMS_BIN_00001000 = 8,
    CMS_BIN_00010000 = 16,
    DDU_BIN_00100000 = 32,
    TBD_BIN_01000000 = 64,
    TBD_BIN_10000000 = 128,
  };

  CanSignalLe<StayAliveRequest> StayAliveRequest{0, 2};
  CanSignalLe<TargetSystem_Byte1> TargetSystem_Byte1{8, 8};
};

struct DoorStatusProprietary_DCS : public CanMessage<0x98FFBDEC, 8> {
  enum class DriverDoorOpenStatus : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PassengerDoorOpenStatus : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DoorBDOpenStatus : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DoorBPOpenStatus : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DriverDoorLockStatus : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PassengerDoorLockStatus : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DoorBDLockStatus : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DoorBPLockStatus : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class DriverWindowOpenStatus : std::uint8_t {
    NOT_OPENED = 0,
    OPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class PassengerWindowOpenStatus : std::uint8_t {
    NOT_OPENED = 0,
    OPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class WindowBDOpenStatus : std::uint8_t {
    NOT_OPENED = 0,
    OPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class WindowBPOpenStatus : std::uint8_t {
    NOT_OPENED = 0,
    OPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<DriverDoorOpenStatus> DriverDoorOpenStatus{0, 2};
  CanSignalLe<PassengerDoorOpenStatus> PassengerDoorOpenStatus{2, 2};
  CanSignalLe<DoorBDOpenStatus> DoorBDOpenStatus{4, 2};
  CanSignalLe<DoorBPOpenStatus> DoorBPOpenStatus{6, 2};
  CanSignalLe<DriverDoorLockStatus> DriverDoorLockStatus{8, 2};
  CanSignalLe<PassengerDoorLockStatus> PassengerDoorLockStatus{10, 2};
  CanSignalLe<DoorBDLockStatus> DoorBDLockStatus{12, 2};
  CanSignalLe<DoorBPLockStatus> DoorBPLockStatus{14, 2};
  CanSignalLe<DriverWindowOpenStatus> DriverWindowOpenStatus{16, 2};
  CanSignalLe<PassengerWindowOpenStatus> PassengerWindowOpenStatus{18, 2};
  CanSignalLe<WindowBDOpenStatus> WindowBDOpenStatus{20, 2};
  CanSignalLe<WindowBPOpenStatus> WindowBPOpenStatus{22, 2};
};

struct ETC1_T : public CanMessage<0x8CF00203, 8> {
  enum class InputShaftSpeed : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<InputShaftSpeed> InputShaftSpeed{40, 16};
};

struct BSM2_BCI : public CanMessage<0x99FF82E6, 8> {
  enum class StopSignalVisual : std::uint8_t {
    NOT_ACTIVE = 0,
    LAMP_ACTIVE = 1,
    DISPLAY_ACTIVE = 2,
    LAMP_AND_DISPLAY_ACTIVE = 3,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  enum class PramsignalVisual : std::uint8_t {
    NOT_ACTIVE = 0,
    LAMP_ACTIVE = 1,
    DISPLAY_ACTIVE = 2,
    LAMP_AND_DISPLAY_ACTIVE = 3,
    ERROR = 6,
    NOTAVAILABLE = 7,
  };

  CanSignalLe<StopSignalVisual> StopSignalVisual{0, 3};
  CanSignalLe<PramsignalVisual> PramsignalVisual{40, 3};
};

struct SuspensionInformationProprietary_CMS : public CanMessage<0x98FFB647, 8> {
  enum class KneelingStatus : std::uint8_t {
    KNEELINGNOTACTIVATED = 0,
    KNEELINGACTIVATED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<KneelingStatus> KneelingStatus{30, 2};
};

struct DoorControl1_Unknown : public CanMessage<0x98FE4EFE, 8> {
  enum class PositionOfDoors : std::uint8_t {
    ATLEAST1DOORISOPEN = 0,
    CLOSINGLASTDOOR = 1,
    ALLDOORSCLOSED = 2,
    ERROR = 14,
    NOTAVAILABLE = 15,
  };

  enum class Ramp_WheelchairLiftStatus : std::uint8_t {
    INSIDEBUS = 0,
    OUTSIDEBUS = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class Status2OfDoors : std::uint8_t {
    ALLBUSDOORSAREDISABLED = 0,
    ATLEAST1BUSDOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<PositionOfDoors> PositionOfDoors{0, 4};
  CanSignalLe<Ramp_WheelchairLiftStatus> Ramp_WheelchairLiftStatus{4, 2};
  CanSignalLe<Status2OfDoors> Status2OfDoors{6, 2};
};

struct DoorControl2_Unknown : public CanMessage<0x98FDA5FE, 8> {
  enum class LockStatusOfDoor1 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor1 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor1 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor2 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor2 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor2 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor3 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor3 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor3 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor4 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor4 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor4 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor5 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor5 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor5 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor6 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor6 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor6 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor7 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor7 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor7 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor8 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor8 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor8 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor9 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor9 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor9 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class LockStatusOfDoor10 : std::uint8_t {
    UNLOCKED = 0,
    LOCKED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class OpenStatusOfDoor10 : std::uint8_t {
    DOORCLOSED = 0,
    DOOROPENED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  enum class EnableStatusOfDoor10 : std::uint8_t {
    DOORDISABLED = 0,
    DOORENABLED = 1,
    ERROR = 2,
    NOTAVAILABLE = 3,
  };

  CanSignalLe<LockStatusOfDoor1> LockStatusOfDoor1{0, 2};
  CanSignalLe<OpenStatusOfDoor1> OpenStatusOfDoor1{2, 2};
  CanSignalLe<EnableStatusOfDoor1> EnableStatusOfDoor1{4, 2};
  CanSignalLe<LockStatusOfDoor2> LockStatusOfDoor2{6, 2};
  CanSignalLe<OpenStatusOfDoor2> OpenStatusOfDoor2{8, 2};
  CanSignalLe<EnableStatusOfDoor2> EnableStatusOfDoor2{10, 2};
  CanSignalLe<LockStatusOfDoor3> LockStatusOfDoor3{12, 2};
  CanSignalLe<OpenStatusOfDoor3> OpenStatusOfDoor3{14, 2};
  CanSignalLe<EnableStatusOfDoor3> EnableStatusOfDoor3{16, 2};
  CanSignalLe<LockStatusOfDoor4> LockStatusOfDoor4{18, 2};
  CanSignalLe<OpenStatusOfDoor4> OpenStatusOfDoor4{20, 2};
  CanSignalLe<EnableStatusOfDoor4> EnableStatusOfDoor4{22, 2};
  CanSignalLe<LockStatusOfDoor5> LockStatusOfDoor5{24, 2};
  CanSignalLe<OpenStatusOfDoor5> OpenStatusOfDoor5{26, 2};
  CanSignalLe<EnableStatusOfDoor5> EnableStatusOfDoor5{28, 2};
  CanSignalLe<LockStatusOfDoor6> LockStatusOfDoor6{30, 2};
  CanSignalLe<OpenStatusOfDoor6> OpenStatusOfDoor6{32, 2};
  CanSignalLe<EnableStatusOfDoor6> EnableStatusOfDoor6{34, 2};
  CanSignalLe<LockStatusOfDoor7> LockStatusOfDoor7{36, 2};
  CanSignalLe<OpenStatusOfDoor7> OpenStatusOfDoor7{38, 2};
  CanSignalLe<EnableStatusOfDoor7> EnableStatusOfDoor7{40, 2};
  CanSignalLe<LockStatusOfDoor8> LockStatusOfDoor8{42, 2};
  CanSignalLe<OpenStatusOfDoor8> OpenStatusOfDoor8{44, 2};
  CanSignalLe<EnableStatusOfDoor8> EnableStatusOfDoor8{46, 2};
  CanSignalLe<LockStatusOfDoor9> LockStatusOfDoor9{48, 2};
  CanSignalLe<OpenStatusOfDoor9> OpenStatusOfDoor9{50, 2};
  CanSignalLe<EnableStatusOfDoor9> EnableStatusOfDoor9{52, 2};
  CanSignalLe<LockStatusOfDoor10> LockStatusOfDoor10{54, 2};
  CanSignalLe<OpenStatusOfDoor10> OpenStatusOfDoor10{56, 2};
  CanSignalLe<EnableStatusOfDoor10> EnableStatusOfDoor10{58, 2};
};

struct EngineTemp_E : public CanMessage<0x98FEEE00, 8> {
  enum class EngineCoolantTemperature : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<EngineCoolantTemperature> EngineCoolantTemperature{0, 8};
};

struct EngineHoursRevolutions_E : public CanMessage<0x98FEE500, 8> {
  enum class TotalEngineHours : std::uint32_t {
    ERROR = 4261412864,
    NOTAVAILABLE = 4278190080,
  };

  CanSignalLe<TotalEngineHours> TotalEngineHours{0, 32};
};

struct ERC1_RD : public CanMessage<0x98F00010, 8> {
  enum class EngineRetarderTorqueMode : std::uint8_t {
    NO_REQUEST_DEFAULT_MODE = 0,
    OPERATOR_SELECTION = 1,
    CRUISE_CONTROL = 2,
    PTO_GOVERNOR = 3,
    ROAD_SPEED_GOVERNING = 4,
    ASR_CONTROL = 5,
    TRANSMISSION_CONTROL = 6,
    ABS_CONTROL = 7,
    TORQUE_LIMITING = 8,
    HIGH_SPEED_GOVERNOR = 9,
    BRAKE_SYSTEM = 10,
    REMOTE_ACCELERATOR = 11,
    NOTDEFINED = 12,
    WHITE_SMOKE_LIMITING = 13,
    OTHER = 14,
    TAKENOACTION = 15,
  };

  enum class ActualRetarderPercentTorque : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<EngineRetarderTorqueMode> EngineRetarderTorqueMode{0, 4};
  CanSignalLe<ActualRetarderPercentTorque> ActualRetarderPercentTorque{8, 8};
};

struct AIR1_APS : public CanMessage<0x98FEAE30, 8> {
  enum class ServiceBrakeAirPressure1 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  enum class ServiceBrakeAirPressure2 : std::uint8_t {
    ERROR = 254,
    NOTAVAILABLE = 255,
  };

  CanSignalLe<ServiceBrakeAirPressure1> ServiceBrakeAirPressure1{16, 8};
  CanSignalLe<ServiceBrakeAirPressure2> ServiceBrakeAirPressure2{24, 8};
};

struct ASC4_F : public CanMessage<0x98FE582F, 8> {
  enum class BellowPressFrontAxleLeft : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class BellowPressFrontAxleRight : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class BellowPressRearAxleLeft : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  enum class BellowPressRearAxleRight : std::uint16_t {
    ERROR = 65024,
    NOTAVAILABLE = 65280,
  };

  CanSignalLe<BellowPressFrontAxleLeft> BellowPressFrontAxleLeft{0, 16};
  CanSignalLe<BellowPressFrontAxleRight> BellowPressFrontAxleRight{16, 16};
  CanSignalLe<BellowPressRearAxleLeft> BellowPressRearAxleLeft{32, 16};
  CanSignalLe<BellowPressRearAxleRight> BellowPressRearAxleRight{48, 16};
};

#undef CanSignalLe
#undef CanSignalBe
#undef CanMessage

using canID_0x98FEE617 = TimeDate_ICL;
using canID_0x98FF554A = ADASP1_RTC;
using canID_0x98FF6027 = GPM1_K;
using canID_0x8CFF6327 = GPM2_K;
using canID_0x98FF4327 = BrakeLiningWearInformation2_K;
using canID_0x98FE5EC8 = RGE23_TT_C8;
using canID_0x98FE5EC0 = RGE23_TT_C0;
using canID_0x98FE5EB8 = RGE23_TT_B8;
using canID_0x98FE5EB0 = RGE23_TT_B0;
using canID_0x98FE5EA8 = RGE23_TT_A8;
using canID_0x98FEEA17 = VehicleWeight_ICL;
using canID_0x99FF4F4A = ImplicitSpeedLimit_RTC;
using canID_0x99FF474A = RTCInformationProprietary2_RTC;
using canID_0x99FF7347 = CI3_CMS;
using canID_0x98FEF127 = CruiseControlVehSpeed_K;
using canID_0x99FF4150 = PBS1_EPB;
using canID_0x8DFF404A = PBC1_RTC;
using canID_0x8CFFFC4A = ExternalControlMessage_RTC;
using canID_0x98FF8227 = DLN9_K;
using canID_0x98F00503 = ETC2_T;
using canID_0x98FEF34A = VehiclePosition_1000_RTC;
using canID_0x98FEE84A = VehiDirSpeed_1000_RTC;
using canID_0x98FF6E03 = TI_T;
using canID_0x8CFF04EF = SecondaryWakeUp_HMS;
using canID_0x98FD7D17 = TelltaleStatus_ICL;
using canID_0x98FF6B27 = GPM3_K;
using canID_0x98FFAE27 = ServiceInformationProp_K;
using canID_0x98FFBE1D = AlarmStatusProp_ALM;
using canID_0x8CFF7427 = PowertrainControl2_K;
using canID_0x99FF2327 = AccumulatedTripDataParams_K;
using canID_0x98FEF527 = AmbientConditions_K;
using canID_0x99FF3C27 = GPM10_K;
using canID_0x98FE6B27 = DI_OnChange_K_FF;
using canID_0x98ECFF27 = TP_CM_K_FF;
using canID_0x98EBFF27 = TP_DT_K_FF;
using canID_0x98FF9027 = PTOInformationProp_K;
using canID_0x9CFF0627 = DriverEvaluationData_K;
using canID_0x9CFF1327 = DriverEvaluationEvents_K;
using canID_0x98EC4A27 = TP_CM_K_RTC;
using canID_0x98EB4A27 = TP_DT_K_RTC;
using canID_0x98FEEC27 = VehicleId_K_RTC;
using canID_0x98FFB11E = CUVInformation_V;
using canID_0x98FFD517 = ICLInformationProprietary_ICL;
using canID_0x99FF8400 = ReductantDTEInformation_E;
using canID_0x8CFE6CEE = TCO1_TCO;
using canID_0x98FEC1EE = HiResVehicleDist_TCO;
using canID_0x98FF400B = BSS2_A;
using canID_0x98FF5F00 = EngineInfoProprietary2_E;
using canID_0x8CF00300 = EEC2_E;
using canID_0x98FD0900 = HRLFC_E;
using canID_0x99FF4600 = OBDInformation2_E;
using canID_0x98FF7C00 = PowertrainInformation3_E;
using canID_0x8CF00400 = EEC1_E;
using canID_0x98FEE900 = FuelConsumption_E;
using canID_0x98FEF200 = FuelEconomy_E;
using canID_0x98FECA00 = DM1_E;
using canID_0x98FF6000 = GPM1_E;
using canID_0x98FD7C00 = DPFC1_E;
using canID_0x98FF7A00 = PowertrainInformation1_E;
using canID_0x98FEBF0B = EBC2_A;
using canID_0x98FFA303 = TransmissionProprietary3_T;
using canID_0x98FDC40B = EBC5_A;
using canID_0x98FEFC47 = DashDisplay_CMS;
using canID_0x98FF7B00 = OBDInformation_E;
using canID_0x98FE4EE6 = DoorControl1_BCI;
using canID_0x98FF6017 = GPM1_ICL;
using canID_0x98E8FFEE = Acknowledgement_TCO_FF;
using canID_0x98EAEE4A = RequestPGN_RTC_TCO;
using canID_0x98EA274A = RequestPGN_RTC_K;
using canID_0x98E8FF27 = Acknowledgement_K_FF;
using canID_0x98FF52EF = HybridInfoProp_HMS;
using canID_0x9CFE9200 = EI_E;
using canID_0x9CFEAF00 = GFC_E;
using canID_0x98FE6BEE = DI_TCO_RTC;
using canID_0x98EC4AEE = TP_CM_TCO_RTC;
using canID_0x98EB4AEE = TP_DT_TCO_RTC;
using canID_0x99FF8347 = DistanceToEmpty_CMS;
using canID_0x99FFA633 = TireConditionProprietary2_TM;
using canID_0x99FFA6AF = TireConditionProprietary2_TPM_AF;
using canID_0x99FFA6B7 = TireConditionProprietary2_TPM_B7;
using canID_0x99FFA6BF = TireConditionProprietary2_TPM_BF;
using canID_0x99FFA6C7 = TireConditionProprietary2_TPM_C7;
using canID_0x99FFA6CF = TireConditionProprietary2_TPM_CF;
using canID_0x99FF2E27 = ZM_PolicyResponse_K;
using canID_0x99FF3F4A = ZM_Status_RTC;
using canID_0x8CFF041D = SecondaryWakeUp_ALM;
using canID_0x8CFF04E6 = SecondaryWakeUp_BCI;
using canID_0x99FF7D4A = StayAlive_RTC;
using canID_0x98FFBDEC = DoorStatusProprietary_DCS;
using canID_0x8CF00203 = ETC1_T;
using canID_0x99FF82E6 = BSM2_BCI;
using canID_0x98FFB647 = SuspensionInformationProprietary_CMS;
using canID_0x98FE4EFE = DoorControl1_Unknown;
using canID_0x98FDA5FE = DoorControl2_Unknown;
using canID_0x98FEEE00 = EngineTemp_E;
using canID_0x98FEE500 = EngineHoursRevolutions_E;
using canID_0x98F00010 = ERC1_RD;
using canID_0x98FEAE30 = AIR1_APS;
using canID_0x98FE582F = ASC4_F;
