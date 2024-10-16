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
/**        \file  flexible_shared_memory_buffer_allocator.h
 *        \brief  Flexible Shared Memory Buffer Allocator
 *
 *      \details  Allocator for FlexibleSharedMemoryBuffer
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_FLEXIBLE_FLEXIBLE_SHARED_MEMORY_BUFFER_ALLOCATOR_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_FLEXIBLE_FLEXIBLE_SHARED_MEMORY_BUFFER_ALLOCATOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <utility>
#include "ara/core/vector.h"
#include "vac/memory/memory_buffer_allocator.h"

namespace vac {
namespace memory {
namespace flexible {

/*!
 * \brief Forward declaration.
 * \tparam IovecType type for io (should contain base_pointer and size, usually struct iovec from <sys/uio.h> is used).
 */
template <class IovecType>
class FlexibleSharedMemoryBuffer;

/*!
 * \brief Memory Allocator for Flexible Shared Memory Buffer
 * \tparam IovecType type for io (should contain base_pointer and size, usually struct iovec from <sys/uio.h> is used).
 */
template <class IovecType>
class FlexibleSharedMemoryBufferAllocator final : public MemoryBufferAllocator<IovecType> {
 public:
  /*!
   * \brief Alias for the base class
   */
  using Base = MemoryBufferAllocator<IovecType>;
  /*!
   * \brief Constructor.
   */
  FlexibleSharedMemoryBufferAllocator() noexcept : Base{}, buffer_{}, buffer_allocated_{false} {}
  /*!
   * \brief Define destructor.
   */
  ~FlexibleSharedMemoryBufferAllocator() noexcept = default;
  /*!
   * \brief Delete copy constructor.
   */
  FlexibleSharedMemoryBufferAllocator(FlexibleSharedMemoryBufferAllocator const &) = delete;
  /*!
   * \brief Delete move constructor.
   */
  FlexibleSharedMemoryBufferAllocator(FlexibleSharedMemoryBufferAllocator &&) = delete;
  /*!
   * \brief Delete copy assignment.
   * \return
   */
  FlexibleSharedMemoryBufferAllocator &operator=(FlexibleSharedMemoryBufferAllocator const &) & = delete;
  /*!
   * \brief Delete move assignment.
   * \return
   */
  FlexibleSharedMemoryBufferAllocator &operator=(FlexibleSharedMemoryBufferAllocator &&) & = delete;
  /*!
   * \brief Alias for container type.
   */
  using BufferType = ara::core::Vector<std::uint8_t>;

  // VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.5.3_exceptionViolatesFunctionsNoexeceptSpec
  // VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec
  /*!
   * \brief   Allocate memory.
   * \details Will terminate if underlying allocator fails to allocate the given size.
   * \param   size Memory size to be allocated.
   * \return  Pointer for the allocated MemoryBuffer; can be nullptr if the shared memory was already allocated and not
   *          yet deallocated.
   */
  typename Base::MemoryBufferPtr Allocate(typename MemoryBuffer<IovecType>::size_type size) noexcept final {
    /*! \brief Alias for memory buffer type. */
    using MemoryBufferType = FlexibleSharedMemoryBuffer<IovecType>;

    typename Base::MemoryBufferPtr allocated_memory{nullptr};

    if (!buffer_allocated_) {
      // The container is actually flexible, however we resize here the given size to enhance performance.
      // resize with larger value is allowed later on.
      buffer_.resize(size);
      buffer_allocated_ = true;
      allocated_memory = std::make_unique<MemoryBufferType>(buffer_, *this);
    }
    return allocated_memory;
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.5.3_exceptionViolatesFunctionsNoexeceptSpec
  // VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec
  /*!
   * \brief Deallocate memory.
   */
  void Deallocate() noexcept {
    buffer_.resize(0U);
    buffer_allocated_ = false;
  };

 private:
  /*!
   * \brief The shared memory container.
   */
  BufferType buffer_;

  /*!
   * \brief Buffer allocated flag. True if buffer is currently allocated and false otherwise.
   */
  bool buffer_allocated_{false};
};

// VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally
/*!
 * \brief   Flexible Shared buffer.
 * \details Flexible: The maximum size of the buffer is flexible; i.e. not constrained.
 *          Shared: This class has a reference of the buffer, which can be shared between several MemoryBuffers.
 * \tparam IovecType type for io (should contain base_pointer and size, usually struct iovec from <sys/uio.h> is used).
 */
template <class IovecType>
class FlexibleSharedMemoryBuffer final : public MemoryBuffer<IovecType> {
 public:
  /*!
   * \brief Alias for the base class
   */
  using Base = MemoryBuffer<IovecType>;

  /*!
   * \brief Alias for MemoryBuffer pointer
   */
  using MemoryBufferPtr = std::unique_ptr<Base>;

  /*!
   * \brief Alias for container type.
   */
  using BufferType = ara::core::Vector<std::uint8_t>;

  /*!
   * \brief Constructor for memory buffer, pre-allocated buffer and allocator.
   * \param buffer    Pre-allocated buffer using FlexibleSharedMemoryBufferAllocator.
   * \param allocator Reference to the allocator that allocated the buffer. Required for later deallocation.
   */
  FlexibleSharedMemoryBuffer(BufferType &buffer, FlexibleSharedMemoryBufferAllocator<IovecType> &allocator)
      : Base{}, buffer_{buffer}, allocator_{allocator} {}

  // VECTOR NC AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief Destructor.
   */
  ~FlexibleSharedMemoryBuffer() noexcept final { allocator_.Deallocate(); }

  /*!
   * \brief Delete copy constructor.
   */
  FlexibleSharedMemoryBuffer(FlexibleSharedMemoryBuffer const &) = delete;
  /*!
   * \brief Delete move constructor.
   */
  FlexibleSharedMemoryBuffer(FlexibleSharedMemoryBuffer &&) = delete;
  /*!
   * \brief Delete copy assignment.
   * \return
   */
  FlexibleSharedMemoryBuffer &operator=(FlexibleSharedMemoryBuffer const &) & = delete;
  /*!
   * \brief Delete move assignment.
   * \return
   */
  FlexibleSharedMemoryBuffer &operator=(FlexibleSharedMemoryBuffer &&) & = delete;

  /*!
   * \brief  Get a view of this memory buffer.
   * \param  offset The offset at which to start.
   * \return A view of the buffer, starting at the desired offset.
   */
  typename Base::MemoryBufferView GetView(typename Base::size_type offset) const noexcept final {
    typename Base::MemoryBufferView view{};
    if (offset < buffer_.size()) {
      view.push_back({&buffer_[offset], buffer_.size() - offset});
    }
    return view;
  }

  /*!
   * \brief  Obtain the size of the buffer.
   * \return The size of the buffer.
   */
  typename Base::size_type size() const noexcept final { return buffer_.size(); }

  // VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.5.3_exceptionViolatesFunctionsNoexeceptSpec
  // VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec
  /*!
   * \brief Resize the buffer.
   * \param nbytes The size to which to resize.
   */
  void resize(typename Base::size_type nbytes) noexcept final {
    buffer_.resize(nbytes);
  }

  /*!
   * \brief Add data to the end of the buffer.
   * \param value The data to add.
   */
  void push_back(typename Base::value_type const &value) noexcept final { buffer_.push_back(value); }

 private:
  /*! \brief Reference of the shared buffer. */
  BufferType &buffer_;

  /*! \brief The allocator that owns the shared memory. */
  FlexibleSharedMemoryBufferAllocator<IovecType> &allocator_;
};

}  // namespace flexible
}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_FLEXIBLE_FLEXIBLE_SHARED_MEMORY_BUFFER_ALLOCATOR_H_
