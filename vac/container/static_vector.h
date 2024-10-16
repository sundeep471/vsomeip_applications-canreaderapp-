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
/*!        \file  static_vector.h
 *        \brief  Static vector manages elements in contiguous memory.
 *
 *      \details  A contiguous memory is allocated from the allocator StaticVector::reserve().
 *                The memory segment will be deallocated when the object is destroyed.
 *                Besides, the elements in the vector can be removed or added.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_VECTOR_H_
#define LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_VECTOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <limits>
#include <memory>
#include <utility>

#include "ara/core/abort.h"
#include "ara/core/span.h"

namespace vac {
namespace container {

// VECTOR Next Construct Metric-OO.WMC.One: MD_VAC_Metric-OO.WMC.One
/*!
 * \brief Class to (owning) manage a contiguous memory segment.
 *        The size of the memory segment can be set once. The memory segment is allocated when calling
 *        StaticVector::reserve() and is deallocated when the object is destroyed. Allocations/Deallocations are
 *        performed from the given allocator.
 * \trace CREQ-158594, CREQ-158595
 */
template <typename T, typename alloc = vac::memory::PhaseManagedAllocator<T>>
class StaticVector final {
 public:
  /*!
   * \brief Typedef for the ara::core::Span used internally.
   */
  using span_type = ara::core::Span<T>;

  /*!
   * \brief Typedef for the size value.
   */
  using size_type = typename span_type::size_type;

  /*!
   * \brief Typedef for the contained element.
   */
  using value_type = typename span_type::value_type;

  /*!
   * \brief Typedef for a pointer.
   */
  using pointer = typename span_type::pointer;

  /*!
   * \brief Typedef for a const pointer.
   */
  using const_pointer = T const*;

  /*!
   * \brief Typedef for a reference.
   */
  using reference = typename span_type::reference;

  /*!
   * \brief Typedef for a const reference.
   */
  using const_reference = T const&;

  /*!
   * \brief Typedef for the allocator used.
   */
  using allocator_type = alloc;

  /*!
   * \brief Typedef for the allocator type used after rebinding.
   */
  using actual_allocator_type = typename allocator_type::template rebind<T>::other;

  /*!
   * \brief Typedef for an iterator.
   * \trace CREQ-160857
   */
  using iterator = typename span_type::iterator;

  /*!
   * \brief Typedef for a const iterator.
   */
  using const_iterator = typename span_type::const_iterator;

  /*!
   * \brief Initialize a StaticVector.
   * \param allocator The allocator to use for initialization, default is allocator_type().
   */
  explicit StaticVector(allocator_type const& allocator = allocator_type()) noexcept(true)
      : delegate_(), allocator_(allocator), max_num_elements_(0) {}

  /*!
   * \brief Default copy constructor deleted.
   */
  StaticVector(StaticVector const&) = delete;
  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   */
  StaticVector& operator=(StaticVector const&) = delete;

  // VECTOR Disable AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  // VECTOR Disable AutosarC++17_10-A12.8.4: MD_VAC_A12.8.4_moveConstructorShallNotUseCopySemantics
  /*!
   * \brief Move constructor.
   * \param other The vector from which to construct the new vector.
   */
  StaticVector(StaticVector&& other) noexcept : StaticVector() { this->swap(other); }
  // VECTOR Enable AutosarC++17_10-A12.8.4

  /*!
   * \brief  Move Assignment.
   * \param  other The vector from which to construct the new vector.
   * \return A reference to the assigned-to object.
   */
  StaticVector& operator=(StaticVector&& other) & noexcept {
    this->swap(other);
    return *this;
  }

  /*!
   * \brief   Destructor.
   * \details Terminates if the allocator throws an exception when deallocating the elements.
   */
  ~StaticVector() noexcept {
    shorten(0);
    if (delegate_.data() != nullptr) {
      allocator_.deallocate(delegate_.data(), max_num_elements_);
    }
  }
  // VECTOR Enable AutosarC++17_10-A15.5.1

  /*!
   * \brief   Allocate a memory segment from the allocator.
   * \details Terminates if the allocator throws an exception when allocating new elements.
   * \param   num_elements The number of elements of T to allocate.
   * \pre     reserve() may only be called once. A second call triggers ara::core::Abort.
   * \trace   CREQ-158592
   */
  void reserve(size_type num_elements) noexcept {
    if (delegate_.data() != nullptr) {
      ara::core::Abort("vac::container::StaticVector::reserve(size_type): The memory has already been reserved!");
    }
    max_num_elements_ = num_elements;
    delegate_ = span_type(allocator_.allocate(num_elements), static_cast<std::size_t>(0));
  }

  /*!
   * \brief Set the vector to be of a certain size.
   * \param num_elements The number of elements of T to allocate.
   *        Note: currently, resize() implicitly calls reserve() if it increases the size.
   * \pre   num_elements <= maximum value for std::ptrdiff_t, otherwise ara::core::Abort will be triggered.
   */
  void resize(size_type num_elements) noexcept {
    if (num_elements > size_type{std::numeric_limits<std::ptrdiff_t>::max()}) {
      ara::core::Abort("vac::container::StaticVector::reserve(size_type): Size exceeds allowed limit!");
    }
    if (num_elements > max_num_elements_) {
      reserve(num_elements);
    }

    // Resize the delegate if resizing is needed.
    if (num_elements > delegate_.size()) {
      // Record previous size of the container.
      size_type const old_num_elements{delegate_.size()};

      // Extend the delegate.
      delegate_ = span_type(delegate_.data(), num_elements);

      // Cache the end iterator. We do not modify the size of the vector here
      // and during profiling, the end() operation turned out to be fairly
      // costly for long vectors.
      iterator const end_it{end()};

      // Initialize new elements.
      // VECTOR Next Construct AutosarC++17_10-M5.0.16: MD_VAC_M5.0.16_lastElementCompare
      // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_castMayTruncateValue
      for (iterator it{std::next(begin(), static_cast<std::ptrdiff_t>(old_num_elements))}; it != end_it; ++it) {
        allocator_.construct(it);
      }
    } else {
      // Destroy excess elements.
      shorten(num_elements);
    }
  }

  /*!
   * \brief   Shorten the vector to the given size.
   * \details Terminates if a removed element throws in its destructor.
   * \param   num_elements The number elements of T the vector shall be shortened to.
   * \pre     num_elements <= maximum value for std::ptrdiff_t, otherwise ara::core::Abort will be triggered.
   * \note    Does nothing if num_elements is not smaller than size().
   */
  void shorten(size_type num_elements) noexcept {
    if (num_elements > size_type{std::numeric_limits<std::ptrdiff_t>::max()}) {
      ara::core::Abort("vac::container::StaticVector::reserve(size_type): Size exceeds allowed limit!");
    }
    // will this #elements actually shorten?
    if (num_elements < delegate_.size()) {
      // Cache the end iterator. We do not modify the size of the vector here
      // and during profiling, the end() operation turned out to be fairly
      // costly for long vectors.
      iterator const end_it{end()};

      // VECTOR Next Construct AutosarC++17_10-M5.0.16: MD_VAC_M5.0.16_lastElementCompare
      // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_castMayTruncateValue
      for (iterator it{std::next(begin(), static_cast<std::ptrdiff_t>(num_elements))}; it != end_it; ++it) {
        allocator_.destroy(it);
      }

      delegate_ = span_type(delegate_.data(), num_elements);
    }
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Swap the contents of two StaticVector objects including their allocator.
   * \param other The second vector whose contents are swapped.
   */
  void swap(StaticVector& other) noexcept {
    std::swap(delegate_, other.delegate_);
    std::swap(allocator_, other.allocator_);
    std::swap(max_num_elements_, other.max_num_elements_);
  }

  /*!
   * \brief  Get the length of the contained array.
   * \return The number of contained elements.
   */
  size_type size() const noexcept { return delegate_.size(); }

  /*!
   * \brief  Get the maximum number of elements this StaticVector can hold.
   * \return The number of elements this vector can store.
   */
  size_type capacity() const noexcept { return max_num_elements_; }

  /*!
   * \brief  Check whether the contained array is empty.
   * \return True if the vector is empty.
   */
  bool empty() const noexcept { return delegate_.empty(); }

  // VECTOR Next Construct AutosarC++17_10-M9.3.3: MD_VAC_M9.3.3_methodCanBeDeclaredConst
  /*!
   * \brief  Get a pointer to the contained data.
   * \return A pointer to the data.
   */
  pointer data() noexcept { return delegate_.data(); }

  /*!
   * \brief  Get a const pointer to the contained data.
   * \return A pointer to the data.
   */
  const_pointer data() const noexcept { return delegate_.data(); }

  // VECTOR Next Construct AutosarC++17_10-M9.3.3: MD_VAC_M9.3.3_methodCanBeDeclaredConst
  /*!
   * \brief  Iterator to the start of the array.
   * \return An iterator to the start of the vector.
   */
  iterator begin() noexcept { return delegate_.begin(); }
  // VECTOR Next Construct AutosarC++17_10-M9.3.3: MD_VAC_M9.3.3_methodCanBeDeclaredConst
  /*!
   * \brief  Past-The-End iterator of the array.
   * \return An iterator to the sentinel value at the end.
   */
  iterator end() noexcept { return delegate_.end(); }

  /*!
   * \brief  Const iterator to the start of the array.
   * \return An const iterator to the start of the vector.
   */
  const_iterator begin() const noexcept { return delegate_.begin(); }
  /*!
   * \brief  Const Past-The-End iterator of the array.
   * \return A const Past-The-End iterator of the array.
   */
  const_iterator end() const noexcept { return delegate_.end(); }

  /*!
   * \brief  Const iterator to the start of the array.
   * \return A const iterator to the start of the array.
   */
  const_iterator cbegin() const noexcept { return delegate_.cbegin(); }
  /*!
   * \brief  Const past-The-End iterator of the array.
   * \return A const iterator to the sentinel value at the end.
   */
  const_iterator cend() const noexcept { return delegate_.cend(); }

  /*!
   * \brief   Get the element at the specified position.
   * \details Abortion will be triggered if pos >= size().
   * \param   pos Index of the element to get.
   * \return  A reference to the resulting element.
   */
  reference at(size_type pos) noexcept { return delegate_[pos]; }

  /*!
   * \brief   Get the element at the specified position (const version).
   * \details Abortion will be triggered if pos >= size().
   * \param   pos Index of the element to get.
   * \return  A reference to the resulting element.
   */
  const_reference at(size_type pos) const noexcept { return delegate_[pos]; }

  /*!
   * \brief  Unchecked array access.
   *         The behavior is unspecified if the container is empty or if pos >= size().
   * \param  pos Index of the element to get.
   * \return A reference to the resulting element.
   */
  reference operator[](size_type pos) noexcept { return delegate_[pos]; }

  /*!
   * \brief  Unchecked array access.
   *         The behavior is unspecified if the container is empty or if pos >= size().
   * \param  pos Index of the element to get.
   * \return A reference to the resulting element.
   */
  const_reference operator[](size_type pos) const noexcept { return delegate_[pos]; }

  /*!
   * \brief  Unchecked access to first element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the first element.
   */
  reference front() noexcept { return delegate_[0]; }

  /*!
   * \brief  Unchecked access to first element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the first element.
   */
  const_reference front() const noexcept { return delegate_[0]; }

  /*!
   * \brief  Unchecked access to last element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the last element.
   */
  reference back() noexcept { return delegate_[this->size() - 1]; }

  /*!
   * \brief  Unchecked access to last element.
   *         The behavior is unspecified if the container is empty.
   * \return A reference to the last element.
   */
  const_reference back() const noexcept { return delegate_[this->size() - 1]; }

  // VECTOR Next Construct AutosarC++17_10-A18.1.5: MD_VAC_A18.1.5_constStdUniquePtrRefAsParam
  /*!
   * \brief   Add an element to the back of the StaticVector.
   * \details Terminates if the added elment throws in its constructor.
   * \param   elem The element to add.
   * \pre     The StaticVector must not be filled, otherwise ara::core::Abort will be triggered.
   */
  void push_back(T const& elem) noexcept {
    size_type const delegate_size{delegate_.size()};
    if (delegate_size >= max_num_elements_) {
      ara::core::Abort(
          "va::container::StaticVector::push_back(const T&): The number of elements cannot exceed the maximum!");
    } else {
      delegate_ = span_type(delegate_.data(), delegate_size + 1);
      allocator_.construct(&delegate_[this->size() - 1], elem);
    }
  }

  /*!
   * \brief   Add an element to the back of the StaticVector.
   * \details Terminates if the added elment throws in its constructor.
   * \param   args Constructor arguments for T.
   * \pre     The StaticVector should not be fulled, otherwise ara::core::Abort will be triggered.
   * \trace   CREQ-158593
   */
  template <typename... Args>
  void emplace_back(Args&&... args) noexcept {
    size_type const delegate_size{delegate_.size()};
    if (delegate_size >= max_num_elements_) {
      ara::core::Abort(
          "vac::container::StaticVector::emplace_back<>(Args&&): The number of elements cannot exceed the maximum!");
    } else {
      delegate_ = span_type(delegate_.data(), delegate_size + 1);
      allocator_.construct(&delegate_[this->size() - 1], std::forward<Args>(args)...);
    }
  }

  /*!
   * \brief   Remove an element to the back of the StaticVector.
   * \details Terminates if the removed elment throws in its destructor.
   * \pre     The StaticVector should not be empty, otherwise ara::core::Abort will be triggered.
   */
  void pop_back() noexcept {
    size_type const delegate_size{delegate_.size()};
    if (delegate_size <= 0) {
      ara::core::Abort("vac::container::StaticVector::pop_back(): Vector cannot be empty!");
    } else {
      reference elem{delegate_[this->size() - 1]};
      allocator_.destroy(&elem);
      delegate_ = span_type(delegate_.data(), delegate_size - 1);
    }
  }

  /*!
   * \brief   Remove an element anywhere in the static vector.
   * \details Terminates if the removed elment throws in its destructor.
   * \param   elem Iterator to the element to be removed.
   * \return  An iterator to the element following the removed element.
   * \trace   CREQ-158593
   */
  iterator erase(iterator elem) noexcept {
    if (elem != end()) {
      iterator current{elem};
      iterator next{std::next(elem, 1)};

      while (next != end()) {
        *current = std::move(*next);
        ++current;
        // VECTOR Next Line AutosarC++17_10-A12.8.3: MD_VAC_A12.8.3_movedFromObjectReadAccessedInLoop
        ++next;
      }

      pop_back();
    }

    return elem;
  }

  /*!
   * \brief   Remove all elements from the vector.
   * \details Terminates if any removed elment throws in its destructor.
   */
  void clear() noexcept { shorten(0); }

 private:
  /*!
   * \brief A Span to implement access to the data.
   *        The Span is also used to hold the pointer to the owned data. Ownership semantics are implemented by the
   *        StaticVector itself.
   *        The size of the delegate_ represents the current length of the vector, not the amount of memory allocated.
   */
  span_type delegate_;

  /*!
   * \brief Allocator used to obtain memory.
   */
  actual_allocator_type allocator_;

  /*!
   * \brief The maximum number of elements (i.e., amount of memory allocated) that this StaticVector can hold.
   */
  size_type max_num_elements_;
};

}  // namespace container
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_CONTAINER_STATIC_VECTOR_H_
