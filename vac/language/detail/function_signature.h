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
/*!        \file  function_signature.h
 *        \brief  Compile time templates to extract properties from a function signature.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_FUNCTION_SIGNATURE_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_FUNCTION_SIGNATURE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include "vac/language/cpp17_backport.h"
#include "vac/language/cpp20_backport.h"

namespace vac {
namespace language {
namespace detail {

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution
/*!
 * \brief  Adds const to T if IsConst is true, else return T as is.
 * \tparam T The type to add const to.
 * \tparam IsConst The bool to check if const should be added.
 */
template <typename T, bool IsConst>
struct AddConditionalConst : vac::language::type_identity<T> {};
/*!
 * \brief  Adds const to T.
 * \tparam T The type to add const to.
 */
template <typename T>
struct AddConditionalConst<T, true> : std::add_const<T> {};

/*!
 * \brief  Adds volatile to T if IsVolatile is true, else return T as is.
 * \tparam T The type to add volatile to.
 * \tparam IsVolatile The bool to check if volatile should be added.
 */
template <typename T, bool IsVolatile>
struct AddConditionalVolatile : vac::language::type_identity<T> {};
/*!
 * \brief  Adds volatile to T.
 * \tparam T The type to add volatile to.
 */
template <typename T>
struct AddConditionalVolatile<T, true> : std::add_volatile<T> {};

/*!
 * \brief  Adds lvalue reference to T if IsLRef is true, else return T as is.
 * \tparam T The type to add lvalue reference to.
 * \tparam IsLRef The bool to check if lvalue reference should be added.
 */
template <typename T, bool IsLRef>
struct AddConditionalLRef : vac::language::type_identity<T> {};

/*!
 * \brief  Adds lvalue reference to T.
 * \tparam T The type to add lvalue reference to.
 */
template <typename T>
struct AddConditionalLRef<T, true> : std::add_lvalue_reference<T> {};

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution
/*!
 * \brief  Adds rvalue reference to T if IsRRef is true, else return T as is.
 * \tparam T The type to add rvalue reference to.
 * \tparam IsLRef The bool to check if rvalue reference should be added.
 */
template <typename T, bool IsRRef>
struct AddConditionalRRef : vac::language::type_identity<T> {};
/*!
 * \brief  Adds rvalue reference to T.
 * \tparam T The type to add lvalue reference to.
 */
template <typename T>
struct AddConditionalRRef<T, true> : std::add_rvalue_reference<T> {};

/*!
 * \brief  Adds cv-qualifiers to T according to the bools IsConst and IsVolatile.
 * \tparam T The type to add cv-qualifiers to.
 * \tparam IsConst The bool to determine if const should be added or not.
 * \tparam IsVolatile The bool to determine if volatile should be added or not.
 */
template <typename T, bool IsConst, bool IsVolatile>
struct AddConditionalCV : AddConditionalConst<typename AddConditionalVolatile<T, IsVolatile>::type, IsConst> {};

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution
/*!
 * \brief  Adds ref-qualifiers to T according to the bools IsLRef and IsRRef.
 * \tparam T The type to add ref-qualifiers to.
 * \tparam IsLRef The bool to determine if lvalue reference should be added or not.
 * \tparam IsRRef The bool to determine if rvalue reference should be added or not.
 */
template <typename T, bool IsLRef, bool IsRRef>
struct AddConditionalLRRef : AddConditionalRRef<typename AddConditionalLRef<T, IsLRef>::type, IsRRef> {};

/*!
 * \brief  Adds cv-qualifiers to T according to the bools IsConst and IsVolatile.
 * \tparam T The type to add cv-qualifiers to.
 * \tparam IsConst The bool to determine if const should be added or not.
 * \tparam IsVolatile The bool to determine if volatile should be added or not.
 */
template <typename T, bool IsConst, bool IsVolatile, bool IsLRef, bool IsRRef>
struct AddConditionalCVLRRef
    : AddConditionalLRRef<typename AddConditionalCV<T, IsConst, IsVolatile>::type, IsLRef, IsRRef> {};

namespace signature {

/*! \brief An argument pack. */
template <typename... Args>
struct SignatureArgs {};

/*! \brief Enumeration for ref qualifiers. */
enum class SignatureRefQual : std::size_t {
  None,
  LRef,
  RRef,
};

/*! \brief Qualifiers of signature, i.e. not return type or argument types. */
template <bool IsConst, bool IsVolatile, SignatureRefQual RefQual>
struct SignatureQualifiers {
  /*! \brief Is const. */
  static constexpr bool kIsConst{IsConst};
  /*! \brief Is volatile. */
  static constexpr bool kIsVolatile{IsVolatile};
  /*! \brief Ref qualifier. */
  static constexpr SignatureRefQual kRef{RefQual};
};

// VECTOR Next Construct AutosarC++17_10-A12.4.1: MD_VAC_A12.4.1_compileTimeResolution
/*! \brief Full description of a signature in an easy to access way. */
template <typename SigQuals, typename R, typename... Args>
class SignatureProperties {
 public:
  /*! \brief Return type. */
  using RetT = R;
  /*! \brief Argument types. */
  using ArgTs = SignatureArgs<Args...>;
  /*! \brief Cv and ref qualifiers. */
  using Qualifiers = SigQuals;
};

/*!
 * \brief Extracts types and qualifiers from signature.
 *        Three dimensions: const, volatile, ref => 2*2*3 = 12 possibilities. (C++17 adds noexcept as well)
 */
template <typename Signature>
struct GetSigProps;

/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...)>
    : SignatureProperties<SignatureQualifiers<false, false, SignatureRefQual::None>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) const>
    : SignatureProperties<SignatureQualifiers<true, false, SignatureRefQual::None>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) volatile>
    : SignatureProperties<SignatureQualifiers<false, true, SignatureRefQual::None>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) const volatile>
    : SignatureProperties<SignatureQualifiers<true, true, SignatureRefQual::None>, R, Args...> {};

/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...)&>
    : SignatureProperties<SignatureQualifiers<false, false, SignatureRefQual::LRef>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) const&>
    : SignatureProperties<SignatureQualifiers<true, false, SignatureRefQual::LRef>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) volatile&>
    : SignatureProperties<SignatureQualifiers<false, true, SignatureRefQual::LRef>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) const volatile&>
    : SignatureProperties<SignatureQualifiers<true, true, SignatureRefQual::LRef>, R, Args...> {};

/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) &&>
    : SignatureProperties<SignatureQualifiers<false, false, SignatureRefQual::RRef>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) const&&>
    : SignatureProperties<SignatureQualifiers<true, false, SignatureRefQual::RRef>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) volatile&&>
    : SignatureProperties<SignatureQualifiers<false, true, SignatureRefQual::RRef>, R, Args...> {};
/*! \brief Specialization for signature properties. */
template <typename R, typename... Args>
struct GetSigProps<R(Args...) const volatile&&>
    : SignatureProperties<SignatureQualifiers<true, true, SignatureRefQual::RRef>, R, Args...> {};

/*! \brief Expands argument pack for invocability requirement for a functor against a signature. */
template <typename F, typename SigProps, typename ArgsT>
class IsInvocableForSigArgExpander {
  constexpr static bool always_false{sizeof(F) == static_cast<size_t>(-1)};
  static_assert(always_false, "ArgsT must be a SignatureArgs.");
};

/*! \brief Checks if invocable for signature with expanded arguments. */
template <typename F, typename SigProps, typename... Args>
class IsInvocableForSigArgExpander<F, SigProps, SignatureArgs<Args...>> {
  static constexpr bool IsRRef{SigProps::Qualifiers::kRef == SignatureRefQual::RRef};
  static constexpr bool IsLRef{!IsRRef};
  static constexpr bool IsConst{SigProps::Qualifiers::kIsConst};
  static constexpr bool IsVolatile{SigProps::Qualifiers::kIsVolatile};
  /*! \brief Return type. */
  using R = typename SigProps::RetT;

 public:
  /*! \brief Value. */
  static constexpr bool value{
      vac::language::is_invocable_r<R, typename AddConditionalCVLRRef<F, IsConst, IsVolatile, IsLRef, IsRRef>::type,
                                    Args...>::value};
};

/*! \brief Checks if F is invocable for signature. */
template <typename F, typename Signature>
class IsInvocableForSig {
  /*! \brief Signature properties. */
  using SigProps = signature::GetSigProps<Signature>;

 public:
  /*! \brief Value. */
  static constexpr bool value{IsInvocableForSigArgExpander<F, SigProps, typename SigProps::ArgTs>::value};
};

}  // namespace signature

}  // namespace detail
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_FUNCTION_SIGNATURE_H_
