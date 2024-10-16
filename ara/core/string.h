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
/*!        \file  ara/core/string.h
 *        \brief  SWS core type ara::core::String.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_STRING_H_
#define LIB_VAC_INCLUDE_ARA_CORE_STRING_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include "ara/core/abort.h"
#include "ara/core/string_view.h"
#include "vac/memory/phase_managed_allocator.h"

namespace ara {
namespace core {

namespace detail {
/*! \brief Alias to set the maximum size of the capacity to determine the small string optimization */
constexpr uint8_t small_string_optimization_max_size{15};
}  // namespace detail

/*! \brief StringView type. */
using StringView = ara::core::StringView;

// VECTOR Next Construct AutosarC++17_10-M8.5.1: MD_VAC_M8.5.1_fieldIsNotInitializedInConstructor
// VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor
// VECTOR Next Construct Metric-OO.WMC.One: MD_VAC_Metric-OO.WMC.One
/*!
 * \brief   A data type that represents sequences of characters.
 * \details Any modifying operation performed on an object of this type must not result in
 *          size() > max_size().
 * \tparam  AllocatorAllocator type, vac::memory::PhaseManagedAllocator<char>.
 * \trace   CREQ-208384
 * \vpublic
 */
template <typename Allocator = vac::memory::PhaseManagedAllocator<char>>
class BasicString final {
 public:
  /*!
   * \brief Alias for the type of traits in this String.
   */
  using traits_type = std::char_traits<char>;

  /*!
   * \brief Alias for the type of value in this String.
   */
  using value_type = typename traits_type::char_type;

  /*!
   * \brief Alias for the type of allocator in this String.
   */
  using allocator_type = Allocator;

  /*!
   * \brief Alias for the type of parameters that indicate a size or a number of values in this String.
   */
  using size_type = typename std::allocator_traits<Allocator>::size_type;

  /*!
   * \brief Alias for the type of parameters that indicate a difference of indexes into this String.
   */
  using difference_type = typename std::allocator_traits<Allocator>::difference_type;

  /*!
   * \brief Alias for the type of reference in this String.
   */
  using reference = value_type&;

  /*!
   * \brief Alias for the type of const reference in this String.
   */
  using const_reference = value_type const&;

  /*!
   * \brief Alias for the type of pointer in this String.
   */
  using pointer = typename std::allocator_traits<Allocator>::pointer;

  /*!
   * \brief Alias for the type of const pointer in this String.
   */
  using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

  /*!
   * \brief Alias for the type of iterator in this String.
   */
  using iterator = pointer;

  /*!
   * \brief Alias for the type of const iterator in this String.
   */
  using const_iterator = const_pointer;

  /*!
   * \brief Alias for the type of reverse iterator in this String.
   */
  using reverse_iterator = std::reverse_iterator<iterator>;

  /*!
   * \brief Alias for the type of const reverse iterator in this String.
   */
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /*!
   * \brief Value returned by various member functions when they fail.
   */
  static size_type const npos;

  /*!
   * \brief    Temporary constructor to improve compatibility with the old String implementation.
   * \vprivate
   */
  explicit BasicString(std::string const& str) noexcept : BasicString(str.c_str()) {}

  /*!
   * \brief Default constructor of String.
   */
  BasicString() noexcept : data_{local_buf_}, size_{}, allocated_capacity_{0} { SetLength(0); }

  /*!
   * \brief Construct a new String from a given allocator.
   * \param a Allocator to construct the String.
   */
  explicit BasicString(Allocator const& a) noexcept : BasicString(local_buf_, a, 0, 0) { SetLength(0); }

  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief Copy constructor of String.
   * \param str The source String used to construct the new String.
   */
  BasicString(BasicString const& str) noexcept
      : BasicString(local_buf_,
                    std::allocator_traits<Allocator>::select_on_container_copy_construction(str.get_allocator()), 0,
                    0) {
    ConstructStrItr(str.begin(), str.end());
  }

  // VECTOR Next Construct AutosarC++17_10-A12.8.1: MD_VAC_A12.8.1_moveConstructorHasSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  // VECTOR Next Construct AutosarC++17_10-A12.8.4: MD_VAC_A12.8.4_moveConstructorShallNotUseCopySemantics
  /*!
   * \brief Move constructor of String.
   * \param str Source String.
   */
  BasicString(BasicString&& str) noexcept : BasicString(local_buf_, std::move(str.GetAllocatorLocal()), 0, 0) {
    if (str.IsLocal()) {
      static_cast<void>(traits_type::copy(static_cast<pointer>(local_buf_), str.local_buf_,
                                          static_cast<std::uint8_t>(local_capacity_ + 1)));
    } else {
      data_.dat_ = str.DataLocal();
      allocated_capacity_ = str.allocated_capacity_;
    }
    size_ = str.size();
    str.data_.dat_ = str.local_buf_;
    str.SetLength(0);
  }

  /*!
   * \brief Construct a new String from copying a subString.
   * \param str String to construct the String.
   * \param pos First character index to copy from.
   * \param n Number of characters to copy from.
   * \param a Allocator to construct the String.
   * \pre   pos <= str.size() must be true.
   */
  BasicString(BasicString const& str, size_type pos, size_type n = npos, Allocator const& a = Allocator()) noexcept
      : BasicString(local_buf_, a, 0, 0) {
    if (pos > str.size()) {
      Abort("ara::core::BasicString::BasicString: Position is invalid!");
    }
    size_type const effective_length{std::min(n, str.size() - pos)};

    // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    const_iterator const start{str.begin() + pos};
    ConstructStrItr(start, start + effective_length);
    // VECTOR Enable AutosarC++17_10-M5.0.15
  }

  // VECTOR Next Construct AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
  // VECTOR Next Construct AutosarC++17_10-M5.0.16: MD_VAC_M5.0.16_pointerArithmetic
  /*!
   * \brief   Construct a new String from a given pointer points to a character String.
   * \details If s is nullptr, and n is zero, an empty string will be created.
   * \param   s Char array to construct the String.
   * \param   n Number of characters to copy from.
   * \param   a Allocator to construct the String.
   * \pre     s must not be nullptr while n is zero.
   * \pre     n must not exceed max_size().
   */
  BasicString(value_type const* s, size_type n, Allocator const& a = Allocator()) noexcept
      : BasicString(local_buf_, a, 0, 0) {
    if (n > this->max_size()) {
      Abort("ara::core::BasicString::BasicString: The number of characters cannot exceed the maximum!");
    }
    if ((n > 0) && (s == nullptr)) {
      Abort(
          "ara::core::BasicString::BasicString: Pointer cannot be nullptr"
          " while the number of characters to copy is greater than zero ");
    }

    if (s != nullptr) {
      // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
      ConstructStrItr(s, s + n);
    }
  }
  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  /*!
   * \brief   Construct a new String from a given C String.
   * \details If s is nullptr, and n is zero, an empty string will be created.
   * \param   s C String to construct the String.
   * \param   a Allocator to construct the String.
   * \pre     s must not be nullptr.
   * \pre     s must not be nullptr while n is zero.
   * \pre     n must not exceed max_size().
   */
  BasicString(value_type const* s, Allocator const& a = Allocator()) noexcept  // NOLINT
      : BasicString(s, ((s == nullptr) ? 0 : traits_type::length(s)), a) {}

  /*!
   * \brief Construct a new String with n copies of character c.
   * \param n Number of characters to construct the String.
   * \param c characters to construct the String.
   * \param a Allocator to construct the String.
   * \pre   n must not be npos.
   * \pre   n must not exceed max_size().
   */
  BasicString(size_type n, value_type c, Allocator const& a = Allocator()) noexcept : BasicString(local_buf_, a, 0, 0) {
    size_type const this_max_size{this->max_size()};
    if ((n == npos) || (n > this_max_size)) {
      Abort("ara::core::BasicString::BasicString: The number of characters cannot exceed the maximum!");
    }
    ConstructStrInt(n, c);
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief Construct a new String from given range.
   * \param begin Start of the range.
   * \param end End of the range.
   * \param a Allocator to construct the String.
   * \pre   The number of elements in the range defined by begin and end must not exceed max_size()
   */
  template <class InputIterator>
  BasicString(InputIterator begin, InputIterator end, Allocator const& a = Allocator()) noexcept
      : BasicString(local_buf_, a, 0, 0) {
    ConstructStrItr(begin, end);
  }

  // VECTOR Next Construct AutosarC++17_10-A8.5.4: MD_VAC_A8.5.4_initializerlistShallBeDefinedInClasses
  // VECTOR Next Construct VectorC++-V8.5.4: MD_VAC_V8-5-4_ctorShallNotBeDefinedToAcceptStdInitializerList
  /*!
   * \brief Construct a new String from given initializer list.
   * \param il Initializer list to construct the String.
   * \param a Allocator to construct the String.
   */
  explicit BasicString(std::initializer_list<value_type> il, Allocator const& a = Allocator()) noexcept
      : BasicString(local_buf_, a, 0, 0) {
    ConstructStrItr(il.begin(), il.end());
  }

  /*!
   * \brief Construct a new String from a given String.
   * \param str The source String used to construct the new String.
   * \param a Allocator to construct the String.
   */
  BasicString(BasicString const& str, Allocator const& a) noexcept : BasicString(local_buf_, a, 0, 0) {
    ConstructStrItr(str.begin(), str.end());
  }

  /*!
   * \brief Move construct a new String.
   * \param str The source String used to construct the new String.
   * \param a Allocator to construct the String.
   */
  BasicString(BasicString&& str, Allocator const& a) noexcept : BasicString(local_buf_, a, 0, 0) {
    if (str.IsLocal()) {
      static_cast<void>(traits_type::copy(static_cast<pointer>(local_buf_), str.local_buf_,
                                          static_cast<std::uint8_t>(local_capacity_ + 1)));
      size_ = str.size();
      str.SetLength(0);
    } else if (str.GetAllocatorLocal() == a) {
      data_.dat_ = str.DataLocal();
      size_ = str.size_;
      allocated_capacity_ = str.allocated_capacity_;
      str.data_.dat_ = str.local_buf_;
      str.SetLength(0);
    } else {
      ConstructStrItr(str.begin(), str.end());
    }
  }

  /*!
   * \brief Constructs a String from a StringView.
   * \param sv StringView to construct from.
   * \pre   sv.size() must not exceed max_size().
   * \vpublic
   */
  explicit BasicString(StringView sv) noexcept : BasicString(sv.data(), sv.size()) {}

  /*!
   * \brief  Constructs a String from a container convertible to a StringView.
   * \tparam T Container type that is convertible to a StringView.
   * \param  t Reference to a container.
   * \param  pos Start position for reading the container.
   * \param  n Number of characters to read.
   * \param  a Allocator to construct the String.
   * \pre    n must not exceed max_size().
   * \vpublic
   */
  template <typename T, typename = std::enable_if_t<std::is_convertible<T const&, StringView>::value>,
            typename = std::enable_if_t<!std::is_convertible<T const&, char const*>::value>>
  BasicString(T const& t, size_type pos, size_type n, Allocator const& a = Allocator()) noexcept
      : BasicString(BasicString(StringView(t).data(), StringView(t).size()), pos, n, a) {}

  // VECTOR Next Construct AutosarC++17_10-A13.5.2: MD_VAC_A13.5.2_userDefinedConversionOperatorCStringView
  /*!
   * \brief Converts the String to a StringView.
   */
  operator StringView() const noexcept { return StringView{data(), size()}; }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief Destructor of the String.
   */
  ~BasicString() noexcept { Dispose(); }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Assigns the contents of String into this String.
   * \param  str Source String.
   * \return Reference to the modified String.
   */
  BasicString& operator=(BasicString const& str) & noexcept {
    bool const alloc_local{GetAllocatorLocal() != str.GetAllocatorLocal()};
    if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
      if ((!IsLocal()) && alloc_local) {
        if (str.size() <= local_capacity_) {
          Dispose();
          data_.dat_ = local_buf_;
          SetLength(0);
        } else {
          size_type const len{str.size()};
          allocator_type alloc{str.get_allocator()};
          pointer const ptr{std::allocator_traits<Allocator>::allocate(alloc, len + 1)};
          Dispose();
          data_.dat_ = ptr;
          allocated_capacity_ = len;
          SetLength(len);
        }
      }
      this->GetAllocatorLocal() = str.GetAllocatorLocal();
    }
    return this->assign(str);
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Move assign operator of this String.
   * \param  str Source String.
   * \return Reference to the modified String.
   */
  BasicString& operator=(BasicString&& str) & noexcept {
    bool alloc_local{GetAllocatorLocal() != str.GetAllocatorLocal()};
    if ((!IsLocal()) && (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) &&
        (alloc_local)) {
      Dispose();
      data_.dat_ = local_buf_;
      SetLength(0);
    }

    if (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
      this->GetAllocatorLocal() = std::move(str.GetAllocatorLocal());
    }

    alloc_local = this->get_allocator() == str.get_allocator();
    if (str.IsLocal()) {
      if (str.size() != 0) {
        this->CopyOptimized(this->DataLocal(), str.DataLocal(), str.size());
      }
      this->SetLength(str.size());
    } else if ((std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) || (alloc_local)) {
      if (!IsLocal()) {
        std::allocator_traits<Allocator>::deallocate(GetAllocatorLocal(), this->DataLocal(), allocated_capacity_ + 1);
      }
      data_.dat_ = str.DataLocal();
      size_ = str.size();
      allocated_capacity_ = str.allocated_capacity_;
      str.data_.dat_ = str.local_buf_;
    } else {
      assign(str);
    }
    str.clear();
    return *this;
  }

  /*!
   * \brief Assigns the contents of pointer into this String.
   * \param s Source pointer.
   * \return Reference to the modified String.
   */
  BasicString& operator=(value_type const* s) & noexcept { return this->assign(s); }

  /*!
   * \brief  Assigns the content of a character into this String.
   * \param  c Source character.
   * \return Reference to the modified String.
   */
  BasicString& operator=(value_type c) & noexcept {
    this->assign(static_cast<size_type>(1), c);
    return *this;
  }

  /*!
   * \brief  Assigns the contents of a initializer list into this String.
   * \param  il Source initializer list.
   * \return Reference to the modified String.
   */
  BasicString& operator=(std::initializer_list<value_type> il) & noexcept {
    this->assign(il.begin(), il.size());
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  // VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_refQualifier
  /*!
   * \brief  Assigns the contents of a StringView into this String.
   * \param  sv StringView to read from.
   * \return Reference to the modified String.
   * \vpublic
   */
  BasicString& operator=(StringView sv) & noexcept { return assign(sv); }

  /*!
   * \brief    Temporary assignment operator to improve compatibility with the old String implementation.
   * \vprivate
   */
  BasicString& operator=(std::string str) & noexcept { return assign(str.c_str()); }

  /*!
   * \brief  Return an iterator to the first element of this String.
   * \return An iterator to the first element of this String.
   */
  iterator begin() noexcept { return iterator(this->DataLocal()); }

  /*!
   * \brief  Return a const_iterator to the first element of this String.
   * \return A const_iterator to the first element of this String.
   */
  const_iterator begin() const noexcept { return const_iterator(this->DataLocal()); }

  // VECTOR NC AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
  /*!
   * \brief  Return an iterator to the last element of this String.
   * \return An iterator to the last element of this String.
   */
  iterator end() noexcept { return iterator(this->DataLocal() + this->size()); }

  // VECTOR NC AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
  // VECTOR Next Construct AutosarC++17_10-M5.0.16: MD_VAC_M5.0.16_pointerArithmetic
  /*!
   * \brief  Return a const_iterator to the last element of this String.
   * \return A const_iterator to the last element of this String.
   */
  const_iterator end() const noexcept { return const_iterator(this->DataLocal() + this->size()); }

  /*!
   * \brief  Return a reverse_iterator to the last element of this String.
   * \return A reverse_iterator to the last element of this String.
   */
  reverse_iterator rbegin() noexcept { return reverse_iterator(this->end()); }

  /*!
   * \brief  Return a const_reverse_iterator to the last element of this String.
   * \return A const_reverse_iterator to the last element of this String.
   */
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(this->end()); }

  /*!
   * \brief  Return a reverse_iterator to the first element of this String.
   * \return A reverse_iterator to the first element of this String.
   */
  reverse_iterator rend() noexcept { return reverse_iterator(this->begin()); }

  /*!
   * \brief  Return a const_reverse_iterator to the first element of this String.
   * \return A const_reverse_iterator to the first element of this String.
   */
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(this->begin()); }

  /*!
   * \brief  Return a const_iterator to the first element of this String.
   * \return A const_iterator to the first element of this String.
   */
  const_iterator cbegin() const noexcept { return const_iterator(this->DataLocal()); }

  /*!
   * \brief  Return a const_iterator to the last element of this String.
   * \return A const_iterator to the last element of this String.
   */
  const_iterator cend() const noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return const_iterator(this->DataLocal() + this->size());
  }

  /*!
   * \brief  Return a const_reverse_iterator to the last element of this String.
   * \return A const_reverse_iterator to the last element of this String.
   */
  const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(this->end()); }

  /*!
   * \brief  Return a const_reverse_iterator to the first element of this String.
   * \return A const_reverse_iterator to the first element of this String.
   */
  const_reverse_iterator crend() const noexcept { return const_reverse_iterator(this->begin()); }

  /*!
   * \brief  Return the size of this String.
   * \return The size of this String.
   */
  size_type size() const noexcept { return size_; };

  /*!
   * \brief  Return the size of this String.
   * \return The size of this String.
   */
  size_type length() const noexcept { return size_; }

  /*!
   * \brief  Return the maximum size of this String.
   * \return The maximum size of this String.
   */
  size_type max_size() const noexcept { return (std::allocator_traits<Allocator>::max_size(get_allocator()) - 1) / 2; }

  /*!
   * \brief Resize the String to the specified number of characters.
   * \param n Number of the characters.
   * \param c Character to insert when the size of this String is increased.
   * \pre   This operation must not result in the size of the string exceeding max_size().
   */
  void resize(size_type n, value_type c) noexcept {
    if (n > this->max_size()) {
      Abort("ara::core::BasicString::resize: The number of characters cannot exceed the maximum!");
    }
    size_type const size{this->size()};
    if (size > n) {
      this->SetLength(n);
    } else if (size < n) {
      // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_underFlow
      // VECTOR NL AutosarC++17_10-A4.7.1: MD_VAC_A4.7.1_underFlow
      this->append(n - size, c);
    } else {  // Nothing to be done when size == n
    }
  }

  /*!
   * \brief Resize the String to the specified number of characters.
   * \param n Number of the characters.
   * \pre   This operation must not result in the size of the string exceeding max_size().
   */
  void resize(size_type n) noexcept { this->resize(n, value_type{}); }

  /*!
   * \brief  Return the total number of characters that this String can hold.
   * \return The total number of characters that this String can hold.
   * \pre   This operation must not result in the size of the string exceeding max_size().
   */
  size_type capacity() const noexcept {
    return IsLocal() ? static_cast<size_type>(local_capacity_) : allocated_capacity_;
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief   Reserve enough memory for specified number of characters.
   * \details Calling reserve() with a res_arg argument less than capacity() is in effect a
   *          non-binding shrink request. A call with res_arg <= size() is in effect a
   *          non-binding shrink-to-fit request.
   * \param   res_arg Number of the characters.
   * \pre     res_arg must exceed max_size().
   */
  void reserve(size_type res_arg = 0) noexcept {
    if (res_arg > this->max_size()) {
      Abort("ara::core::BasicString::reserve: The number of characters cannot exceed the maximum!");
    }
    if (res_arg < length()) {
      res_arg = length();
    }
    size_type const cap{capacity()};

    if (res_arg != cap) {
      if ((res_arg > cap) || (res_arg > static_cast<size_type>(local_capacity_))) {
        pointer const data_p{Create(res_arg, cap)};
        this->CopyOptimized(data_p, this->DataLocal(), this->size() + 1);
        Dispose();
        data_.dat_ = data_p;
        allocated_capacity_ = res_arg;
      } else if (!IsLocal()) {
        this->CopyOptimized(static_cast<pointer>(local_buf_), this->DataLocal(), this->size() + 1);
        Dispose();
        data_.dat_ = local_buf_;
      } else {  // Nothing to be done
      }
    }
  }

  /*!
   * \brief A non-binding request to reduce capacity() to size().
   */
  void shrink_to_fit() noexcept {
    if (capacity() > size()) {
      reserve(0);
    }
  }

  /*!
   * \brief Erase the String to empty.
   */
  void clear() noexcept { static_cast<void>(this->erase(this->begin(), this->end())); }

  /*!
   * \brief  Return true if the String is empty.
   * \return True if the String is empty.
   */
  bool empty() const noexcept { return this->size() == 0; }

  /*!
   * \brief  Access the specified character in this String.
   * \param  pos The index of the specified character.
   * \return A const_reference to the specified character.
   * \pre    pos <= size() must be true.
   */
  const_reference operator[](size_type pos) const noexcept {
    if (pos > size()) {
      Abort("ara::core::BasicString::operator[]: Position is invalid!");
    }
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return data_.dat_[pos];
  }

  /*!
   * \brief  Access the specified character in this String.
   * \param  pos The index of the specified character.
   * \return A reference to the specified character.
   * \pre    pos <= size() must be true.
   */
  reference operator[](size_type pos) noexcept {
    if (pos > size()) {
      Abort("ara::core::BasicString::operator[]: Position is invalid!");
    }
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return data_.dat_[pos];
  }

  /*!
   * \brief  Access the specified character in this String.
   * \param  pos The index of the specified character.
   * \return A const_reference to the specified character.
   * \pre    pos < size() must be true.
   */
  const_reference at(size_type pos) const noexcept {
    if (pos >= size()) {
      Abort("ara::core::BasicString::at: Position is invalid!");
    }
    return this->operator[](pos);
  }

  /*!
   * \brief  Access the specified character in this String.
   * \param  pos The index of the specified character.
   * \return A reference to the specified character.
   * \pre    pos < size() must be true.
   */
  reference at(size_type pos) noexcept {
    if (pos >= size()) {
      Abort("ara::core::BasicString::at: Position is invalid!");
    }
    return this->operator[](pos);
  }

  /*!
   * \brief   Return a const reference to the first character of this String.
   * \return  A const reference to the first character of this String.
   * \pre     The string must not be empty when calling this function.
   */
  const_reference front() const noexcept {
    if (empty()) {
      Abort("ara::core::BasicString::front: The String cannot be empty!");
    }
    return this->operator[](0);
  }

  /*!
   * \brief   Return a reference to the first character of this String.
   * \return  A reference to the first character of this String.
   * \pre     The string must not be empty when calling this function.
   */
  reference front() noexcept {
    if (empty()) {
      Abort("ara::core::BasicString::front: The String cannot be empty!");
    }
    return this->operator[](0);
  }

  /*!
   * \brief   Return a const reference to the last character of this String.
   * \return  A const reference to the last character of this String.
   * \pre     The string must not be empty when calling this function.
   */
  const_reference back() const noexcept {
    if (empty()) {
      Abort("ara::core::BasicString::back: The String cannot be empty!");
    }
    return this->operator[](size() - 1);
  }

  /*!
   * \brief   Return a reference to the last character of this String.
   * \return  A reference to the last character of this String.
   * \pre     The string must not be empty when calling this function.
   */
  reference back() noexcept {
    if (empty()) {
      Abort("ara::core::BasicString::back: The String cannot be empty!");
    }
    return this->operator[](size() - 1);
  }

  // VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
  /*!
   * \brief  Append a String to this String.
   * \param  str String to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& operator+=(BasicString const& str) & noexcept { return this->append(str); }

  // VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
  /*!
   * \brief  Append a C String to this String.
   * \param  s C String to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& operator+=(value_type const* s) & noexcept { return this->append(s); }

  // VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
  /*!
   * \brief  Append a character to this String.
   * \param  c Character to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& operator+=(value_type c) & noexcept { return this->append(1, c); }

  // VECTOR Next Construct AutosarC++17_10-M5.17.1 : MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
  /*!
   * \brief  Append an initializer list to this String.
   * \param  il Initializer list to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& operator+=(std::initializer_list<value_type> il) & noexcept {
    return this->append(il.begin(), il.size());
  }

  // VECTOR Next Construct AutosarC++17_10-A12.8.7: MD_VAC_A12.8.7_refQualifier
  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  // VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17.1_overloadForCorrespondingAssignmenOperator
  /*!
   * \brief  Append a StringView to this String.
   * \param  sv StringView to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& operator+=(StringView sv) noexcept { return append(sv); }

  /*!
   * \brief  Append a String to this String.
   * \param  str String to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& append(BasicString const& str) noexcept { return this->append(str.data(), str.size()); }

  /*!
   * \brief  Append a subString to this String.
   * \param  str String to append.
   * \param  pos Index of the first character of the String to append.
   * \param  n Number of characters to append.
   * \return Reference to this String.
   * \pre    pos <= str.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& append(BasicString const& str, size_type pos, size_type n = npos) noexcept {
    if (pos > str.size()) {
      Abort("ara::core::BasicString::append: Position is invalid!");
    }
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return this->append(str.data() + pos, std::min(n, str.size() - pos));
  }

  /*!
   * \brief  Append a sub C String to this String.
   * \param  s C String to append.
   * \param  n Number of characters to append.
   * \return Reference to this String.
   * \pre    s must not be nullptr when n is greater than zero.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& append(value_type const* s, size_type n) noexcept {
    if ((this->size() + n) > this->max_size()) {
      Abort("ara::core::BasicString::append: The number of characters cannot exceed the maximum!");
    }
    if ((s == nullptr) && (n != 0)) {
      Abort("ara::core::BasicString::append: C String pointer cannot be nullptr!");
    }
    return this->replace(this->size(), static_cast<size_type>(0), s, n);
  }

  /*!
   * \brief  Append a C String to this String.
   * \param  s C String to append.
   * \return Reference to this String.
   * \pre    s must not be nullptr.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& append(value_type const* s) noexcept {
    if (s == nullptr) {
      Abort("ara::core::BasicString::append: Pointer cannot be nullptr!");
    }
    return this->append(s, traits_type::length(s));
  }

  /*!
   * \brief  Append characters to this String.
   * \param  n Number of characters to append.
   * \param  c Characters to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& append(size_type n, value_type c) noexcept { return this->append(BasicString(n, c)); }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief   Append a range of characters to this String.
   * \param   first Iterator referencing the first character to append.
   * \param   last Iterator referencing the last character to append.
   * \return  Reference to this String.
   * \pre     The iterators must refer to a valid range. It is not allowed for first to point after last.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  template <class InputIterator>
  BasicString& append(InputIterator first, InputIterator last) noexcept {
    if (first > last) {
      Abort("ara::core::BasicString::append: Invalid range!");
    }
    return this->append(BasicString(first, last));
  }

  /*!
   * \brief  Append an initializer list to this String.
   * \param  il Initializer list to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& append(std::initializer_list<value_type> il) noexcept { return this->append(il.begin(), il.size()); }

  /*!
   * \brief  Append a StringView to the String.
   * \param  sv StringView to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& append(StringView sv) noexcept {
    append(sv.data(), sv.size());
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Appends a container to the String.
   * \tparam T Container type that is convertible to a StringView.
   * \param  t Reference to a container.
   * \param  pos Start position for reading the container.
   * \param  n Number of character to read, npos to read until the end.
   * \return Reference to this String.
   * \pre    pos <= t.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  template <typename T, typename = std::enable_if_t<std::is_convertible<T const&, StringView>::value>,
            typename = std::enable_if_t<!std::is_convertible<T const&, char const*>::value>>
  BasicString& append(T const& t, size_type pos, size_type n = npos) noexcept {
    // VECTOR Next Construct AutosarC++17_10-A8.5.2: MD_VAC_A8.5.2_provideExplicitInitializer
    StringView sv{t};
    if (pos > sv.size()) {
      Abort("ara::core::BasicString::append: Position is invalid!");
    }
    sv = sv.substr(pos, n);
    if (sv.size() > 0) {
      this->append(sv);
    }
    return *this;
  }

  /*!
   * \brief  Append a single character to this String.
   * \param  c Character to append.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  void push_back(value_type c) noexcept { this->append(static_cast<size_type>(1), c); }

  /*!
   * \brief  Assign the contents of a String into this String.
   * \param  str Source String.
   * \return Reference to the modified String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& assign(BasicString const& str) noexcept { return this->assign(str, static_cast<size_type>(0), npos); }

  /*!
   * \brief  Assign the contents of String into this String.
   * \param  str Source String.
   * \return Reference to the modified String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& assign(BasicString&& str) noexcept {
    this->swap(str);
    return *this;
  }

  /*!
   * \brief  Assign the contents of a subString into this String.
   * \param  str Source String.
   * \param  pos Index of the first character to assign from str..
   * \param  n Number of characters.
   * \return Reference to the modified String.
   * \pre    pos <= str.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& assign(BasicString const& str, size_type pos, size_type n = npos) noexcept {
    if (pos > str.size()) {
      Abort("ara::core::BasicString::assign: Position is invalid!");
    }
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return this->assign(str.data() + pos, std::min(n, str.size() - pos));
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Assign the contents of a sub C String into this String.
   * \param  s Source C String.
   * \param  n Number of characters.
   * \return Reference to the modified String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& assign(value_type const* s, size_type n) noexcept {
    if (n > this->max_size()) {
      Abort("ara::core::BasicString::assign: The number of characters cannot exceed the maximum!");
    }
    return this->replace(static_cast<size_type>(0), this->size(), s, n);
  }

  /*!
   * \brief  Assign the contents of a C String into this String.
   * \param  s Source C String.
   * \return Reference to the modified String.
   * \pre    s must not be nullptr.
   */
  BasicString& assign(value_type const* s) noexcept {
    if (s == nullptr) {
      Abort("ara::core::BasicString::assign: Pointer cannot be nullptr!");
    }
    return this->assign(s, traits_type::length(s));
  }

  /*!
   * \brief  Assign the contents of an initializer list into this String.
   * \param  il Source initializer list.
   * \return Reference to the modified String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& assign(std::initializer_list<value_type> il) noexcept { return this->assign(il.begin(), il.end()); }

  /*!
   * \brief  Assign the content of a character into this String.
   * \param  n Number of characters.
   * \param  c Characters to assign.
   * \return Reference to the modified String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& assign(size_type n, value_type c) noexcept { return this->assign(BasicString(n, c)); }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Assign a range of characters to this String.
   * \param  first Iterator referencing the first character to assign.
   * \param  last Iterator referencing the last character to assign.
   * \return Reference to this String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  template <class InputIterator>
  BasicString& assign(InputIterator first, InputIterator last) noexcept {
    return this->assign(BasicString(first, last));
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Assign the contents of a StringView to the String.
   * \param  sv StringView to read from.
   * \return Reference to the modified String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   * \vpublic
   */
  BasicString& assign(StringView sv) noexcept {
    assign(sv.data(), sv.size());
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Assign the contents of a container to the String.
   * \tparam T Container type that is convertible to a StringView.
   * \param  t Reference to a container.
   * \param  pos Start position for reading the container.
   * \param  n Number of character to read, npos to read until the end.
   * \return Reference to the modified String.
   * \pre    t <= str.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   * \vpublic
   */
  template <typename T, typename = std::enable_if_t<std::is_convertible<T const&, StringView>::value>,
            typename = std::enable_if_t<!std::is_convertible<T const&, char const*>::value>>
  BasicString& assign(T const& t, size_type pos, size_type n = npos) noexcept {
    // VECTOR Next Construct AutosarC++17_10-A8.5.2: MD_VAC_A8.5.2_provideExplicitInitializer
    StringView sv{t};
    if (pos > sv.size()) {
      Abort("ara::core::BasicString::assign: Position is invalid!");
    }
    sv = sv.substr(pos, n);
    this->assign(sv.data(), sv.size());
    return *this;
  }

  /*!
   * \brief  Insert the contents of a String into this String.
   * \param  pos Iterator referencing location in String to insert.
   * \param  str The String to insert.
   * \return Reference to the modified String.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& insert(size_type pos, BasicString const& str) noexcept {
    return this->insert(pos, str.data(), str.size());
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Insert the contents of a subString into this String.
   * \param  pos1 Iterator referencing location in the String to insert.
   * \param  str The subString to insert.
   * \param  pos2 Start of characters in the subString to insert.
   * \param  n Number of characters.
   * \return Reference to the modified String.
   * \pre    pos1 <= size() must be true.
   * \pre    pos2 <= str.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& insert(size_type pos1, BasicString const& str, size_type pos2, size_type n = npos) noexcept {
    size_type const this_size{this->size()};
    size_type const str_size{str.size()};
    if ((pos1 > this_size) || (pos2 > str_size)) {
      Abort("ara::core::BasicString::insert: Position is invalid!");
    }
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return this->insert(pos1, str.data() + pos2, std::min(n, str_size - pos2));
  }

  /*!
   * \brief  Insert the contents of a C String into this String.
   * \param  pos Iterator referencing location in String to insert.
   * \param  s The C String to insert.
   * \param  n Number of characters.
   * \return Reference to the modified String.
   * \pre    pos <= size() must be true.
   * \pre    s must not be nullptr when n is greater than zero.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& insert(size_type pos, value_type const* s, size_type n) noexcept {
    if ((s == nullptr) && (n != 0)) {
      Abort("ara::core::BasicString::insert: C String pointer cannot be nullptr!");
    }
    return this->replace(pos, static_cast<size_type>(0), s, n);
  }

  /*!
   * \brief  Insert the contents of a C String into this String.
   * \param  pos Iterator referencing location in String to insert.
   * \param  s The C String to insert.
   * \return Reference to the modified String.
   * \pre    pos <= size() must be true.
   * \pre    s must not be nullptr.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& insert(size_type pos, value_type const* s) noexcept {
    return this->insert(pos, s, (s == nullptr) ? 0 : traits_type::length(s));
  }

  /*!
   * \brief  Insert the contents of characters into this String.
   * \param  pos Iterator referencing location in String to insert.
   * \param  n Number of characters.
   * \param  c Characters to insert.
   * \return Reference to the modified String.
   * \pre    pos <= size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& insert(size_type pos, size_type n, value_type c) noexcept {
    return this->insert(pos, BasicString(n, c));
  }

  /*!
   * \brief  Insert the contents of one character into this String.
   * \param  p Const iterator referencing position to insert.
   * \param  c The character to insert.
   * \return Iterator referencing the first inserted character.
   * \pre    p must be a valid iterator on this string.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  iterator insert(const_iterator p, value_type c) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    size_type const pos{static_cast<size_type>(p - this->begin())};
    this->replace(p, p, static_cast<size_type>(1), c);
    // VECTOR NC AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return iterator(this->data() + pos);
  }

  /*!
   * \brief  Insert the contents of characters into this String.
   * \param  p Const iterator referencing position to insert.
   * \param  n Number of characters.
   * \param  c Character to insert.
   * \return Iterator referencing the first inserted character.
   * \pre    p must be a valid iterator on this string.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  iterator insert(const_iterator p, size_type n, value_type c) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    size_type const pos{static_cast<size_type>(p - this->begin())};
    this->replace(p, p, n, c);
    // VECTOR NC AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return iterator(this->data() + pos);
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Insert a range of characters into this String.
   * \param  p Const iterator referencing position to insert.
   * \param  first Start of the range.
   * \param  last End of the range.
   * \return Iterator referencing the first inserted character.
   * \pre    p must be a valid iterator on this string.
   * \pre    first and last must refer to a valid range.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  template <class InputIterator>
  auto insert(const_iterator p, InputIterator first, InputIterator last) noexcept -> iterator {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    size_type const pos{static_cast<size_type>(p - this->begin())};
    this->replace(p, p, first, last);
    // VECTOR NC AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return iterator(this->data() + pos);
  }

  /*!
   * \brief  Insert a range of characters into this String.
   * \param  p Const iterator referencing position to insert.
   * \param  il Initializer list to insert.
   * \return Iterator referencing the first inserted character.
   * \pre    p must be a valid iterator on this string.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  iterator insert(const_iterator p, std::initializer_list<value_type> il) noexcept {
    return this->insert(p, il.begin(), il.end());
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Inserts the contents of a StringView into the String.
   * \param  pos Position in the String to insert to.
   * \param  sv StringView to insert from.
   * \return Reference to the modified String.
   * \pre    pos <= size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& insert(size_type pos, StringView sv) noexcept {
    if (sv.size() > 0) {
      this->insert(pos, sv.data(), sv.size());
    }
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Inserts the contents of a container into the String.
   * \tparam T Container type that is convertible to a StringView.
   * \param  pos1 Position in the String to insert to.
   * \param  t Reference to a container.
   * \param  pos2 Position to start reading the container from.
   * \param  n Number of characters to read from the container, npos to read until the end.
   * \return Reference to the modified String.
   * \pre    pos1 <= size() must be true.
   * \pre    pos2 <= t.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  template <typename T, typename = std::enable_if_t<std::is_convertible<T const&, StringView>::value>,
            typename = std::enable_if_t<!std::is_convertible<T const&, char const*>::value>>
  BasicString& insert(size_type pos1, T const& t, size_type pos2, size_type n = npos) noexcept {
    // VECTOR NL AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    StringView const sv{t};
    if ((pos1 > this->size()) || (pos2 > sv.size())) {
      Abort("ara::core::BasicString::insert: Position is invalid!");
    }
    return this->insert(pos1, sv.substr(pos2, n));
  }

  /*!
   * \brief  Remove characters from this String.
   * \param  pos Index of the first character to remove.
   * \param  n Number of characters.
   * \return Reference to this String.
   * \pre    pos <= size() must be true.
   */
  BasicString& erase(size_type pos = 0, size_type n = npos) noexcept {
    if (pos > this->size()) {
      Abort("ara::core::BasicString::erase: Position is invalid!");
    }
    if (n == npos) {
      this->SetLength(pos);
    } else if (n != 0) {
      size_type const effective_length{std::min(n, this->size() - pos)};
      size_type const n_trailing_chars{this->size() - pos - effective_length};
      if (n_trailing_chars != 0) {
        // VECTOR Next Line AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
        this->MoveOptimized(this->DataLocal() + pos, this->data() + pos + effective_length, n_trailing_chars);
      }
      this->SetLength(this->size() - effective_length);
    } else {  // Nothing to be done when there is no trailing chars.
    }
    return *this;
  }

  /*!
   * \brief  Remove one character from this String.
   * \param  p Iterator referencing the character to remove.
   * \return Iterator referencing the same location after removal.
   * \pre    p must be a valid iterator on this string.
   */
  iterator erase(const_iterator p) noexcept {
    const_iterator const end_iterator{this->end()};
    if ((p < this->begin()) || (p > end_iterator)) {
      Abort("ara::core::BasicString::erase: p must be a valid iterator on the String.");
    }
    // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    size_type const pos{static_cast<size_type>(p - this->begin())};
    size_type const n_trailing_chars{this->size() - pos - 1};
    if (n_trailing_chars != 0) {
      this->MoveOptimized(this->DataLocal() + pos, this->data() + pos + 1, n_trailing_chars);
    }
    this->SetLength(this->size() - 1);
    // VECTOR NC AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    return iterator(this->data() + pos);
    // VECTOR Enable AutosarC++17_10-M5.0.15
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief   Remove a range of characters from this String.
   * \param   first Iterator referencing the first character to remove.
   * \param   last Iterator referencing the last character to remove.
   * \return  Iterator referencing location of the first character after removal.
   * \pre     Requires that 'first' and 'last' are valid iterators on the String.
   */
  iterator erase(const_iterator first, const_iterator last) noexcept {
    const_iterator const end_iterator{this->end()};
    if ((first < this->begin()) || (first > last) || (last > end_iterator)) {
      Abort("ara::core::BasicString::erase: first and last must define a valid range [first,last)");
    }
    // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    size_type const pos{static_cast<size_type>(first - this->begin())};
    if (last == this->end()) {
      this->SetLength(pos);
    } else {
      // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
      size_type const distance{static_cast<size_type>(last - first)};
      size_type const n_trailing_chars{this->size() - pos - distance};
      if (distance != 0) {
        this->MoveOptimized(this->DataLocal() + pos, this->data() + pos + distance, n_trailing_chars);
      }
      this->SetLength(this->size() - distance);
    }
    // VECTOR NC AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    return iterator(this->data() + pos);
    // VECTOR Enable AutosarC++17_10-M5.0.15
  }

  /*!
   * \brief   Remove the last character from this String.
   * \pre     The string must not be empty when calling this function.
   */
  void pop_back() noexcept {
    if (empty()) {
      Abort("ara::core::BasicString::pop_back: The String cannot be empty!");
    }
    this->erase(this->size() - 1, 1);
  }

  /*!
   * \brief  Replace the contents of this String from a String.
   * \param  pos1 Index of the first character to replace.
   * \param  n1 Number of characters.
   * \param  str String to replace from.
   * \return Reference to this String.
   * \pre    pos1 <= size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(size_type pos1, size_type n1, BasicString const& str) noexcept {
    return this->replace(pos1, n1, str.data(), str.size());
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Replace the contents of this String from a String.
   * \param  pos1 Index of the first character to replace.
   * \param  n1 Number of characters.
   * \param  str String to replace from.
   * \param  pos2 Index of the first character of String to replace from.
   * \param  n2 Number of characters.
   * \return Reference to this String.
   * \pre    pos1 <= size() must be true.
   * \pre    pos2 <= str.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(size_type pos1, size_type n1, BasicString const& str, size_type pos2,
                       size_type n2 = npos) noexcept {
    size_type const str_size{str.size()};
    if ((pos1 > this->size()) || (pos2 > str_size)) {
      Abort("ara::core::BasicString::replace: Position is invalid!");
    }
    size_type const effective_length{std::min(n2, str_size - pos2)};
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return this->replace(pos1, n1, str.data() + pos2, effective_length);
  }

  /*!
   * \brief  Replace the contents of this String from a C String.
   * \param  pos1 Index of the first character to replace.
   * \param  n1 Number of characters.
   * \param  s C String to replace from.
   * \param  n2 Number of characters.
   * \return Reference to this String.
   * \pre    pos1 <= size() must be true.
   * \pre    s must not be nullptr when n2 is greater than zero.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(size_type pos1, size_type n1, value_type const* s, size_type n2) noexcept {
    // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    if (pos1 > this->size()) {
      Abort("ara::core::BasicString::replace: Position is invalid!");
    }
    if ((s == nullptr) && (n2 != 0)) {
      Abort("ara::core::BasicString::replace: C String pointer cannot be nullptr!");
    }
    size_type const effective_length_n1{std::min(n1, this->size() - pos1)};
    size_type const how_much_left{this->size() - pos1 - effective_length_n1};
    size_type const new_size{(this->size() + n2) - effective_length_n1};
    size_type const new_capacity{new_size};
    size_type const this_max_size{this->max_size()};

    if (((this->size() - effective_length_n1) >= (this_max_size - n2)) || (new_capacity > this_max_size)) {
      Abort("ara::core::BasicString::replace: The number of characters cannot exceed the maximum!");
    }
    if (new_size <= this->capacity()) {
      pointer const data_p{this->DataLocal() + pos1};
      AdjustReplaceSize(data_p, effective_length_n1, how_much_left, s, n2);
    } else {
      AdjustReplaceSize(pos1, how_much_left, effective_length_n1, new_capacity, s, n2);
    }
    this->SetLength(new_size);
    return *this;
    // VECTOR Enable AutosarC++17_10-M5.0.15
  }

  /*!
   * \brief  Replace the contents of this String from a C String.
   * \param  pos Index of the first character to replace.
   * \param  n Number of characters.
   * \param  s C String to replace from.
   * \return Reference to this String.
   * \pre    pos <= size() must be true.
   * \pre    s must not be nullptr.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(size_type pos, size_type n, value_type const* s) noexcept {
    return this->replace(pos, n, s, (s == nullptr) ? 0 : traits_type::length(s));
  }

  /*!
   * \brief  Replace the contents of this String from characters.
   * \param  pos1 Index of the first character to replace.
   * \param  n1 Number of characters.
   * \param  n2 Number of characters.
   * \param  c Characters to replace from.
   * \return Reference to this String.
   * \pre    pos1 <= size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(size_type pos1, size_type n1, size_type n2, value_type c) noexcept {
    return this->replace(pos1, n1, BasicString(n2, c));
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Replace the contents of this String from a range of characters of a String.
   * \param  i1 Iterator referencing start of the range to replace.
   * \param  i2 Iterator referencing end of the range to replace.
   * \param  str String to replace from.
   * \return Reference to this String.
   * \pre    i1 and i2 must be a valid range on this string.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(const_iterator i1, const_iterator i2, BasicString const& str) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    return this->replace(static_cast<size_type>(i1 - this->begin()), static_cast<size_type>(i2 - i1), str);
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Replace the contents of this String from a range of characters of a C String.
   * \param  i1 Iterator referencing start of the range to replace.
   * \param  i2 Iterator referencing end of the range to replace.
   * \param  s C String to replace from.
   * \param  n Number of characters.
   * \return Reference to this String.
   * \pre    i1 and i2 must be a valid range on this string.
   * \pre    s must not be nullptr.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(const_iterator i1, const_iterator i2, value_type const* s, size_type n) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    return this->replace(static_cast<size_type>(i1 - this->begin()), static_cast<size_type>(i2 - i1), s, n);
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Replace the contents of this String from a range of characters of a C String.
   * \param  i1 Iterator referencing start of the range to replace.
   * \param  i2 Iterator referencing end of the range to replace.
   * \param  s C String to replace from.
   * \return Reference to this String.
   * \pre    i1 and i2 must be a valid range on this string.
   * \pre    s must not be nullptr.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(const_iterator i1, const_iterator i2, value_type const* s) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    return this->replace(static_cast<size_type>(i1 - this->begin()), static_cast<size_type>(i2 - i1), s,
                         traits_type::length(s));
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Replace the contents of this String from characters.
   * \param  i1 Iterator referencing start of the range to replace.
   * \param  i2 Iterator referencing end of the range to replace.
   * \param  n Number of characters.
   * \param  c Characters to replace from.
   * \return Reference to this String.
   * \pre    i1 and i2 must be a valid range on this string.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(const_iterator i1, const_iterator i2, size_type n, value_type c) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    return this->replace(static_cast<size_type>(i1 - this->begin()), static_cast<size_type>(i2 - i1),
                         BasicString(n, c));
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Replace the contents of this String from a range of characters with range.
   * \param  i1 Iterator referencing start of the range to replace.
   * \param  i2 Iterator referencing end of the range to replace.
   * \param  j1 Iterator referencing start of the range to replace from.
   * \param  j2 Iterator referencing end of the range to replace from.
   * \return Reference to this String.
   * \pre    i1 and i2 must be a valid range on this string.
   * \pre    j1 and j2 must be a valid range.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  template <class InputIterator>
  BasicString& replace(const_iterator i1, const_iterator i2, InputIterator j1, InputIterator j2) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    return this->replace(static_cast<size_type>(i1 - this->begin()), static_cast<size_type>(i2 - i1),
                         BasicString(j1, j2));
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Replace the contents of this String from an initializer list.
   * \param  it1 Iterator referencing start of the range to replace.
   * \param  it2 Iterator referencing end of the range to replace.
   * \param  init_list Initializer list to replace from.
   * \return Reference to this String.
   * \pre    it1 and it2 must be a valid range on this string.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(const_iterator it1, const_iterator it2, std::initializer_list<value_type> init_list) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    return this->replace(static_cast<size_type>(it1 - this->begin()), static_cast<size_type>(it2 - it1),
                         init_list.begin(), init_list.size());
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Replaces part of the String with the contents of a StringView.
   * \param  pos1 Position in the String to replace from.
   * \param  n1 Number of characters to replace.
   * \param  sv StringView to read from.
   * \return Reference to the modified String.
   * \pre    pos1 <= size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(size_type pos1, size_type n1, StringView sv) noexcept {
    this->replace(pos1, n1, sv.data(), sv.size());
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Replaces part of the String with the contents of a container.
   * \tparam T Container type that is convertible to a StringView.
   * \param  pos1 Position in the String to replace from. pos1 <= size() must be true.
   * \param  n1 Number of characters to replace.
   * \param  t Reference to a container.
   * \param  pos2 Position to start reading the container from. pos2 <= t.size() must be true.
   * \param  n2 Of characters to read from the container, npos to read until the end.
   * \return Reference to the modified String.
   * \pre    pos1 <= size() must be true.
   * \pre    pos2 <= t.size() must be true.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  template <typename T, typename = std::enable_if_t<std::is_convertible<T const&, StringView>::value>,
            typename = std::enable_if_t<!std::is_convertible<T const&, char const*>::value>>
  BasicString& replace(size_type pos1, size_type n1, T const& t, size_type pos2, size_type n2 = npos) noexcept {
    // VECTOR NL AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    StringView sv{t};
    if ((pos1 > this->size()) || (pos2 > sv.size())) {
      Abort("ara::core::BasicString::replace: Position is invalid!");
    }
    sv = sv.substr(pos2, n2);
    return replace(pos1, n1, sv);
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_parametersBasedOnBuiltInTypes
  /*!
   * \brief  Replaces an iterator-bounded part of the String with the contents of a StringView.
   * \param  i1 Iterator to the start of the replacement range (inclusive).
   * \param  i2 Iterator to the end of the replacement range (exclusive).
   * \param  sv StringView to read from.
   * \return Reference to the modified String.
   * \pre    i1 and i2 must be a valid iterators on this string.
   * \pre    This operation must not result in the size of the string exceeding max_size().
   */
  BasicString& replace(const_iterator i1, const_iterator i2, StringView sv) noexcept {
    // VECTOR NC AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    // To use this workaround is to adapt to toolchains implementing erroneous std::String::replace signature since it
    // does not comply with C++11
    this->replace(static_cast<size_type>(i1 - begin()), static_cast<size_type>(i2 - i1), sv.data(), sv.size());
    return *this;
  }

  /*!
   * \brief  Copy subString into the C String.
   * \param  s Dest C String.
   * \param  n Number of characters to be copied.
   * \param  pos Index of the first character to copy. pos <= size() must be true.
   * \return Number of characters being copied.
   * \pre    s must not be nullptr.
   * \pre    pos <= size() must be true.
   */
  size_type copy(value_type* s, size_type n, size_type pos = 0) const noexcept {
    size_type effective_length{0};
    // VECTOR NL AutosarC++17_10-M0.1.2: MD_VAC_M0.1.2_infeasiblePaths
    if ((s != nullptr)) {
      if (pos <= this->size()) {
        effective_length = std::min(n, this->size() - pos);
        if (effective_length != 0) {
          // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
          this->CopyOptimized(s, this->DataLocal() + pos, effective_length);
        }
      } else {
        Abort("ara::core::BasicString::copy: Position is invalid!");
      }
    } else {
      Abort("ara::core::BasicString::copy: C String pointer cannot be nullptr!");
    }
    return effective_length;
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Swap the contents with a String.
   * \param s String to swap with.
   */
  void swap(BasicString& s) noexcept { std::swap(*this, s); }

  /*!
   * \brief  Return const pointer to contents.
   * \return Const pointer to contents.
   */
  const_pointer c_str() const noexcept { return this->DataLocal(); }

  /*!
   * \brief  Return const pointer to contents.
   * \return Const pointer to contents.
   */
  const_pointer data() const noexcept { return this->DataLocal(); }

  /*!
   * \brief  Return copy of allocator in this String.
   * \return Copy of allocator in this String.
   */
  allocator_type get_allocator() const noexcept { return this->GetAllocatorLocal(); }

  /*!
   * \brief  Find position of a String in this String.
   * \param  str String to find.
   * \param  pos Index of character to search from.
   * \return Index of start of first occurrence, or npos if there are no occurrences.
   */
  size_type find(BasicString const& str, size_type pos = 0) const noexcept {
    return this->find(str.data(), pos, str.size());
  }

  /*!
   * \brief  Find position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \param  n Number of characters.
   * \return Index of start of first occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find(value_type const* s, size_type pos, size_type n) const noexcept {
    return this->find(StringView{s, n}, pos);
  }

  /*!
   * \brief  Find position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \return Index of start of first occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find(value_type const* s, size_type pos = 0) const noexcept { return this->find(StringView{s}, pos); }

  /*!
   * \brief  Find position of a character in this String.
   * \param  c Character to find.
   * \param  pos Index of character to search from.
   * \return Index of first occurrence, or npos if there are no occurrences.
   */
  size_type find(value_type c, size_type pos = 0) const noexcept { return this->find(StringView{&c, 1}, pos); }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Finds the first subString equal to a StringView.
   * \param  sv StringView with data to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the first character of the found subString, or npos if not found.
   * \pre    pos <= size() must be true.
   */
  size_type find(StringView sv, size_type pos = 0) const noexcept { return StringView(*this).find(sv, pos); }

  /*!
   * \brief  Find last position of a String in this String.
   * \param  str String to find.
   * \param  pos Index of character to search from.
   * \return Index of start of last occurrence, or npos if there are no occurrences.
   */
  size_type rfind(BasicString const& str, size_type pos = npos) const noexcept {
    return this->rfind(StringView{str}, pos);
  }

  /*!
   * \brief  Find last position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \param  n Number of characters.
   * \return Index of start of last occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type rfind(value_type const* s, size_type pos, size_type n) const noexcept {
    return this->rfind(StringView{s, n}, pos);
  }

  /*!
   * \brief  Find last position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \return Index of start of last occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type rfind(value_type const* s, size_type pos = npos) const noexcept { return this->rfind(StringView{s}, pos); }

  /*!
   * \brief  Find last position of a character in this String.
   * \param  c Character to find.
   * \param  pos Index of character to search from.
   * \return Index of last occurrence, or npos if there are no occurrences.
   */
  size_type rfind(value_type c, size_type pos = npos) const noexcept { return this->rfind(StringView{&c, 1}, pos); }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Find the last subString equal to a StringView.
   * \param  sv StringView with data to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the first character of the found subString, or npos if not found.
   */
  size_type rfind(StringView sv, size_type pos = npos) const noexcept { return StringView(*this).rfind(sv, pos); }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Find position of a String in this String.
   * \param  str String to find.
   * \param  pos Index of character to search from.
   * \return Index of first occurrence, or npos if there are no occurrences.
   */
  size_type find_first_of(BasicString const& str, size_type pos = 0) const noexcept {
    return this->find_first_of(StringView{str}, pos);
  }

  /*!
   * \brief  Find position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \param  n Number of characters.
   * \return Index of first occurrence. If there are no occurrences or s is nullptr, String::npos is returned.
   */
  size_type find_first_of(value_type const* s, size_type pos, size_type n) const noexcept {
    StringView sv{};
    sv = StringView(s, n);
    return this->find_first_of(sv, pos);
  }

  /*!
   * \brief  Find position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \return Index of first occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find_first_of(value_type const* s, size_type pos = 0) const noexcept {
    return this->find_first_of(StringView{s}, pos);
  }

  /*!
   * \brief  Find position of a character in this String.
   * \param  c Character to find.
   * \param  pos Index of character to search from.
   * \return Index of first occurrence.
   */
  size_type find_first_of(value_type c, size_type pos = 0) const noexcept {
    return this->find_first_of(StringView{&c, 1}, pos);
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Finds the first character equal to one of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, or npos if not found.
   * \vpublic
   */
  size_type find_first_of(StringView sv, size_type pos = 0) const noexcept {
    return StringView{*this}.find_first_of(sv, pos);
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Find last position of a String in this String.
   * \param  str String to find.
   * \param  pos Index of character to search from.
   * \return Index of last occurrence, or npos if there are no occurrences.
   */
  size_type find_last_of(BasicString const& str, size_type pos = npos) const noexcept {
    return this->find_last_of(StringView{str}, pos);
  }

  /*!
   * \brief  Find last position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \param  n Number of characters.
   * \return Index of last occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find_last_of(value_type const* s, size_type pos, size_type n) const noexcept {
    StringView sv{};
    sv = StringView(s, n);
    return this->find_last_of(sv, pos);
  }

  /*!
   * \brief  Find last position of a C String in this String.
   * \param  s C String to find.
   * \param  pos Index of character to search from.
   * \return Index of last occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find_last_of(value_type const* s, size_type pos = npos) const noexcept {
    return this->find_last_of(StringView{s}, pos);
  }

  /*!
   * \brief  Find last position of a character in this String.
   * \param  c Character to find.
   * \param  pos Index of character to search from.
   * \return Index of last occurrence, or npos if there are no occurrences.
   */
  size_type find_last_of(value_type c, size_type pos = npos) const noexcept {
    StringView sv{};
    sv = StringView(&c, 1);
    return this->find_last_of(sv, pos);
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Finds the last character equal to one of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, of npos if not found.
   */
  size_type find_last_of(StringView sv, size_type pos = npos) const noexcept {
    return StringView(*this).find_last_of(sv, pos);
  }

  /*!
   * \brief  Finds the first character not equal to any of the characters in a String.
   * \param  str String to avoid.
   * \param  pos Index of character to search from.
   * \return Index of first occurrence, or npos if there are no occurrences.
   * \pre    pos <= size() must be true.
   */
  size_type find_first_not_of(BasicString const& str, size_type pos = 0) const noexcept {
    return this->find_first_not_of(StringView{str}, pos);
  }

  /*!
   * \brief  Finds the first character not equal to any of the characters in a C String.
   * \param  s C String to avoid.
   * \param  pos Index of character to search from.
   * \param  n Number of characters.
   * \return Index of first occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find_first_not_of(value_type const* s, size_type pos, size_type n) const noexcept {
    return this->find_first_not_of(StringView{s, n}, pos);
  }

  /*!
   * \brief  Finds the first character not equal to any of the characters in a C String.
   * \param  s C String to avoid.
   * \param  pos Index of character to search from.
   * \return Index of first occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find_first_not_of(value_type const* s, size_type pos = 0) const noexcept {
    return this->find_first_not_of(StringView{s}, pos);
  }

  /*!
   * \brief  Finds the first character not equal to a character.
   * \param  c Character to avoid.
   * \param  pos Index of character to search from.
   * \return Index of first occurrence, or npos if there are no occurrences.
   */
  size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept {
    StringView sv{};
    sv = StringView(&c, 1);
    return this->find_first_not_of(sv, pos);
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Finds the first character not equal to any of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, of npos if not found.
   */
  size_type find_first_not_of(StringView sv, size_type pos = 0) const noexcept {
    return StringView(*this).find_first_not_of(sv, pos);
  }

  /*!
   * \brief  Finds the last character not equal to any of the characters in a String.
   * \param  str String to avoid.
   * \param  pos Index of character to search from.
   * \return Index of last occurrence, or npos if there are no occurrences.
   */
  size_type find_last_not_of(BasicString const& str, size_type pos = npos) const noexcept {
    return this->find_last_not_of(StringView{str}, pos);
  }

  /*!
   * \brief  Finds the last character not equal to any of the characters in a C String.
   * \param  s C String to avoid.
   * \param  pos Index of character to search from.
   * \param  n Number of characters.
   * \return Index of last occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find_last_not_of(value_type const* s, size_type pos, size_type n) const noexcept {
    StringView sv{};
    sv = StringView(s, n);
    return this->find_last_not_of(sv, pos);
  }

  /*!
   * \brief  Finds the last character not equal to any of the characters in a C String.
   * \param  s C String to avoid.
   * \param  pos Index of character to search from.
   * \return Index of last occurrence. If there are no occurrences or s is nullptr, npos is returned.
   */
  size_type find_last_not_of(value_type const* s, size_type pos = npos) const noexcept {
    return this->find_last_not_of(StringView{s}, pos);
  }

  /*!
   * \brief  Finds the last character not equal to a character.
   * \param  c Character to avoid.
   * \param  pos Index of character to search from.
   * \return Index of last occurrence, or npos if there are no occurrences.
   */
  size_type find_last_not_of(value_type c, size_type pos = npos) const noexcept {
    StringView sv{};
    sv = StringView(&c, 1);
    return this->find_last_not_of(sv, pos);
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Finds the last character not equal to any of the characters in a StringView.
   * \param  sv StringView with characters to search for.
   * \param  pos Position in the String to begin the search from.
   * \return Position of the found character, of npos if not found.
   */
  size_type find_last_not_of(StringView sv, size_type pos = npos) const noexcept {
    return StringView(*this).find_last_not_of(sv, pos);
  }

  /*!
   * \brief  Return a subString of this String.
   * \param  pos Index of the first character. pos <= size() must be true.
   * \param  n Number of characters.
   * \return A subString of this String.
   * \pre    pos <= size() must be true.
   */
  BasicString substr(size_type pos = 0, size_type n = npos) const noexcept {
    if (pos > this->size()) {
      Abort("ara::core::BasicString::substr: Position is invalid!");
    }
    size_type const effective_length{std::min(n, this->size() - pos)};
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    return BasicString(this->data() + pos, effective_length);
  }

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief  Compare a String to this String.
   * \param  str String to compare.
   * \return Integer < 0, 0, or > 0.
   * \pre    pos <= size() must be true.
   */
  int compare(BasicString const& str) const noexcept {
    return this->compare(static_cast<size_type>(0), this->size(), str);
  }

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief  Compare a String to the subString of this String.
   * \param  pos1 Index of first character of subString.
   * \param  n1 Number of characters in this string to compare.
   * \param  str String to compare.
   * \return Integer < 0, 0, or > 0.
   * \pre    pos1 <= size() must be true.
   */
  int compare(size_type pos1, size_type n1, BasicString const& str) const noexcept {
    return this->compare(pos1, n1, str, static_cast<size_type>(0), str.size());
  }

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief  Compare a subString of a String to the subString of this String.
   * \param  pos1 Index of first character of the String. pos1 <= size() must be true.
   * \param  n1 Number of characters in this string to compare.
   * \param  str subString to compare.
   * \param  pos2 Index of first character of the subString. pos2 <= str.size() must be true.
   * \param  n2 Number of characters from the subString to compare.
   * \return Integer < 0, 0, or > 0.
   * \pre    pos1 <= size() must be true.
   * \pre    pos2 <= str.size() must be true.
   */
  int compare(size_type pos1, size_type n1, BasicString const& str, size_type pos2, size_type n2 = npos) const
      noexcept {
    size_type const str_size{str.size()};
    if ((pos1 > this->size()) || (pos2 > str_size)) {
      Abort("ara::core::BasicString::compare: Position is invalid!");
    }
    // VECTOR NL AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    StringView const sv_str{StringView{str}.substr(pos2, n2)};
    return StringView{*this}.substr(pos1, n1).compare(sv_str);
  }

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief   Compare a C String to this String.
   * \details Comparing against a nullptr is equivalent to comparing with an empty string.
   * \param   s C String to compare.
   * \return  Integer < 0, 0, or > 0.
   */
  int compare(value_type const* s) const noexcept { return this->compare(static_cast<size_type>(0), this->size(), s); }

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief   Compare a C String to the subString of this String.
   * \details Comparing against a nullptr is equivalent to comparing with an empty string.
   * \param   pos Index of first character of subString.
   * \param   n1 Number of characters in this string to compare.
   * \param   s C String to compare.
   * \return  Integer < 0, 0, or > 0.
   * \pre     pos <= size() must be true.
   */
  int compare(size_type pos, size_type n1, value_type const* s) const noexcept {
    return this->compare(pos, n1, s, (s == nullptr) ? 0 : traits_type::length(s));
  }

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief   Compare the first n characters of a character array to the subString of this String.
   * \details Comparing against a nullptr is equivalent to comparing with an empty string.
   * \param   pos Index of first character of this String. pos <= size() must be true.
   * \param   n1 Number of characters in this string to compare.
   * \param   s Character array to compare.
   * \param   n2 Number of characters from the characater array to compare.
   * \return  Integer < 0, 0, or > 0.
   * \pre     pos <= size() must be true.
   */
  int compare(size_type pos, size_type n1, value_type const* s, size_type n2) const noexcept {
    if (pos > this->size()) {
      Abort("ara::core::BasicString::compare: Position is invalid!");
    }
    // VECTOR NL AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    StringView const sv{s, n2};
    return StringView{*this}.substr(pos, n1).compare(sv);
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief  Compares the String to a StringView lexicographically.
   * \param  sv StringView to compare to.
   * \return Negative value if this is lexicographically before the sv parameter, positive value if
   *         this is after the sv parameter, and 0 if this String and sv are equivalent.
   */
  int compare(StringView sv) const noexcept { return StringView(*this).compare(sv); }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief  Compares the subString of this String to a StringView lexicographically.
   * \param  pos1 Starting position of the subString.
   * \param  n1 Number of characters in the subString.
   * \param  sv StringView to compare to.
   * \return Negative value if the subString is lexicographically before the sv parameter, positive value if
   *         the subString is after the sv parameter, and 0 if they are equivalent.
   * \pre    pos1 <= size() must be true.
   */
  int compare(size_type pos1, size_type n1, StringView sv) const noexcept {
    return StringView(*this).substr(pos1, n1).compare(sv);
  }

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  // VECTOR Next Construct AutosarC++17_10-A8.5.2: MD_VAC_A8.5.2_provideExplicitInitializer
  /*!
   * \brief  Compares the subString of this String to a subString of a container lexicographically.
   * \tparam T Container type that is convertible to a StringView.
   * \param  pos1 Starting position of the subString. pos1 <= size() must be true.
   * \param  n1 Number of characters in the subString.
   * \param  t Reference to a container.
   * \param  pos2 Starting position in the container. pos2 <= t.size() must be true.
   * \param  n2 Number of characters to read from the container.
   * \return Negative value if the subString is lexicographically before the container, positive value if
   *         the subString is after the container, and 0 if they are equivalent.
   * \pre    pos1 <= size() must be true.
   * \pre    pos2 <= t.size() must be true.
   */
  template <typename T>
  auto compare(size_type pos1, size_type n1, T const& t, size_type pos2, size_type n2 = npos) const noexcept
      -> std::enable_if_t<(std::is_convertible<T const&, StringView>::value) &&
                              (!std::is_convertible<T const&, char const*>::value),
                          int> {
    // VECTOR NL AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    StringView sv{t};
    size_type const this_size{this->size()};
    if ((pos1 > this_size) || (pos2 > sv.size())) {
      Abort("ara::core::BasicString::compare: Position is invalid!");
    }
    sv = sv.substr(pos2, n2);
    return StringView(*this).substr(pos1, n1).compare(sv);
  }

  /*!
   * \brief    Temporary string conversion operator to improve compatibility with the old String implementation.
   * \vprivate
   */
  explicit operator std::string() const noexcept { return std::string{this->data(), this->size()}; }

 private:
  /*! \brief  Capacity when the String is stored in a local buffer. */
  static uint8_t const local_capacity_{detail::small_string_optimization_max_size / sizeof(char)};

  // VECTOR Next Construct AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*! \brief  Delegate Constructor */
  BasicString(value_type (&dat)[local_capacity_ + 1], Allocator a, size_type size, size_type capacity) noexcept
      : data_{{dat}, {a}}, size_{size}, allocated_capacity_{capacity} {};

  /*! \brief  Helper method for replace(size_type, size_type, value_type const* , size_type) */
  void AdjustReplaceSize(size_type pos1, size_type how_much_left, size_type effective_length_n1, size_type new_capacity,
                         value_type const* s, size_type n2) noexcept {
    pointer const data_p{this->Create(new_capacity, this->capacity())};
    if (pos1 != 0) {
      this->CopyOptimized(data_p, this->data(), pos1);
    }
    // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmetic
    this->CopyOptimized(data_p + pos1, s, n2);
    if (how_much_left != 0) {
      this->CopyOptimized(data_p + pos1 + n2, this->data() + pos1 + effective_length_n1, how_much_left);
    }
    // VECTOR Enable AutosarC++17_10-M5.0.15
    this->Dispose();
    data_.dat_ = data_p;
    allocated_capacity_ = new_capacity;
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*! \brief  Helper method for replace(size_type, size_type, value_type const* , size_type) */
  void AdjustReplaceSize(pointer const& str_pos, size_type n1_length, size_type how_much_left, value_type const* s,
                         size_type n2) noexcept {
    // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    if (this->Disjunct(s)) {
      this->MoveOptimized(str_pos + n2, str_pos + n1_length, how_much_left);
      this->CopyOptimized(str_pos, s, n2);
    } else {
      if ((how_much_left != 0)) {
        // Move the right unreplaced part of the current String to the position
        // after the replaced position.
        this->MoveOptimized(str_pos + n2, str_pos + n1_length, how_much_left);
      }
      if ((n2 != 0) && (n2 <= n1_length)) {
        this->MoveOptimized(str_pos, s, n2);
      }
      AdjustReplaceSize(str_pos, n1_length, s, n2);
    }
    // VECTOR Enable AutosarC++17_10-M5.0.15
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*! \brief  Helper method for replace(size_type, size_type, value_type const* , size_type) */
  void AdjustReplaceSize(pointer const& str_pos, size_type n1_length, value_type const* s, size_type n2) noexcept {
    if (n2 > n1_length) {
      // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
      // VECTOR NL AutosarC++17_10-M5.0.16: MD_VAC_M5.0.16_pointerArithmetic
      if ((s + n2) <= (str_pos + n1_length)) {
        // Replace the current String with n2 characters from the source.
        this->MoveOptimized(str_pos, s, n2);
        // VECTOR SL AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_underFlow
        // VECTOR NC AutosarC++17_10-A4.7.1: MD_VAC_A4.7.1_underFlow
      } else if (s >= (str_pos + n1_length)) {
        // VECTOR NC AutosarC++17_10-M5.0.16: MD_VAC_M5.0.16_pointerArithmetic
        value_type const* const s_mutated{s + (n2 - n1_length)};
        // After moving the unreplaced part to the right, the source 's' has mutated. There is (n2 -n1_length)
        // characters added at the beginning, so the correct starting position of 's' now is 's + (n2 - n1_length)'.
        this->CopyOptimized(str_pos, s_mutated, n2);
      } else {
        // VECTOR NC AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
        // Determine how many characters are left without being affected by moving the unreplaced part to the right.
        size_type const left{static_cast<size_type>((str_pos + n1_length) - s)};

        // Move the unaffected characters to the beginning of the current String.
        this->MoveOptimized(str_pos, s, left);

        // Move the rest.
        this->CopyOptimized(str_pos + left, str_pos + n2, n2 - left);
      }
      // VECTOR Enable AutosarC++17_10-M5.0.15
    }
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypesg
  /*!
   * \brief  Wrapper function to construct String from a given range.
   * \tparam Iterator Iterator data type.
   * \param  beg Iterator referencing the start of the range.
   * \param  eng Iterator referencing the end of the range.
   */
  template <typename InIterator>
  void ConstructStrItr(InIterator beg, InIterator end) noexcept {
    ConstructStrItr(beg, end, typename std::iterator_traits<InIterator>::iterator_category{});
  }

  /*!
   * \brief  Specialization when iterator type is input iterator.
   * \tparam Iterator Iterator data type.
   * \param  beg Iterator referencing the start of the range.
   * \param  eng Iterator referencing the end of the range.
   * \pre    The number of elements in the range defined by beg and end must not exceed max_size()
   */
  template <typename InIterator>
  void ConstructStrItr(InIterator beg, InIterator end, std::input_iterator_tag) noexcept {
    size_type len{0};
    size_type capacity{static_cast<size_type>(local_capacity_)};

    while ((beg != end) && (len < capacity)) {
      // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
      data_.dat_[len++] = *beg;
      ++beg;
    }

    while (beg != end) {
      if (len == capacity) {
        if (len >= this->max_size()) {
          // The abort message refers to the constructor since this function is only called from it,
          // and it is more helpful to the user.
          Abort("ara::core::BasicString::BasicString: The number of characters cannot exceed the maximum!");
        }
        capacity = len + 1;
        // VECTOR Next Construct AutosarC++17_10-A7.1.1:
        // MD_VAC_A7.1.1_constexprOrConstSpecifiersShallBeUsedForImmutableDataDeclaration
        pointer const another{Create(capacity, len)};
        this->CopyOptimized(another, this->DataLocal(), len);
        Dispose();
        data_.dat_ = another;
        allocated_capacity_ = capacity;
      }
      data_.dat_[len++] = *beg;
      ++beg;
    }
    // VECTOR Enable AutosarC++17_10-M5.0.15
    SetLength(len);
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief   Specialization when iterator type is forward iterator.
   * \details beg and end must be valid iterators.
   * \tparam  Iterator Iterator data type.
   * \param   beg Iterator referencing the start of the range.
   * \param   eng Iterator referencing the end of the range.
   * \pre     The number of elements in the range defined by beg and end must not exceed max_size()
   */
  template <typename InIterator>
  void ConstructStrItr(InIterator beg, InIterator end, std::forward_iterator_tag) noexcept {
    size_type distance{static_cast<size_type>(std::distance(beg, end))};
    if (distance > this->max_size()) {
      // The abort message refers to the constructor since this function is only called from it,
      // and it is more helpful to the user.
      Abort("ara::core::BasicString::BasicString: The number of characters cannot exceed the maximum!");
    }
    if (distance > static_cast<size_type>(local_capacity_)) {
      // VECTOR NL AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
      data_.dat_ = this->Create(distance, static_cast<size_type>(0));
      allocated_capacity_ = distance;
    }
    this->CopyOptimizedItr(this->DataLocal(), beg, end);
    SetLength(distance);
  }

  /*!
   * \brief  Wrapper function to construct String from given characters.
   * \param  n Number of characters.
   * \param  c Characters to construct.
   */
  void ConstructStrInt(size_type n, value_type c) noexcept {
    if (n > static_cast<size_type>(local_capacity_)) {
      data_.dat_ = Create(n, static_cast<size_type>(0));
      allocated_capacity_ = n;
    }
    if (n > 0) {
      this->AssignOptimized(this->DataLocal(), n, c);
    }

    SetLength(n);
  }

  /*!
   * \brief  Verify if the characters of this String is stored locally.
   * \return True if it is stored locally.
   */
  bool IsLocal() const noexcept { return this->DataLocal() == local_buf_; }

  /*!
   * \brief  Deallocate the memories.
   */
  void Dispose() noexcept {
    if (!IsLocal()) {
      std::allocator_traits<Allocator>::deallocate(this->GetAllocatorLocal(), this->DataLocal(),
                                                   allocated_capacity_ + 1);
    }
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Optimized copy function.
   * \param  d Pointer to a character String to copy to.
   * \param  s Pointer to a character String to copy from.
   * \param  n Number of characters.
   */
  static void CopyOptimized(value_type* d, value_type const* s, size_type n) noexcept {
    if (n == 1) {
      traits_type::assign(*d, *s);
    } else {
      static_cast<void>(traits_type::copy(d, s, n));
    }
  }

  /*!
   * \brief  Optimized copy function for iterators.
   * \tparam Iterator Iterator data type.
   * \param  p Pointer to a character String to copy to.
   * \param  it1 Iterator referencing the start of the range.
   * \param  it2 Iterator referencing the end of the range.
   */
  template <typename Iterator>
  static void CopyOptimizedItr(value_type* p, Iterator it1, Iterator it2) noexcept {
    while (it1 != it2) {
      traits_type::assign(*p, *it1);
      ++it1;
      // VECTOR Disable AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
      ++p;
      // VECTOR Enable AutosarC++17_10-M5.0.15
    }
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Optimized copy function for char pointer iterators.
   * \param  p Pointer to a character String to copy to.
   * \param  it1 Char pointer iterator referencing the start of the range.
   * \param  it2 Char pointer iterator referencing the end of the range.
   */
  static void CopyOptimizedItr(value_type* p, iterator it1, iterator it2) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    CopyOptimized(p, it1, static_cast<size_type>(it2 - it1));
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Optimized copy function for const char pointer iterators.
   * \param  p Pointer to a character String to copy to.
   * \param  it1 Const char pointer iterator referencing the start of the range.
   * \param  it2 Const char pointer iterator referencing the end of the range.
   */
  static void CopyOptimizedItr(value_type* p, const_iterator it1, const_iterator it2) noexcept {
    // VECTOR NL AutosarC++17_10-M5.0.9: MD_VAC_M5.0.9_explicitConversionChangesSignedness
    CopyOptimized(p, it1, static_cast<size_type>(it2 - it1));
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_usingStrongTypes
  /*!
   * \brief  Optimized move function.
   * \param  d Pointer to a character String to move to.
   * \param  s Pointer to a character String to move from.
   * \param  n Number of characters.
   */
  static void MoveOptimized(value_type* d, value_type const* s, size_type n) noexcept {
    if (n == 1) {
      traits_type::assign(*d, *s);
    } else {
      static_cast<void>(traits_type::move(d, s, n));
    }
  }

  /*!
   * \brief  Optimized assign function.
   * \param  d Pointer to a character String to assign to.
   * \param  n Number of characters.
   * \param  c C String to assign from.
   */
  static void AssignOptimized(value_type* d, size_type n, value_type c) noexcept {
    if (n == 1) {
      traits_type::assign(*d, c);
    } else {
      static_cast<void>(traits_type::assign(d, n, c));
    }
  }

  // VECTOR Next Construct AutosarC++17_10-A8.4.4: MD_VAC_A8.4.4_useReturnValueInsteadOfOutputParameter
  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_parametersBasedOnBuiltInTypes
  /*!
   * \brief  Allocate memories for String.
   * \param  cap New capacity to be allocated.
   * \param  old_cap Old capacity.
   * \return The pointer returned by the call to data_.allocate(cap + 1).
   */
  pointer Create(size_type& cap, size_type const old_cap) noexcept {
    // VECTOR NC AutosarC++17_10-A4.7.1: MD_VAC_A4.7.1_overFlow
    // VECTOR NC AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_overFlow
    if ((cap > old_cap) && (cap < (2 * old_cap))) {
      cap = std::min((2 * old_cap), this->max_size());
    }
    return std::allocator_traits<Allocator>::allocate(GetAllocatorLocal(), cap + 1);
  }

  // VECTOR Next Construct AutosarC++17_10-M5.0.16: MD_VAC_M5.0.16_pointerArithmetic
  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief  Set the length of this String.
   * \param  n Length.
   */
  void SetLength(size_type n) noexcept {
    size_ = n;
    // VECTOR NL AutosarC++17_10-M5.0.15: MD_VAC_M5.0.15_pointerArithmeticAllocatorIterator
    traits_type::assign(DataLocal()[n], value_type{});
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief Return true if the C String does not overlap with this String.
   * \param s C String to check.
   */
  bool Disjunct(value_type const* s) noexcept {
    bool const ret_value{std::less<value_type const*>()(this->end(), s)};
    return (std::less<value_type const*>()(s, this->begin()) || ret_value);
  }

  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief  Return pointer to contents.
   * \return Pointer to contents.
   */
  pointer DataLocal() const noexcept { return data_.dat_; }

  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief  Return reference of allocator of this String.
   * \return Reference of allocator of this String.
   */
  allocator_type& GetAllocatorLocal() noexcept { return data_; }

  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief  Return const reference of allocator of this String.
   * \return Const reference of allocator of this String.
   */
  allocator_type const& GetAllocatorLocal() const noexcept { return data_; }

  // VECTOR Next Construct VectorC++-V11-0-3: MD_VAC_V11-0-3_structShallContainOnlyPublicMember
  /*!
   * \brief  Allocator type for this String.
   */
  class AllocT : public allocator_type {
   public:
    // VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst
    /*!
     * \brief  Construct allocator type from given pointer and allocator.
     * \param  dat Pointer points to characters.
     * \param  a Allocator to construct from.
     */
    AllocT(value_type (&dat)[local_capacity_ + 1], Allocator const& a) noexcept : allocator_type{a}, dat_{dat} {}

    // VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst
    /*!
     * \brief  Move constructor.
     * \param  dat Pointer points to characters.
     * \param  a Allocator to construct from.
     */
    explicit AllocT(value_type (&dat)[local_capacity_ + 1], Allocator&& a = Allocator()) noexcept
        : allocator_type{std::move(a)}, dat_{dat} {}

    // VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
    /*!
     * \brief  Actual data member.
     */
    pointer dat_;
  };

  /*!
   * \brief  Allocator data member of this String.
   */
  AllocT data_;

  /*!
   * \brief  Size data member of this String.
   */
  size_type size_;

  // VECTOR Next Construct AutosarC++17_10-A9.5.1: MD_VAC_A9.5.1_unionsShallNotBeUsed
  // VECTOR Next Construct AutosarC++17_10-A18.1.1: MD_VAC_A18.1.1_cStyleArraysShouldNotBeUsed
  // VECTOR Next Construct VectorC++-V3.0.1: MD_VAC_V3-0-1_cStyleArraysShouldNotBeUsed
  /*!
   * \brief  Union contains either local buffer or allocated capacity.
   */
  union {
    /*! \brief Local buffer for String. */
    char local_buf_[local_capacity_ + 1];  // NOLINT

    /*! \brief Capacity allocated in heap. */
    size_t allocated_capacity_;
  };
};

/*! \brief String type. */
using String = BasicString<>;

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
/*!
 * \brief  Concatenate two Strings.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator> const& lhs, BasicString<Allocator> const& rhs) noexcept
    -> BasicString<Allocator> {
  BasicString<Allocator> str{lhs};
  str.append(rhs);
  return str;
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
/*!
 * \brief  Concatenate two Strings.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator>&& lhs, BasicString<Allocator> const& rhs) noexcept -> BasicString<Allocator> {
  return std::move(lhs.append(rhs));
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
/*!
 * \brief  Concatenate two Strings.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator> const& lhs, BasicString<Allocator>&& rhs) noexcept -> BasicString<Allocator> {
  return std::move(rhs.insert(0, lhs));
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
/*!
 * \brief  Concatenate two Strings.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator>&& lhs, BasicString<Allocator>&& rhs) noexcept -> BasicString<Allocator> {
  // VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
  /*! \brief  Alias for size_type */
  using SizeType = typename BasicString<Allocator>::size_type;
  SizeType const sum{lhs.size() + rhs.size()};
  bool const con{(sum > lhs.capacity()) && (sum <= rhs.capacity())};
  return con ? std::move(rhs.insert(0, lhs)) : std::move(lhs.append(rhs));
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1 : MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
// VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
/*!
 * \brief  Concatenate a C String and a String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(char const* lhs, BasicString<Allocator> const& rhs) noexcept -> BasicString<Allocator> {
  /*! \brief Alias for size_type.*/
  using SizeType = typename BasicString<Allocator>::size_type;
  SizeType const len{std::char_traits<char>::length(lhs)};
  BasicString<Allocator> str;
  str.reserve(len + rhs.size());
  str.append(lhs, len);
  str.append(rhs);
  return str;
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1 : MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
/*!
 * \brief  Concatenate a C String and a String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(char const* lhs, BasicString<Allocator>&& rhs) noexcept -> BasicString<Allocator> {
  return std::move(rhs.insert(0, lhs));
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1 : MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
// VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
/*!
 * \brief  Concatenate a character and a String.
 * \param  lhs Character.
 * \param  rhs String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(char lhs, BasicString<Allocator> const& rhs) noexcept -> BasicString<Allocator> {
  /*! \brief Alias for size_type.*/
  using SizeType = typename BasicString<Allocator>::size_type;
  SizeType const len{rhs.size()};
  BasicString<Allocator> str;
  str.reserve(len + 1);
  str.append(SizeType(1), lhs);
  str.append(rhs);
  return str;
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1 : MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
// VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
/*!
 * \brief  Concatenate a character and a String.
 * \param  lhs Character.
 * \param  rhs String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(char lhs, BasicString<Allocator>&& rhs) noexcept -> BasicString<Allocator> {
  /*! \brief Alias for size_type.*/
  using SizeType = typename BasicString<Allocator>::size_type;
  return std::move(rhs.insert(SizeType(0), SizeType(1), lhs));
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
/*!
 * \brief  Concatenate a String and a C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator> const& lhs, char const* rhs) noexcept -> BasicString<Allocator> {
  BasicString<Allocator> str{lhs};
  str.append(rhs);
  return str;
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
/*!
 * \brief  Concatenate a String and a C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator>&& lhs, char const* rhs) noexcept -> BasicString<Allocator> {
  return std::move(lhs.append(rhs));
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
// VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
/*!
 * \brief  Concatenate a String and a character.
 * \param  lhs String.
 * \param  rhs Character.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator> const& lhs, char rhs) noexcept -> BasicString<Allocator> {
  /*! \brief Alias for size_type.*/
  using SizeType = typename BasicString<Allocator>::size_type;
  BasicString<Allocator> str{lhs};
  str.append(SizeType(1), rhs);
  return str;
}

// VECTOR Next Construct AutosarC++17_10-M5.17.1: MD_VAC_M5.17_overloadForCorrespondingAssignmenOperator
// VECTOR Next Construct AutosarC++17_10-A2.11.2: MD_VAC_A2.11.2_usingNameReusedInLocalScope
/*!
 * \brief  Concatenate a String and a character.
 * \param  lhs String.
 * \param  rhs Character.
 * \return New String with the value of lhs + rhs.
 */
template <typename Allocator>
auto operator+(BasicString<Allocator>&& lhs, char rhs) noexcept -> BasicString<Allocator> {
  /*! \brief Alias for size_type.*/
  using SizeType = typename BasicString<Allocator>::size_type;
  return std::move(lhs.append(SizeType(1), rhs));
}

/*!
 * \brief  Test equivalence of two Strings.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return Ture if lhs.compare(rhs) == 0, false otherwise.
 */
template <typename Allocator>
bool operator==(BasicString<Allocator> const& lhs, BasicString<Allocator> const& rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/*!
 * \brief  Test equivalence of a C String and a String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return Ture if lhs.compare(rhs) == 0, false otherwise.
 */
template <typename Allocator>
bool operator==(char const* lhs, BasicString<Allocator> const& rhs) noexcept {
  return rhs.compare(lhs) == 0;
}

/*!
 * \brief  Test equivalence of a String and a C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return Ture if lhs.compare(rhs) == 0, false otherwise.
 */
template <typename Allocator>
bool operator==(BasicString<Allocator> const& lhs, char const* rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/*!
 * \brief  Test difference of two Strings.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return Ture if lhs.compare(rhs) != 0, false otherwise.
 */
template <typename Allocator>
bool operator!=(BasicString<Allocator> const& lhs, BasicString<Allocator> const& rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief  Test difference of a C String and a String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return Ture if lhs.compare(rhs) != 0, false otherwise.
 */
template <typename Allocator>
bool operator!=(char const* lhs, BasicString<Allocator> const& rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief  Test difference of a String and a C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return Ture if lhs.compare(rhs) != 0, false otherwise.
 */
template <typename Allocator>
bool operator!=(BasicString<Allocator> const& lhs, char const* rhs) noexcept {
  return !(lhs == rhs);
}

/*!
 * \brief  Test if String is less than String.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return Ture if lhs < rhs, false otherwise.
 */
template <typename Allocator>
bool operator<(BasicString<Allocator> const& lhs, BasicString<Allocator> const& rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/*!
 * \brief  Test if String is less than C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return Ture if lhs < rhs, false otherwise.
 */
template <typename Allocator>
bool operator<(BasicString<Allocator> const& lhs, char const* rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/*!
 * \brief  Test if C String is less than String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return Ture if lhs < rhs, false otherwise.
 */
template <typename Allocator>
bool operator<(char const* lhs, BasicString<Allocator> const& rhs) noexcept {
  return rhs.compare(lhs) > 0;
}

/*!
 * \brief  Test if String is greater than String.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return Ture if lhs > rhs, false otherwise.
 */
template <typename Allocator>
bool operator>(BasicString<Allocator> const& lhs, BasicString<Allocator> const& rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief  Test if String is greater than C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return Ture if lhs > rhs, false otherwise.
 */
template <typename Allocator>
bool operator>(BasicString<Allocator> const& lhs, char const* rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief  Test if C String is greater than String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return Ture if lhs > rhs, false otherwise.
 */
template <typename Allocator>
bool operator>(char const* lhs, BasicString<Allocator> const& rhs) noexcept {
  return rhs < lhs;
}

/*!
 * \brief  Test if String is less/equal than String.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return Ture if lhs <= rhs, false otherwise.
 */
template <typename Allocator>
bool operator<=(BasicString<Allocator> const& lhs, BasicString<Allocator> const& rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief  Test if String is less/equal than C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return Ture if lhs <= rhs, false otherwise.
 */
template <typename Allocator>
bool operator<=(BasicString<Allocator> const& lhs, char const* rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief  Test if C String is less/equal than String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return Ture if lhs <= rhs, false otherwise.
 */
template <typename Allocator>
bool operator<=(char const* lhs, BasicString<Allocator> const& rhs) noexcept {
  return !(lhs > rhs);
}

/*!
 * \brief  Test if String is greater/equal than String.
 * \param  lhs First String.
 * \param  rhs Second String.
 * \return Ture if lhs >= rhs, false otherwise.
 */
template <typename Allocator>
bool operator>=(BasicString<Allocator> const& lhs, BasicString<Allocator> const& rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief  Test if String is greater/equal than C String.
 * \param  lhs String.
 * \param  rhs C String.
 * \return Ture if lhs >= rhs, false otherwise.
 */
template <typename Allocator>
bool operator>=(BasicString<Allocator> const& lhs, char const* rhs) noexcept {
  return !(lhs < rhs);
}

/*!
 * \brief  Test if C String is greater/equal than String.
 * \param  lhs C String.
 * \param  rhs String.
 * \return Ture if lhs >= rhs, false otherwise.
 */
template <typename Allocator>
bool operator>=(char const* lhs, BasicString<Allocator> const& rhs) noexcept {
  return !(lhs < rhs);
}

// VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
/*!
 * \brief Swaps two Strings.
 * \param lhs Reference to the first String.
 * \param rhs Reference to the second String.
 */
template <typename Allocator>
void swap(BasicString<Allocator>& lhs, BasicString<Allocator>& rhs) noexcept {
  lhs.swap(rhs);
}

/*!
 * \brief Temporary output stream operator to improve compatibility with the old String implementation.
 * \vprivate
 */
template <class CharT, class Traits, class Allocator>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                              ara::core::BasicString<Allocator> const& str) noexcept {
  return os << str.c_str();
}

/*!
 * \brief Value returned by various member functions when they fail.
 */
template <typename Allocator>
typename BasicString<Allocator>::size_type const BasicString<Allocator>::npos{static_cast<size_type>(-1)};
}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_STRING_H_
