/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2021 by Vector Informatik GmbH. All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**        \file  ara/core/promise.h
 *        \brief  Specific implementation of a Promise for ara::core.
 *
 *      \details  ara::core::Promise is the corresponding promise that returns an ara::core::Future.
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_PROMISE_H_
#define LIB_VAC_INCLUDE_ARA_CORE_PROMISE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <memory>
#include <utility>

#include "ara/core/future.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/internal/future_continuation.h"
#include "ara/core/internal/shared_state.h"

namespace ara {
namespace core {

/*!
 * \brief  ara::core specific Promise.
 * \tparam T Value type.
 * \tparam E Error type.
 * \trace  SPEC-7552479
 * \trace  CREQ-200633
 * \vpublic
 */
template <typename T, typename E = ErrorCode>
class Promise final {
  /*! \brief The promised type. */
  using ValueType = T;

  /*! \brief Alias for Result. */
  using R = Result<ValueType, E>;

  /*! \brief The future continuation pointer type. */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<ValueType, E>>;

  /*! \brief Shared pointer type for SharedState. */
  using SharedStatePtr = std::shared_ptr<internal::SharedState<R, future_errc>>;

 public:
  /*!
   * \brief Default constructor.
   * \trace SPEC-7552480
   * \vpublic
   */
  Promise() = default;

  /*!
   * \brief Default copy constructor deleted.
   * \trace SPEC-7552481
   * \vpublic
   */
  Promise(Promise const&) = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Move constructor.
   * \trace SPEC-7552482
   * \vpublic
   */
  Promise(Promise&&) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor.
   * \trace SPEC-7552483
   * \vpublic
   */
  ~Promise() noexcept {
    // VECTOR Disable AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
    if (shared_state_) {
      shared_state_->BreakPromise();
      if (future_continuation_->IsCallBackSet()) {
        // Call back has been set, but not executed.
        // If this occurs, it means that the Promise is destroyed before a value has been set.
        // BreakPromise() has now set the value to future_errc::broken_promise.
        future_continuation_->ExecuteCallBack();
      }
    }
    // VECTOR Enable AutosarC++17_10-M0.3.1
  }

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552484
   * \vpublic
   */
  Promise& operator=(Promise const&) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Move assignment operator.
   * \return A reference to "*this".
   * \trace  SPEC-7552485
   * \vpublic
   */
  Promise& operator=(Promise&&) & noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Exchanges the shared states of this and other.
   * \param other The other instance.
   * \trace SPEC-7552486
   * \trace CREQ-200643
   * \vpublic
   */
  void swap(Promise& other) noexcept {
    using std::swap;
    swap(shared_state_, other.shared_state_);
    swap(future_continuation_, other.future_continuation_);
  }

  /*!
   * \brief   Return a Future with the same shared state.
   * \details The returned Future is set as soon as this Promise receives the result or an error.
   *          This method must only be called once as it is not allowed to have multiple Futures per Promise.
   * \return  Future with same shared state.
   * \trace   SPEC-7552487
   * \trace   CREQ-200644
   * \vpublic
   */
  Future<ValueType, E> get_future() noexcept {
    shared_state_->MakeValid();
    return ara::core::Future<ValueType, E>(shared_state_, future_continuation_);
  }

  /*!
   * \brief Copy result into the Future.
   * \param value Value to be set.
   * \trace SPEC-7552488
   * \trace CREQ-200645
   * \vpublic
   */
  void set_value(ValueType const& value) noexcept {
    R const r{R::template FromValue(value)};

    SetValueAndExecuteCallBack(r);
  }

  /*!
   * \brief Move the result into the Future.
   * \param value Value to be set.
   * \trace SPEC-7552489
   * \trace CREQ-200645
   * \vpublic
   */
  void set_value(ValueType&& value) noexcept {
    R r{R::FromValue(std::move(value))};

    SetValueAndExecuteCallBack(std::move(r));
  }

  /*!
   * \brief Move an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552490
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E&& err) noexcept {
    R r{R::FromError(std::move(err))};

    SetValueAndExecuteCallBack(std::move(r));
  }

  /*!
   * \brief Copy an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552491
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E const& err) noexcept {
    R const r{R::template FromError(err)};

    SetValueAndExecuteCallBack(r);
  }

 private:
  /*! \brief Future continuation object to manage the call back function. */
  FutureContinuationPtr future_continuation_{std::make_shared<ara::core::internal::FutureContinuation<ValueType, E>>()};

  /*! \brief The shared state between *this and the connected Future. */
  SharedStatePtr shared_state_{std::make_shared<internal::SharedState<R, future_errc>>()};

  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R const& r) noexcept {
    // A local copy of future_continuation to support the following code:
    //
    // Thread A:               Thread B:
    // {                       {
    //   Promise p
    //   Future f
    //
    //   f.GetResult()           p.set_value()
    // }                       }
    //
    // The local copy prevents premature destruction of future_continuation when Thread A goes out of scope
    // before thread B has completed p.set_value().
    // VECTOR Next Line AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
    FutureContinuationPtr fut_cont{future_continuation_};
    std::lock_guard<std::mutex> const lock{fut_cont->GetCallBackMutex()};
    shared_state_->SetData(r);
    if (fut_cont->IsCallBackSet()) {
      fut_cont->ExecuteCallBack();
    }
  }

  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R&& r) noexcept {
    // A copy here to prevent the shared state to destruct prematurely.
    // See SetValueAndExecuteCallBack(R const&) for details.
    // VECTOR Next Line AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
    FutureContinuationPtr fut_cont{future_continuation_};
    std::lock_guard<std::mutex> const lock{fut_cont->GetCallBackMutex()};
    shared_state_->SetData(std::move(r));
    if (fut_cont->IsCallBackSet()) {
      fut_cont->ExecuteCallBack();
    }
  }
};

/*!
 * \brief Promise specialization for void.
 * \tparam E Error type.
 * \trace CREQ-200642
 * \vpublic
 */
template <typename E>
class Promise<void, E> final {
  /*! \brief The promised type. */
  using ValueType = void;

  /*! \brief Alias for Result. */
  using R = Result<ValueType, E>;

  /*! \brief The future continuation pointer type. */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<ValueType, E>>;

  /*! \brief Shared pointer type for SharedState. */
  using SharedStatePtr = std::shared_ptr<internal::SharedState<R, future_errc>>;

 public:
  /*!
   * \brief Default constructor.
   * \trace SPEC-7552480
   * \vpublic
   */
  Promise() = default;

  /*!
   * \brief Default copy constructor deleted.
   * \trace SPEC-7552481
   * \vpublic
   */
  Promise(Promise const&) = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Move constructor.
   * \trace SPEC-7552482
   * \vpublic
   */
  Promise(Promise&&) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor.
   * \trace SPEC-7552483
   * \vpublic
   */
  ~Promise() noexcept {
    // VECTOR Disable AutosarC++17_10-M0.3.1: MD_VAC_M0.3.1_dynamicMemPossiblyUsedAfterReleased
    if (shared_state_) {
      shared_state_->BreakPromise();
      if (future_continuation_->IsCallBackSet()) {
        // Call back has been set, but not executed.
        // If this occurs, it means that the Promise is destroyed before a value has been set.
        // BreakPromise() has now set the value to future_errc::broken_promise.
        future_continuation_->ExecuteCallBack();
      }
    }
    // VECTOR Enable AutosarC++17_10-M0.3.1
  }

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552484
   * \vpublic
   */
  Promise& operator=(Promise const&) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Move assignment operator.
   * \return A reference to "*this".
   * \trace  SPEC-7552485
   * \vpublic
   */
  Promise& operator=(Promise&&) & noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Exchanges the shared states of this and other.
   * \param other The other instance.
   * \trace SPEC-7552486
   * \trace CREQ-200643
   * \vpublic
   */
  void swap(Promise& other) noexcept {
    using std::swap;
    swap(shared_state_, other.shared_state_);
    swap(future_continuation_, other.future_continuation_);
  }
  /*!
   * \brief   Return a Future with the same shared state.
   * \details The returned Future is set as soon as this Promise receives the result or an error.
   *          This method must only be called once as it is not allowed to have multiple Futures per Promise.
   * \return  Future with same shared state.
   * \trace   SPEC-7552487
   * \trace   CREQ-200644
   * \vpublic
   */
  Future<ValueType, E> get_future() noexcept {
    shared_state_->MakeValid();
    return ara::core::Future<ValueType, E>(shared_state_, future_continuation_);
  }

  /*!
   * \brief Set a value with void.
   * \trace CREQ-200645
   * \vpublic
   */
  void set_value() noexcept {
    R const r{R::FromValue()};

    SetValueAndExecuteCallBack(r);
  }

  /*!
   * \brief Move an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552490
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E&& err) noexcept {
    R r{R::FromError(std::move(err))};

    SetValueAndExecuteCallBack(std::move(r));
  }

  /*!
   * \brief Copy an error into the Future.
   * \param err The error to store.
   * \trace SPEC-7552491
   * \trace CREQ-200646
   * \vpublic
   */
  void SetError(E const& err) noexcept {
    R const r{R::template FromError(err)};

    SetValueAndExecuteCallBack(r);
  }

 private:
  /*! \brief Future continuation object to manage the call back function. */
  FutureContinuationPtr future_continuation_{std::make_shared<ara::core::internal::FutureContinuation<ValueType, E>>()};

  /*! \brief The shared state between *this and the connected Future. */
  SharedStatePtr shared_state_{std::make_shared<internal::SharedState<R, future_errc>>()};

  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R const& r) noexcept {
    // A copy here to prevent the shared state to destruct prematurely.
    // See the non void Promise definition of SetValueAndExecuteCallBack(R const&) for details.
    // VECTOR Next Line AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
    FutureContinuationPtr fut_cont{future_continuation_};
    std::lock_guard<std::mutex> const lock{fut_cont->GetCallBackMutex()};
    shared_state_->SetData(r);
    if (fut_cont->IsCallBackSet()) {
      fut_cont->ExecuteCallBack();
    }
  }
  /*!
   * \brief Set value and execute call back if one exist.
   * \param r The Result to set.
   */
  void SetValueAndExecuteCallBack(R&& r) noexcept {
    // A copy here to prevent the shared state to destruct prematurely.
    // See the non void Promise definition of SetValueAndExecuteCallBack(R const&) for details.
    // VECTOR Next Line AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
    FutureContinuationPtr fut_cont{future_continuation_};
    std::lock_guard<std::mutex> const lock{fut_cont->GetCallBackMutex()};
    shared_state_->SetData(std::move(r));
    if (fut_cont->IsCallBackSet()) {
      fut_cont->ExecuteCallBack();
    }
  }
};

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_PROMISE_H_
