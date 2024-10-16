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
/*!        \file  cpp17_backport.h
 *        \brief  Contains useful backports of STL features from C++17 to C++11.
 *
 *      \details  The C++17 backported features include also the backported C++14 ones.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP17_BACKPORT_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP17_BACKPORT_H_

#include <utility>
#include "../../ara/core/utility.h"
#include "../../vac/language/cpp14_backport.h"

namespace vac {
namespace language {

namespace detail {

// VECTOR Next Construct AutosarC++17_10-M7.3.6: MD_VAC_M7.3.6_usingDeclarationInHeaderFile
// VECTOR Disable AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution
using std::swap;

/*!
 * \brief Helper template in namespace including swap (aka using swap;) to test if a call to swap is well formed.
 */
class swap_test {
 public:
  /*!
   * \brief  Overload for swappable resolution.
   * \return True type.
   */
  template <typename T, typename = decltype(swap(std::declval<T&>(), std::declval<T&>()))>
  static constexpr std::true_type can_swap(std::uint8_t) noexcept {
    return std::true_type();
  }

  // VECTOR Next Construct AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
  /*!
   * \brief  Overload for non-swappable resolution.
   * \return False type.
   */
  template <typename>
  static constexpr std::false_type can_swap(...) noexcept;
};

/*!
 * \brief Helper template to test if a call to swap is well formed.
 */
template <typename T>
class swappable_impl : public swap_test {
 public:
  /*!
   * \brief Type containing the deduction result.
   */
  using type = decltype(can_swap<T>(0));
};

/*!
 * \brief Test implementation for swappable.
 * \trace CREQ-158610
 */
template <typename T, bool = (std::is_const<typename std::remove_reference<T>::type>::value) ||
                             (std::is_function<typename std::remove_reference<T>::type>::value)>
struct is_swappable;

/*!
 * \brief Specialization if the tested type is const.
 */
template <typename T>
struct is_swappable<T, true> : public std::false_type {};

/*!
 * \brief Specialization if the tested type is non-const.
 */
template <typename T>
struct is_swappable<T, false> : swappable_impl<T>::type {};

/*!
 * \brief Implementation of nothrow swappable.
 */
template <typename T, bool = is_swappable<T>::value>
struct is_nothrow_swappable;

/*!
 * \brief Specialization for swappable types.
 */
template <typename T>
struct is_nothrow_swappable<T, true>
    : std::integral_constant<bool, noexcept(swap(std::declval<T&>(), std::declval<T&>()))> {};

/*!
 * \brief Specialization for non-swappable types.
 */
template <typename T>
struct is_nothrow_swappable<T, false> : std::false_type {};

/*!
 * \brief   Wrapper for void_t.
 * \details For some older compilers, unused parameters in alias templates are not guaranteed to ensure SFINAE and can
 *          be ignored.
 */
template <typename... Ts>
class make_void {
 public:
  /*! \brief Setting the type to void. */
  using type = void;
};
}  // namespace detail

/*!
 * \brief Is true if swap<T> is possible.
 */
template <typename T>
struct is_swappable : detail::is_swappable<T> {};

/*!
 * \brief Is true if swap<T> is noexcept.
 */
template <typename T>
struct is_nothrow_swappable : detail::is_nothrow_swappable<T> {};

/*! \brief Utility metafunction that maps a sequence of any types to the type void. */
template <typename... Ts>
using void_t = typename detail::make_void<Ts...>::type;

/*!
 * \brief Conjunction.
 */
template <class...>
struct conjunction : std::true_type {};
/*!
 * \brief Conjunction.
 */
template <class B1>
struct conjunction<B1> : B1 {};
/*!
 * \brief Conjunction.
 */
template <class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

/*! \brief Disjunction. */
template <typename...>
struct disjunction : std::false_type {};

/*! \brief Disjunction. */
template <typename B1>
struct disjunction<B1> : B1 {};

/*! \brief Disjunction. */
template <typename B1, typename... Bn>
struct disjunction<B1, Bn...> : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type {};

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
// VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst
// VECTOR Next Construct AutosarC++17_10-A7.5.1: MD_VAC_A7.5.1_ReferenceOrPointerToAParameterPassedByReferenceToConst
/*!
 * \brief  Forms lvalue reference to const type of t.
 * \tparam T The value type.
 * \param  t The non-const value.
 * \return The const-reference form of the value.
 * \trace  CREQ-171850
 */
template <typename T>
constexpr add_const_t<T>& as_const(T& t) noexcept {
  return t;
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief  Const rvalue reference overload is deleted to disallow rvalue arguments.
 * \tparam T Value type.
 * \trace  CREQ-171850
 */
template <typename T>
void as_const(T const&&) = delete;

namespace detail {
/*!
 * \brief Default type of is_invocable_r_impl.
 */
template <typename Void, typename R, typename F, typename... Args>
struct is_invocable_r_impl : std::false_type {};

/*!
 * \brief Specialization of is_invocable_r_impl when F is invocable.
 */
template <typename R, typename F, typename... Args>
struct is_invocable_r_impl<void_t<std::result_of_t<F(Args...)>>, R, F, Args...>
    : disjunction<std::is_void<R>, std::is_convertible<std::result_of_t<F(Args...)>, R>> {};
}  // namespace detail

/*!
 * \brief  Rough implementation of C++17 std::is_invocable_r.
 * \tparam R The type to check conversion against.
 * \tparam F The callable type.
 * \tparam Args... The arguments to F.
 */
template <typename R, typename F, typename... Args>
struct is_invocable_r : detail::is_invocable_r_impl<void, R, F, Args...> {};

// VECTOR Enable AutosarC++17_10-A12.4.1
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_CPP17_BACKPORT_H_
