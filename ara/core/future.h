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
/**        \file
 *        \brief  Specific implementation of a Future for ara::core.
 *
 *      \details  ara::core::Future is an asynchronous return object similar to std::future but implemented for use with
 *                ara::core::Result.
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_FUTURE_H_
#define LIB_VAC_INCLUDE_ARA_CORE_FUTURE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <chrono>
#include <memory>
#include <utility>
#include "ara/core/error_code.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/internal/shared_state.h"
#include "ara/core/result.h"
#include "vac/language/compile_exceptions.h"

namespace ara {
namespace core {

template <typename T, typename E>
class Future;

template <typename T, typename E>
class Promise;

template <typename E>
class Promise<void, E>;

namespace internal {

// Forward declaration of FutureContinuation to avoid circular dependency.
template <typename T, typename E>
class FutureContinuation;

/*!
 * \brief  Sets a value or error to a Promise p given a Result res.
 * \tparam T The value type of the Promise and Result.
 * \tparam E The error type of the Promise and Result.
 * \param  p The Promise to set a value or error to.
 * \param  res The Result to get the value or error from.
 * \vprivate
 */
template <typename T, typename E>
void SetValueOrError(Promise<T, E>& p, Result<T, E> res) noexcept {
  if (res.HasValue()) {
    p.set_value(res.Value());
  } else {
    p.SetError(res.Error());
  }
}

/*!
 * \brief  Sets a value or error to a Promise p given a Result res when T is void.
 * \tparam E The error type of the Promise and Result.
 * \param  p The Promise to set a value or error to.
 * \param  res The Result to get the value or error from.
 * \vprivate
 */
template <typename E>
void SetValueOrError(Promise<void, E>& p, Result<void, E> res) noexcept {
  if (res.HasValue()) {
    p.set_value();
  } else {
    p.SetError(res.Error());
  }
}

/*!
 * \brief  Helper class to get a valid Future based on a given callable.
 * \remark Given the following specializations, T2 should be equal to U if this specialization is used.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam U  The callable's return type.
 * \tparam T2 The unwrapped type of U.
 * \tparam E2 The unwrapped error type of U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename U, typename T2, typename E2>
class GetFuture {
 public:
  /*!
   * \brief  Set the callable's return value to a Promise and returns a connected Future.
   * \param  func The callable, taking rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) noexcept -> Future<T2, E2> {
    Promise<T2, E2> promise_new;
    U const func_value{func(std::move(rdy_future))};
    promise_new.set_value(func_value);
    return promise_new.get_future();
  }
};

/*!
 * \brief  Helper class to get a valid Future based on a given callable which returns void.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam E2 The unwrapped error type of U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename E2>
class GetFuture<T1, E1, F, void, void, E2> {
 public:
  // VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst
  /*!
   * \brief  Set the callable's return value to a Promise and returns a connected Future.
   * \param  func The callable, taking rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) noexcept -> Future<void, E2> {
    Promise<void, E2> promise_new;
    func(std::move(rdy_future));
    promise_new.set_value();
    return promise_new.get_future();
  }
};

// VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst
/*!
 * \brief  Helper class to get a valid Future based on a given callable which returns Result<T2, E2>.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam T2 The unwrapped type of U.
 * \tparam E2 The unwrapped error type of U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename T2, typename E2>
class GetFuture<T1, E1, F, Result<T2, E2>, T2, E2> {
 public:
  /*!
   * \brief  Unwrapps the callable's returned Result, sets its contents to a Promise, and returns a connected Future.
   * \param  func The callable, taking rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) noexcept -> Future<T2, E2> {
    Promise<T2, E2> promise_new;
    SetValueOrError(promise_new, func(std::move(rdy_future)));
    return promise_new.get_future();
  }
};

// VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst
/*!
 * \brief  Helper class to get a valid Future based on a given callable which returns Future<T2, E2>.
 * \tparam T1 The type of the calling Future.
 * \tparam E1 The error type of the calling Future.
 * \tparam F  The callable's type.
 * \tparam T2 The unwrapped type of U.
 * \tparam E2 The unwrapped error type of U.
 * \vprivate
 */
template <typename T1, typename E1, typename F, typename T2, typename E2>
class GetFuture<T1, E1, F, Future<T2, E2>, T2, E2> {
 public:
  /*!
   * \brief  Returns the callable's returned Future.
   * \param  func The callable, taking rdy_future as input parameter.
   * \param  rdy_future A ready Future.
   * \return a new Future.
   * \vprivate
   */
  auto operator()(F&& func, Future<T1, E1>&& rdy_future) noexcept -> Future<T2, E2> {
    return func(std::move(rdy_future));
  }
};

/*!
 * \brief  Helper class to unwrap the return type of the callable sent to Future::then().
 * \tparam U The callable's return type.
 * \tparam E The calling Future's error type.
 * \vprivate
 */
template <typename U, typename E>
class TypeUnwrapping {
 public:
  /*! \brief Type alias. */
  using type = Future<U, E>;

  /*! \brief Type alias. */
  using value_type = U;

  /*! \brief Type alias. */
  using error_type = E;
};

/*!
 * \brief  Helper class to do implicit Result unwrapping.
 * \tparam E The calling Future's error type.
 * \vprivate
 */
template <typename T2, typename E2, typename E>
class TypeUnwrapping<Result<T2, E2>, E> {
 public:
  /*! \brief Type alias. */
  using type = Future<T2, E2>;

  /*! \brief Type alias. */
  using value_type = T2;

  /*! \brief Type alias. */
  using error_type = E2;
};

/*!
 * \brief  Helper class to do implicit Future unwrapping.
 * \tparam E The calling Future's error type.
 * \vprivate
 */
template <typename T2, typename E2, typename E>
class TypeUnwrapping<Future<T2, E2>, E> {
 public:
  /*! \brief Type alias. */
  using type = Future<T2, E2>;

  /*! \brief Type alias. */
  using value_type = T2;

  /*! \brief Type alias. */
  using error_type = E2;
};

/*!
 * \brief  Helper class to check if a Functor is nullptr.
 * \tparam F The Functor.
 * \vprivate
 */
template <typename F>
class IsNullPtr {
 public:
  template <typename P = F, typename std::enable_if_t<std::is_pointer<P>::value, std::int32_t> = 0>
  static bool check(P const& p) noexcept {
    return p == nullptr;
  }
  template <typename P = F, typename std::enable_if_t<!std::is_pointer<P>::value, std::int32_t> = 0>
  static bool check(P const&) noexcept {
    return false;
  }
};

}  // namespace internal

/*!
 * \brief Operation status for the timed wait functions wait_for() and wait_until().
 * \trace SPEC-7552463
 * \vpublic
 */
enum class future_status : uint8_t {
  /*!
   * \brief The shared state is ready.
   */
  ready = 0,
  /*!
   * \brief The shared state did not become ready before the specified timeout has passed.
   */
  timeout = 1
};

/*!
 * \brief  Provides ara::core specific Future operations to collect the results of an asynchronous call.
 * \tparam T Value type
 * \tparam E Error type
 * \trace  SPEC-7552464
 * \trace  CREQ-200633
 * \vpublic
 */
template <typename T, typename E = ErrorCode>
class Future final {
  /*!
   * \brief Alias for Result.
   */
  using R = Result<T, E>;

  /*! \brief Shared pointer type for SharedState. */
  using SharedStatePtr = std::shared_ptr<internal::SharedState<R, future_errc>>;

  /*! \brief The future continuation pointer type. */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<T, E>>;

 public:
  /*!
   * \brief Default constructor.
   * \trace SPEC-7552465
   * \vpublic
   */
  Future() noexcept = default;

  /*!
   * \brief Default copy constructor deleted.
   * \trace SPEC-7552466
   * \vpublic
   */
  Future(Future const&) = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Move constructor.
   * \param other The other instance.
   * \trace SPEC-7552467
   * \vpublic
   */
  Future(Future&& other) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor.
   * \trace SPEC-7552468
   * \vpublic
   */
  ~Future() noexcept = default;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552469
   * \vpublic
   */
  Future& operator=(Future const&) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Move assignment operator.
   * \param  other The other instance.
   * \return A reference to "*this".
   * \trace  SPEC-7552470
   * \vpublic
   */
  Future& operator=(Future&& other) & noexcept = default;

  /*!
   * \brief  Get the result (does not throw exceptions).
   * \return The value stored in the shared state.
   * \trace  SPEC-7552472
   * \trace  CREQ-200636
   * \vpublic
   */
  R GetResult() noexcept {
    R ret_val{R::FromError(future_errc::no_state)};
    if (valid()) {
      ret_val = shared_state_->GetData();
    }
    return std::move(ret_val);
  }

  /*!
   * \brief  Get the value.
   * \return The value stored in the shared state.
   * \throws ara::core::FutureException Internal Future/Promise errors see FutureErrc.
   * \throws ara::core::Exception Any error which is set in SetError. The concrete exception type depends on the
   *         Exception type corresponding to the error domain of the ErrorCode.
   * \trace  SPEC-7552471
   * \trace  CREQ-200636
   * \vpublic
   */
  template <bool enabled = vac::language::internal::kCompileWithExceptions, typename = std::enable_if_t<enabled>>
  T get() noexcept(false) {
    static_assert((!enabled) || vac::language::internal::kCompileWithExceptions,
                  "Should not be enabled when exceptions are disabled.");
    R res{GetResult()};
    if (!res.HasValue()) {
      res.Error().ThrowAsException();
    }
    return std::move(res).Value();
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Check if the Future has any shared state.
   * \return True if *this refers to a shared state, otherwise false.
   * \trace  SPEC-7552473
   * \trace  CREQ-200637
   * \vpublic
   */
  bool valid() const noexcept { return shared_state_ && shared_state_->Valid(); }

  /*!
   * \brief   Block until the shared state is ready.
   * \details Returns directly if shared state is invalid.
   * \trace   SPEC-7552474
   * \trace   CREQ-200639
   * \vpublic
   */
  void wait() const noexcept {
    if (valid()) {
      shared_state_->Wait();
    }
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief   Wait for a specified relative time.
   * \details Returns future_status::timeout directly if shared state is invalid.
   * \tparam  Rep An arithmetic type representing the number of ticks.
   * \tparam  Period Type representing the tick period.
   * \param   timeout_duration Maximum duration to wait for.
   * \return  Status that indicates whether the timeout hit or if a value is available.
   * \trace   SPEC-7552475
   * \trace   CREQ-200638
   * \vpublic
   */
  template <typename Rep, typename Period>
  future_status wait_for(std::chrono::duration<Rep, Period> const& timeout_duration) const noexcept(false) {
    return (valid() && shared_state_->WaitFor(timeout_duration)) ? future_status::ready : future_status::timeout;
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief   Wait until a specified absolute time.
   * \details Returns future_status::timeout directly if shared state is invalid.
   * \tparam  Clock The clock on which this time point is measured.
   * \tparam  Duration A std::chrono::duration type used to measure the time since epoch.
   * \param   abs_time maximum time point to block until.
   * \return  Status that indicates whether the time was reached or if a value is available.
   * \trace   SPEC-7552476
   * \trace   CREQ-200640
   * \vpublic
   */
  template <typename Clock, typename Duration>
  future_status wait_until(std::chrono::time_point<Clock, Duration> const& abs_time) const noexcept(false) {
    return (valid() && shared_state_->WaitUntil(abs_time)) ? future_status::ready : future_status::timeout;
  }

  // VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
  /*!
   * \brief   Set a continuation for when the shared state is ready.
   * \details When func is called, it is guaranteed that get() and GetResult() will not block.
   *          Returns an invalid Future if shared state is invalid.
   * \remark  func may be called in the context of this call or in the context of Promise::set_value() or
   *          Promise::SetError() or somewhere else.
   *          Warning: This function might use dynamic memory allocation. Use with caution!
   * \tparam  Type of continuation function.
   * \param   func A continuation function to be attached.
   * \return  A new Future instance for the result of the continuation.
   * \trace   SPEC-7552477
   * \trace   CREQ-200641
   * \vpublic
   */
  template <typename F>
  auto then(F&& func) noexcept -> typename internal::TypeUnwrapping<decltype(func(std::move(*this))), E>::type {
    /*! \brief Alias for the type of the value contained in the Future returned from this function. */
    using U = decltype(func(std::move(*this)));

    /*! \brief Alias for the type of the value contained in the Future returned from this function. */
    using T2 = typename internal::TypeUnwrapping<U, E>::value_type;

    /*! \brief Alias for the type of the value contained in the Future returned from this function. */
    using E2 = typename internal::TypeUnwrapping<U, E>::error_type;

    Future<T2, E2> future;

    bool const is_null_ptr{internal::IsNullPtr<F>::check(func)};
    // Returns an invalid Future if *this is not valid and/or func is nullptr.
    if ((valid()) && (!is_null_ptr)) {
      FutureContinuationPtr fut_cont{future_continuation_};
      std::lock_guard<std::mutex> const lock{fut_cont->GetCallBackMutex()};
      // Call the function if Future state is ready.
      if (is_ready()) {
        future = internal::GetFuture<T, E, F, U, T2, E2>()(std::forward<F>(func), std::move(*this));

        // Save the call back function in continuation to be called later when Promise is set.
      } else {
        // Move the future_continuation pointer to f_continuation to be used after this Future object gets invalidated.
        FutureContinuationPtr f_continuation{std::move(future_continuation_)};
        future = f_continuation->SetCallBackHandler(std::forward<F>(func), std::move(*this));
      }
    }
    return future;
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Return true only when the shared state is ready. This method will return immediately and shall not do a
   *         blocking wait.
   * \return True if the future contains a value (or exception), false if not.
   * \trace  SPEC-7552478
   * \trace  CREQ-200642
   * \vpublic
   */
  bool is_ready() const noexcept { return shared_state_ && shared_state_->Ready(); }

 private:
  /*!
   * \brief Parameterized constructor.
   * \param shared_state object pointer to connect with Promise.
   * \param future_continuation Future continuation pointer to manage the call back function.
   * \vprivate
   */
  Future(SharedStatePtr shared_state, FutureContinuationPtr future_continuation) noexcept
      : shared_state_(shared_state), future_continuation_(future_continuation) {}

  /*! \brief The shared state between *this and the connected Promise. */
  SharedStatePtr shared_state_{nullptr};

  /*! \brief Future continuation pointer to manage the call back function. */
  FutureContinuationPtr future_continuation_;

  /*!
   * \brief  Friend to only provide the parameterized ctor for use in Promise::get_future().
   * \tparam T The value type of the Promise and Result.
   * \tparam E The error type of the Promise and Result.
   */
  friend class Promise<T, E>;  // VECTOR SL AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend
};

/*!
 * \brief  void specialized Future
 * \tparam E Error type
 * \trace  CREQ-158607
 * \vpublic
 */
template <typename E>
class Future<void, E> final {
  /*!
   * \brief Alias for result
   */
  using R = Result<void, E>;

  /*! \brief Shared pointer type for SharedState. */
  using SharedStatePtr = std::shared_ptr<internal::SharedState<R, future_errc>>;

  /*! \brief The future continuation pointer type. */
  using FutureContinuationPtr = std::shared_ptr<ara::core::internal::FutureContinuation<void, E>>;

 public:
  /*!
   * \brief Default constructor
   * \trace SPEC-7552465
   * \vpublic
   */
  Future() noexcept = default;

  /*!
   * \brief Default copy constructor deleted
   * \trace SPEC-7552466
   * \vpublic
   */
  Future(Future const&) = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Move constructor.
   * \param other The other instance.
   * \trace SPEC-7552467
   * \vpublic
   */
  Future(Future&& other) noexcept = default;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief Destructor.
   * \trace SPEC-7552468
   * \vpublic
   */
  ~Future() noexcept = default;

  /*!
   * \brief  Default copy assignment operator deleted.
   * \return
   * \trace  SPEC-7552469
   * \vpublic
   */
  Future& operator=(Future const&) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*!
   * \brief  Move assignment operator.
   * \param  other The other instance.
   * \return A reference to "*this".
   * \trace  SPEC-7552470
   * \vpublic
   */
  Future& operator=(Future&& other) & noexcept = default;

  /*!
   * \brief  Get the result (does not throw exceptions).
   * \return The value stored in the shared state.
   * \trace  SPEC-7552472
   * \trace  CREQ-200636
   * \vpublic
   */
  R GetResult() noexcept {
    R ret_val{R::FromError(future_errc::no_state)};
    if (valid()) {
      ret_val = shared_state_->GetData();
    }
    return std::move(ret_val);
  }

  /*!
   * \brief  Get the value.
   * \return The value stored in the shared state.
   * \throws ara::core::FutureException Internal Future/Promise errors see FutureErrc.
   * \throws ara::core::Exception Any error which is set in SetError. The concrete exception type depends on the
   *         Exception type corresponding to the error domain of the ErrorCode.
   * \trace  SPEC-7552471
   * \trace  CREQ-200636
   * \vpublic
   */
  template <bool enabled = vac::language::internal::kCompileWithExceptions, typename = std::enable_if_t<enabled>>
  void get() noexcept(false) {
    static_assert((!enabled) || vac::language::internal::kCompileWithExceptions,
                  "Should not be enabled when exceptions are disabled.");
    R res{GetResult()};
    if (!res.HasValue()) {
      res.Error().ThrowAsException();
    }
    return std::move(res).Value();
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Check if the Future has any shared state.
   * \return True if *this refers to a shared state, otherwise false.
   * \trace  SPEC-7552473
   * \trace  CREQ-200637
   * \vpublic
   */
  bool valid() const noexcept { return shared_state_ && shared_state_->Valid(); }

  /*!
   * \brief   Block until the shared state is ready.
   * \details Returns directly if shared state is invalid.
   * \trace   SPEC-7552474
   * \trace   CREQ-200639
   * \vpublic
   */
  void wait() const noexcept {
    if (valid()) {
      shared_state_->Wait();
    }
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief   Wait for a specified relative time.
   * \details Returns future_status::timeout directly if shared state is invalid.
   * \tparam  Rep An arithmetic type representing the number of ticks.
   * \tparam  Period Type representing the tick period.
   * \param   timeout_duration Maximum duration to wait for.
   * \return  Status that indicates whether the timeout hit or if a value is available.
   * \trace   SPEC-7552475
   * \trace   CREQ-200638
   * \vpublic
   */
  template <typename Rep, typename Period>
  future_status wait_for(std::chrono::duration<Rep, Period> const& timeout_duration) const noexcept(false) {
    return (valid() && shared_state_->WaitFor(timeout_duration)) ? future_status::ready : future_status::timeout;
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief   Wait until a specified absolute time.
   * \details Returns future_status::timeout directly if shared state is invalid.
   * \tparam  Clock The clock on which this time point is measured.
   * \tparam  Duration A std::chrono::duration type used to measure the time since epoch.
   * \param   abs_time maximum time point to block until.
   * \return  Status that indicates whether the time was reached or if a value is available.
   * \trace   SPEC-7552476
   * \trace   CREQ-200640
   * \vpublic
   */
  template <typename Clock, typename Duration>
  future_status wait_until(std::chrono::time_point<Clock, Duration> const& abs_time) const noexcept(false) {
    return (valid() && shared_state_->WaitUntil(abs_time)) ? future_status::ready : future_status::timeout;
  }

  // VECTOR Next Construct AutosarC++17_10-A18.5.8: MD_VAC_A18.5.8_localObjectsShallBeAllocatedOnStack
  /*!
   * \brief   Set a continuation for when the shared state is ready.
   * \details When func is called, it is guaranteed that get() and GetResult() will not block.
   *          Returns an invalid Future if shared state is invalid.
   * \remark  func may be called in the context of this call or in the context of Promise::set_value() or
   *          Promise::SetError() or somewhere else.
   *          Warning: This function might use dynamic memory allocation. Use with caution!
   * \tparam  Type of continuation function.
   * \param   func A continuation function to be attached.
   * \return  A new Future instance for the result of the continuation.
   * \trace   SPEC-7552477
   * \trace   CREQ-200641
   * \vpublic
   */
  template <typename F>
  auto then(F&& func) noexcept -> typename internal::TypeUnwrapping<decltype(func(std::move(*this))), E>::type {
    /*! \brief Alias for the type of the value contained in the Future returned from this function. */
    using U = decltype(func(std::move(*this)));

    /*! \brief Alias for the type of the value contained in the Future returned from this function. */
    using T2 = typename internal::TypeUnwrapping<U, E>::value_type;

    /*! \brief Alias for the type of the value contained in the Future returned from this function. */
    using E2 = typename internal::TypeUnwrapping<U, E>::error_type;

    Future<T2, E2> future;

    // Returns an invalid Future if *this is not valid.
    if (valid()) {
      FutureContinuationPtr fut_cont{future_continuation_};
      std::lock_guard<std::mutex> const lock{fut_cont->GetCallBackMutex()};
      // Call the function if Future state is ready.
      if (is_ready()) {
        future = internal::GetFuture<void, E, F, U, T2, E2>()(std::forward<F>(func), std::move(*this));

        // Save the call back function in continuation to be called later when Promise is set.
      } else {
        // Move the future_continuation pointer to f_continuation to be used after this Future object gets invalidated.
        FutureContinuationPtr f_continuation{std::move(future_continuation_)};
        future = f_continuation->SetCallBackHandler(std::forward<F>(func), std::move(*this));
      }
    }

    return future;
  }

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  Return true only when the shared state is ready. This method will return immediately and shall not do a
   *         blocking wait.
   * \return True if the future contains a value (or exception), false if not.
   * \trace  SPEC-7552478
   * \trace  CREQ-200642
   * \vpublic
   */
  bool is_ready() const noexcept { return shared_state_ && shared_state_->Ready(); }

 private:
  /*!
   * \brief Parameterized constructor.
   * \param shared_state object pointer to connect with Promise.
   * \param future_continuation Future continuation pointer to manage the call back function.
   * \vprivate
   */
  Future(SharedStatePtr shared_state, FutureContinuationPtr future_continuation) noexcept
      : shared_state_(shared_state), future_continuation_(future_continuation) {}

  /*! \brief The shared state between *this and the connected Promise. */
  SharedStatePtr shared_state_{nullptr};

  /*! \brief Future continuation pointer to manage the call back function. */
  FutureContinuationPtr future_continuation_;

  /*!
   * \brief  Friend to only provide the parameterized ctor for use in Promise::get_future().
   * \tparam E The error type of the Promise and Result.
   */
  friend class Promise<void, E>;  // VECTOR SL AutosarC++17_10-A11.3.1: MD_VAC_A11.3.1_doNotUseFriend
};

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_FUTURE_H_
