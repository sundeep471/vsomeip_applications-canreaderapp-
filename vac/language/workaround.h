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
/*!        \file  workaround.h
 *        \brief  Reimplements functionality from the C++ stardard library.
 *
 *      \details  Provides implementations from the  C++ stardard library for constructs that are not working
 *                correctly in all toolchains.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_WORKAROUND_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_WORKAROUND_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <type_traits>

#include "vac/language/cpp17_backport.h"

namespace vac {
namespace language {
namespace workaround {

namespace detail {

/*!
 * \brief Determines whether Alloc supports rebind<U>::other.
 */
template <typename Alloc, typename U, typename = void>
struct HasRebindOther : std::false_type {};

/*!
 * \brief Specialization when Alloc has rebind<U>::other.
 */
template <typename Alloc, typename U>
struct HasRebindOther<Alloc, U, vac::language::void_t<typename Alloc::template rebind<U>::other>> : std::true_type {};

}  // namespace detail

/*!
 * \brief Rebinds the allocator Alloc for the type U.
 * \trace CREQ-VaCommonLib-RebindAllocator
 */
template <typename Alloc, typename U, bool = detail::HasRebindOther<Alloc, U>::value>
class RebindAllocator;

/*!
 * \brief Rebinds the allocator Alloc for the type U when Alloc implements rebind.
 * \trace CREQ-VaCommonLib-RebindAllocator
 */
template <template <typename, typename...> class Alloc, typename T, typename... Args, typename U>
class RebindAllocator<Alloc<T, Args...>, U, true> {
 public:
  /*! \brief Rebound allocator type. */
  using type = typename Alloc<T, Args...>::template rebind<U>::other;
};

/*!
 * \brief Rebinds the allocator Alloc for the type U when Alloc does not implement rebind.
 * \trace CREQ-VaCommonLib-RebindAllocator
 */
template <template <typename, typename...> class Alloc, typename T, typename... Args, typename U>
class RebindAllocator<Alloc<T, Args...>, U, false> {
 public:
  /*! \brief Rebound allocator type. */
  using type = Alloc<U, Args...>;
};

}  // namespace workaround
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_WORKAROUND_H_
