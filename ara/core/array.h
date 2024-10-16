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
/*!        \file  ara/core/array.h
 *        \brief  SWS core type ara::core::Array.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_ARRAY_H_
#define LIB_VAC_INCLUDE_ARA_CORE_ARRAY_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <array>
#include <utility>

namespace ara {
namespace core {

/*!
 * \brief  Array class template for ara::core.
 * \tparam T Value type.
 * \tparam N std::size_t type.
 * \trace  SPEC-7552492
 * \trace  CREQ-215926
 * \vpublic
 */
template <typename T, std::size_t N>
using Array = std::array<T, N>;

// VECTOR Next Construct AutosarC++17_10-A2.11.4: MD_VAC_A2.11.4_nameOfObjectOrFunctionReused
// VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief  Overloaded std::swap function for ara::core::Array data type.
 * \tparam T Value type.
 * \tparam N std::size_t type.
 * \param  lhs Left hand side Array to swap.
 * \param  rhs Right hand side Array to swap.
 * \trace  SPEC-7552493
 * \vpublic
 */
template <typename T, std::size_t N>
void swap(Array<T, N>& lhs, Array<T, N>& rhs) noexcept(noexcept(std::swap(std::declval<T&>(), std::declval<T&>()))) {
  lhs.swap(rhs);
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_ARRAY_H_
