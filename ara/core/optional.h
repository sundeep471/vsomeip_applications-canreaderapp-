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
/*!        \file  ara/core/optional.h
 *        \brief  SWS core type ara::core::Optional.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_OPTIONAL_H_
#define LIB_VAC_INCLUDE_ARA_CORE_OPTIONAL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cassert>
#include <functional>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

#include "../../ara/core/abort.h"
#include "../../ara/core/utility.h"
#include "../../vac/language/cpp17_backport.h"
#include "../../vac/language/detail/either.h"
#include "../../vac/testing/test_adapter.h"

namespace ara {
namespace core {

/*!
 * \brief Token type used to construct an empty Optional.
 * \vpublic
 */
struct nullopt_t {};

// VECTOR Next Construct AutosarC++17_10-M17.0.2: MD_VAC_M17.0.2_standardLibraryObjectNameOverride
// VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally
/*!
 * \brief Named token instance to use when constructing an empty Optional.
 * \vpublic
 */
constexpr nullopt_t nullopt{};

/*!
 * \brief Class to manage a static memory location in which an object of given type can be constructed.
 *        (Partial) Backport from C++17.
 * \trace CREQ-158617
 * \vpublic
 */
template <class T>
class Optional final {
  /*!
   * \brief Internal marker used for the Either.
   */
  struct Empty {};

  /*!
   * \brief Helper trait to determine if Optional is no-throw move assignable.
   * \vprivate
   */
  constexpr static bool is_nothrow_move_assignable{(std::is_nothrow_move_assignable<T>::value) &&
                                                   (std::is_nothrow_move_constructible<T>::value) &&
                                                   (std::is_nothrow_destructible<T>::value)};

 public:
  /*!
   * \brief Value type for accessing inner type.
   */
  using value_type = T;

  // VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor
  /*!
   * \brief Default constructor.
   * \vpublic
   */
  constexpr Optional() noexcept : data_(Empty{}) {}

  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  /*!
   * \brief Constructor from nullopt to construct empty Optional.
   * \vpublic
   */
  constexpr Optional(nullopt_t) noexcept : Optional() {}  // NOLINT(runtime/explicit)

  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  // VECTOR Disable AutosarC++17_10-A2.8.2: MD_VAC_A2.8.2_nolintComment
  /*!
   * \brief   Construction from preexisting value.
   * \details If an exception is raised by the copy constructor in T the process is terminated.
   * \param   rhs The value to construct the Optional from.
   * \vpublic
   */
  Optional(T const& rhs) noexcept : Optional(in_place, rhs) {}  // NOLINT[runtime/explicit]

  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  /*!
   * \brief   Move Constructor from preexisting value.
   * \details If an exception is raised by the move constructor in T the process is terminated.
   * \param   rhs The value to construct the Optional from.
   * \vpublic
   */
  Optional(T&& rhs) noexcept : Optional(in_place, std::move(rhs)) {}  // NOLINT[runtime/explicit]

  // VECTOR Enable AutosarC++17_10-A2.8.2
  /*!
   * \brief   Inplace construction.
   * \details If an exception is raised by the constructor in T the process is terminated.
   * \param   args The arguments to pass to the values constructor.
   * \vpublic
   */
  template <class... Args>
  constexpr explicit Optional(in_place_t, Args&&... args) noexcept
      : data_(in_place_type_t<T>{}, std::forward<Args>(args)...) {}

  // VECTOR Next Construct AutosarC++17_10-A8.5.4: MD_VAC_A8.5.4_initializerlistShallBeDefinedInClasses
  // VECTOR Next Construct VectorC++-V8.5.4: MD_VAC_V8-5-4_ctorShallNotBeDefinedToAcceptStdInitializerList
  /*!
   * \brief   Inplace construction.
   * \details If an exception is raised by the constructor in T the process is terminated.
   * \param   ilist The initializer list to pass to the constructor.
   * \param   args The arguments to pass to the values constructor.
   * \vpublic
   */
  template <class U, class... Args>
  constexpr explicit Optional(in_place_t, std::initializer_list<U> ilist, Args&&... args) noexcept
      : data_(in_place_type_t<T>{}, ilist, std::forward<Args>(args)...) {}

  /*!
   * \brief   Copy construction.
   * \details If an exception is raised by the copy constructor in T the process is terminated.
   * \param   rhs The Optional to copy from.
   * \vpublic
   */
  constexpr Optional(Optional const& rhs) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief   Move constructor.
   * \details If an exception is raised by the move constructor in T the process is terminated.
   * \param   rhs The moved Optional from which to construct a new Optional.
   * \vpublic
   */
  constexpr Optional(Optional&& rhs) = default;

  /*!
   * \brief   Copy assignment.
   * \details If an exception is raised by the constructor or assignment in T the process is terminated.
   * \param   rhs The Optional to copy from.
   * \return  The copied Optional.
   * \vpublic
   */
  Optional& operator=(Optional const& rhs) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief   Move assignment.
   * \details If an exception is raised by the constructor or assignment in T the process is terminated.
   * \param   rhs The Optional to move from.
   * \return  The copied Optional.
   * \vpublic
   */
  Optional& operator=(Optional&& rhs) & = default;

  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor that also destroys the contained object, if it exists.
   * \vpublic
   */
  ~Optional() noexcept = default;

  /*!
   * \brief   Constructor function used to build the contained element.
   * \details If an exception is raised by the constructor in T the process is terminated.
   * \param   args Arguments to pass to the underlying types constructor.
   * \return  A reference to the underlying value.
   * \trace   CREQ-158618
   * \vpublic
   */
  template <typename... Args>
  T& emplace(Args&&... args) noexcept {
    return this->data_.EmplaceRight(std::forward<Args>(args)...);
  }

  /*!
   * \brief Destructor function used to call the destructor of the contained elements.
   * \trace CREQ-158618
   * \vpublic
   */
  void reset() noexcept {
    if (this->has_value()) {
      this->data_.EmplaceLeft(Empty{});
    }
  }

  /*!
   * \brief  Return a reference to the contained value.
   * \pre    The Optional must contain a value, otherwise Abort() is called.
   * \return A reference to the contained value if the value exists.
   * \trace  CREQ-158620
   * \vpublic
   */
  T& value() noexcept {
    if (!(this->has_value())) {
      ara::core::Abort("ara::core::Optional::value(): Optional does not contain a value!");
    }
    return **this;
  }

  /*!
   * \brief  Return a reference to the contained value.
   * \pre    The Optional must contain a value, otherwise Abort() is called.
   * \return A reference to the contained value if the value exists.
   * \vpublic
   */
  T const& value() const noexcept {
    if (!(this->has_value())) {
      ara::core::Abort("ara::core::Optional::value(): Optional does not contain a value!");
    }
    return **this;
  }

  /*!
   * \brief  Return a reference to the contained value or the default value.
   * \param  default_value The value to use in case ptr_ is empty.
   * \return A reference to the contained value if the value exists. Else returns the default.
   * \vpublic
   */
  template <class U>
      T value_or(U&& default_value) && noexcept {
    T ret_value{static_cast<T>(std::forward<U>(default_value))};
    // VECTOR NL AutosarC++17_10-A7.2.1: MD_VAC_A7.2.1_valueIsNotWithinEnumeratorRange
    if (this->has_value()) {
      ret_value = std::move(this->data_.RightUnsafe());
      this->data_.EmplaceLeft(Empty{});
    }
    return ret_value;
  }

  // VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
  /*!
   * \brief  Return a reference to the contained value or the default value.
   * \param  default_value The value to use in case ptr_ is empty.
   * \return A reference to the contained value if the value exists. Else returns the default.
   * \vpublic
   */
  template <class U>
  T value_or(U&& default_value) const& noexcept {
    T ret_value{std::forward<U>(default_value)};
    if (this->has_value()) {
      ret_value = **this;
    }
    return ret_value;
  }

  /*!
   * \brief  Checks whether *this contains a value.
   * \return True if *this contains a value, false if *this does not contain a value.
   * \trace  CREQ-158619
   * \vpublic
   */
  explicit operator bool() const noexcept { return this->data_.IsRight(); }

  /*!
   * \brief  Checks whether *this contains a value.
   * \return True if *this contains a value, false if *this does not contain a value.
   * \trace  CREQ-158619
   * \vpublic
   */
  bool has_value() const noexcept { return this->data_.IsRight(); }

  /*!
   * \brief  Access the contained value.
   * \return Pointer to the contained object if an object exists, else behavior is undefined.
   * \vpublic
   */
  T* operator->() noexcept { return &this->data_.RightUnsafe(); }

  /*!
   * \brief  Access the contained value.
   * \return Pointer to the contained object if an object exists, else behavior is undefined.
   * \vpublic
   */
  T const* operator->() const noexcept { return &this->data_.RightUnsafe(); }

  /*!
   * \brief  Access the contained value.
   * \return Reference to the contained object. The behavior is undefined if no object is contained.
   * \vpublic
   */
  T& operator*() noexcept { return this->data_.RightUnsafe(); }

  /*!
   * \brief  Access the contained value.
   * \return Reference to the contained object. The behavior is undefined if no object is contained.
   * \vpublic
   */
  T const& operator*() const noexcept { return this->data_.RightUnsafe(); }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Exchange the state of this Optional with other Optional.
   * \param  other The other Optional to swap with.
   * \vpublic
   */
  void swap(Optional& other) noexcept {
    using std::swap;
    swap(this->data_, other.data_);
  }

 private:
  /*!
   * \brief Union-like data type - either nothing or the data.
   */
  vac::language::detail::Either<Empty, T> data_;

  FRIEND_TEST(OptionalTestFixture, Construct);
  FRIEND_TEST(OptionalTestFixture, Value);
  FRIEND_TEST(OptionalTestFixture, Size);
};

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
/*!
 * \brief  Creates an optional object from value.
 * \param  value The value to construct optional object with.
 * \return The constructed optional object.
 * \vpublic
 */
template <typename T>
auto make_optional(T&& value) noexcept -> Optional<std::decay_t<T>> {
  return Optional<std::decay_t<T>>{std::forward<T>(value)};
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
// VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
/*!
 * \brief  Creates an optional object constructed in-place from args.
 * \param  args Arguments to be passed to the constructor of T.
 * \return The constructed optional object.
 * \vpublic
 */
template <typename T, typename... Args>
auto make_optional(Args&&... args) noexcept -> Optional<T> {
  return Optional<T>(in_place, std::forward<Args>(args)...);
}

// VECTOR Next Construct AutosarC++17_10-M17.0.3: MD_VAC_M17.0.3_standardLibraryFunctionNameOverride
// VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
/*!
 * \brief  Creates an optional object constructed in-place from args.
 * \param  il Initializer list to be passed to the constructor of T.
 * \param  args Arguments to be passed to the constructor of T.
 * \return The constructed optional object.
 * \vpublic
 */
template <typename T, typename U, typename... Args>
auto make_optional(std::initializer_list<U> il, Args&&... args) noexcept -> Optional<T> {
  return Optional<T>(in_place, il, std::forward<Args>(args)...);
}

/*!
 * \brief  Compares two Optional objects on equality.
 * \param  lhs An Optional object to compare.
 * \param  rhs An Optional object to compare.
 * \return True if both Optional objects are empty or both are filled and the contained values are equal.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator==(Optional<T> const& lhs, Optional<U> const& rhs) noexcept {
  bool const l_has{lhs.has_value()};
  bool const r_has{rhs.has_value()};
  bool result{l_has == r_has};
  if (l_has && r_has) {
    T const& lhs_val{*lhs};
    U const& rhs_val{*rhs};
    result = lhs_val == rhs_val;
  }
  return result;
}

/*!
 * \brief  Compares an Optional object with a value on equality.
 * \param  lhs An Optional object to compare.
 * \param  rhs An object to compare.
 * \return True if the Optional object is full and both values are equal.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator==(Optional<T> const& lhs, U const& rhs) noexcept {
  bool result{false};
  if (lhs.has_value()) {
    result = *lhs == rhs;
  }
  return result;
}

/*!
 * \brief  Compares an Optional object with a value on equality.
 * \param  lhs An object to compare.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is full and both values are equal.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator==(U const& lhs, Optional<T> const& rhs) noexcept {
  return rhs == lhs;
}

/*!
 * \brief  Compares two Optional objects on inequality.
 * \param  lhs An Optional object to compare.
 * \param  rhs An Optional object to compare.
 * \return False if both Optional objects are empty or both are filled and the contained values are equal.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator!=(Optional<T> const& lhs, Optional<U> const& rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief  Compares an Optional object with a value on inequality.
 * \param  lhs An Optional object to compare.
 * \param  rhs A value object to compare.
 * \return True if the Optional object is empty or both values are not equal.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator!=(Optional<T> const& lhs, U const& rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief  Compares an Optional object with a value on inequality.
 * \param  lhs A value object to compare.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is empty or both values are not equal.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator!=(U const& lhs, Optional<T> const& rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief  Compares two Optional objects on less.
 * \param  lhs An Optional object to compare.
 * \param  rhs An Optional object to compare.
 * \return False if both Optional objects are empty or lhs is not less than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator<(Optional<T> const& lhs, Optional<U> const& rhs) noexcept {
  bool result{false};
  if (lhs.has_value() && rhs.has_value()) {
    T const& lhs_val{*lhs};
    U const& rhs_val{*rhs};
    result = lhs_val < rhs_val;
  } else {
    result = rhs.has_value();
  }
  return result;
}

/*!
 * \brief  Compares an Optional objects with a value on less.
 * \param  lhs An Optional object to compare.
 * \param  rhs A value to compare.
 * \return True if the Optional object is empty or lhs is less than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator<(Optional<T> const& lhs, U const& rhs) noexcept {
  bool result{true};
  if (lhs.has_value()) {
    result = *lhs < rhs;
  }
  return result;
}

/*!
 * \brief  Compares an Optional objects with a value on less.
 * \param  lhs A value to compare.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is empty or lhs is less than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator<(U const& lhs, Optional<T> const& rhs) noexcept {
  bool result{false};
  if (rhs.has_value()) {
    result = lhs < *rhs;
  }
  return result;
}

/*!
 * \brief  Compares two Optional objects on greater.
 * \param  lhs An Optional object to compare.
 * \param  rhs An Optional object to compare.
 * \return False if both Optional objects are empty or lhs is not greater than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator>(Optional<T> const& lhs, Optional<U> const& rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief  Compares an Optional objects with a value on greater.
 * \param  lhs An Optional object to compare.
 * \param  rhs A value to compare.
 * \return True if the Optional object is empty or lhs is greater than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator>(Optional<T> const& lhs, U const& rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief  Compares an Optional objects with a value on greater.
 * \param  lhs A value to compare.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is empty or lhs is greater than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator>(U const& lhs, Optional<T> const& rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief  Compares two Optional objects on less equal.
 * \param  lhs An Optional object to compare.
 * \param  rhs An Optional object to compare.
 * \return True if both Optional objects are empty or lhs is less equal than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator<=(Optional<T> const& lhs, Optional<U> const& rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief  Compares an Optional objects with a value on less equal.
 * \param  lhs An Optional object to compare.
 * \param  rhs A value to compare.
 * \return True if the Optional object is empty or lhs is less equal than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator<=(Optional<T> const& lhs, U const& rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief  Compares an Optional objects with a value on less equal.
 * \param  lhs A value to compare.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is empty or lhs is less equal than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator<=(U const& lhs, Optional<T> const& rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief  Compares two Optional objects on greater equal.
 * \param  lhs An Optional object to compare.
 * \param  rhs An Optional object to compare.
 * \return True if both Optional objects are empty or lhs is greater equal than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator>=(Optional<T> const& lhs, Optional<U> const& rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief  Compares an Optional objects with a value on greater equal.
 * \param  lhs An Optional object to compare.
 * \param  rhs A value to compare.
 * \return True if the Optional object is empty or lhs is greater equal than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator>=(Optional<T> const& lhs, U const& rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief  Compares an Optional objects with a value on greater equal.
 * \param  lhs A value to compare.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is empty or lhs is greater equal than rhs.
 * \vpublic
 */
template <class T, class U>
constexpr bool operator>=(U const& lhs, Optional<T> const& rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief  Check equality between an Optional and nullopt.
 * \param  lhs An Optional object to compare.
 * \return True if the Optional object is empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator==(Optional<T> const& lhs, nullopt_t) noexcept {
  return !lhs;
}

/*!
 * \brief  Check equality between an Optional and nullopt.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator==(nullopt_t, Optional<T> const& rhs) noexcept {
  return !rhs;
}

/*!
 * \brief  Check inequality for an Optional and nullopt.
 * \param  lhs An Optional object to compare.
 * \return True if the Optional object is not empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator!=(Optional<T> const& lhs, nullopt_t) noexcept {
  return static_cast<bool>(lhs);
}

/*!
 * \brief  Check inequality for an Optional and nullopt.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is not empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator!=(nullopt_t, Optional<T> const& rhs) noexcept {
  return static_cast<bool>(rhs);
}

/*!
 * \brief  Check less than for an Optional and nullopt.
 * \return False.
 * \vpublic
 */
template <typename T>
constexpr bool operator<(Optional<T> const&, nullopt_t) noexcept {
  return false;
}

/*!
 * \brief  Check less than for an Optional and nullopt.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is not empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator<(nullopt_t, Optional<T> const& rhs) noexcept {
  return static_cast<bool>(rhs);
}

/*!
 * \brief  Check less than or equal for an Optional and nullopt.
 * \param  lhs An Optional object to compare.
 * \return True if the Optional object is empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator<=(Optional<T> const& lhs, nullopt_t) noexcept {
  return !lhs;
}

/*!
 * \brief  Check less than or equal for an Optional and nullopt.
 * \return True.
 * \vpublic
 */
template <typename T>
constexpr bool operator<=(nullopt_t, Optional<T> const&) noexcept {
  return true;
}

/*!
 * \brief  Check greater than for an Optional and nullopt.
 * \param  lhs An Optional object to compare.
 * \return True if the Optional object is not empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator>(Optional<T> const& lhs, nullopt_t) noexcept {
  return static_cast<bool>(lhs);
}

/*!
 * \brief  Check greater than for an Optional and nullopt.
 * \return False.
 * \vpublic
 */
template <typename T>
constexpr bool operator>(nullopt_t, Optional<T> const&) noexcept {
  return false;
}

/*!
 * \brief  Check greater than or equal for an Optional and nullopt.
 * \return True.
 * \vpublic
 */
template <typename T>
constexpr bool operator>=(Optional<T> const&, nullopt_t) noexcept {
  return true;
}

/*!
 * \brief  Check greater than or equal for an Optional and nullopt.
 * \param  rhs An Optional object to compare.
 * \return True if the Optional object is empty.
 * \vpublic
 */
template <typename T>
constexpr bool operator>=(nullopt_t, Optional<T> const& rhs) noexcept {
  return !rhs;
}

// VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
/*!
 * \brief Swaps two Optional objects.
 * \param lhs Optional object to be swapped.
 * \param rhs Optional object to be swapped.
 * \vpublic
 */
template <typename T>
void swap(Optional<T>& lhs, Optional<T>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_OPTIONAL_H_
