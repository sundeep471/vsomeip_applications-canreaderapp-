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

#ifndef LIB_VAC_INCLUDE_VAC_MEMORY_OBJECT_POOL_H_
#define LIB_VAC_INCLUDE_VAC_MEMORY_OBJECT_POOL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <mutex>
#include <new>
#include <utility>

#include "ara/core/abort.h"
#include "vac/container/static_vector.h"
#include "vac/testing/test_adapter.h"

namespace vac {
namespace memory {

// VECTOR Next Construct AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend
/*!
 * \brief   Class to manage pools of objects of a specific type.
 * \details Note: The lifetime of the ObjectPool must exceed the lifetime of the allocated objects. Otherwise, all
 *          pointers obtained from the pool will be dangling. Note: Currently, there is only the singleton instance
 *          supported, i.e., the object pool does not allow on-stack allocations.
 *          Note: ObjectPoolImpl is an internal helper class. Only use ObjectPool or SmartObjectPool in your code. The
 *          current implementation of this class heavily relies on exceptions. This is purely for development purposes:
 *          The occurrence of any of these exceptions indicates either a serious programming error
 *          (returning memory to the wrong pool) or a misconfiguration (an insufficient amount of resources was
 *          configured). For both cases, it must be ensured prior to production that the error conditions do not occur.
 *          TODO(PAASR-1154): Make all methods noexcept.
 */
template <class T, bool moveable, typename alloc = std::allocator<T>>
class ObjectPoolImpl {
  /*! \brief Friendly class declaration. */
  friend class ObjectPoolTest;
  /*! \brief Friendly class declaration. */
  friend class SmartBaseTypeObjectPoolTestFixture;

  FRIEND_TEST(ObjectPoolTest, ReservedSizeAndCapacity);

  static_assert(!(std::is_const<T>::value), "Object pool cannot be used with const Objects.");

 public:
  /*!
   * \brief Typedef for raw pointers.
   */
  using pointer = T*;

  /*!
   * \brief Typedef for raw pointers.
   */
  using const_pointer = T const*;

  /*!
   * \brief Typedef for the size type used in this implementation.
   */
  using size_type = std::size_t;

  /*!
   * \brief Constructor to create an empty ObjectPool.
   */
  ObjectPoolImpl() noexcept {}

  /*!
   * \brief Deleted copy constructor.
   */
  ObjectPoolImpl(ObjectPoolImpl const&) = delete;

  /*!
   * \brief Deleted copy assignment.
   * \return
   */
  ObjectPoolImpl& operator=(ObjectPoolImpl const&) = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Default move constructor.
   */
  constexpr ObjectPoolImpl(ObjectPoolImpl&&) noexcept = default;

  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  ObjectPoolImpl& operator=(ObjectPoolImpl&&) & noexcept = default;

  /*!
   * \brief Default destructor which does not call destructor of objects in owned memory.
   * \trace CREQ-158625
   */
  virtual ~ObjectPoolImpl() noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief   Move constructor.
   * \details This constructor is only available of the "moveable" template parameter is set to true.
   * \param   other The ObjectPoolImpl to move from.
   */
  template <typename U = T, typename = typename std::enable_if<moveable, U>::type>
  ObjectPoolImpl(ObjectPoolImpl&& other) noexcept {
    std::lock(other.free_list_mutex_, free_list_mutex_);
    std::lock_guard<std::mutex> const other_lock{{other.free_list_mutex_}, {std::adopt_lock}};
    std::lock_guard<std::mutex> const this_lock{{free_list_mutex_}, {std::adopt_lock}};

    storage_ = std::move(other.storage_);
    std::swap(free_list_, other.free_list_);
    std::swap(allocation_count_, other.allocation_count_);
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief   Move assignment.
   * \details This constructor is only available of the "moveable" template parameter is set to true.
   * \param   other The ObjectPoolImpl to move from.
   * \return  A reference to the moved-to ObjectPoolImpl.
   */
  template <typename U = T, typename = typename std::enable_if<moveable, U>::type>
      ObjectPoolImpl& operator=(ObjectPoolImpl&& other) & noexcept {
    std::lock(other.free_list_mutex_, free_list_mutex_);
    std::lock_guard<std::mutex> const other_lock{{other.free_list_mutex_}, {std::adopt_lock}};
    std::lock_guard<std::mutex> const this_lock{{free_list_mutex_}, {std::adopt_lock}};

    storage_.swap(other.storage_);
    std::swap(free_list_, other.free_list_);
    std::swap(allocation_count_, other.allocation_count_);

    return *this;
  }

  /*!
   * \brief   Update the memory allocation.
   * \details The current implementation only allows a single allocation. All further reservations will only limit the
   *          visible memory. All subsequent calls to reserve() where new_capacity is greater than the initial
   *          new_capacity will be ignored.
   * \param   new_capacity The number of T's to reserve space for.
   * \trace   CREQ-158622
   */
  void reserve(size_type new_capacity) noexcept {
    std::lock_guard<std::mutex> const lock{free_list_mutex_};
    if (new_capacity > storage_.size()) {
      // Resize default-constructs all StoredType unions. Their default constructor activates the StoredType* member.
      storage_.resize(new_capacity);
      // Create the free list inside of storage
      free_list_ = nullptr;
      for (StoredType& elem : storage_) {
        elem.free = free_list_;
        free_list_ = &elem;
      }
    }
  }

  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_parametersBasedOnBuiltInTypes
  /*!
   * \brief   Allocate and initialize an object of the pool.
   * \details Calls the constructor with the given set of arguments.
   *          For all other properties, see allocate(). Terminates if the constructor of the object throws.
   * \param   args Arguments forwarded to the constructor.
   * \return  A pointer to an initialized T.
   * \trace   CREQ-158624
   */
  template <typename... Args>
  auto create(Args&&... args) noexcept -> pointer {
    pointer const ptr{this->allocate()};
    new (ptr) T(std::forward<Args>(args)...);
    return ptr;
  }

  /*!
   * \brief   Destroy an object and return it to the pool.
   * \details Calls the destructor. For all other properties, see deallocate().
   *          Terminates if the destructor of the object throws.
   * \param   ptr A pointer to an initialized T.
   * \pre     The pointer should be managed, otherwise ara::core::Abort will be triggered.
   * \trace   CREQ-158624
   */
  void destroy(pointer ptr) noexcept {
    if (ptr != nullptr) {
      if (IsManaged(ptr)) {
        ptr->~T();
        deallocate(ptr);
      } else {
        ara::core::Abort("vac::memory::ObjectPoolImpl::destroy(pointer): Pointer is not managed!");
      }
    }
  }

  /*!
   * \brief   Destroy an object and return it to the pool.
   * \details Calls the destructor. For all other properties, see deallocate().
   *          Terminates if the destructor of the object throws.
   * \param   ptr A pointer to an initialized T.
   * \pre     The pointer should be managed, otherwise ara::core::Abort will be triggered.
   * \trace   CREQ-158624
   */
  void destroy(const_pointer ptr) noexcept {
    // VECTOR NL AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_constCastReducesCodeDuplication
    destroy(const_cast<pointer>(ptr));
  }

  /*!
   * \brief  Determine whether the allocated memory is full.
   * \return False if there is space left in this object pool, i.e., the next call to create() or allocate() will
   *         succeed. True otherwise.
   */
  bool full() const noexcept { return free_list_ == nullptr; }

  /*!
   * \brief  Determine whether there are not objects currently allocated.
   * \return True if no element is allocated inside this object pool. false otherwise.
   */
  bool empty() const noexcept { return allocation_count_ == 0; }

  /*!
   * \brief  The total number of objects that can be allocated in this pool.
   * \return The total number of objects that can be allocated in this pool.
   */
  size_type capacity() const noexcept { return storage_.size(); }

  /*!
   * \brief  The number of objects currently allocated in this pool.
   * \return The number of objects currently allocated in this pool.
   */
  size_type size() const noexcept { return allocation_count_; }

 private:
  // VECTOR Next Construct AutosarC++17_10-A9.5.1: MD_VAC_A9.5.1_unionsShallNotBeUsed
  /*!
   * \brief   Union type for the storage.
   * \details Allows the storage to be created with the size of max(sizeof(T), sizeof(FreeListNodeType));.
   *          Note: Unions are not styleguide conformant, this is a deliberate styleguide violation. Justification:
   *          The Idea is that the same block of memory can either be interpreted as a node of the free list (state "not
   *          allocated") or as an actual block of data (state "allocated"). Changing the meaning of the
   *          block upon allocation/deallocation is a fairly common practice.
   *          Using the union here is dangerous in the way that the underlying memory may be accessed in the way in
   *          which it is not initialized. This can happen for two reasons:
   *            (1) Programming Error inside ObjectPool. ObjectPool is the class that makes the switch between
   *                both types. This must be handled using code inspection and testing.
   *            (2) Accessing through a dangling pointer to the data member. This is just as bad as any dangling pointer
   *                access and is independent of the use of a union.
   *          There are multiple other solutions to this problem, but none of them are any better:
   *            (1) Drop the union and use a reinterpret_cast to convert the pointer. This means you have to
   *                cast *a lot* in this class. It also means that we need to juggle min/max(sizeof's), which we get for
   *                free using the union.
   *            (2) Make the Free List intrusive, e.g., Create a "Poolable" Interface that classes must implement.
   *                However, it is not clear how you can destroy an object and then still use its free list
   *                interpretation.
   *            (3) Make a dedicated, non-intrusive free list (class StoredType { T data; StoredType* free; }
   *                rather than union StoredType). However, this requires management of an additional "non-free-list" to
   *                store allocated blocks, requires a search in this this list to find the correct block upon
   *                deallocation, and also uses more memory.
   *          Therefore, using the union in this particular case is no worse than any of the other solutions while
   *          requiring the least amount of code.
   */
  union StoredType {
    // VECTOR Disable VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
    /*!
     * \brief Member for the free list.
     */
    StoredType* free;
    /*!
     * \brief Member for the payload data.
     */
    T data;

    /*!
     * \brief Constructor.
     */
    StoredType() noexcept : free{nullptr} {}

    // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
    /*!
     * \brief Destructor.
     */
    ~StoredType() noexcept { free = nullptr; }

    /*!
     * \brief Default copy constructor.
     */
    StoredType(StoredType const& other) noexcept = default;

    // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
    /*!
     * \brief Default move constructor.
     */
    StoredType(StoredType&& other) noexcept = default;

    /*!
     * \brief Default copy operator.
     */
    StoredType& operator=(StoredType const& other) & noexcept = default;

    // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
    /*!
     * \brief Default move operator.
     */
    StoredType& operator=(StoredType&& other) & noexcept = default;
  };
  // VECTOR Enable VectorC++-V11.0.2

  /*!
   * \brief  Determine whether a pointed-to location is managed by this ObjectPool.
   * \param  ptr The pointer to check.
   * \return True if the location is managed, false otherwise. Also returns false on nullptr.
   */
  bool IsManaged(T const* const ptr) const noexcept {
    bool ret_value{false};
    if ((storage_.data() != nullptr) && (ptr != nullptr)) {
      // VECTOR NL AutosarC++17_10-M5.0.18: MD_VAC_M5.0.18_comparingUnrelatedPointers
      ret_value = (static_cast<void const*>(storage_.data()) <= static_cast<void const*>(ptr)) &&
                  (static_cast<void const*>(ptr) <= static_cast<void const*>(&storage_.back()));
    }
    return ret_value;
  }

  /*!
   * \brief   Return a pointer to a chunk of memory large enough for storing a T.
   * \details When the pool is exhausted, ara::core::Abort() is called.
   * \pre     The head pointer to free_list can not be nullptr, otherwise ara::core::Abort will be triggered.
   * \return  Returns a pointer to a chunk of memory large enough for storing a T. The memory is not initialized, no T
   *          is constructed in this place.
   */
  T* allocate() noexcept {
    if (free_list_ == nullptr) {
      // Out of memory.
      ara::core::Abort("vac::memory::ObjectPoolImpl::allocate(): The pool is out of memory!");
    }
    std::unique_lock<std::mutex> lock{free_list_mutex_};
    StoredType* const element{free_list_};
    free_list_ = element->free;
    ++allocation_count_;
    lock.unlock();
    // Deactivate free list member.
    element->free = nullptr;
    return &element->data;
  }

  /*!
   * \brief  Return a chunk of memory to the pool. The passed object is not destructed.
   * \param  ptr Pointer to the chunk of memory to be returned to the pool.
   * \pre    The pointer should be managed, otherwise ara::core::Abort will be triggered.
   */
  void deallocate(T* ptr) noexcept {
    if (ptr != nullptr) {
      if (this->IsManaged(ptr)) {
        std::lock_guard<std::mutex> const lock{free_list_mutex_};

        // VECTOR NL AutosarC++17_10-A5.2.4: MD_VAC_A5.2.4_reinterpretCast
        StoredType* const element{reinterpret_cast<StoredType*>(ptr)};  // Convert from data member back to union.
        // Activate free list member.
        element->free = free_list_;
        free_list_ = element;
        --allocation_count_;
      } else {
        ara::core::Abort("vac::memory::ObjectPoolImpl::deallocate(T*): Pointer is not managed!");
      }
    }
  }

  /*!
   * \brief The actual memory location managed by the pool.
   */
  vac::container::StaticVector<StoredType, alloc> storage_{};

  /*!
   * \brief Head pointer to the free list.
   */
  StoredType* free_list_{nullptr};

  /*!
   * \brief Mutex to synchronize access to the free_list_.
   */
  std::mutex free_list_mutex_{};

  /*!
   * \brief Number of elements the ObjectPool has handed out and that have not been returned.
   */
  size_type allocation_count_{0};
};

/*!
 * \brief Non-moveable ObjectPool for public usage.
 * \trace CREQ-158621, CREQ-158626
 */
template <class T, typename alloc = std::allocator<T>>
using ObjectPool = ObjectPoolImpl<T, false, alloc>;

/*!
 * \brief Moveable ObjectPool for public usage.
 */
template <class T, typename alloc = std::allocator<T>>
using MoveableObjectPool = ObjectPoolImpl<T, true, alloc>;

/*!
 * \brief Deleter for unique_ptrs to ObjectPool-managed objects.
 */
template <class T, typename alloc = std::allocator<T>>
class SmartObjectPoolDeleter final {
 public:
  /*!
   * \brief Noexcept constructor.
   */
  SmartObjectPoolDeleter() noexcept : SmartObjectPoolDeleter(nullptr) {}

  /*!
   * \brief Constructor.
   * \param pool Object pool to return an object to on destruction.
   */
  explicit SmartObjectPoolDeleter(ObjectPool<T, alloc>* pool) noexcept : pool_{pool} {}

  /*!
   * \brief Copy constructor.
   */
  SmartObjectPoolDeleter(SmartObjectPoolDeleter const&) noexcept = default;

  /*!
   * \brief  Copy assignment.
   * \return A reference to the assigned-to object.
   */
  SmartObjectPoolDeleter& operator=(SmartObjectPoolDeleter const&) & noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Move constructor.
   */
  SmartObjectPoolDeleter(SmartObjectPoolDeleter&&) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Move assignment.
   * \return A reference to the assigned-to object.
   */
  SmartObjectPoolDeleter& operator=(SmartObjectPoolDeleter&&) & noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Default destructor.
   */
  ~SmartObjectPoolDeleter() noexcept = default;

  /*!
   * \brief The actual deleter function.
   * \param ptr A pointer to the ObjectPool.
   */
  void operator()(typename ObjectPool<T, alloc>::pointer ptr) noexcept {
    if (pool_ != nullptr) {
      pool_->destroy(ptr);
    } else {
      if (ptr != nullptr) {
        ptr->~T();
      }
    }
  }

  /*!
   * \brief The actual deleter function for const T.
   * \param ptr A pointer to the ObjectPool.
   */
  void operator()(typename ObjectPool<T, alloc>::const_pointer ptr) noexcept {
    if (pool_ != nullptr) {
      pool_->destroy(ptr);
    } else {
      if (ptr != nullptr) {
        // VECTOR NL AutosarC++17_10-A5.2.3: MD_VAC_A5.2.3_impicitCastRemovesConstQualification
        ptr->~T();
      }
    }
  }

 private:
  /*!
   * \brief The ObjectPool to return an object to on destruction.
   */
  ObjectPool<T, alloc>* pool_;
};

/*!
 * \brief Smart Pointer type used by the ObjectPool.
 */
template <class T, typename alloc = std::allocator<T>>
using SmartObjectPoolUniquePtr = std::unique_ptr<T, SmartObjectPoolDeleter<T, alloc>>;

/*!
 * \brief Smart Pointer type used by the ObjectPool for const Objects.
 */
template <class T, typename alloc = std::allocator<T>>
using SmartObjectPoolUniquePtrToConst = std::unique_ptr<T const, SmartObjectPoolDeleter<T, alloc>>;

// VECTOR Next Construct AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend
/*!
 * \brief ObjectPool that deals in unique_ptrs instead of raw pointers.
 * \trace CREQ-158627
 */
template <class T, typename alloc = std::allocator<T>>
class SmartObjectPool final {
  /*! \brief Friendly class declaration. */
  friend class ObjectPoolTest;
  FRIEND_TEST(ObjectPoolTest, ReservedSizeAndCapacity);

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
  using Deleter = SmartObjectPoolDeleter<T, alloc>;

  /*!
   * \brief Smart Pointer type used by the ObjectPool.
   */
  using UniquePtr = SmartObjectPoolUniquePtr<T, alloc>;

  /*!
   * \brief Smart Pointer type used by the ObjectPool.
   */
  using UniquePtrToConst = SmartObjectPoolUniquePtrToConst<T, alloc>;

  /*!
   * \brief Default constructor.
   */
  SmartObjectPool() noexcept = default;

  /*!
   * \brief Deleted copy constructor.
   */
  SmartObjectPool(SmartObjectPool const&) = delete;

  /*!
   * \brief Deleted copy assignment.
   * \return
   */
  SmartObjectPool& operator=(SmartObjectPool const&) = delete;

  /*!
   * \brief Deleted move constructor.
   */
  SmartObjectPool(SmartObjectPool&&) = delete;

  /*!
   * \brief Deleted move assignment.
   * \return
   */
  SmartObjectPool& operator=(SmartObjectPool&&) = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Default destructor.
   */
  ~SmartObjectPool() noexcept = default;

  /*!
   * \brief Update the memory allocation.
   *        The current implementation only allows a single allocation. All further reservations will only limit the
   *        visible memory. All subsequent calls to reserve() where new_capacity is greater than the initial
   *        new_capacity will be rejected with a bad_alloc. \param new_capacity The number of T's to reserve space for.
   */
  void reserve(size_type new_capacity) noexcept { pool_.reserve(new_capacity); }

  // VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
  /*!
   * \brief   Allocate and initialize an object of the pool.
   * \details Calls the constructor with the given set of arguments. For all other properties, see allocate().
   *          Terminates if the constructor of the object throws.
   * \param   args Arguments forwarded to the constructor.
   * \return  A unique pointer to an initialized T.
   */
  template <typename... Args>
  auto create(Args&&... args) noexcept -> UniquePtr {
    typename StoragePoolType::pointer const ptr{pool_.create(std::forward<Args>(args)...)};
    UniquePtr smart_ptr{ptr, Deleter(&pool_)};
    return smart_ptr;
  }

  /*!
   * \brief  Determine whether the allocated memory is full.
   * \return False if there is space left in this object pool, i.e., the next call to create() will
   *         succeed.
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
   * \brief ObjectPool used for storage.
   */
  StoragePoolType pool_;
};

}  // namespace memory
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_MEMORY_OBJECT_POOL_H_
