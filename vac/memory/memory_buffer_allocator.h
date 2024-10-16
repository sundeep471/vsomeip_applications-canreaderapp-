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
/**        \file  memory_buffer_allocator.h
 *        \brief  TODO
 *
 *      \details  TODO
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_MEMORY_BUFFER_ALLOCATOR_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_MEMORY_BUFFER_ALLOCATOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <utility>
#include "vac/memory/memory_buffer.h"

namespace vac {
namespace memory {

/*!
 * \brief Memory Buffer Allocator
 * \tparam IovecType type for io (should contain base_pointer and size, usually osabstraction::io::ConstIOBuffer or
 *         osabstraction::io::MutableIOBuffer is used).
 */
template <class IovecType>
class MemoryBufferAllocator {
 public:
  /*!
   * \brief Alias for MemoryBuffer pointer
   */
  using MemoryBufferPtr = std::unique_ptr<MemoryBuffer<IovecType>>;
  /*!
   * \brief Default constructor.
   */
  MemoryBufferAllocator() noexcept = default;
  /*!
   * \brief Delete copy constructor.
   */
  MemoryBufferAllocator(MemoryBufferAllocator const &) = delete;
  /*!
   * \brief Delete move constructor.
   */
  MemoryBufferAllocator(MemoryBufferAllocator &&) = delete;
  /*!
   * \brief Delete copy assignment.
   * \return
   */
  MemoryBufferAllocator &operator=(MemoryBufferAllocator const &) & = delete;
  /*!
   * \brief Delete move assignment.
   * \return
   */
  MemoryBufferAllocator &operator=(MemoryBufferAllocator &&) & = delete;
  /*!
   * \brief Allocate the given memory size, and return the created MemoryBuffer pointer.
   * \remark The exact implementation must be done in the subclasses.
   * \remark Deallocate method can be implemented upon need within concrete subclasses.
   * The deallocation must be triggered first from the destructor of the concrete MemoryBuffer and then, if required,
   * signaled to the allocator to free the allocated memory (if owned by allocator).
   * Signature of Deallocate function can differ according the allocated MemoryBuffer type, therefore it is not a part
   * of this interface.
   *
   * \param size Memory size to be allocated.
   *
   * \return Pointer for the allocated MemoryBuffer, null if the requested memory could not be allocated.
   */
  virtual MemoryBufferPtr Allocate(typename MemoryBuffer<IovecType>::size_type size) = 0;

 protected:
  /*!
   * \brief Default destructor.
   */
  ~MemoryBufferAllocator() noexcept = default;
};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_MEMORY_BUFFER_ALLOCATOR_H_
