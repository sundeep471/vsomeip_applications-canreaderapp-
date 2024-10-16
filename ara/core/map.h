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
/*!        \file  ara/core/map.h
 *        \brief  SWS core type ara::core::Map.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_MAP_H_
#define LIB_VAC_INCLUDE_ARA_CORE_MAP_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <map>
#include <utility>
#include "vac/memory/phase_managed_allocator.h"

namespace ara {
namespace core {
/*!
 * \brief Map data type that represents a container of key-value pair with unique keys.
 * \trace SPEC-7552502
 * \trace CREQ-214738
 * \vpublic
 */
template <typename K, typename V, typename C = std::less<K>,
          typename Alloc = vac::memory::PhaseManagedAllocator<std::pair<K const, V>>>
using Map = std::map<K, V, C, Alloc>;

// VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief  An overloaded std::swap function for ara::core::Map data type.
 * \tparam K key for (key-value) pair for ara::core::Map data type.
 * \tparam V Value for (key-value) pair for ara::core::Map data type.
 * \tparam C Comparer for K (std::less<K>) for (key-value) pair for ara::core::Map data type.
 * \tparam Alloc phase managed allocator for ara::core::Map data type.
 * \param  lhs Left hand side Map to swap.
 * \param  rhs Right hand side Map to swap.
 * \trace  SPEC-7552503
 * \vpublic
 */
template <typename K, typename V, typename C = std::less<K>,
          typename Alloc = vac::memory::PhaseManagedAllocator<std::pair<K const, V>>>
void swap(Map<K, V, C, Alloc>& lhs, Map<K, V, C, Alloc>& rhs) noexcept {
  std::swap(lhs, rhs);
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_MAP_H_
