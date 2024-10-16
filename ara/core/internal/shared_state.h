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
/**        \file ara/core/internal/shared_state.h
 *        \brief Contains SharedState to be used by ara::core::Future and ara::core::Promise as a means to communicate.
 *
 *      \details Is thread-safe and exceptionless.
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_SHARED_STATE_H_
#define LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_SHARED_STATE_H_
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <utility>

namespace ara {
namespace core {
namespace internal {

/*!
 * \brief  A threadsafe state to be used by ara::core::Promise and ara::core::Future to share data.
 * \tparam R  the Result type containing the same value/error types as the connected Future/Promise pair has.
 * \tparam Ec Future error code.
 * \vprivate
 */
template <typename R, typename Ec>
class SharedState final {
 public:
  /*!
   * \brief   Default constructor.
   * \details Called only by Promise.
   */
  SharedState() : data_{Ec::no_state}, is_ready_{false}, is_valid_{false}, cv_{}, mutex_{} {}

  /*! \brief Deleted copy constructor. */
  SharedState(SharedState const&) = delete;

  /*! \brief Deleted move constructor. */
  SharedState(SharedState&&) = delete;

  /*! \brief Deleted copy assignment operator. */
  SharedState& operator=(SharedState const&) & = delete;

  /*! \brief Deleted move assignment operator. */
  SharedState& operator=(SharedState&&) & = delete;

  // VECTOR Next Construct AutosarC++17_10-A15.5.1: MD_VAC_A15.5.1_explicitNoexceptIfAppropriate
  /*! \brief Defaulted destructor. */
  ~SharedState() noexcept = default;

  /*!
   * \brief   Called by the attached Promise when destructed, sets state to ready. If state is not ready, set
   *          broken_promise.
   * \details Called only by Promise.
   */
  void BreakPromise() noexcept {
    {
      std::lock_guard<std::mutex> const lock{mutex_};
      if (!is_ready_) {
        data_.EmplaceError(Ec::broken_promise);
        is_ready_ = true;
      }
    }
    cv_.notify_one();
  }

  /*!
   * \brief   Wait until a value has been set and the state is ready.
   * \details Called only by Future.
   */
  void Wait() noexcept {
    // VECTOR Next Line AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    std::unique_lock<std::mutex> lock{mutex_};
    cv_.wait(lock, [this]() -> bool { return is_ready_; });
  }

  /*!
   * \brief   Waits until the state is ready or until the given time is reached, whichever comes first.
   * \details Called only by Future.
   * \tparam  Clock The clock on which this time point is measured.
   * \tparam  Duration A std::chrono::duration type used to measure the time since epoch.
   * \param   abs_time maximum time point to block until.
   * \return  bool True if the state is ready and false if it is not.
   */
  template <typename Clock, typename Duration>
  bool WaitUntil(std::chrono::time_point<Clock, Duration> const& abs_time) noexcept(false) {
    // VECTOR Next Line AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    std::unique_lock<std::mutex> lock{mutex_};
    return cv_.wait_until(lock, abs_time, [this]() -> bool { return is_ready_; });
  }

  /*!
   * \brief   Waits until the state is ready or until the given time has passed, whichever comes first.
   * \details Called only by Future.
   * \tparam  Rep An arithmetic type representing the number of ticks.
   * \tparam  Period Type representing the tick period.
   * \param   timeout_duration Maximum duration to wait for.
   * \return  bool True if the state is ready and false if it is not.
   */
  template <typename Rep, typename Period>
  bool WaitFor(std::chrono::duration<Rep, Period> const& timeout_duration) noexcept(false) {
    // VECTOR Next Line AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    std::unique_lock<std::mutex> lock{mutex_};
    return cv_.wait_for(lock, timeout_duration, [this]() -> bool { return is_ready_; });
  }

  /*!
   * \brief   Sets the given lvalue to the contained data_.
   * \details If the data has already been set then promise_already_satisfied will be set instead. If trying to set
   *          after the data has been retrieved then no_state will be set instead.
   *          Called only by Promise.
   * \param   data lvalue to be copied to data_.
   */
  void SetData(R const& data) noexcept {
    {
      std::lock_guard<std::mutex> const lock{mutex_};
      if (!SetError()) {
        data_ = data;
        is_ready_ = true;  // If SetError() returns true, then is_ready_ is already true.
      }
    }
    cv_.notify_one();
  }

  /*!
   * \brief   Sets the given rvalue to the contained data_.
   * \details If the data has already been set then promise_already_satisfied will be set instead. If trying to set
   *          after the data has been retrieved then no_state will be set instead.
   *          Called only by Promise.
   * \param   data rvalue to be moved to data_.
   */
  void SetData(R&& data) noexcept {
    {
      std::lock_guard<std::mutex> const lock{mutex_};
      if (!SetError()) {
        data_ = std::move(data);
        is_ready_ = true;  // If SetError() returns true, then is_ready_ is already true.
      }
    }
    cv_.notify_one();
  }

  /*!
   * \brief   Retrieves the contained data_.
   * \details If the state is not ready, it will wait until it is ready.
   *          Called only by Future.
   * \return  data The contained data.
   */
  R GetData() noexcept {
    // VECTOR Next Line AutosarC++17_10-M8.5.2: MD_VAC_M8.5.2_provideExplicitInitializer
    std::unique_lock<std::mutex> lock{mutex_};
    cv_.wait(lock, [this]() -> bool { return is_ready_; });
    is_valid_ = false;
    return std::move(data_);
  }

  /*!
   * \brief   Makes state valid, called by Promise::get_future() to indicate that a connection has been made.
   * \details If the data has already been retrieved, it will set a future_already_retrieved error.
   */
  void MakeValid() noexcept {
    {
      std::lock_guard<std::mutex> const lock{mutex_};
      if (is_valid_) {
        data_.EmplaceError(Ec::future_already_retrieved);
        is_ready_ = true;
      }
      is_valid_ = true;
    }
    cv_.notify_one();
  }

  /*!
   * \brief   Checks if the state is valid.
   * \details Called only by Future.
   * \return  bool True if valid, false if not.
   */
  bool Valid() noexcept {
    std::lock_guard<std::mutex> const lock{mutex_};
    return is_valid_;
  }

  /*!
   * \brief   Checks if the state is ready.
   * \details Called only by Future.
   * \return  bool True if ready, false if not.
   */
  bool Ready() noexcept {
    std::lock_guard<std::mutex> const lock{mutex_};
    return is_ready_;
  }

 private:
  /*!
   * \brief  Sets error if trying to write to data_ during an unallowed state.
   * \return true if an error has been set, false otherwise.
   */
  bool SetError() noexcept {
    if (is_ready_) {
      if (!is_valid_) {
        data_.EmplaceError(Ec::no_state);  // Occurs when trying to set data after reading data.
      } else {
        data_.EmplaceError(Ec::promise_already_satisfied);  // Occurs when setting data multiple times.
      }
    }
    return is_ready_;
  }

  /*! \brief The contained data as a Result<T, E>. */
  R data_{};

  /*! \brief Bool to check if a value has been set to data_. */
  bool is_ready_{};

  /*! \brief Bool to check if a connection between Promise and Future has been made. */
  bool is_valid_{};

  /*! \brief Condition variable to be used when waiting. */
  std::condition_variable cv_{};

  /*! \brief Mutex to ensure thread-safety. */
  std::mutex mutex_{};
};  // namespace internal

}  // namespace internal
}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_INTERNAL_SHARED_STATE_H_
