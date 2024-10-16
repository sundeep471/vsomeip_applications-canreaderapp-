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
/**        \file  unique_function.h
 *        \brief  A move-only equivalent of std::function.
 *
 *      \details  vac::language::UniqueFunction provides a move-only equivalent of std::function.
 *                This implementation always allocates dynamic memory to store the callable.
 *                Uses virtual functions to allow UniqueFunction to call any callable type and correctly
 *                deallocate memory and destroy stored callables and allocators.
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_UNIQUE_FUNCTION_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_UNIQUE_FUNCTION_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <type_traits>
#include <utility>

#include "ara/core/abort.h"
#include "vac/language/allocator_arg.h"
#include "vac/language/cpp17_backport.h"
#include "vac/language/cpp20_backport.h"
#include "vac/language/detail/function_signature.h"
#include "vac/language/workaround.h"
#include "vac/memory/phase_managed_allocator.h"

namespace vac {
namespace language {
namespace detail {
namespace uniquefunction {

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_noVirtualDtor
/*! \brief Holds a unique pointer to dynamically allocated storage for a callable and an allocator. */
template <typename Signature>
class DynamicStorage;

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_noVirtualDtor
/*!
 * \brief Call operator implementation for signatures with cv ref qualifiers. Inherits from DynamicStorage.
 *        Provides the call operator and callable storage for UniqueFunction.
 */
template <typename Sig1, typename Sig2>
class StorageAndOperatorImpl;

/*! \brief Specialization for one the six supported cv ref qualifiers. */
template <typename Signature, typename R, typename... Args>
class StorageAndOperatorImpl<Signature, R(Args...)> : public DynamicStorage<Signature> {
 public:
  /*!
   * \brief  Call operator.
   * \param  args Input of callable.
   * \return Result after invoking the callable.
   */
  R operator()(Args&&... args) noexcept {
    this->AssertCallable();
    // VECTOR NC AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
    // VECTOR NL AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
    return static_cast<R>(this->function_manager_ptr_->Invoke(std::forward<Args>(args)...));
  }
};

/*! \brief Specialization for one the six supported cv ref qualifiers. */
template <typename Signature, typename R, typename... Args>
class StorageAndOperatorImpl<Signature, R(Args...) const> : public DynamicStorage<Signature> {
 public:
  // VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
  /*!
   * \brief Call operator.
   * \param  args Input of callable.
   * \return Result after invoking the callable.
   */
  R operator()(Args&&... args) const noexcept {
    this->AssertCallable();
    // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
    return static_cast<R>(this->function_manager_ptr_->Invoke(std::forward<Args>(args)...));
  }
};

/*! \brief Specialization for one the six supported cv ref qualifiers. */
template <typename Signature, typename R, typename... Args>
class StorageAndOperatorImpl<Signature, R(Args...)&> : public DynamicStorage<Signature> {
 public:
  // VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
  /*!
   * \brief Call operator.
   * \param  args Input of callable.
   * \return Result after invoking the callable.
   */
  R operator()(Args&&... args) & noexcept {
    this->AssertCallable();
    return static_cast<R>(this->function_manager_ptr_->Invoke(std::forward<Args>(args)...));
  }
};

/*! \brief Specialization for one the six supported cv ref qualifiers. */
template <typename Signature, typename R, typename... Args>
class StorageAndOperatorImpl<Signature, R(Args...) const&> : public DynamicStorage<Signature> {
 public:
  // VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
  /*!
   * \brief  Call operator.
   * \param  args Input of callable.
   * \return Result after invoking the callable.
   */
  R operator()(Args&&... args) const& noexcept {
    this->AssertCallable();
    return static_cast<R>(this->function_manager_ptr_->Invoke(std::forward<Args>(args)...));
  }
};

/*! \brief Specialization for one the six supported cv ref qualifiers. */
template <typename Signature, typename R, typename... Args>
class StorageAndOperatorImpl<Signature, R(Args...) &&> : public DynamicStorage<Signature> {
 public:
  // VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
  /*!
   * \brief  Call operator.
   * \param  args Input of callable.
   * \return Result after invoking the callable.
   */
  R operator()(Args&&... args) && noexcept {
    this->AssertCallable();
    // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
    return static_cast<R>(this->function_manager_ptr_->Invoke(std::forward<Args>(args)...));
  }
};

/*! \brief Specialization for one the six supported cv ref qualifiers. */
template <typename Signature, typename R, typename... Args>
class StorageAndOperatorImpl<Signature, R(Args...) const&&> : public DynamicStorage<Signature> {
 public:
  // VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
  /*!
   * \brief Call operator.
   * \param  args Input of callable.
   * \return Result after invoking the callable.
   */
  R operator()(Args&&... args) const&& noexcept {
    this->AssertCallable();
    // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
    return static_cast<R>(this->function_manager_ptr_->Invoke(std::forward<Args>(args)...));
  }
};

/*! \brief An alias which forwards Signature twice instead of having UniqueFunction do this. */
template <typename Signature>
using StorageAndCallOperator = StorageAndOperatorImpl<Signature, Signature>;

/*!
 * \brief Interface for a stored callable object. Enables virtual calls, deallocation, and destruction.
 *        Used from StorageAndCallOperator::operator() inherited by UniqueFunction.
 */
template <typename R, typename... Args>
class IFunctionManager {
 public:
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  virtual ~IFunctionManager() noexcept = default;

  /*!
   * \brief Invocation interface
   * \param ... Input to be invoked.
   * \return
   */
  virtual R Invoke(Args&&...) noexcept = 0;

  /*! \brief onDelete allows the allocator to be type erased. */
  virtual void OnDelete() noexcept = 0;

  // VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_noVirtualDtor
  /*! \brief Deleter functor to be used in std::unique_ptr. This delegates the deletion to the virtual onDelete. */
  class Deleter {
   public:
    /*!
     * \brief Deletion operator.
     * \param obj_p Callable object to be deleted.
     */
    // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
    void operator()(IFunctionManager* obj_p) noexcept { obj_p->OnDelete(); }
  };

  /*! \brief Default constructor. */
  constexpr IFunctionManager() noexcept = default;

  /*! \brief Deleted copy constructor. */
  IFunctionManager(IFunctionManager const&) = delete;

  /*! \brief Deleted moveconstructor. */
  IFunctionManager(IFunctionManager&&) = delete;

  /*!
   * \brief Deleted copy assignment.
   * \return
   */
  IFunctionManager& operator=(IFunctionManager const&) = delete;

  /*!
   * \brief Deleted move assignment.
   * \return
   */
  IFunctionManager& operator=(IFunctionManager&&) = delete;
};

/*!
 * \brief Stores a callable object together with an allocator. Handles deallocation of self with custom allocator.
 *        This is the storage implementation that DynamicStorage actually points to.
 */
template <typename Alloc, typename FuncT, bool InvokeAsRRef, bool InvokeAsConst, typename R, typename... Args>
class FunctionManager final : public IFunctionManager<R, Args...> {
 public:
  /*! \brief Rebound allocator type for FuncT. */
  using ReboundAlloc = typename vac::language::workaround::RebindAllocator<Alloc, FunctionManager>::type;

  // VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor
  // VECTOR Disable AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
  /*!
   * \brief Move constructor for callables.
   * \param alloc The allocator to construct the FunctionManager.
   * \param func The callables to construct the FunctionManager.
   */
  explicit FunctionManager(Alloc const& alloc, FuncT&& func) noexcept
      : func_{std::forward<FuncT>(func)}, alloc_{alloc} {}
  // VECTOR Enable AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference

  /*!
   * \brief Copy constructor for callables.
   * \param alloc The allocator to copy construct the FunctionManager.
   * \param func The callables to construct the FunctionManager.
   */
  explicit FunctionManager(Alloc const& alloc, FuncT const& func) noexcept : func_{func}, alloc_{alloc} {}

  /*!
   * \brief In place constructor.
   * \param alloc The allocator to construct the FunctionManager.
   * \param args Input to construct the FunctionManager.
   */
  template <typename... CArgs>
  explicit FunctionManager(Alloc const& alloc, ara::core::in_place_t, CArgs&&... args) noexcept
      : func_{std::forward<CArgs>(args)...}, alloc_{alloc} {}

  // VECTOR Next Construct AutosarC++17_10-A8.5.4: MD_VAC_A8.5.4_initializerlistShallBeDefinedInClasses
  // VECTOR Next Construct VectorC++-V8.5.4: MD_VAC_V8-5-4_ctorShallNotBeDefinedToAcceptStdInitializerList
  /*!
   * \brief In place constructor for initializer list.
   * \param alloc The allocator to construct the FunctionManager.
   * \param il Initializer list to construct the FunctionManager.
   * \param args Input to construct the FunctionManager.
   */
  template <typename I, typename... CArgs>
  explicit FunctionManager(Alloc const& alloc, ara::core::in_place_t, std::initializer_list<I> il,
                           CArgs&&... args) noexcept
      : func_{std::move(il), std::forward<CArgs>(args)...}, alloc_{alloc} {}

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~FunctionManager() noexcept final = default;

  /*! \brief Deleted copy constructor. */
  FunctionManager(FunctionManager const&) = delete;

  /*! \brief Deleted moveconstructor. */
  FunctionManager(FunctionManager&&) = delete;

  /*!
   * \brief Deleted copy assignment.
   * \return
   */
  FunctionManager& operator=(FunctionManager const&) = delete;

  /*!
   * \brief Deleted move assignment.
   * \return
   */
  FunctionManager& operator=(FunctionManager&&) = delete;

  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-A18.9.2: MD_VAC_A18.9.2_useStdMoveIfTheValueIsARvalueReference
  /*!
   * \brief Invokes the callable with InvQuals.
   * \param args Invocation argument types.
   * \return Invocation result.
   */
  R Invoke(Args&&... args) noexcept final {
    /*! \brief Callable type with conditional const and ref qualifiers. */
    using CallableCVRef =
        typename AddConditionalCVLRRef<FuncT, InvokeAsConst, false, InvokeAsRRef, !InvokeAsRRef>::type;
    return static_cast<R>(static_cast<CallableCVRef>(func_)(std::forward<Args>(args)...));
  }

  /*! \brief onDelete creates correct allocator to deallocate memory. */
  void OnDelete() noexcept final {
    // Create a rebound allocator on the stack to enable deallocation after destruction of *this.
    ReboundAlloc reboundAlloc{alloc_};
    this->~FunctionManager();
    reboundAlloc.deallocate(this, 1);
  }

 private:
  /*! \brief Callable. */
  FuncT func_;

  /*! \brief Allocator. */
  Alloc alloc_;
};

/*! \brief Expands the argument type pack and provides the fully specified FunctionManager type. */
template <typename Alloc, typename FuncT, typename SigProps, typename ArgsT>
class FunctionManagerSelectorArgExpander;

/*! \brief Expands the argument type pack and provides the fully specified FunctionManager type. */
template <typename Alloc, typename FuncT, typename SigProps, typename... Args>
class FunctionManagerSelectorArgExpander<Alloc, FuncT, SigProps, signature::SignatureArgs<Args...>> {
  static constexpr bool InvokeAsRRef{SigProps::Qualifiers::kRef == signature::SignatureRefQual::RRef};
  static constexpr bool InvokeAsConst{SigProps::Qualifiers::kIsConst};

 public:
  /*! \brief Type. */
  using type = FunctionManager<Alloc, FuncT, InvokeAsRRef, InvokeAsConst, typename SigProps::RetT, Args...>;
};

/*! \brief Provides the FunctionManager type using some indirections. */
template <typename Alloc, typename FuncT, typename Signature>
class FunctionManagerSelector {
  /*! \brief Signature properties. */
  using SigProps = signature::GetSigProps<Signature>;

 public:
  /*! \brief Type. */
  using type = typename FunctionManagerSelectorArgExpander<Alloc, FuncT, SigProps, typename SigProps::ArgTs>::type;
};

/*! \brief Expands the argument type pack and provides the fully specified IFunctionManager type. */
template <typename SigProps, typename ArgsT>
struct IFunctionManagerSelectorArgExpander;

/*! \brief Expands the argument type pack and provides the fully specified IFunctionManager type. */
template <typename SigProps, typename... Args>
struct IFunctionManagerSelectorArgExpander<SigProps, signature::SignatureArgs<Args...>>
    : vac::language::type_identity<IFunctionManager<typename SigProps::RetT, Args...>> {};

/*! \brief Provides the IFunctionManager type using some indirections. */
template <typename Signature>
class IFunctionManagerSelector {
  /*! \brief Signature properties. */
  using SigProps = signature::GetSigProps<Signature>;

 public:
  /*! \brief Type. */
  using type = typename IFunctionManagerSelectorArgExpander<SigProps, typename SigProps::ArgTs>::type;
};

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_noVirtualDtor
/*!
 * \brief Stores a dynamically allocated callable together with an allocator in FunctionManager.
 *        Manages a pointer to an interface allowing calls, deallocation, and destruction.
 */
template <typename Signature>
class DynamicStorage {
  /*! \brief FunctionManager for F and signature. */
  template <typename Allocator, typename F>
  using FuncMan = typename FunctionManagerSelector<std::decay_t<Allocator>, std::decay_t<F>, Signature>::type;

  /*! \brief Type for FunctionManager Interface. */
  using IFuncMan = typename IFunctionManagerSelector<Signature>::type;

 public:
  // VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
  /*! \brief Pointer to function manager with custom deleter. */
  std::unique_ptr<IFuncMan, typename IFuncMan::Deleter> function_manager_ptr_;

  /*! \brief Default constructor. */
  DynamicStorage() noexcept = default;

  /*!
   * \brief Construct using allocator and forwarded arguments.
   * \tparam F Callable type.
   * \tparam Alloc Allocator type.
   * \tparam Args Construction argument types.
   * \param alloc Allocator.
   * \param args Construction arguments.
   */
  template <typename F, typename Alloc, typename... Args>
  void Construct(Alloc const& alloc, Args&&... args) noexcept {
    /*! \brief Exact type of FuncMan. */
    using ExactFuncMan = FuncMan<Alloc, F>;
    /*! \brief Allocator type for ExactFuncMan. */
    using ReboundAlloc = typename ExactFuncMan::ReboundAlloc;

    ReboundAlloc rebound_alloc{alloc};
    ExactFuncMan* const new_storage{rebound_alloc.allocate(1)};
    std::allocator_traits<ReboundAlloc>::construct(rebound_alloc, new_storage, alloc, std::forward<Args>(args)...);
    function_manager_ptr_.reset(new_storage);
  }

  /*! \brief Clear storage. */
  void Clear() noexcept { function_manager_ptr_.reset(); }

  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_voidFunctionHasNoExternalSideEffect
  /*! \brief Aborts if called without callable. Used from StorageAndOperatorImpl::operator(). */
  void AssertCallable() const noexcept {
    if (!function_manager_ptr_) {
      ara::core::Abort("UniqueFunction called without callable");
    }
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Swaps two DynamicStorage.
   * \param other The DynamicStorage to be swapped.
   */
  void Swap(DynamicStorage& other) noexcept { std::swap(function_manager_ptr_, other.function_manager_ptr_); }

  /*!
   * \brief  Checks if empty.
   * \return True if DynamicStorage is empty, otherwise false.
   */
  bool IsEmpty() const noexcept { return !static_cast<bool>(function_manager_ptr_); }
};
}  // namespace uniquefunction

/*! \brief Check if a type is comparable with nullptr. */
template <typename T, typename = void>
struct IsPossiblyNullptr : std::false_type {};

/*!\brief Check if a type is comparable with nullptr. */
template <typename T>
struct IsPossiblyNullptr<T,
                         vac::language::void_t<decltype(std::declval<T>() == nullptr),
                                               std::enable_if_t<!std::is_function<std::remove_reference_t<T>>::value>>>
    : std::true_type {};

}  // namespace detail

/*!
 * \brief   UniqueFunction is a move-only version of std::function.
 * \details The callable is currently always stored in dynamically allocated memory. An interface allows this class
 *          which is typed only with a signature to call any callable and correctly handle allocation and destruction.
 * \tparam  Signature The signature of the callable. May be const qualified and use lvalue or rvalue reference
 *          qualifier.
 * \trace   CREQ-VaCommonLib-UniqueFunction
 */
template <typename Signature>
class UniqueFunction final : private detail::uniquefunction::StorageAndCallOperator<Signature> {
  /*! \brief Default allocator if no construction argument is given. */
  using DefAlloc = vac::memory::PhaseManagedAllocator<char>;

  /*! \brief Invocability requirement. */
  template <typename F>
  using IsInvocableReq = detail::signature::IsInvocableForSig<std::decay_t<F>, Signature>;

  /*! \brief Typedef to privately inherited superclass with storage and operator(). */
  using StorageT = typename detail::uniquefunction::StorageAndCallOperator<Signature>;

 public:
  /*! \brief Default constructor. */
  UniqueFunction() noexcept = default;

  /*! \brief Constructor for nullptr. */
  explicit UniqueFunction(std::nullptr_t) noexcept {}

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Move constructor. */
  UniqueFunction(UniqueFunction&&) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  // VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
  /*!
   * \brief  Move assignment.
   * \param  rhs UniqueFunction is being move assigned from.
   * \return This UniqueFunction object.
   */
  UniqueFunction& operator=(UniqueFunction&& rhs) & noexcept {
    UniqueFunction{std::move(rhs)}.swap(*this);
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
  /*!
   * \brief  Assignment to nullptr.
   * \return This UniqueFunction object.
   */
  UniqueFunction& operator=(std::nullptr_t) & noexcept {
    StorageT::Clear();
    return *this;
  }

  /*! \brief Copy constructor deleted. */
  UniqueFunction(UniqueFunction const&) = delete;

  /*!
   * \brief  Copy assignment deleted.
   * \return
   */
  UniqueFunction& operator=(UniqueFunction const&) & noexcept = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~UniqueFunction() noexcept = default;

  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  /*!
   * \brief   Construct from a callable.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   callable Either a function object or a function pointer.
   */
  template <typename F,
            typename = std::enable_if_t<(!std::is_same<vac::language::remove_cvref_t<F>, UniqueFunction>::value) &&
                                        std::is_constructible<std::decay_t<F>, F>::value && IsInvocableReq<F>::value>>
  UniqueFunction(F&& callable) noexcept {  // NOLINT(runtime/explicit)
    ConstructWithAlloc<F>(DefAlloc{}, std::forward<F>(callable));
  }

  /*!
   * \brief   In place construct a callable.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   args Arguments for in place construction.
   */
  template <
      typename F, typename... Args,
      typename = std::enable_if_t<std::is_constructible<std::decay_t<F>, Args...>::value && IsInvocableReq<F>::value>>
  explicit UniqueFunction(ara::core::in_place_type_t<F>, Args&&... args) noexcept {
    ConstructWithAlloc<F>(DefAlloc{}, ara::core::in_place, std::forward<Args>(args)...);
  }

  // VECTOR Next Construct AutosarC++17_10-A8.5.4: MD_VAC_A8.5.4_initializerlistShallBeDefinedInClasses
  // VECTOR Next Construct VectorC++-V8.5.4: MD_VAC_V8-5-4_ctorShallNotBeDefinedToAcceptStdInitializerList
  /*!
   * \brief   In place construct a callable with an initializer list.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   il Initializer list for in place construction.
   * \param   args Arguments for in place construction.
   */
  template <
      typename F, typename I, typename... Args,
      typename = std::enable_if_t<std::is_constructible<std::decay_t<F>, std::initializer_list<I>, Args...>::value &&
                                  IsInvocableReq<F>::value>>
  UniqueFunction(ara::core::in_place_type_t<F>, std::initializer_list<I> il, Args&&... args) noexcept {
    ConstructWithAlloc<F>(DefAlloc{}, ara::core::in_place, std::move(il), std::forward<Args>(args)...);
  }

  /*!
   * \brief   Construct from a callable and an allocator.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   alloc An allocator that shall be used to allocate dynamic memory.
   * \param   callable Either a function object or a function pointer.
   */
  template <typename Alloc, typename F,
            typename = std::enable_if_t<(!std::is_same<vac::language::remove_cvref_t<F>, UniqueFunction>::value) &&
                                        std::is_constructible<std::decay_t<F>, F>::value && IsInvocableReq<F>::value>>
  UniqueFunction(vac::language::allocator_arg_t, Alloc const& alloc, F&& callable) noexcept {
    ConstructWithAlloc<F>(alloc, std::forward<F>(callable));
  }

  /*!
   * \brief   In place construct a callable and an allocator.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   alloc An allocator that shall be used to allocate dynamic memory.
   * \param   args Arguments for in place construction.
   */
  template <
      typename Alloc, typename F, typename... Args,
      typename = std::enable_if_t<std::is_constructible<std::decay_t<F>, Args...>::value && IsInvocableReq<F>::value>>
  UniqueFunction(vac::language::allocator_arg_t, Alloc const& alloc, ara::core::in_place_type_t<F>,
                 Args&&... args) noexcept {
    ConstructWithAlloc<F>(alloc, ara::core::in_place, std::forward<Args>(args)...);
  }

  // VECTOR Next Construct AutosarC++17_10-A8.5.4: MD_VAC_A8.5.4_initializerlistShallBeDefinedInClasses
  // VECTOR Next Construct VectorC++-V8.5.4: MD_VAC_V8-5-4_ctorShallNotBeDefinedToAcceptStdInitializerList
  /*!
   * \brief   In place construct a callable with an initializer list and an allocator.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   alloc An allocator that shall be used to allocate dynamic memory.
   * \param   il Initializer list for in place construction.
   * \param   args Arguments for in place construction.
   */
  template <
      typename Alloc, typename F, typename I, typename... Args,
      typename = std::enable_if_t<std::is_constructible<std::decay_t<F>, std::initializer_list<I>, Args...>::value &&
                                  IsInvocableReq<F>::value>>
  UniqueFunction(vac::language::allocator_arg_t, Alloc const& alloc, ara::core::in_place_type_t<F>,
                 std::initializer_list<I> il, Args&&... args) noexcept {
    ConstructWithAlloc<F>(alloc, ara::core::in_place, std::move(il), std::forward<Args>(args)...);
  }

  /*!
   * \brief   Assign from a callable.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   callable Either a function object or a function pointer.
   * \return  A new UniqueFunction based on the provided callable.
   */
  template <typename F,
            typename = std::enable_if_t<(!std::is_same<vac::language::remove_cvref_t<F>, UniqueFunction>::value) &&
                                        std::is_move_constructible<std::decay_t<F>>::value && IsInvocableReq<F>::value>>
      UniqueFunction& operator=(F&& callable) & noexcept {
    UniqueFunction{std::forward<F>(callable)}.swap(*this);
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Swap the content of *this with the content of other.
   * \param other The other UniqueFunction to swap with.
   */
  void swap(UniqueFunction& other) noexcept { StorageT::Swap(other); }

  /*! \brief Returns whether a callable is stored. */
  explicit operator bool() const noexcept { return !StorageT::IsEmpty(); }

  /*!
   * \brief Call operator with the same cv ref qualifiers as Signature for this UniqueFunction.
   */
  using detail::uniquefunction::StorageAndCallOperator<Signature>::operator();

 private:
  /*!
   * \brief  Construct using allocator and forwarded arguments.
   * \tparam F The type of the callable.
   */
  template <typename F, typename Alloc, typename... Args,
            std::enable_if_t<(sizeof...(Args) != 1) || (!detail::IsPossiblyNullptr<F>::value), std::int32_t> = 0>
  void ConstructWithAlloc(Alloc const& alloc, Args&&... args) noexcept {
    StorageT::template Construct<std::decay_t<F>>(alloc, std::forward<Args>(args)...);
  }

  /*!
   * \brief Checks if the callable is equal to nullptr.
   */
  template <typename F>
  static bool CallableIsEqualToNullptr(F&& f) noexcept {
    return f == nullptr;
  }

  // VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
  /*!
   * \brief  Construct using allocator and forwarded arguments.
   * \tparam FPtr A function pointer.
   */
  template <typename F, typename Alloc, typename... Args,
            std::enable_if_t<(sizeof...(Args) == 1) && detail::IsPossiblyNullptr<F>::value, std::int32_t> = 0>
  void ConstructWithAlloc(Alloc const& alloc, Args&&... args) noexcept {
    if (!CallableIsEqualToNullptr(std::forward<Args>(args)...)) {
      StorageT::template Construct<std::decay_t<F>>(alloc, std::forward<Args>(args)...);
    }
  }
};

// VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
/*!
 * \brief  Swaps the content of lhs and rhs.
 * \tparam Signature The signature of the contained callables.
 * \param  lhs First object to swap with.
 * \param  rhs Second object to swap with.
 */
template <typename Signature>
void swap(UniqueFunction<Signature>& lhs, UniqueFunction<Signature>& rhs) noexcept {
  lhs.swap(rhs);
}

/*!
 * \brief  Checks a UniqueFunction with a nullptr_t.
 * \param  lhs A UniqueFunction object to check.
 * \return True if the UniqueFunction does not contain a callable.
 */
template <typename Signature>
bool operator==(UniqueFunction<Signature> const& lhs, std::nullptr_t) noexcept {
  return !lhs;
}

/*!
 * \brief  Compares a UniqueFunction with a nullptr_t.
 * \param  rhs A UniqueFunction object to check.
 * \return True if the UniqueFunction does not contains a callable.
 */
template <typename Signature>
bool operator==(std::nullptr_t, UniqueFunction<Signature> const& rhs) noexcept {
  return !rhs;
}

/*!
 * \brief  Compares a UniqueFunction with a nullptr_t.
 * \param  lhs A UniqueFunction object to check.
 * \return True if the UniqueFunction contains a callable.
 */
template <typename Signature>
bool operator!=(UniqueFunction<Signature> const& lhs, std::nullptr_t) noexcept {
  return static_cast<bool>(lhs);
}

/*!
 * \brief  Compares a UniqueFunction with a nullptr_t.
 * \param  rhs A UniqueFunction object to check.
 * \return True if the UniqueFunction contains a callable.
 */
template <typename Signature>
bool operator!=(std::nullptr_t, UniqueFunction<Signature> const& rhs) noexcept {
  return static_cast<bool>(rhs);
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_UNIQUE_FUNCTION_H_
