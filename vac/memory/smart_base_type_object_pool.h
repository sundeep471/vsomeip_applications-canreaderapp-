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
/*!        \file  object_pool.h
 *        \brief  ObjectPool class that can dynamically allocate a set of objects and then never deallocates them but
 *                recycles the memory for new objects.
 *
 *      \details  Currently, this class supports only one initial allocation of memory. All memory is allocated
 *                immediately, even if no objects are created.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_SMART_BASE_TYPE_OBJECT_POOL_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_SMART_BASE_TYPE_OBJECT_POOL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cassert>
#include <memory>
#include <utility>

#include "vac/memory/object_pool.h"

namespace vac {
namespace memory {

// Forward declaration
class DeleteableSmartPoolObject;

/*!
 * \brief Interface for the actual Deleter for a smart pool object.
 */
class SmartObjectPoolDeleterContext {
 public:
  /*!
   * \brief Default constructor.
   */
  SmartObjectPoolDeleterContext() noexcept = default;

  /*!
   * \brief Default virtual destructor.
   */
  virtual ~SmartObjectPoolDeleterContext() noexcept = default;

  /*!
   * \brief Actual Deleter function.
   * \param ptr Pointer to smart pool object.
   */
  virtual void destroy(DeleteableSmartPoolObject* ptr) = 0;

  /*!
   * \brief Actual Deleter function.
   * \param ptr Const Pointer to smart pool object.
   */
  virtual void destroy(DeleteableSmartPoolObject const* ptr) = 0;

 protected:
  /*!
   * \brief Default copy constructor.
   */
  SmartObjectPoolDeleterContext(SmartObjectPoolDeleterContext const&) noexcept = default;

  /*!
   * \brief Default move constructor.
   */
  SmartObjectPoolDeleterContext(SmartObjectPoolDeleterContext&&) noexcept = default;

  /*!
   * \brief  Default copy assignment operator.
   * \return A reference to the assigned-to object.
   */
  SmartObjectPoolDeleterContext& operator=(SmartObjectPoolDeleterContext const&) & noexcept = default;

  /*!
   * \brief  Default move assignment operator.
   * \return A reference to the assigned-to object.
   */
  SmartObjectPoolDeleterContext& operator=(SmartObjectPoolDeleterContext&&) & noexcept = default;
};

/*!
 * \brief Proxy on the object side for deleting SmartPoolObjects.
 */
class DeleteableSmartPoolObject {
 public:
  /*!
   * \brief Constructor for SmartPoolObjects.
   * \param deleter_context DeleterContext for the current object.
   */
  explicit DeleteableSmartPoolObject(SmartObjectPoolDeleterContext* deleter_context) noexcept
      : deleter_context_(deleter_context) {}

  /*!
   * \brief  Returns the DeleterContext for deleting the current object.
   * \return DeleterContext.
   */
  SmartObjectPoolDeleterContext* GetDeleterContext() const noexcept { return deleter_context_; }

  /*!
   * \brief Destructor.
   */
  virtual ~DeleteableSmartPoolObject() noexcept = default;

 protected:
  /*!
   * \brief Default copy constructor.
   */
  DeleteableSmartPoolObject(DeleteableSmartPoolObject const&) noexcept = default;

  /*!
   * \brief Default move constructor.
   */
  DeleteableSmartPoolObject(DeleteableSmartPoolObject&&) noexcept = default;

  /*!
   * \brief Default copy assignment operator.
   * \return A reference to the assigned-to object.
   */
  DeleteableSmartPoolObject& operator=(DeleteableSmartPoolObject const&) & noexcept = default;

  /*!
   * \brief Default move assignment operator.
   * \return A reference to the assigned-to object.
   */
  DeleteableSmartPoolObject& operator=(DeleteableSmartPoolObject&&) & noexcept = default;

 private:
  /*!
   * \brief Deleter context for a smart pool object.
   */
  SmartObjectPoolDeleterContext* deleter_context_;
};

namespace detail {
/*!
 * \brief Master Deleter class using the DeleterContext from the object to delete.
 */
class SmartBaseTypeObjectPoolDeleter final {
 public:
  /*!
   * \brief The actual deleter function for const SmartPoolObject.
   * \param ptr Pointer to object.
   */
  void operator()(DeleteableSmartPoolObject* ptr) noexcept {
    if (ptr != nullptr) {
      SmartObjectPoolDeleterContext* const context{ptr->GetDeleterContext()};
      // VECTOR NL AutosarC++17_10-M0.1.2: MD_VAC_M0.1.2_infeasiblePaths
      if (context != nullptr) {
        context->destroy(ptr);
      } else {
        // VECTOR NC AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
        // VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction
        delete ptr;
      }
    }
  }

  /*!
   * \brief The actual deleter function for const SmartPoolObject.
   * \param ptr Pointer to object.
   */
  void operator()(DeleteableSmartPoolObject const* ptr) noexcept {
    if (ptr != nullptr) {
      SmartObjectPoolDeleterContext* const context{ptr->GetDeleterContext()};
      // VECTOR NL AutosarC++17_10-M0.1.2: MD_VAC_M0.1.2_infeasiblePaths
      if (context != nullptr) {
        context->destroy(ptr);
      } else {
        // VECTOR NC AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
        // VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction
        delete ptr;
      }
    }
  }
};

// VECTOR Next Construct AutosarC++17_10-M5.2.3: MD_VAC_M5.2.3_castFromPolymorphicBaseClassToDerivedClass
// VECTOR Next Construct VectorC++-V5.2.3: MD_VAC_V5-2-3_castFromPolymorphicBaseClassToDerivedClass
/*!
 * \brief Deleter for unique_ptrs to ObjectPool-managed objects
 *        This class is only used internally in SmartBaseTypeObjectPool<T> as a deleter for std::unique_ptr<T>, using
 *        SmartBaseTypeObjectPoolDeleter as a delegate. In this context, it is safe to use static_cast<T*>.
 */
template <class T, typename alloc>
class SmartObjectPoolDeleterContextImpl : public SmartObjectPoolDeleterContext {
  static_assert(std::is_base_of<DeleteableSmartPoolObject, T>::value, "T must implement DeleteableSmartPoolObject");

 public:
  /*!
   * \brief Default copy constructor.
   */
  SmartObjectPoolDeleterContextImpl(SmartObjectPoolDeleterContextImpl const&) noexcept = default;

  /*!
   * \brief Default move constructor.
   */
  SmartObjectPoolDeleterContextImpl(SmartObjectPoolDeleterContextImpl&&) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Default copy assignment operator.
   * \return A reference to the assigned-to object.
   */
  SmartObjectPoolDeleterContextImpl& operator=(SmartObjectPoolDeleterContextImpl const&) & noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Default move assignment operator.
   * \return A reference to the assigned-to object.
   */
  SmartObjectPoolDeleterContextImpl& operator=(SmartObjectPoolDeleterContextImpl&&) & noexcept = default;
  /*!
   * \brief Default overridden destructor.
   */
  ~SmartObjectPoolDeleterContextImpl() noexcept override = default;

  /*!
   * \brief Constructor.
   * \param pool Object pool to return an object to on destruction.
   */
  explicit SmartObjectPoolDeleterContextImpl(ObjectPool<T, alloc>* pool) noexcept
      : SmartObjectPoolDeleterContext{}, pool_{pool} {}

  // VECTOR NC AutosarC++17_10-M0.1.2: MD_VAC_M0.1.2_infeasiblePaths
  /*!
   * \copydoc SmartObjectPoolDeleterContext::destroy
   */
  void destroy(DeleteableSmartPoolObject* ptr) noexcept override {
    if (ptr != nullptr) {
      if (pool_ != nullptr) {
        pool_->destroy(static_cast<T*>(ptr));
      } else {
        // VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction
        delete ptr;
      }
    }
  }

  // VECTOR NC AutosarC++17_10-M0.1.2: MD_VAC_M0.1.2_infeasiblePaths
  /*!
   * \copydoc SmartObjectPoolDeleterContext::destroy
   */
  void destroy(DeleteableSmartPoolObject const* ptr) noexcept override {
    if (ptr != nullptr) {
      if (pool_ != nullptr) {
        pool_->destroy(static_cast<T const*>(ptr));
      } else {
        // VECTOR Next Line AutosarC++17_10-A18.5.2: MD_VAC_A18.5.2_libraryFunction
        delete ptr;
      }
    }
  }

 private:
  /*!
   * \brief The ObjectPool to return an object to on destruction.
   */
  ObjectPool<T, alloc>* pool_;
};
}  // namespace detail

/*!
 * \brief Smart Pointer type used by the ObjectPool.
 */
template <class T>
using SmartBaseTypeObjectPoolUniquePtr = std::unique_ptr<T, detail::SmartBaseTypeObjectPoolDeleter>;

/*!
 * \brief Smart Pointer type used by the ObjectPool for const Objects.
 */
template <class T>
using SmartBaseTypeObjectPoolUniquePtrToConst = std::unique_ptr<T const, detail::SmartBaseTypeObjectPoolDeleter>;

// VECTOR Next Construct AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend
/*!
 * \brief ObjectPool that deals in unique_ptrs instead of raw pointers, allowing for base type pointers.
 * \trace CREQ-158628
 */
template <class T, typename alloc = std::allocator<T>>
class SmartBaseTypeObjectPool {
  /*! \brief Friendly class declaration. */
  friend class SmartBaseTypeObjectPoolTestFixture;
  static_assert(std::is_base_of<DeleteableSmartPoolObject, T>::value, "T must implement DeleteableSmartPoolObject");

 public:
  /*!
   * \brief Typedef for the raw-pointer ObjectPool used for storage.
   */
  using StoragePoolType = ObjectPool<T, alloc>;

  /*!
   * \brief Typedef for the size type used in this ObjectPool.
   */
  using size_type = typename StoragePoolType::size_type;

  /*!
   * \brief Deleter for unique_ptrs to ObjectPool-managed objects.
   */
  using Deleter = detail::SmartBaseTypeObjectPoolDeleter;

  /*!
   * \brief Smart Pointer type used by the ObjectPool.
   */
  using UniquePtr = SmartBaseTypeObjectPoolUniquePtr<T>;

  /*!
   * \brief Smart Pointer type used by the ObjectPool.
   */
  using UniquePtrToConst = SmartBaseTypeObjectPoolUniquePtrToConst<T>;

  /*!
   * \brief Constructor.
   */
  SmartBaseTypeObjectPool() noexcept {}

  /*!
   * \brief Default copy constructor.
   */
  SmartBaseTypeObjectPool(SmartBaseTypeObjectPool const&) noexcept = default;

  /*!
   * \brief Default move constructor.
   */
  SmartBaseTypeObjectPool(SmartBaseTypeObjectPool&&) noexcept = default;

  /*!
   * \brief  Default copy assignment operator.
   * \return A reference to the assigned-to object.
   */
  SmartBaseTypeObjectPool& operator=(SmartBaseTypeObjectPool const&) & noexcept = default;

  /*!
   * \brief  Default move assignment operator.
   * \return A reference to the assigned-to object.
   */
  SmartBaseTypeObjectPool& operator=(SmartBaseTypeObjectPool&&) & noexcept = default;

  /*!
   * \brief Default virtual destructor.
   */
  virtual ~SmartBaseTypeObjectPool() noexcept = default;

  /*!
   * \brief Update the memory allocation.
   *        The current implementation only allows a single allocation. All further reservations will only limit the
   *        visible memory. All subsequent calls to reserve() where new_capacity is greater than the initial
   *        new_capacity will be rejected with a bad_alloc.
   * \param new_capacity The number of T's to reserve space for.
   */
  void reserve(size_type new_capacity) noexcept { pool_.reserve(new_capacity); }

  // VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
  /*!
   * \brief   Allocate and initialize an object of the pool.
   * \details Calls the constructor with the given set of arguments. For all other properties, see allocate().
   *          Terminates if the constructor of the new object throws.
   * \param   args Arguments forwarded to the constructor.
   * \return  A pointer to an initialized T.
   */
  template <typename... Args>
  auto create(Args&&... args) noexcept -> UniquePtr {
    typename StoragePoolType::pointer const ptr{pool_.create(std::forward<Args>(args)..., &deleter_context_)};
    UniquePtr smart_ptr{ptr, Deleter()};
    return smart_ptr;
  }

  /*!
   * \brief  Determine whether the allocated memory is full.
   * \return False if there is space left in this object pool, i.e., the next call to create() will
   *         succeed. Otherwise true.
   */
  bool full() const noexcept { return pool_.full(); }

  /*!
   * \brief  Determine whether there are not objects currently allocated.
   * \return True if no element is allocated inside this object pool. false otherwise.
   */
  bool empty() const noexcept { return pool_.empty(); }

  /*!
   * \brief  The total number of objects that can be allocated in this pool.
   * \return The total number of objects that can be allocated in this pool.
   */
  size_type capacity() const noexcept { return pool_.capacity(); }

  /*!
   * \brief  The number of objects currently allocated in this pool.
   * \return The number of objects currently allocated in this pool.
   */
  size_type size() const noexcept { return pool_.size(); }

 private:
  /*!
   * \brief pool used for storage.
   */
  StoragePoolType pool_{};

  /*!
   * \brief deleter context for ObjectPool-managed objects.
   */
  detail::SmartObjectPoolDeleterContextImpl<T, alloc> deleter_context_{&pool_};
};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_SMART_BASE_TYPE_OBJECT_POOL_H_
