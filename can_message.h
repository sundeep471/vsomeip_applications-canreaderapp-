/**
**************************************************************************************
** \file can_message.h
** \brief Definition of CAN message class.
**************************************************************************************
*/
#ifndef INCLUDE_UTIL_CAN_MESSAGE_H_
#define INCLUDE_UTIL_CAN_MESSAGE_H_

/*
**====================================================================================
** Imported interfaces
**====================================================================================
*/
#include <cstdint>
#include "can_identifier.h"

/*
**====================================================================================
** Named Namespaces
**====================================================================================
*/
namespace util {

/**
** \brief Definition of CAN message class.
**
** \tparam RawIdentifier Value of message raw identifier.
** \tparam Length        Value of message length in bytes.
** \tparam CycleTime     Value of message cycle time in milliseconds.
*/
template <std::uint32_t RawIdentifier, std::uint16_t Length, std::uint32_t CycleTime = 0>
class CanMessage {
 public:
  /** ** \brief Gets CAN message identifier.  ** ** \return CAN message identifier.  */
  static constexpr auto GetCanIdentifier() {
    return CanIdentifier{IsExtendedFrame(), RawIdentifier};  // NOLINT
  }

  /** ** \brief Gets raw message identifier.  ** ** \return Raw message identifier.  */
  static constexpr auto GetRawIdentifier() {
    return RawIdentifier;
  }
  /** ** \brief Gets message length in bytes.  ** ** \return Message length in bytes.  */
  static constexpr auto GetLength() {
    return Length;
  }

  /** ** \brief Gets message cycle time in milliseconds.  ** ** \return Message cycle time in milliseconds.  */
  static constexpr auto GetCycleTime() {
    return CycleTime;
  }

  /** ** \brief Checks if CAN message uses extended frame format.  ** ** \return True if CAN message uses extended frame format or false in case of **         standard frame format.  */
  static constexpr auto IsExtendedFrame() {
    return static_cast<bool>(RawIdentifier & 0x80000000);  // NOLINT
  }

  /** ** \brief Checks if CAN message is transmitted in multiple frames.  ** ** \return True if CAN message is transmitted in multiple frames.  */
  static constexpr auto IsMultiFrame() {
    return Length > 8;  // NOLINT
  }
};

}  // namespace util

#endif  // INCLUDE_UTIL_CAN_MESSAGE_H_

/************************************************************************************/
