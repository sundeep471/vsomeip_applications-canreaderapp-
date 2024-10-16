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
/**        \file  flexible_unique_memory_buffer_allocator.h
 *        \brief  Flexible Unique Memory Buffer Allocator
 *
 *      \details  Allocator for FlexibleUniqueMemoryBuffer
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_FLEXIBLE_FLEXIBLE_UNIQUE_MEMORY_BUFFER_ALLOCATOR_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_FLEXIBLE_FLEXIBLE_UNIQUE_MEMORY_BUFFER_ALLOCATOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <utility>
#include <vector>
#include "vac/memory/memory_buffer_allocator.h"

namespace vac {
namespace memory {
namespace flexible {

/*!
 * \brief Forward declaration.
 * \tparam IovecType type for io (should contain base_pointer and size, usually struct iovec from <sys/uio.h> is used).
 */
template <class IovecType>
class FlexibleUniqueMemoryBuffer;

// VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally
/*!
 * \brief Memory Allocator for Flexible Unique Memory Buffers
 * \tparam IovecType type for io (should contain base_pointer and size, usually struct iovec from <sys/uio.h> is used).
 */
template <class IovecType>
class FlexibleUniqueMemoryBufferAllocator : public MemoryBufferAllocator<IovecType> {
 public:
  /*!
   * \brief Alias for the base class
   */
  using Base = MemoryBufferAllocator<IovecType>;

  /*!
   * \brief default Constructor
   */
  FlexibleUniqueMemoryBufferAllocator() noexcept = default;

  /*!
   * \brief Define destructor.
   */
  virtual ~FlexibleUniqueMemoryBufferAllocator() noexcept = default;
  /*!
   * \brief Delete copy constructor.
   */
  FlexibleUniqueMemoryBufferAllocator(FlexibleUniqueMemoryBufferAllocator const &) = delete;
  /*!
   * \brief Delete move constructor.
   */
  FlexibleUniqueMemoryBufferAllocator(FlexibleUniqueMemoryBufferAllocator &&) = delete;
  /*!
   * \brief Delete copy assignment.
   * \return
   */
  FlexibleUniqueMemoryBufferAllocator &operator=(FlexibleUniqueMemoryBufferAllocator const &) & = delete;
  /*!
   * \brief Delete move assignment.
   * \return
   */
  FlexibleUniqueMemoryBufferAllocator &operator=(FlexibleUniqueMemoryBufferAllocator &&) & = delete;

  /*!
   * \brief Alias for container type
   */
  using Container = ara::core::Vector<std::uint8_t>;

  /*!
   * \brief Constructor
   */

  // VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.5.3_exceptionViolatesFunctionsNoexeceptSpec
  // VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec
  /*!
   * \brief   Allocate memory.
   * \details Will terminate if underlying allocator fails to allocate the given size.
   * \param   size The size of memory to allocate.
   * \return  The allocated memory.
   */
  typename Base::MemoryBufferPtr Allocate(typename MemoryBuffer<IovecType>::size_type size) noexcept override {
    /*! \brief Alias for memory buffer type. */
    using MemoryBufferType = FlexibleUniqueMemoryBuffer<IovecType>;

    // The container is actually flexible, however we resize here the given size to enhance performance.
    // resize with larger value is allowed later on.
    Container container;

    container.resize(size);
    // Create a unique container
    typename Base::MemoryBufferPtr allocated_memory{std::make_unique<MemoryBufferType>(std::move(container))};
    return allocated_memory;
  }
};

// VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally
/*!
 * \brief   Flexible Unique buffer.
 * \details Flexible: The maximum size of the buffer is flexible; i.e. not constrained.
 *          Unique: This class owns its buffer, not shared with any other MemoryBuffer.
 * \tparam IovecType type for io (should contain base_pointer and size, usually struct iovec from <sys/uio.h> is used).
 */
template <class IovecType>
class FlexibleUniqueMemoryBuffer final : public MemoryBuffer<IovecType> {
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
   * \brief Constructor for memory buffer and pre-allocated buffer.
   * \param buffer Pre-allocated buffer using FlexibleUniqueMemoryBufferAllocator.
   */
  explicit FlexibleUniqueMemoryBuffer(BufferType &&buffer)
      : MemoryBuffer<IovecType>{},
        /*!
         * \brief The unique buffer.
         */
        buffer_{std::move(buffer)} {}

  /*!
   * \brief  Obtain a view of the memory buffer.
   * \param  offset The offset at which to start.
   * \return The desired view of the memory buffer.
   */
  typename Base::MemoryBufferView GetView(typename Base::size_type offset) const noexcept final {
    typename Base::MemoryBufferView view{};
    if (offset < buffer_.size()) {
      // VECTOR NL AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_CastRemovesConstQualification
      view.push_back({const_cast<BufferType::value_type *>(&buffer_[offset]), buffer_.size() - offset});
    }
    return view;
  }

  /*!
   * \brief  Obtain the size of the memory buffer.
   * \return The size of the buffer.
   */
  typename Base::size_type size() const noexcept final { return buffer_.size(); }

  // VECTOR Next Construct AutosarC++17_10-A15.5.3: MD_VAC_A15.5.3_exceptionViolatesFunctionsNoexeceptSpec
  // VECTOR Next Construct AutosarC++17_10-A15.4.2: MD_VAC_A15.4.2_exceptionViolatesFunctionsNoexeceptSpec
  /*!
   * \brief Resize the memory buffer.
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
  /*!
   * \brief The unique buffer.
   */
  BufferType buffer_;
};

}  // namespace flexible
}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_FLEXIBLE_FLEXIBLE_UNIQUE_MEMORY_BUFFER_ALLOCATOR_H_
