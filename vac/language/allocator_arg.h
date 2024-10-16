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
/*!        \file  vac/language/allocator_arg.h
 *        \brief  Tag for allocator argument constructor.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_ALLOCATOR_ARG_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_ALLOCATOR_ARG_H_

namespace vac {
namespace language {

/*!
 * \brief Tag to indicate allocator construction. Similar to in_place_t.
 */
struct allocator_arg_t {};

// VECTOR Next Construct AutosarC++17_10-M17.0.2: MD_VAC_M17.0.2_standardLibraryObjectNameOverride
/*!
 * \brief Singleton instance of allocator_arg_t.
 */
constexpr allocator_arg_t allocator_arg{};

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_ALLOCATOR_ARG_H_
