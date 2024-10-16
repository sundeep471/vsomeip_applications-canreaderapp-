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
/*!        \file  c_string_view.h
 *        \brief  Implements a string_view object that is guaranteed to be null-terminated.
 *
 *      \details  Decays into a regular string_view for methods like substr.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_C_STRING_VIEW_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_C_STRING_VIEW_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <cstring>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "ara/core/abort.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/utility.h"

namespace vac {
namespace container {

/*!
 * \brief Basic_string_view Alias for ara::core::basic_string_view.
 */
template <typename T>
using basic_string_view = ara::core::basic_string_view<T>;

template <typename T>
class BasicCStringView;

/*!
 * \brief CStringView is a constant assortment of characters.
 * \trace CREQ-165660
 */
using CStringView = BasicCStringView<char>;

/*!
 * \brief U16CStringView is a constant assortment of UCS-2 characters.
 * \trace CREQ-165660
 */
using U16CStringView = BasicCStringView<char16_t>;

/*!
 * \brief U32CStringView is a constant assortment of UCS-4 characters.
 * \trace CREQ-165660
 */
using U32CStringView = BasicCStringView<char32_t>;

// VECTOR Next Construct Metric-OO.WMC.One: MD_VAC_Metric-OO.WMC.One
/*!
 * \brief Represents a const view on some string buffer that is guaranteed to be null terminated.
 *        Does not own the string buffer.
 * \trace CREQ-165659
 */
template <typename Char>
class BasicCStringView final {
 public:
  /*!
   * \brief Typedef for the internal storage type.
   */
  using storage_t = ara::core::basic_string_view<Char>;

  /*!
   * \brief Type of contained element.
   */
  using value_type = typename storage_t::value_type;

  /*!
   * \brief Type of the size field.
   */
  using size_type = typename storage_t::size_type;

  /*!
   * \brief Type of the differences between indices.
   */
  using difference_type = typename storage_t::difference_type;

  /*!
   * \brief Const reference type for contained elements.
   */
  using const_reference = typename storage_t::const_reference;

  /*!
   * \brief Const pointer type for contained elements.
   */
  using const_pointer = typename storage_t::const_pointer;

  /*!
   * \brief Const iterator for the BasicCStringView.
   */
  using const_iterator = typename storage_t::const_iterator;

  /*!
   * \brief Const reversed iterator for the BasicCStringView.
   */
  using const_reverse_iterator = typename storage_t::const_reverse_iterator;

  /*!
   * \brief       Construct a terminated_string_view from a raw char*.
   * \param       data Contains the string of characters.
   * \param       length The length of the string including \0.
   * \pre         The input data must be a valid c-string. Otherwise ara::core::Abort is called.
   * \internal
   *              - #10 Check that the string fits a \0 character.
   *              - #20 Check that the data pointer is valid.
   *              - #30 Check that only the last character is \0
   * \endinternal
   */
  BasicCStringView(const_pointer data, size_type length) noexcept
      : BasicCStringView(ara::core::in_place, data, (length - 1)) {
    // #20 Check that the data pointer is valid
    if (data == nullptr) {
      ara::core::Abort(
          "vac::container::BasicCStringView<char>::BasicCStringView(const_pointer, size_type): No nullptr allowed");
    }
    // #10 Check that the string fits a \0 character
    if (length == 0) {
      ara::core::Abort(
          "vac::container::BasicCStringView<char>::BasicCStringView(const_pointer, size_type): Empty Strings cannot be "
          "zero terminated");
    }
    // #30 Check that only the last character is \0
    if (this->delegate_.find(static_cast<value_type>('\0')) != (length - 1)) {
      ara::core::Abort(
          "vac::container::BasicCStringView<char>::BasicCStringView(const_pointer, size_type): String is not null "
          "terminated");
    }
  }

  /*!
   * \brief   Templated constructor to convert from arbitrary containers.
   * \param   container A container providing contiguous data, ContainerType::data() and ContainerType::size().
   * \pre     The contained data must be a valid null-terminated c-string. Otherwise ara::core::Abort is called.
   * \details Is constexpr starting with C++14.
   */
  template <typename ContainerType, typename DataType = decltype(std::declval<ContainerType const>().data()),
            typename = typename std::enable_if<std::is_same<DataType, const_pointer>::value>::type>
  explicit BasicCStringView(ContainerType const& container) noexcept
      : BasicCStringView(container.data(), container.size()) {}

  // VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed
  // VECTOR Next Construct VectorC++-V3.0.1: MD_VAC_V3-0-1_cStyleArraysShouldNotBeUsed
  /*!
   * \brief  Creates a view of a string literal.
   * \tparam N Size of the char array, including null terminator.
   * \param  array C string literal.
   * \pre    The input data must be a valid null-terminated c-string. Otherwise ara::core::Abort is called.
   */
  template <std::size_t N>
  explicit BasicCStringView(Char const (&array)[N]) noexcept : BasicCStringView(&array[0], N) {}

  /*!
   * \brief   Explicitly disallow construction from only a raw pointer.
   * \details This deletion avoids constructing from an implicitly converted temporary std::string.
   *          Uses template arguments to avoid ambiguity towards array type.
   * \tparam  T Type of raw pointer.
   * \param   ptr The raw pointer.
   */
  template <typename T,
            typename = std::enable_if_t<(std::is_same<T, Char const*>::value) || (std::is_same<T, Char*>::value)>>
  explicit BasicCStringView(T ptr) = delete;

  /*!
   * \brief   Constructor to convert from std::string.
   * \param   std_string An std::string to construct the CStringView from.
   * \details Is constexpr starting with C++14. This constructor is used because std::string violates the assumption of
   *          the templated constructor. For std::string, size() does not include the null terminator byte.
   */
  explicit BasicCStringView(std::string const& std_string) noexcept
      : BasicCStringView(std_string.data(), std_string.size() + 1) {}

  /*!
   * \brief   Constructor to convert from ara::core::String.
   * \param   ara_core_string An ara::core::String to construct the CStringView from.
   * \details This constructor is used because ara::core::String violates the assumption of the templated
   *          constructor. For ara::core::String, size() does not include the null terminator byte.
   */
  explicit BasicCStringView(ara::core::String const& ara_core_string) noexcept
      : BasicCStringView(ara_core_string.data(), ara_core_string.size() + 1) {}

  // VECTOR Disable AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Swap the contents of two string views.
   * \param  that The string view to swap.
   * \return void.
   */
  auto swap(BasicCStringView& that) noexcept -> void { std::swap(this->delegate_, that.delegate_); }

  /*!
   * \brief Copy constructor.
   */
  constexpr BasicCStringView(BasicCStringView const&) noexcept = default;

  /*!
   * \brief Copy assignment.
   * \return A reference to the assigned-to object.
   */
  auto operator=(BasicCStringView const&) & noexcept -> BasicCStringView& = default;

  /*!
   * \brief Move constructor.
   */
  constexpr BasicCStringView(BasicCStringView&&) noexcept = default;

  /*! \brief Default destructor. */
  ~BasicCStringView() noexcept = default;

  /*!
   * \brief  Move assignment.
   * \return A reference to the assigned-to object.
   */
  auto operator=(BasicCStringView&&) & noexcept -> BasicCStringView& = default;
  // VECTOR Enable AutosarC++17_10-A15.5.1

  /*!
   * \brief  Get the length of the string without \0 terminator.
   * \return The length of the string without \0 terminator.
   */
  constexpr auto size() const noexcept -> size_type { return this->delegate_.size() - 1; }

  /*!
   * \brief  Get the length of the string without \0 terminator.
   * \return The length of the string without \0 terminator.
   */
  constexpr auto length() const noexcept -> size_type { return this->size(); }

  /*!
   * \brief  Determine whether the string is empty.
   * \return True if the string is empty, otherwise false.
   */
  constexpr auto empty() const noexcept -> bool { return this->size() == 0; }

  /*!
   * \brief  Get the raw data.
   * \return The raw data.
   */
  constexpr auto data() const noexcept -> const_pointer { return this->delegate_.data(); }

  /*!
   * \brief  Get the raw C string.
   * \return The raw C string.
   */
  constexpr auto c_str() const noexcept -> const_pointer { return this->data(); }

  /*!
   * \brief  Access a character at a specific index.
   * \param  index The location in the contained string.
   * \return A reference to the character at the specified index.
   * \pre    Index must be in bounds. Otherwise ara::core::Abort is called.
   */
  auto at(size_type index) const noexcept -> const_reference {
    // Extra check so we don't return the terminating null byte
    if (index >= this->size()) {
      ara::core::Abort("vac::container::BasicCStringView<char>::at(size_type): Index out of range");
    }
    return this->delegate_.at(index);
  }

  /*!
   * \brief  Access a character at a specific location.
   * \param  index The location in the contained string.
   * \return A reference to the character at the specified index.
   * \pre    Index must be in bounds. The behaviour is undefined otherwise.
   */
  auto operator[](size_type index) const noexcept -> const_reference { return this->at(index); }

  /*!
   * \brief  Iterator to the start of the string.
   * \return Iterator to the start of the string.
   */
  constexpr auto cbegin() const noexcept -> const_iterator { return this->delegate_.cbegin(); }

  /*!
   * \brief  Iterator past the end  of the string.
   * \return Iterator past the end  of the string.
   */
  constexpr auto cend() const noexcept -> const_iterator { return &this->delegate_.at(this->size()); }

  /*!
   * \brief  Iterator to the start of the string.
   * \return Iterator to the start of the string.
   */
  constexpr auto begin() const noexcept -> const_iterator { return this->cbegin(); }

  /*!
   * \brief  Iterator past the end  of the string.
   * \return Iterator past the end  of the string.
   */
  constexpr auto end() const noexcept -> const_iterator { return this->cend(); }

  /*!
   * \brief  Reference to the first character.
   * \return Reference to the first character.
   */
  auto front() const noexcept -> const_reference { return this->delegate_.front(); }

  /*!
   * \brief  Reference to the last character.
   * \return A reference to the last character.
   * \pre    size() must not be zero. Otherwise ara::core::Abort is called.
   */
  auto back() const noexcept -> const_reference {
    if (this->size() == 0) {
      ara::core::Abort("vac::container::BasicCStringView<char>::back(): Cannot call back on empty view");
    }
    return this->delegate_.at(this->size() - 1);
  }

  /*!
   * \brief   Returns a view of the substring.
   * \details Returns a view of the substring [pos, pos + rcount),
   *          where rcount is the smaller of count and size() - pos.
   * \param   pos Position of the first character.
   * \param   count Requested length.
   * \return  The substring view; empty if pos>=size().
   */
  auto substr(size_type pos = 0, size_type count = ara::core::basic_string_view<Char>::npos) const noexcept
      -> ara::core::basic_string_view<Char> {
    return this->delegate_.substr(pos, count);
  }

  // VECTOR Next Construct AutosarC++17_10-A13.5.2: MD_VAC_A13.5.2_userDefinedConversionOperatorCStringView
  /*!
   * \brief  Decays the C string view into a normal, non-zero terminated string view.
   * \return The decayed string_view without the trailing \0.
   */
  constexpr operator ara::core::basic_string_view<Char>() const noexcept { return {this->data(), this->size()}; }

  /*!
   * \brief     Create a string view from a literal that is guaranteed to be null terminated.
   * \param     data Contains the string of characters.
   * \param     length The length of the string excluding \0.
   * \return    The constructed C String.
   * \details   Needed because of GCC bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61648
   *            Otherwise the user-defined literals could be declared as a friend instead.
   * \attention This function does no check if the string is really \0 terminated and shall only be used by _sv
   *            operator.
   */
  constexpr static auto FromLiteral(Char const* data, std::size_t length) noexcept -> BasicCStringView<Char> {
    return {ara::core::in_place, data, length};
  }

  /*!
   * \brief  Create a null-terminated string view from an std::basic_string.
   * \param  basic_string Contains the string of characters.
   * \return The constructed C String.
   */
  template <typename Allocator>
  static auto FromString(std::basic_string<Char, std::char_traits<Char>, Allocator> const& basic_string) noexcept
      -> BasicCStringView<Char> {
    return {ara::core::in_place, basic_string.c_str(), basic_string.size()};
  }

  /*!
   * \brief  Create a null-terminated string view from an ara::core::BasicString.
   * \param  basic_string Contains the string of characters.
   * \return The constructed C String.
   */
  template <typename Allocator>
  static auto FromString(ara::core::BasicString<Allocator> const& basic_string) noexcept -> BasicCStringView<Char> {
    return {ara::core::in_place, basic_string.c_str(), basic_string.size()};
  }

  /*!
   * \brief   Creates an owned std::string from the CStringView.
   * \details Terminates if the allocator of the returned string throws.
   * \param   alloc Allocator for string instantiation.
   * \return  The created string.
   */
  template <typename Allocator = memory::PhaseManagedAllocator<Char>>
  auto ToString(Allocator const& alloc = Allocator()) const noexcept
      -> std::basic_string<Char, std::char_traits<Char>, Allocator> {
    return {this->data(), this->size(), alloc};
  }

  /*!
   * \brief  Creates an owned ara::core::String from the CStringView.
   * \param  alloc Allocator for string instantiation.
   * \return The created string.
   */
  template <typename Allocator = memory::PhaseManagedAllocator<Char>>
  auto ToAraCoreString(Allocator const& alloc = Allocator()) const noexcept -> ara::core::BasicString<Allocator> {
    return {this->data(), this->size(), alloc};
  }

 private:
  /*!
   * \brief Construct a terminated_string_view from a string with length without checks.
   * \param data Contains the string of characters.
   * \param length The length of the string excluding \0.
   */
  constexpr BasicCStringView(ara::core::in_place_t, const_pointer data, size_type length) noexcept
      : delegate_(data, length + 1) {}

  /*!
   * \brief Delegate String View.
   */
  storage_t delegate_;
};

/*!
 * \brief  Enable formatted output for BasicCStringView.
 * \tparam Char The type of character encoding.
 * \param  os The output stream to write in.
 * \param  sv The string_view to write.
 * \return The same reference to the output stream.
 */
template <typename Char>
auto operator<<(std::basic_ostream<Char>& os, BasicCStringView<Char> const& sv) noexcept(false)
    -> std::basic_ostream<Char>& {
  return os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
}

/*!
 * \brief  Checks if the contents of both string views are the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if both contents are the same.
 */
template <typename Char>
constexpr auto operator==(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept -> bool {
  ara::core::basic_string_view<Char> const temp_lhs{lhs};
  ara::core::basic_string_view<Char> const temp_rhs{rhs};
  return temp_lhs == temp_rhs;
}

/*!
 * \brief  Checks if the contents of both string views are the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if both contents are the same.
 */
template <typename Char>
constexpr auto operator==(BasicCStringView<Char> const& lhs, ara::core::basic_string_view<Char> const& rhs) noexcept
    -> bool {
  return ara::core::basic_string_view<Char>(lhs) == rhs;
}

/*!
 * \brief  Checks if the contents of both string views are not the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if contents are not the same.
 */
template <typename Char>
constexpr auto operator!=(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept -> bool {
  return !(lhs == rhs);
}

/*!
 * \brief  Checks if the contents of both string views are not the same.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if contents are not the same.
 */
template <typename Char>
constexpr auto operator!=(BasicCStringView<Char> const& lhs, ara::core::basic_string_view<Char> const& rhs) noexcept
    -> bool {
  return !(ara::core::basic_string_view<Char>(lhs) == rhs);
}

/*!
 * \brief  Checks if the contents of this string view is smaller than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller than the other.
 */
template <typename Char>
constexpr auto operator<(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept -> bool {
  ara::core::basic_string_view<Char> const temp_lhs{lhs};
  ara::core::basic_string_view<Char> const temp_rhs{rhs};
  return temp_lhs < temp_rhs;
}

/*!
 * \brief  Checks if the contents of this string view is smaller than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller than the other.
 */
template <typename Char>
constexpr auto operator<(BasicCStringView<Char> const& lhs, ara::core::basic_string_view<Char> const& rhs) noexcept
    -> bool {
  return ara::core::basic_string_view<Char>(lhs) < rhs;
}

/*!
 * \brief  Checks if the contents of this string view is smaller or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller or equal to the other.
 */
template <typename Char>
constexpr auto operator<=(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept -> bool {
  return !(rhs < lhs);
}

/*!
 * \brief  Checks if the contents of this string view is smaller or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is smaller or equal to the other.
 */
template <typename Char>
constexpr auto operator<=(BasicCStringView<Char> const& lhs, ara::core::basic_string_view<Char> const& rhs) noexcept
    -> bool {
  return ara::core::basic_string_view<Char>(lhs) <= rhs;
}

/*!
 * \brief  Checks if the contents of this string view is larger than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger than the other.
 */
template <typename Char>
constexpr auto operator>(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept -> bool {
  return rhs < lhs;
}

/*!
 * \brief  Checks if the contents of this string view is larger than the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger than the other.
 */
template <typename Char>
constexpr auto operator>(BasicCStringView<Char> const& lhs, ara::core::basic_string_view<Char> const& rhs) noexcept
    -> bool {
  return ara::core::basic_string_view<Char>(lhs) > rhs;
}

/*!
 * \brief  Checks if the contents of this string view is larger or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger or equal to the other.
 */
template <typename Char>
constexpr auto operator>=(BasicCStringView<Char> const& lhs, BasicCStringView<Char> const& rhs) noexcept -> bool {
  return !(lhs < rhs);
}

/*!
 * \brief  Checks if the contents of this string view is larger or equal to the other.
 * \param  lhs The first string view to compare.
 * \param  rhs The second string view to compare.
 * \return True if this is larger or equal to the other.
 */
template <typename Char>
constexpr auto operator>=(BasicCStringView<Char> const& lhs, ara::core::basic_string_view<Char> const& rhs) noexcept
    -> bool {
  return ara::core::basic_string_view<Char>(lhs) >= rhs;
}

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_C_STRING_VIEW_H_
