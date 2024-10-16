/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2020 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/*!        \file  vac/language/byte.h
 *        \brief  Backport of the C++17 type std::byte.
 *
 *      \details  byte is a distinct type that implements the concept of byte as specified in the C++ language
 *                definition.
 *                Like char and unsigned char, it can be used to access raw memory occupied by other objects (object
 *                representation), but unlike those types, it is not a character type and is not an arithmetic type. A
 *                byte is only a collection of bits, and the only operators defined for it are the bitwise ones.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_BYTE_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_BYTE_H_

#include <cstdint>
#include <stdexcept>

#include "vac/language/cpp14_backport.h"

namespace vac {
namespace language {
/*!
 * \brief Equivalent to C++17 std::byte.
 * \trace CREQ-204265
 */
enum class byte : std::uint8_t {};

/*!
 * \brief  Cast any integer type to byte.
 * \tparam IntegerType The integer type to cast.
 * \param  b The integer to cast (must be smaller or equal to 0xFF).
 * \return The value as a byte.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte to_byte(IntegerType b) noexcept {
  return static_cast<byte>(b);
}

inline namespace literals {
// VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
/*!
 * \brief  Creates a byte from a literal. The target variable must be a constexpr.
 * \param  i The integer literal from which to create the byte.
 * \return The value as a byte.
 * \pre i must be less than 256, otherwise the returned value will be 0xFF.
 */
constexpr byte operator"" _byte(unsigned long long int i) noexcept {  // NOLINT(runtime/int)
  return (i <= 0xFF) ? to_byte(i) : to_byte(255);
}
}  // namespace literals

/*!
 * \brief  Cast byte to any other integer type.
 * \tparam IntegerType The integer type to cast.
 * \param  b The integer to cast (must be smaller or equal to 0xFF).
 * \return The value after cast.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr IntegerType to_integer(byte b) noexcept {
  return static_cast<IntegerType>(b);
}

// VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_castMayTruncateValue
// VECTOR Next Construct AutosarC++17_10-A4.7.1: MD_VAC_A4.7.1_castMayTruncateValue
/*!
 * \brief   Bitwise left shift.
 * \details The value will be truncated when using this shift operation.
 * \tparam  IntegerType The integer type to shift.
 * \param   b The integer to shift.
 * \param   shift Number of bits to shift.
 * \return  The value after shift.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte operator<<(byte b, IntegerType shift) noexcept {
  return to_byte(static_cast<std::uint8_t>(static_cast<std::uint8_t>(b) << shift));
}

/*!
 * \brief  Bitwise right shift.
 * \tparam IntegerType The integer type to shift.
 * \param  b The integer to shift.
 * \param  shift Number of bits to shift.
 * \return The value after shift.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte operator>>(byte b, IntegerType shift) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(b) >> shift);
}

// VECTOR Next Construct AutosarC++17_10-A8.4.4: MD_VAC_A8.4.4_useReturnValueInsteadOfOutputParameter
// VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_assignmentOperatorWithRefqualifier
/*!
 * \brief  Bitwise left shift assignment.
 * \tparam IntegerType The integer type to shift.
 * \param  b The integer to shift.
 * \param  shift Number of bits to shift
 * \return The value after shift.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte& operator<<=(byte& b, IntegerType shift) noexcept {
  // VECTOR Next Construct AutosarC++17_10-M5.0.21: MD_VAC_M5.0.21_bitwiseOperatorOnEnumType
  // VECTOR Next Line AutosarC++17_10-A4.5.1: MD_VAC_A4.5.1_useOfEnumOperandInArithmetic
  b = b << shift;
  return b;
}

// VECTOR Next Construct AutosarC++17_10-A8.4.4: MD_VAC_A8.4.4_useReturnValueInsteadOfOutputParameter
// VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_assignmentOperatorWithRefqualifier
/*!
 * \brief  Bitwise right shift assignment.
 * \tparam IntegerType The integer type to shift.
 * \param  b The integer to shift.
 * \param  shift Number of bits to shift
 * \return The value after shift.
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte& operator>>=(byte& b, IntegerType shift) noexcept {
  // VECTOR Next Construct AutosarC++17_10-M5.0.21: MD_VAC_M5.0.21_bitwiseOperatorOnEnumType
  // VECTOR Next Line AutosarC++17_10-A4.5.1: MD_VAC_A4.5.1_useOfEnumOperandInArithmetic
  b = b >> shift;
  return b;
}

/*!
 * \brief  Bitwise or operator.
 * \param  l Left operand.
 * \param  r Right operand.
 * \return The result of or operation.
 */
constexpr byte operator|(byte l, byte r) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(l) | static_cast<std::uint8_t>(r));
}
/*!
 * \brief  Bitwise and operator.
 * \param  l Left operand.
 * \param  r Right operand.
 * \return The result of and operation.
 */
constexpr byte operator&(byte l, byte r) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(l) & static_cast<std::uint8_t>(r));
}
/*!
 * \brief  Bitwise xor operator.
 * \param  l Left operand.
 * \param  r Right operand.
 * \return The result of xor operation.
 */
constexpr byte operator^(byte l, byte r) noexcept {
  return static_cast<byte>(static_cast<std::uint8_t>(l) ^ static_cast<std::uint8_t>(r));
}

// VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_castMayTruncateValue
// VECTOR Next Construct AutosarC++17_10-A4.7.1: MD_VAC_A4.7.1_castMayTruncateValue
/*!
 * \brief  Bitwise not operator.
 * \param  b Operand.
 * \return The result of not operation.
 */
constexpr byte operator~(byte b) noexcept { return to_byte(static_cast<std::uint8_t>(~static_cast<std::uint8_t>(b))); }

// VECTOR Next Construct AutosarC++17_10-A8.4.4: MD_VAC_A8.4.4_useReturnValueInsteadOfOutputParameter
// VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_assignmentOperatorWithRefqualifier
/*!
 * \brief  Bitwise or assignment.
 * \param  l Left operand.
 * \param  r Right operand.
 * \return The result of or assignment operation.
 */
constexpr inline byte& operator|=(byte& l, byte r) noexcept {
  // VECTOR Next Line AutosarC++17_10-A4.5.1: MD_VAC_A4.5.1_useOfEnumOperandInArithmetic
  l = l | r;
  return l;
}
// VECTOR Next Construct AutosarC++17_10-A8.4.4: MD_VAC_A8.4.4_useReturnValueInsteadOfOutputParameter
// VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_assignmentOperatorWithRefqualifier
/*!
 * \brief  Bitwise and assignment.
 * \param  l Left operand.
 * \param  r Right operand.
 * \return The result of and assignment operation.
 */
constexpr inline byte& operator&=(byte& l, byte r) noexcept {
  // VECTOR Next Line AutosarC++17_10-A4.5.1: MD_VAC_A4.5.1_useOfEnumOperandInArithmetic
  l = l & r;
  return l;
}
// VECTOR Next Construct AutosarC++17_10-A8.4.4: MD_VAC_A8.4.4_useReturnValueInsteadOfOutputParameter
// VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_assignmentOperatorWithRefqualifier
/*!
 * \brief  Bitwise xor assignment.
 * \param  l Left operand.
 * \param  r Right operand.
 * \return The result of xor assignment operation.
 */
constexpr inline byte& operator^=(byte& l, byte r) noexcept {
  // VECTOR Next Line AutosarC++17_10-A4.5.1: MD_VAC_A4.5.1_useOfEnumOperandInArithmetic
  l = l ^ r;
  return l;
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_BYTE_H_
