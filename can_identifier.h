/*
**************************************************************************************
** Copyright (c) Traton
**
** This document contains proprietary information belonging to Traton.
** Passing on and copying of this document, and communication of its contents
** is not permitted without prior written authorization.
**
**************************************************************************************
*/

/**
**************************************************************************************
** \file can_identifier.h
**
** \brief Definition of CAN identifier class.
**
**************************************************************************************
*/
#ifndef INCLUDE_UTIL_CAN_IDENTIFIER_H_
#define INCLUDE_UTIL_CAN_IDENTIFIER_H_

/*
**====================================================================================
** Imported interfaces
**====================================================================================
*/
#include <cstdint>

/*
**====================================================================================
** Named Namespaces
**====================================================================================
*/
namespace util {

/**
** \brief Definition of CAN identifier class.
*/
class CanIdentifier {
 public:
  /**
  ** \brief Class constructor.
  **
  ** \param[in] can_id CAN identifier value.
  */
  constexpr explicit CanIdentifier(std::uint32_t can_id) : id_{can_id} {}

  /**
  ** \brief Class constructor.
  **
  ** \param[in] extended Indicates if CAN identifier value has standard 11-bit (false)
  **                     or extended 29-bit (true) length.
  ** \param[in] can_id   CAN identifier value.
  */
  constexpr CanIdentifier(bool extended, std::uint32_t can_id)
      : id_{can_id & (extended ? 0x1FFFFFFF : 0x7FF)} {}  // NOLINT

  /**
  ** \brief Gets priority bits.
  **
  ** \return Priority bits.
  */
  constexpr auto GetPriority() const {
    return (id_ >> 26) & 0x7;  // NOLINT
  }

  /**
  ** \brief Gets extended data page (EDP) bit.
  **
  ** \return Extended data page bit.
  */
  constexpr auto GetExtendedDataPage() const {
    return (id_ >> 25) & 0x1;  // NOLINT
  }

  /**
  ** \brief Gets data page (DP) bit.
  **
  ** \return Data page bit.
  */
  constexpr auto GetDataPage() const {
    return (id_ >> 24) & 0x1;  // NOLINT
  }

  /**
  ** \brief Gets PDU format (PF) bits.
  **
  ** \return PDU format bits.
  */
  constexpr auto GetPduFormat() const {
    return (id_ >> 16) & 0xFF;  // NOLINT
  }

  /**
  ** \brief Gets PDU specific (PS) bits.
  **
  ** \return PDU specific bits.
  */
  constexpr auto GetPduSpecific() const {
    return (id_ >> 8) & 0xFF;  // NOLINT
  }

  /**
  ** \brief Gets source address (SA) bits.
  **
  ** \return Source address bits.
  */
  constexpr auto GetSourceAddress() const {
    return id_ & 0xFF;  // NOLINT
  }

  /**
  ** \brief Checks if message is a unicast message (PDU format 1).
  **
  ** \return True if message is a unicast message.
  */
  constexpr auto IsUnicast() const {
    return GetPduFormat() < 0xF0;  // NOLINT
  }

  /**
  ** \brief Checks if message is a broadcast message (PDU format 2).
  **
  ** \return True if message is a broadcast message.
  */
  constexpr auto IsBroadcast() const {
    return GetPduFormat() >= 0xF0;  // NOLINT
  }

  /**
  ** \brief Gets parameter group number (PGN) bits.
  **
  ** \return Parameter group number bits.
  */
  constexpr auto GetParameterGroupNumber() const {
    return (id_ >> 8) & (IsUnicast() ? 0x3FF00 : 0x3FFFF);  // NOLINT
  }

  /**
  ** \brief Indirection operator.
  **
  ** \return CAN identifier value.
  */
  constexpr auto operator*() const {
    return id_;
  }

  /**
  ** \brief Equal operator for CanIdentifier type.
  **
  ** \param[in] other Other CanIdentifier object.
  **
  ** \return True if equal.
  */
  constexpr bool operator==(const CanIdentifier& other) const {
    return id_ == other.id_;
  }

  /**
  ** \brief Not equal operator for CanIdentifier type.
  **
  ** \param[in] other Other CanIdentifier object.
  **
  ** \return True if not equal.
  */
  constexpr bool operator!=(const CanIdentifier& other) const {
    return id_ != other.id_;
  }

 private:
  std::uint32_t id_;
};

/**
** \brief Equal operator for CanIdentifier and uint32_t types.
**
** \param[in] can_id CanIdentifier object.
** \param[in] value  CAN identifier value.
**
** \return True if equal.
*/
constexpr bool operator==(const CanIdentifier& can_id, const std::uint32_t& value) {
  return *can_id == value;
}

/**
** \brief Equal operator for uint32_t and CanIdentifier types.
**
** \param[in] value  CAN identifier value.
** \param[in] can_id CanIdentifier object.
**
** \return True if equal.
*/
constexpr bool operator==(const std::uint32_t& value, const CanIdentifier& can_id) {
  return value == *can_id;
}

/**
** \brief Not equal operator for CanIdentifier and uint32_t types.
**
** \param[in] can_id CanIdentifier object.
** \param[in] value  CAN identifier value.
**
** \return True if not equal.
*/
constexpr bool operator!=(const CanIdentifier& can_id, const std::uint32_t& value) {
  return *can_id != value;
}

/**
** \brief Not equal operator for uint32_t and CanIdentifier types.
**
** \param[in] value  CAN identifier value.
** \param[in] can_id CanIdentifier object.
**
** \return True if not equal.
*/
constexpr bool operator!=(const std::uint32_t& value, const CanIdentifier& can_id) {
  return value != *can_id;
}

}  // namespace util

#endif  // INCLUDE_UTIL_CAN_IDENTIFIER_H_
