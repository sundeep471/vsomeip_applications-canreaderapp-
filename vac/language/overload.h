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
/*!        \file  overload.h
 *        \brief  Helper for constructing a single visitor from multiple functors.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_OVERLOAD_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_OVERLOAD_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

namespace vac {
namespace language {

namespace detail {

// VECTOR Next Construct AutosarC++17_10-A10.1.1: MD_VAC_A10.1.1_multipleInheritance
// VECTOR Next Construct AutosarC++17_10-M10.2.1: MD_VAC_M10.2.1_uniqueEntityNamesInMultipleInheritance
// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution
// Forward declaration of helper for constructing a single visitor from multiple functors.
template <typename... TFunctors>
struct overload;

// VECTOR Next Construct AutosarC++17_10-A10.1.1: MD_VAC_A10.1.1_multipleInheritance
// VECTOR Next Construct AutosarC++17_10-A12.1.6: MD_VAC_A12.1.6_useInheritingConstructor
/*!
 * \brief   Helper for constructing a single visitor from multiple functors.
 * \details Recursive declaration: add overload for first type,
 *          and recursively add overload(s) for the trailing type list.
 */
template <typename TFirst, typename... TTail>
class overload<TFirst, TTail...> : TFirst, overload<TTail...> {
 public:
  /*!
   * \brief Constructor.
   * \param first First functor.
   * \param tail List of remaining functors.
   */
  overload(TFirst first, TTail... tail) noexcept : TFirst(first), overload<TTail...>(tail...) {}

  /*!
   * \brief Overload operator().
   */
  using TFirst::operator();
  /*!
   * \brief Overload operator().
   */
  using overload<TTail...>::operator();
};

// VECTOR Next Construct AutosarC++17_10-A12.1.6: MD_VAC_A12.1.6_useInheritingConstructor
/*!
 * \brief   Helper for constructing a single visitors from multiple functors.
 * \details Recursive declaration: case to end recursion.
 */
template <typename TLast>
class overload<TLast> : TLast {
 public:
  /*!
   * \brief Constructor.
   * \param last Only/last functor.
   */
  explicit overload(TLast last) noexcept : TLast(last) {}

  /*!
   * \brief Overload operator().
   */
  using TLast::operator();
};

}  // namespace detail

/*!
 * \brief   Create a visitor from a list of lambda expressions.
 * \details Example usage:
 *          auto visitor = make_overload([](const int&) { return 1; },
 *                                       [](const bool&) { return 2; },
 *                                       [](const uint32_t&) { return 3; },
 *                                       [](const std::string&) { return 4; });.
 * \param   functors List of functor objects.
 * \return  A visitor providing all the functors passed as overloaded operator().
 * \trace   CREQ-158611
 */
template <typename... TFunctors>
auto make_overload(TFunctors... functors) noexcept -> detail::overload<TFunctors...> {
  return detail::overload<TFunctors...>(functors...);
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_OVERLOAD_H_
