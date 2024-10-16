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
/**        \file  vac/language/function_ref.h
 *        \brief  A lightweight non-owning reference to a callable.
 *
 *      \details  vac::language::FunctionRef provides a similar functionality for a callable,
 *                as a std::string_view does for a std::string. It can be used as a replacement for
 *                std::function, in certain cases, where strict requirements on dynamic memory allocation exist.
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_FUNCTION_REF_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_FUNCTION_REF_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include <utility>

#include "vac/language/cpp17_backport.h"
#include "vac/language/cpp20_backport.h"
#include "vac/language/detail/function_signature.h"

namespace vac {
namespace language {
namespace detail {

// VECTOR Next Construct AutosarC++17_10-A9.5.1: MD_VAC_A9.5.1_unionsShallNotBeUsed
/*!
 * \brief   Storage for the erased callable.
 * \details This is a union in order to be able to save either a function object pointer or a function pointer without
 *          triggering undefined behavior.
 */
union callable_ptr_t {
  // VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
  /*! \brief For function objects. */
  void* fun_obj;

  // VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
  /*! \brief For function pointers. */
  void (*fun_ptr)();
};

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_noVirtualDtor
/*!
 * \brief  Contains the type erased callable reference and provides the callable operator.
 * \tparam IsConst True if the signature has a const qualifier.
 * \tparam IsVolatile True if the signature has a volatile qualifier.
 * \tparam R The return value of the callable.
 * \tparam Args The input paramater types of the callable.
 */
template <bool IsConst, bool IsVolatile, typename R, typename... Args>
class StorageAndOperator {
 public:
  // VECTOR Next Construct VectorC++-V3.9.2: MD_VAC_V3-9-2_parametersBasedOnBuiltInTypes
  /*!
   * \brief  Executes the referenced callable.
   * \param  args Any arguments that the referenced callable requires.
   * \return The value of the executed callable.
   */
  R operator()(Args... args) const noexcept { return erased_function_(erased_object_, std::forward<Args>(args)...); }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Swap the content of *this with the content of other.
   * \param other The other StorageAndOperator to swap with.
   */
  void Swap(StorageAndOperator& other) noexcept {
    std::swap(erased_object_, other.erased_object_);
    std::swap(erased_function_, other.erased_function_);
  }

  // VECTOR Next Construct AutosarC++17_10-M5.2.8: MD_VAC_M5.2.8_voidPointerToTypedPointer
  /*!
   * \brief  Sets the members when fun_obj is a function object pointer.
   * \tparam Callable The signature of fun_obj.
   * \param  fun_obj The function object pointer to be saved.
   */
  template <typename Callable>
  void Bind(Callable& fun_obj) noexcept {
    /*! \brief Callable type with conditional const volatile qualifiers. */
    using CallableCV = typename AddConditionalCV<std::remove_reference_t<Callable>, IsConst, IsVolatile>::type;
    erased_object_.fun_obj = static_cast<void*>(std::addressof(fun_obj));
    erased_function_ = [](detail::callable_ptr_t ecall, Args... args) -> R {
      // The explicit cast to R avoids a compile error when R is void and the callable returns non-void.
      return static_cast<R>((*static_cast<CallableCV*>(ecall.fun_obj))(std::forward<Args>(args)...));
    };
  }

  // VECTOR Next Construct AutosarC++17_10-A5.2.4: MD_VAC_A5.2.4_reinterpretCast
  // VECTOR Next Construct AutosarC++17_10-M5.2.6: MD_VAC_M5.2.6_functionPointerCast
  /*!
   * \brief  Sets the members when fun_ptr is a function pointer.
   * \tparam Res The return type of fun_ptr.
   * \tparam ArgTypes The input parameter types of fun_ptr.
   * \param  fun_ptr The function pointer to be saved.
   */
  template <typename Res, typename... ArgTypes>
  void Bind(Res (*fun_ptr)(ArgTypes...)) noexcept {
    erased_object_.fun_ptr = reinterpret_cast<void (*)()>(fun_ptr);
    erased_function_ = [](detail::callable_ptr_t ecall, Args... args) -> R {
      // The explicit cast to Res avoids a compile error when Res is void and the callable returns non-void.
      return static_cast<R>((reinterpret_cast<Res (*)(ArgTypes...)>(ecall.fun_ptr))(std::forward<Args>(args)...));
    };
  }

  // VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
  /*! \brief Either a void pointer or a void function pointer. */
  detail::callable_ptr_t erased_object_;

  // VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
  /*! \brief Pointer to the callable. */
  R (*erased_function_)(detail::callable_ptr_t, Args...);
};

/*!
 * \brief  Expands the argument type pack and provides the fully specified StorageAndOperator type.
 * \tparam SigProps Signature properties received from detail::signature::GetSigProps.
 * \tparam ArgsT A tuple type with all the arguments for calling the callable.
 */
template <typename SigProps, typename ArgsT>
class StorageAndOperatorSelectorArgExpander;

/*! \brief Expands the argument type pack and provides the fully specified StorageAndOperator type. */
template <typename SigProps, typename... Args>
class StorageAndOperatorSelectorArgExpander<SigProps, signature::SignatureArgs<Args...>> {
  /*! \brief True if the callable should be invoked as const. */
  static constexpr bool InvokeAsConst{SigProps::Qualifiers::kIsConst};
  /*! \brief True if the callable should be invoked as volatile. */
  static constexpr bool InvokeAsVolatile{SigProps::Qualifiers::kIsVolatile};

 public:
  /*! \brief The resolved type. */
  using type = StorageAndOperator<InvokeAsConst, InvokeAsVolatile, typename SigProps::RetT, Args...>;
};

/*!
 * \brief  Provides the StorageAndOperator type using some indirections.
 * \tparam SigProps Signature properties received from detail::signature::GetSigProps.
 */
template <typename Signature>
class StorageAndOperatorSelector {
  /*! \brief Signature properties. */
  using SigProps = signature::GetSigProps<Signature>;

 public:
  /*! \brief Resolved type. */
  using type = typename StorageAndOperatorSelectorArgExpander<SigProps, typename SigProps::ArgTs>::type;
};

}  // namespace detail

// VECTOR Next Construct AutosarC++17_10-A12.4.2: MD_VAC_A12.4.2_classNotFinal
/*!
 * \brief  Class definition of FunctionRef. Refers to a callable that is stored somewhere else.
 * \tparam Signature The full signature of the referred callable.
 * \trace  CREQ-VaCommonLib-FunctionRef
 */
template <typename Signature>
class FunctionRef : private detail::StorageAndOperatorSelector<Signature>::type {
  /*! \brief Invocability requirement. */
  template <typename F>
  using IsInvocableReq = detail::signature::IsInvocableForSig<std::remove_reference_t<F>, Signature>;

  /*! \brief Typedef to privately inherited superclass with storage and operator(). */
  using StorageT = typename detail::StorageAndOperatorSelector<Signature>::type;

 public:
  /*! \brief Default constructor deleted. */
  FunctionRef() = delete;

  // FunctionRef is neither meant to have a move constructor nor a move assignment operator.
  // But these are not explicitly declared as deleted, because that causes issues with overload resolution when
  // initializing or assigning FunctionRef with r-values.
  //
  // Instead, the move constructor and move assignment operator are implicitly deleted.
  // This is done by providing a user declared copy constructor and a user declared copy assigment operator.

  /*! \brief Copy constructor. */
  FunctionRef(FunctionRef const&) noexcept = default;

  // VECTOR Disable AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief   Move constructor.
   * \details The move constructor is defined to perform copy construction.
   *          The move constructor is needed pre C++17 to allow FunctionRef function parameter initialization
   *          from r-value callables.
   * \param   other The FunctionRef to move from.
   */
  FunctionRef(FunctionRef&& other) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
  /*!
   * \brief  Copy assignment.
   * \return A reference to the assigned-to object.
   */
  FunctionRef& operator=(FunctionRef const&) & noexcept = default;

  /*!
   * \brief  Move assignment deleted to avoid dangling references.
   * \return
   */
  FunctionRef& operator=(FunctionRef&&) & noexcept = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~FunctionRef() noexcept = default;

  // VECTOR Next Construct VectorC++-V12.1.4: MD_VAC_V12-1-4_constructorShallBeDeclaredExplicit
  /*!
   * \brief   Construct from a callable.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument type Args are supplied.
   * \tparam  F The type of the callable.
   * \param   callable Either a function object or a function pointer.
   */
  template <typename F,
            typename = std::enable_if_t<(!std::is_same<vac::language::remove_cvref_t<F>, FunctionRef>::value) &&
                                        (IsInvocableReq<F>::value)>>
  FunctionRef(F&& callable) noexcept {  // NOLINT(runtime/explicit)
    StorageT::Bind(callable);
  }
  // VECTOR Enable AutosarC++17_10-A15.5.1
  /*!
   * \brief   Assign from a callable.
   * \details The callable must not be of the same type as *this. The return value of the callable must be
   *          convertible to R when the argument types Args are supplied.
   * \tparam  F The type of the callable.
   * \param   callable Either a function object or a function pointer.
   * \return  A new FunctionRef based on the provided callable.
   */
  template <typename F,
            typename = std::enable_if_t<(!std::is_same<vac::language::remove_cvref_t<F>, FunctionRef>::value) &&
                                        (IsInvocableReq<F>::value) && (std::is_lvalue_reference<F&&>::value)>>
      FunctionRef& operator=(F&& callable) & noexcept {
    StorageT::Bind(callable);
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Swap the content of *this with the content of other.
   * \param other The other FunctionRef to swap with.
   */
  void swap(FunctionRef& other) noexcept { StorageT::Swap(static_cast<StorageT&>(other)); }

  /*!
   * \brief Makes the call operator public. Has same R(Args...) as Signature.
   */
  using StorageT::operator();
};

// VECTOR Next Construct AutosarC++17_10-A2.11.4: MD_VAC_A2.11.4_nameOfObjectOrFunctionReused
// VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
/*!
 * \brief  Swaps the content of lhs and rhs.
 * \tparam Signature The signature of the contained callables.
 * \param  lhs First object to swap with.
 * \param  rhs Second object to swap with.
 */
template <typename Signature>
void swap(FunctionRef<Signature>& lhs, FunctionRef<Signature>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_FUNCTION_REF_H_
