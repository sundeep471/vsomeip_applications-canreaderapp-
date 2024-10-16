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
/*!        \file  reference_wrapper.h
 *        \brief  constexpr version of std::reference_wrapper.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_REFERENCE_WRAPPER_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_REFERENCE_WRAPPER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "vac/language/cpp14_backport.h"

namespace vac {
namespace language {

// VECTOR Next Construct AutosarC++17_10-A12.0.1: MD_VAC_A12.0.1_missingMoveConstructorAssignmentOperator
/*!
 * \brief  Constexpr version of std::reference_wrapper.
 * \tparam T Wrapped type.
 */
template <class T>
class reference_wrapper final {
 public:
  /*!
   * \brief Type.
   */
  using type = T;

  /*!
   * \brief Constructor.
   * \param x Reference to value.
   */
  constexpr explicit reference_wrapper(T& x) noexcept : ptr_(&x) {}

  /*!
   * \brief Copy Constructor.
   */
  constexpr reference_wrapper(reference_wrapper const&) noexcept = default;

  /*!
   * \brief Default destructor.
   */
  ~reference_wrapper() noexcept = default;

  /*!
   * \brief  Copy Assignment.
   * \param  other Other object.
   * \return A reference to the assigned-to object.
   */
  auto operator=(reference_wrapper<T> const& other) & noexcept -> reference_wrapper& = default;

  /*!
   * \brief  Get the reference reference.
   * \return The reference reference.
   */
  constexpr auto get() const noexcept -> T const& {
    // Can never be nullptr.
    return *ptr_;
  }

  /*!
   * \brief  Get the reference reference.
   * \return The reference reference.
   */
  auto get() noexcept -> T& {
    // Can never be nullptr.
    return *ptr_;
  }

  /*!
   * \brief  Explicit conversion to reference.
   * \return The reference reference.
   */
  explicit constexpr operator T const&() const noexcept { return get(); }

  /*!
   * \brief  Explicit conversion to reference.
   * \return The reference reference.
   */
  explicit operator T&() noexcept { return get(); }

 private:
  /*!
   * \brief Reference.
   */
  T* ptr_;
};

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_REFERENCE_WRAPPER_H_
