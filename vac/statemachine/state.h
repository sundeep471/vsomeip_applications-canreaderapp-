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
/*!        \file  state.h
 *        \brief  The header file of the statemachine that defines states.
 *
 *      \details  OnEnter, context sensitive method called when state is entered.
 *                OnLeave, context sensitive method called when state is left.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_H_
#define LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace vac {
namespace statemachine {

/*!
 * \brief Interface for State classes.
 * \trace CREQ-158649
 */
template <typename StateHandle, class Context>
class State {
 public:
  /*!
   * \brief The Type of the StateHandle.
   *        The StateHandle is used for identifying a particular state by the value of a basic data type, such
   *        as enum values.
   */
  using HandleType = StateHandle;

  /*!
   * \brief The Type of the Context.
   */
  using ContextType = Context;

  /*!
   * \brief Constructor.
   * \param handle StateHandle to be constructed from.
   */
  explicit State(StateHandle const handle) noexcept : handle_(handle) {}

  /*!
   * \brief Copy constructor.
   */
  State(State const&) = delete;

  /*!
   * \brief Deleted copy assignment.
   * \return
   */
  State& operator=(State const&) = delete;

  /*!
   * \brief Move constructor.
   */
  State(State&&) = delete;

  /*!
   * \brief Move assignment.
   * \return
   */
  State& operator=(State&&) = delete;

  /*!
   * \brief Destructor.
   */
  virtual ~State() noexcept = default;

  /*!
   * \brief Context sensitive method called when state is entered.
   * \param context Context of state machine.
   * \trace CREQ-158651
   */
  virtual void OnEnter(Context& context) = 0;

  /*!
   * \brief Context sensitive method called when state is left.
   * \param context Context of state machine.
   * \trace CREQ-158651
   */
  virtual void OnLeave(Context& context) = 0;

  /*!
   * \brief  Checks if change from current state to state with given handle is valid.
   * \param  handle The handle of the requested state.
   * \return True if change is valid, otherwise returns false.
   * \trace  CREQ-158650
   */
  virtual bool IsValidChange(StateHandle const handle) const = 0;

  /*!
   * \brief  Returns the handle of this State.
   * \return The handle of this State.
   */
  StateHandle const GetHandle() const noexcept { return handle_; }

 private:
  /*!
   * \brief The handle for this State.
   */
  StateHandle const handle_;
};

}  // namespace statemachine
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_STATEMACHINE_STATE_H_
