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
/*!        \file  state_pool.h
 *        \brief  The header file for the statemachine to get the current state and verify the validity.
 *
 *      \details  GetState returns state of given handle and IsValid checks if handle refers to an existing state.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_POOL_H_
#define LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_POOL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace vac {
namespace statemachine {

/*!
 * \brief State Pool. The state pool is responsible for resources of states.
 * \trace CREQ-158649
 */
template <class State>
class StatePool {
 public:
  /*!
   * \brief Type of the State Handle.
   */
  using Handle = typename State::HandleType;

  /*!
   * \brief Default constructor.
   */
  StatePool() = default;

  // VECTOR Next Construct AutosarC++17_10-A12.8.6:MD_VAC_A12.8.6_copyMoveConstructorAssignmentShallBeProtectedOrDeleted
  /*!
   * \brief Default copy constructor.
   */
  StatePool(StatePool const&) = default;

  // VECTOR Next Construct AutosarC++17_10-A12.8.6:MD_VAC_A12.8.6_copyMoveConstructorAssignmentShallBeProtectedOrDeleted
  /*!
   * \brief Default move constructor.
   */
  StatePool(StatePool&&) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A12.8.6:MD_VAC_A12.8.6_copyMoveConstructorAssignmentShallBeProtectedOrDeleted
  /*!
   * \brief Default copy assignment.
   * \return A reference to the assigned-to object.
   */
  StatePool& operator=(StatePool const&) & noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A12.8.6:MD_VAC_A12.8.6_copyMoveConstructorAssignmentShallBeProtectedOrDeleted
  /*!
   * \brief Default move assignment.
   * \return A reference to the assigned-to object.
   */
  StatePool& operator=(StatePool&&) & noexcept = default;

  /*!
   * \brief Default destructor.
   */
  virtual ~StatePool() noexcept = default;

  /*!
   * \brief  Returns State of given handle.
   * \param  state_handle The state handle.
   * \return The State with given handle. If handle does not refer to a State,
   *         exception of type std::out_of_range is thrown.
   */
  virtual State* GetState(Handle const state_handle) = 0;

  /*!
   * \brief  Checks if handle refers to an existing state.
   * \param  state_handle The state handle.
   * \return True if state exists with given handle, otherwise returns false.
   */
  virtual bool IsValid(Handle const state_handle) const = 0;
};

}  // namespace statemachine
}  // namespace vac
#endif  // LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_POOL_H_
