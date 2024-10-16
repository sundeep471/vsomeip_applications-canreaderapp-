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
/*!        \file  ara/core/byte.h
 *        \brief  Implementation of ara::core::Byte
 *
 *      \details  Byte is a distinct type that implements the concept of byte as specified in the C++ language
 *                definition.
 *                Like char and unsigned char, it can be used to access raw memory occupied by other objects (object
 *                representation), but unlike those types, it is not a character type and is not an arithmetic type. A
 *                Byte is only a collection of bits, and the only operators defined for it are the bitwise ones.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_BYTE_H_
#define LIB_VAC_INCLUDE_ARA_CORE_BYTE_H_

#include "vac/language/byte.h"

namespace ara {
namespace core {

/*!
 * \brief   Implementation of ara::core::Byte.
 * \details This is not a complete implementation, it is lacking support for SWS_CORE_10103. Therefore, a literal
 *          operator has been provided as _byte in the literals namespace.
 * \trace   CREQ-204265
 * \vpublic
 */
using Byte = vac::language::byte;

/*!
 * \brief  Cast byte to any other integer type.
 * \tparam IntegerType The integer type to cast to.
 * \param  b The byte value to cast.
 * \return An integer with the same value as b.
 * \vpublic
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr IntegerType to_integer(Byte b) noexcept {
  return static_cast<IntegerType>(b);
}

/*!
 * \brief  Cast any integer type to byte.
 * \tparam IntegerType The integer type to cast.
 * \param  b The integer to cast (must be smaller or equal to 0xFF).
 * \return The value as a byte.
 * \vprivate
 */
template <typename IntegerType, typename = vac::language::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr Byte to_byte(IntegerType b) noexcept {
  return static_cast<Byte>(b);
}

inline namespace literals {
// VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
/*!
 * \brief  Creates a byte from a literal. The target variable must be a constexpr.
 * \param  i The integer literal from which to create the byte.
 * \return The value as a byte.
 * \pre    i must be less than 256, otherwise the returned value will be 0xFF.
 * \vpublic
 */
constexpr Byte operator"" _byte(unsigned long long int i) noexcept {  // NOLINT(runtime/int)
  return (i <= 0xFF) ? vac::language::to_byte(i) : vac::language::to_byte(255);
}
}  // namespace literals

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_BYTE_H_
