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
/*!        \file  either_storage.h
 *        \brief  Contains the memory handling of the Either type.
 *
 *      \details  Implements the rule of 5 & union for storing the two types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_STORAGE_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_STORAGE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <type_traits>
#include <utility>

#include "../../../ara/core/utility.h"
#include "../../../vac/language/cpp17_backport.h"
#include "../../../vac/language/detail/copy_move_enabler.h"
#include "../../../vac/language/detail/either_trait.h"

namespace vac {
namespace language {
namespace detail {

/*!
 * \brief  An alias for ara::core::in_place_type_t.
 */
template <typename T>
using in_place_type_t = ara::core::in_place_type_t<T>;

/*! \brief Type to use in uninitialized union. */
struct EmptyUnionT {};

// VECTOR Next Construct AutosarC++17_10-A9.5.1: MD_VAC_A9.5.1_unionsShallNotBeUsed
// VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
/*! \brief Union for L and R type of Either when both are trivially destructible. */
template <typename L, typename R, bool = EitherTrait<L, R>::is_trivially_destructible>
union EitherUnion {
  /*! \brief Default constructor without member initialization. */
  constexpr EitherUnion() noexcept : empty_{} {}

  /*!
   * \brief In place constructor for Left type.
   * \param args Input to construct the EitherUnion.
   */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<L>, Args&&... args) noexcept : left_(std::forward<Args>(args)...) {}

  /*!
   * \brief In place constructor for Right type.
   * \param args Input to construct the EitherUnion.
   */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<R>, Args&&... args) noexcept : right_(std::forward<Args>(args)...) {}

  /*!
   * \brief  Retrieves Left type.
   * \return Reference to left type.
   */
  constexpr auto Left() noexcept -> L& { return left_; }

  /*!
   * \brief  Retrieves Left type.
   * \return Const reference to left type.
   */
  constexpr auto Left() const noexcept -> L const& { return left_; }

  /*!
   * \brief  Retrieves Right type.
   * \return Reference to right type.
   */
  constexpr auto Right() noexcept -> R& { return right_; }

  /*!
   * \brief  Retrieves Right type.
   * \return Const reference to right type.
   */
  constexpr auto Right() const noexcept -> R const& { return right_; }

 private:
  /*! \brief Left type. */
  L left_;
  /*! \brief Right type. */
  R right_;
  /*! \brief Unused uninitialized type. */
  EmptyUnionT empty_;
};

// VECTOR Next Construct AutosarC++17_10-A9.5.1: MD_VAC_A9.5.1_unionsShallNotBeUsed
// VECTOR Next Construct VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
/*! \brief Union for L and R type of Either when both are not trivially destructible. */
template <typename L, typename R>
union EitherUnion<L, R, false> {
  /*! \brief Default constructor without member initialization. */
  constexpr EitherUnion() noexcept : empty_{} {}

  /*!
   * \brief In place constructor for Left type.
   * \param args Left type input to construct the EitherUnion.
   */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<L>, Args&&... args) noexcept : left_(std::forward<Args>(args)...) {}

  /*!
   * \brief In place constructor for Right type.
   * \param args Right type input to construct the EitherUnion.
   */
  template <typename... Args>
  constexpr explicit EitherUnion(in_place_type_t<R>, Args&&... args) noexcept : right_(std::forward<Args>(args)...) {}

  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Non-trivial destructor. */
  ~EitherUnion() noexcept {}

  /*! \brief Default copy constructor */
  EitherUnion(EitherUnion const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*! \brief Default move constructor */
  EitherUnion(EitherUnion&&) = default;

  /*!
   * \brief  Default copy assignment
   * \return A reference to the assigned-to object.
   */
  EitherUnion& operator=(EitherUnion const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment
   * \return A reference to the assigned-to object.
   */
  EitherUnion& operator=(EitherUnion&&) & = default;

  /*!
   * \brief  Retrieves Left type.
   * \return Reference to left type.
   */
  constexpr auto Left() noexcept -> L& { return left_; }

  /*!
   * \brief  Retrieves Left type.
   * \return Const reference to left type.
   */
  constexpr auto Left() const noexcept -> L const& { return left_; }

  /*!
   * \brief  Retrieves Right type.
   * \return Reference to right type.
   */
  constexpr auto Right() noexcept -> R& { return right_; }

  /*!
   * \brief  Retrieves Right type.
   * \return Const reference to right type.
   */
  constexpr auto Right() const noexcept -> R const& { return right_; }

 private:
  /*! \brief Left type. */
  L left_;
  /*! \brief Right type. */
  R right_;
  /*! \brief Unused uninitialized type. */
  EmptyUnionT empty_;
};

/*! \brief Token for calling non-trivial constructors for EitherPayloadTrivial. */
struct NotTrivialEitherToken {};

// VECTOR Disable AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
/*!
 * \brief Contains EitherUnion and provides utility functions for non-trivial copy/move construct/assignment.
 *        This layer only supports trivial construct/assignment. Non-trivial constructor/assignment is added by
 *        additional layers using utility functions defined here.
 */
template <typename L, typename R>
class EitherPayloadTrivial {
 protected:
  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~EitherPayloadTrivial() noexcept = default;

  /*! \brief Default copy constructor. */
  EitherPayloadTrivial(EitherPayloadTrivial const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*! \brief Default move constructor. */
  EitherPayloadTrivial(EitherPayloadTrivial&&) = default;

  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadTrivial& operator=(EitherPayloadTrivial const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadTrivial& operator=(EitherPayloadTrivial&&) & = default;

  /*! \brief In place constructor for Left type. */
  /*! \brief Left type without const specifier */
  using NonConstL = std::remove_const_t<L>;
  /*! \brief Right type without const specifier */
  using NonConstR = std::remove_const_t<R>;

  // VECTOR Disable AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor
 public:
  /*!
   * \brief In place constructor for Left type
   * \param args Input for constructing the storage.
   */
  template <typename... Args>
  constexpr explicit EitherPayloadTrivial(in_place_type_t<L>, Args&&... args) noexcept
      : storage_{in_place_type_t<L>{}, std::forward<Args>(args)...}, is_left_{true} {}

  /*!
   * \brief In place constructor for Right type.
   * \param args Input for constructing the storage.
   */
  template <typename... Args>
  constexpr explicit EitherPayloadTrivial(in_place_type_t<R>, Args&&... args) noexcept
      : storage_{in_place_type_t<R>{}, std::forward<Args>(args)...}, is_left_{false} {}

  /*!
   * \brief Non-trivial copy constructor auxiliary.
   * \param other Reference to EitherPayloadTrivial to be copy constructed from.
   */
  constexpr EitherPayloadTrivial(NotTrivialEitherToken, EitherPayloadTrivial const& other) noexcept
      : storage_{}, is_left_{false} {
    if (other.is_left_) {
      ConstructLeft(other.GetL());
    } else {
      ConstructRight(other.GetR());
    }
  }

  /*!
   * \brief Non-trivial move constructor auxiliary.
   * \param other Reference to EitherPayloadTrivial to be move constructed from.
   * */
  constexpr EitherPayloadTrivial(NotTrivialEitherToken, EitherPayloadTrivial&& other) noexcept
      : storage_{}, is_left_{false} {
    if (other.is_left_) {
      ConstructLeft(std::move(other.GetL()));
    } else {
      // VECTOR NL AutosarC++17_10-A18.9.3: MD_VAC_A18.9.3_moveWithConstArgument
      ConstructRight(std::move(other.GetR()));
    }
  }

  // VECTOR Enable AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor
  // VECTOR Next Construct AutosarC++17_10-M5.2.12: MD_VAC_M5.2.12_arraytoPointerDecay
  /*!
   * \brief Constructs Left type in place without calling destructor.
   * \param args Input for constructing left type.
   */
  template <typename... Args>
  void ConstructLeft(Args&&... args) noexcept {
    // VECTOR Next Construct AutosarC++17_10-M4.5.1: MD_VAC_M4.5.1_boolOperandInNew
    new (&storage_.Left()) NonConstL(std::forward<Args>(args)...);
    is_left_ = true;
  }

  // VECTOR Next Construct AutosarC++17_10-M5.2.12: MD_VAC_M5.2.12_arraytoPointerDecay
  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief Constructs Right type in place without calling destructor.
   * \param args Input for constructing right type.
   */
  template <typename... Args>
  void ConstructRight(Args&&... args) noexcept {
    // VECTOR Next Construct AutosarC++17_10-M4.5.1: MD_VAC_M4.5.1_boolOperandInNew

    new (&storage_.Right()) NonConstR(std::forward<Args>(args)...);
    is_left_ = false;
  }

  /*!
   * \brief Copy assignment auxiliary.
   * \param other Reference to EitherPayloadTrivial to be copy assigned from.
   */
  constexpr void CopyAssign(EitherPayloadTrivial const& other) noexcept {
    if (is_left_ != other.is_left_) {
      Reset();
      if (is_left_) {
        ConstructRight(other.GetR());
      } else {
        ConstructLeft(other.GetL());
      }
    } else if (is_left_ && other.is_left_) {
      GetL() = other.GetL();
    } else {
      GetR() = other.GetR();
    }
  }

  /*!
   * \brief Move assignment auxiliary.
   * \param other Reference to EitherPayloadTrivial to be move assigned from.
   */
  constexpr void MoveAssign(EitherPayloadTrivial&& other) noexcept {
    if (is_left_ != other.is_left_) {
      Reset();
      if (is_left_) {
        ConstructRight(std::move(other.GetR()));
      } else {
        ConstructLeft(std::move(other.GetL()));
      }
    } else if (is_left_ && other.is_left_) {
      GetL() = std::move(other.GetL());
    } else {
      GetR() = std::move(other.GetR());
    }
  }

  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*! \brief Calls the destructor for the stored type. */
  constexpr void Reset() noexcept {
    if (is_left_) {
      storage_.Left().~NonConstL();
    } else {
      storage_.Right().~NonConstR();
    }
  }

  /*!
   * \brief  Retrieves stored Left type. Requires Left to be the stored type.
   * \return Stored reference of left type.
   */
  constexpr L& GetL() noexcept { return storage_.Left(); }

  /*!
   * \brief  Retrieves stored Left type. Requires Left to be the stored type.
   * \return Stored const reference of left type.
   */
  constexpr L const& GetL() const noexcept { return storage_.Left(); }

  /*!
   * \brief  Retrieves stored Right type. Requires Right to be the stored type.
   * \return Stored reference of right type.
   */
  constexpr R& GetR() noexcept { return storage_.Right(); }

  /*!
   * \brief  Retrieves stored Right type. Requires Right to be the stored type.
   * \return Stored const reference of right type.
   */
  constexpr R const& GetR() const noexcept { return storage_.Right(); }

  /*!
   * \brief  Indicates whether a left value type is stored.
   * \return True if a left value is stored, otherwise false.
   */
  constexpr bool IsLeft() const noexcept { return is_left_; }

  /*! \brief The union storage for Left and Right. */
 private:
  /*! \brief The union storage for Left and Right */
  EitherUnion<NonConstL, NonConstR> storage_;
  /*! \brief Indicator for which type is stored. */
  bool is_left_{false};
};

/*! \brief Adds destructor, copy and move assignment if they are not trivial. */
template <typename L, typename R, bool = EitherTrait<L, R>::is_trivially_destructible,
          bool = EitherTrait<L, R>::is_trivially_copy_assignable,
          bool = EitherTrait<L, R>::is_trivially_move_assignable>
class EitherPayloadAssign;

/*! \brief Uses defaults. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, true, true> : public EitherPayloadTrivial<L, R> {
 protected:
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~EitherPayloadAssign() noexcept = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;
};

/*! \brief Adds copy assignment. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, false, true> : public EitherPayloadTrivial<L, R> {
 protected:
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~EitherPayloadAssign() noexcept = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;

  /*! \brief Default constructor. */
  EitherPayloadAssign() noexcept = default;

  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadAssign& operator=(EitherPayloadAssign&&) & = default;

  /*!
   * \brief  Non-trivial copy assignment.
   * \param  other Const reference to EitherPayloadAssign to be copy assigned from.
   * \return Reference to this EitherPayloadAssign object.
   */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign const& other) & noexcept {
    this->CopyAssign(other);
    return *this;
  }
};

/*! \brief Adds move assignment. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, true, false> : public EitherPayloadTrivial<L, R> {
 protected:
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~EitherPayloadAssign() noexcept = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;

  /*! \brief Default constructor. */
  EitherPayloadAssign() noexcept = default;

  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;

  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadAssign& operator=(EitherPayloadAssign const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Non-trivial move assignment.
   * \param  other Reference to EitherPayloadAssign to be move assigned from.
   * \return Reference to this EitherPayloadAssign object.
   */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign&& other) & noexcept {
    this->MoveAssign(std::move(other));
    return *this;
  }
};

/*! \brief Adds copy and move assignment. */
template <typename L, typename R>
class EitherPayloadAssign<L, R, true, false, false> : public EitherPayloadTrivial<L, R> {
 protected:
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  /*! \brief Default destructor. */
  ~EitherPayloadAssign() noexcept = default;

  using EitherPayloadTrivial<L, R>::EitherPayloadTrivial;

  /*! \brief Default constructor. */
  EitherPayloadAssign() noexcept = default;

  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;

  /*!
   * \brief  Non-trivial copy assignment.
   * \param  other Reference to EitherPayloadAssign to be copy assigned from.
   * \return Reference to this EitherPayloadAssign object.
   */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign const& other) & noexcept {
    this->CopyAssign(other);
    return *this;
  }

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Non-trivial move assignment.
   * \param  other Reference to EitherPayloadAssign to be move assigned from.
   * \return Reference to this EitherPayloadAssign object.
   */
  constexpr EitherPayloadAssign& operator=(EitherPayloadAssign&& other) & noexcept {
    this->MoveAssign(std::move(other));
    return *this;
  }
};

/*! \brief Adds destructor, copy and move assignment. */
template <typename L, typename R, bool IsTrivialCopyAssign, bool IsTrivialMoveAssign>
class EitherPayloadAssign<L, R, false, IsTrivialCopyAssign, IsTrivialMoveAssign>
    : public EitherPayloadAssign<L, R, true, false, false> {
 protected:
  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Protected non-trivial destructor. */
  ~EitherPayloadAssign() noexcept { this->Reset(); }

  using EitherPayloadAssign<L, R, true, false, false>::EitherPayloadAssign;

  /*! \brief Default constructor. */
  EitherPayloadAssign() noexcept = default;

  /*! \brief Default copy constructor. */
  EitherPayloadAssign(EitherPayloadAssign const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*! \brief Default move constructor. */
  EitherPayloadAssign(EitherPayloadAssign&&) = default;

  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadAssign& operator=(EitherPayloadAssign const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move constructor.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadAssign& operator=(EitherPayloadAssign&&) & = default;
};

// VECTOR Disable AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor
/*! \brief Adds copy and move constructors. */
template <typename L, typename R, bool = EitherTrait<L, R>::is_trivially_copy_constructible,
          bool = EitherTrait<L, R>::is_trivially_move_constructible>
class EitherPayloadConstruct final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;
  /*!
   * \brief Non-trivial copy constructor.
   * \param other Reference to EitherPayloadConstruct to be copy constructed from.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const& other) noexcept
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, other) {}

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  // VECTOR Next Construct AutosarC++17_10-A12.8.4: MD_VAC_A12.8.4_moveConstructorShallNotUseCopySemantics
  /*!
   * \brief Non-trivial move constructor.
   * \param other Reference to EitherPayloadConstruct to be move constructed from.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&& other) noexcept
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, std::move(other)) {}

  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;

  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~EitherPayloadConstruct() noexcept = default;
};
// VECTOR Enable AutosarC++17_10-A12.1.1: MD_VAC_A12.1.1_baseClassIsNotExplicitlyInitializedInConstructor

/*! \brief Adds copy constructor. */
template <typename L, typename R>
class EitherPayloadConstruct<L, R, false, true> final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;

  /*!
   * \brief Non-trivial copy constructor.
   * \param other Reference to EitherPayloadConstruct to be copy constructed from.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const& other) noexcept
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, other) {}

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief Default move constructor.
   * \param other Reference to EitherPayloadConstruct to be move constructed from.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&& other) = default;

  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;
};

/*! \brief Adds move constructor. */
template <typename L, typename R>
class EitherPayloadConstruct<L, R, true, false> final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;

  /*!
   * \brief Default copy constructor.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  // VECTOR Next Construct AutosarC++17_10-A12.8.4: MD_VAC_A12.8.4_moveConstructorShallNotUseCopySemantics
  /*!
   * \brief Non-trivial move constructor.
   * \param other Reference to EitherPayloadConstruct to be move constructed from.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&& other) noexcept
      : EitherPayloadAssign<L, R>::EitherPayloadAssign(NotTrivialEitherToken{}, std::move(other)) {}

  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;
};

/*! \brief Uses defaults. */
template <typename L, typename R>
class EitherPayloadConstruct<L, R, true, true> final : public EitherPayloadAssign<L, R> {
 public:
  using EitherPayloadAssign<L, R>::EitherPayloadAssign;

  /*!
   * \brief Default copy constructor.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief Default move constructor.
   */
  constexpr EitherPayloadConstruct(EitherPayloadConstruct&&) = default;

  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  EitherPayloadConstruct& operator=(EitherPayloadConstruct&&) & = default;
};
// VECTOR Enable AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction

/*!
 * \brief  Memory handling class for Either.
 * \tparam L The left type.
 * \tparam R The right type.
 */
template <typename L, typename R>
class EitherStorage final
    : private CopyMoveEnabler<EitherTrait<L, R>::is_copy_constructible, EitherTrait<L, R>::is_move_constructible,
                              EitherTrait<L, R>::is_copy_assignable, EitherTrait<L, R>::is_move_assignable> {
 public:
  /*! \brief Traits for storage of L and R. */
  using Trait = EitherTrait<L, R>;

  /*! \brief Default copy constructor. */
  EitherStorage(EitherStorage const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*! \brief Default move constructor. */
  EitherStorage(EitherStorage&&) = default;

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  /*!
   * \brief  Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  EitherStorage& operator=(EitherStorage const&) & = default;

  // VECTOR Next Construct AutosarC++17_10-A10.2.1: MD_VAC_A10.2.1_redefinitionOfNonVirtualFunction
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_defaultedFunctionInheritsNoexcept
  /*!
   * \brief  Default move assignment.
   * \return A reference to the assigned-to object.
   */
  EitherStorage& operator=(EitherStorage&&) & = default;

  // VECTOR Next Construct AutosarC++17_10-M0.1.8: MD_VAC_M0.1.8_functionHasNoExternalSideEffect
  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Default destructor. */
  ~EitherStorage() noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A12.1.5: MD_VAC_A12.1.5_useDelegatingConstructor
  /*!
   * \brief   In place constructor for Left type.
   * \details direct-initializes the selected type in place using the given arguments.
   * \tparam  Args Types to construct the left value with.
   * \param   args Values to construct the left value with.
   */
  template <typename... Args, typename = std::enable_if_t<std::is_constructible<L, Args...>::value>>
  constexpr explicit EitherStorage(in_place_type_t<L>, Args&&... args) noexcept
      : storage_{in_place_type_t<L>{}, std::forward<Args>(args)...} {}

  /*!
   * \brief   In place constructor for Right type.
   * \details direct-initializes the selected type in place using the given arguments.
   * \tparam  Args Types to construct the right value with.
   * \param   args Values to construct the right value with.
   */
  template <typename... Args, typename = std::enable_if_t<std::is_constructible<R, Args...>::value>>
  constexpr explicit EitherStorage(in_place_type_t<R>, Args&&... args) noexcept
      : storage_{in_place_type_t<R>{}, std::forward<Args>(args)...} {}

  /*!
   * \brief  Emplace a left value.
   * \tparam Args Types for the left constructor.
   * \param  args Values for the left constructor.
   * \return Nothing.
   */
  template <typename... Args>
  void EmplaceLeft(Args&&... args) noexcept {
    storage_.Reset();
    storage_.ConstructLeft(std::forward<Args>(args)...);
  }

  /*!
   * \brief  Emplace a right value.
   * \tparam Args Types for the right constructor.
   * \param  args Values for the right constructor.
   * \return Nothing.
   */
  template <typename... Args>
  void EmplaceRight(Args&&... args) noexcept {
    storage_.Reset();
    storage_.ConstructRight(std::forward<Args>(args)...);
  }

  // VECTOR Next Construct AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
  /*!
   * \brief  Returns true if a left value is contained.
   * \return True if a left value is contained, otherwise false.
   */
  constexpr auto IsLeft() const noexcept -> bool { return storage_.IsLeft(); }

  /*!
   * \brief  Returns a const reference to the contained left value.
   * \return Const reference to the contained left value.
   */
  constexpr auto Left() const noexcept -> L const& {
    assert(IsLeft());
    return storage_.GetL();
  }
  /*!
   * \brief  Returns a const reference to the contained right value.
   * \return Const reference to the contained right value.
   */
  constexpr auto Right() const noexcept -> R const& {
    assert(!IsLeft());
    return storage_.GetR();
  }

  /*!
   * \brief  Returns a reference to the contained left value.
   * \return Reference to the contained left value.
   */
  constexpr auto Left() noexcept -> L& {
    assert(IsLeft());
    return storage_.GetL();
  }

  /*!
   * \brief Returns a reference to the contained right value.
   * \return Reference to the contained right value.
   */
  constexpr auto Right() noexcept -> R& {
    assert(!IsLeft());
    return storage_.GetR();
  }

 private:
  /*! \brief Storage for Left and Right type with constructors and assignment. */
  EitherPayloadConstruct<L, R> storage_;
};

}  // namespace detail
}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_DETAIL_EITHER_STORAGE_H_
