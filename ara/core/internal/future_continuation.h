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
/*!        \file  ara/core/internal/future_continuation.h
 *        \brief  Continuation class to register, store and execute the callback for future then.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_FUTURE_CONTINUATION_H_
#define LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_FUTURE_CONTINUATION_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <memory>
#include <utility>
#include "ara/core/future.h"
#include "ara/core/promise.h"
#include "vac/language/unique_function.h"

namespace ara {
namespace core {
namespace internal {

/*!
 * \brief CallBackHolder interface.
 * \vprivate
 */
class CallBackHolderInterface {
 public:
  /*!
   * \brief Default constructor.
   */
  CallBackHolderInterface() noexcept = default;

  /*!
   * \brief Default copy constructor.
   */
  CallBackHolderInterface(CallBackHolderInterface const& other) = delete;

  /*!
   * \brief Default move constructor.
   */
  CallBackHolderInterface(CallBackHolderInterface&& other) = delete;

  /*!
   * \brief Deleted copy assignment operator.
   */
  CallBackHolderInterface& operator=(CallBackHolderInterface const& other) & = delete;

  /*!
   * \brief Deleted move assignment operator.
   */
  CallBackHolderInterface& operator=(CallBackHolderInterface&& other) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor.
   */
  virtual ~CallBackHolderInterface() noexcept {}

  /*!
   * \brief Executes the callback.
   */
  virtual void ExecuteCallBack() noexcept = 0;
};

/*!
 * \brief  Callback holder base class to perform the callback execution.
 * \tparam T is type for the calling Future.
 * \tparam E is the error type for the calling Future.
 * \tparam U is the type returned back from the registered call back function.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename U, typename T2, typename E2>
class CallBackHolderBase : public CallBackHolderInterface {
 public:
  /*!
   * \brief Callback handler type.
   */
  using CallBackHandler = vac::language::UniqueFunction<U(ara::core::Future<T, E>)>;

  /*!
   * \brief Constructor.
   * \param new_promise New promise to be set when the callback is called (it needs to be set with the return value
   *        from the callback).
   * \param calling_future The future on which the then function is called (calling future needs to be passed as a
   *        parameter to the call back function and it gets invalidated once then() is called).
   * \param handler The handler to the callback function.
   */
  CallBackHolderBase(ara::core::Promise<T2, E2>&& new_promise, ara::core::Future<T, E>&& calling_future,
                     CallBackHandler handler) noexcept
      : CallBackHolderInterface(),
        new_promise_(std::make_unique<ara::core::Promise<T2, E2>>(std::move(new_promise))),
        future_(std::move(calling_future)),
        callback_handler_(std::move(handler)) {}

 protected:
  // VECTOR Disable VectorC++-V11.0.2: MD_VAC_V11-0-2_mutableMemberShallBePrivate
  /*!
   * \brief New promise to be set when the callback is called.
   */
  std::unique_ptr<ara::core::Promise<T2, E2>> new_promise_;

  /*!
   * \brief Future on which the function is called.
   */
  ara::core::Future<T, E> future_;

  /*! \brief Callback handler. */
  CallBackHandler callback_handler_;

  // VECTOR Enable VectorC++-V11.0.2
};

/*!
 * \brief  Callback holder class to perform the callback execution.
 * \tparam T The type for the calling Future.
 * \tparam E The error type for the calling Future.
 * \tparam U The type returned back from the registered call back function.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename U, typename T2, typename E2>
class CallBackHolder : public CallBackHolderBase<T, E, U, T2, E2> {
 public:
  using CallBackHolderBase<T, E, U, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() noexcept override {
    U const ret_val{this->callback_handler_(std::move(this->future_))};
    this->new_promise_->set_value(ret_val);
  }
};

/*!
 * \brief  Specialization callback holder class to perform the callback execution for when U is void.
 * \tparam T is type for the calling future.
 * \tparam E is the error type for the calling Future.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename T2, typename E2>
class CallBackHolder<T, E, void, T2, E2> : public CallBackHolderBase<T, E, void, T2, E2> {
 public:
  using CallBackHolderBase<T, E, void, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() noexcept override {
    this->callback_handler_(std::move(this->future_));
    this->new_promise_->set_value();
  }
};

/*!
 * \brief  Specialization callback holder class to perform the callback execution for when U is Future<T2, E2>.
 * \tparam T is type for the calling future.
 * \tparam E is the error type for the calling Future.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename T2, typename E2>
class CallBackHolder<T, E, Future<T2, E2>, T2, E2> : public CallBackHolderBase<T, E, Future<T2, E2>, T2, E2> {
 public:
  using CallBackHolderBase<T, E, Future<T2, E2>, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() noexcept override {
    Future<T2, E2> chain_future{this->callback_handler_(std::move(this->future_))};
    // VECTOR NL VectorC++-V50.1.1: MD_VAC_V50-1-1_futureThenShallNotBeUsed
    static_cast<void>(chain_future.then([this](Future<T2, E2> unwrapped_future) {
      Result<T2, E2> const res{unwrapped_future.GetResult()};
      SetValueOrError(*(this->new_promise_), res);
    }));
  }
};

/*!
 * \brief  Specialization callback holder class to perform the callback execution for when U is Result<T2, E2>.
 * \tparam T is type for the calling future.
 * \tparam E is the error type for the calling Future.
 * \tparam T2 The unwrapped value type returned back from the registered call back function.
 * \tparam E2 The unwrapped error type returned back from the registered call back function.
 * \vprivate
 */
template <typename T, typename E, typename T2, typename E2>
class CallBackHolder<T, E, Result<T2, E2>, T2, E2> : public CallBackHolderBase<T, E, Result<T2, E2>, T2, E2> {
 public:
  using CallBackHolderBase<T, E, Result<T2, E2>, T2, E2>::CallBackHolderBase;

  /*!
   * \brief   Function to execute the registered call back.
   * \details Sets the new promise.
   */
  void ExecuteCallBack() noexcept override {
    Result<T2, E2> const ret_val{this->callback_handler_(std::move(this->future_))};
    SetValueOrError(*(this->new_promise_), ret_val);
  }
};

/*!
 * \brief   Future continuation class accessible to the Future and Promise.
 * \details Promise creates object of FutureContinuation and passes it to the Future to be able to store and access the
 *          callback function.
 * \tparam  T The type for the calling Future.
 * \tparam  E The error type for the calling Future.
 * \vprivate
 */
template <typename T, typename E = ErrorCode>
class FutureContinuation final {
  // TODO(STORY-12266) Analyze the need for FutureContinuation
 public:
  // VECTOR Disable AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Default constructor.
   */
  FutureContinuation() noexcept = default;

  /*!
   * \brief Move constructor.
   */
  FutureContinuation(FutureContinuation&&) noexcept = default;

  /*!
   * \brief  Move assignment operator.
   * \return A reference to the assigned-to object.
   */
  FutureContinuation& operator=(FutureContinuation&&) & noexcept = default;

  /*!
   * \brief Destructor.
   */
  ~FutureContinuation() noexcept = default;
  // VECTOR Enable AutosarC++17_10-A15.5.1
  /*!
   * \brief Default copy constructor deleted.
   */
  FutureContinuation(FutureContinuation const&) = delete;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   */
  FutureContinuation& operator=(FutureContinuation const&) = delete;

  /*!
   * \brief  Registers a callback handler to be called when the Promise is set and the state is ready.
   * \param  handler A callback handler.
   * \param  fut The calling Future.
   * \tparam Func The type for the callback function.
   * \return new future.
   */
  template <typename Func>
  auto SetCallBackHandler(Func&& handler, ara::core::Future<T, E>&& fut) noexcept ->
      typename TypeUnwrapping<decltype(handler(std::move(fut))), E>::type {
    /*! \brief Alias for the return type of the callable */
    using U = decltype(handler(std::move(fut)));
    /*! \brief Alias for the unwrapped return value type of the callable */
    using T2 = typename TypeUnwrapping<U, E>::value_type;
    /*! \brief Alias for the unwrapped return error type of the callable */
    using E2 = typename TypeUnwrapping<U, E>::error_type;

    ara::core::Promise<T2, E2> new_promise;
    ara::core::Future<T2, E2> new_future{new_promise.get_future()};
    call_back_ = std::make_unique<CallBackHolder<T, E, U, T2, E2>>(std::move(new_promise), std::move(fut),
                                                                   std::forward<Func>(handler));
    return new_future;
  }

  /*!
   * \brief Executes the callback.
   */
  void ExecuteCallBack() noexcept {
    call_back_ready_ = false;
    call_back_->ExecuteCallBack();
  }

  /*!
   * \brief  Returns the registered state of the call back.
   * \return True if the callback is registered, false otherwise.
   */
  bool IsCallBackSet() noexcept { return call_back_ready_ && (call_back_ != nullptr); }

  /*!
   * \brief  Get the call_back_mutex.
   * \return The call back mutex.
   */
  std::mutex& GetCallBackMutex() noexcept { return call_back_mutex_; }

 private:
  /*! \brief A mutex to prevent setting a value while a call back is being set and vice versa. */
  std::mutex call_back_mutex_;

  /*! \brief Pointer to CallBackHolderInterface. */
  std::unique_ptr<CallBackHolderInterface> call_back_;

  /*! \brief Boolean to keep track of if the call back has been executed or not. */
  std::atomic<bool> call_back_ready_{true};
};

}  // namespace internal
}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_FUTURE_CONTINUATION_H_
