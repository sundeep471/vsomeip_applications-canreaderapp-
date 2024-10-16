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
/*!        \file  throw_or_terminate.h
 *        \brief  ThrowOrTerminate implementation for differentiating between throw and terminate
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_THROW_OR_TERMINATE_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_THROW_OR_TERMINATE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "amsr/generic/write_std_stream.h"
#include "vac/language/compile_exceptions.h"

namespace vac {
namespace language {
namespace internal {

/*!
 * \brief Calls std::terminate.
 */
[[noreturn]] inline void Terminate() noexcept {
  // VECTOR Next Line AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
#ifdef _BullseyeCoverage
  static_cast<void>(cov_write());
#endif
  // VECTOR Next Construct AutosarC++17_10-A15.5.2: MD_VAC_A15.5.2_terminateByDesign
  std::terminate();
}

/*!
 * \brief Prints the message on the error console and calls terminate.
 * \param message Termination message.
 */
[[noreturn]] inline void Terminate(char const* message) noexcept {
  static_cast<void>(amsr::generic::WriteToStderr(message));
  static_cast<void>(amsr::generic::WriteToStderr("\n"));
  Terminate();
}
/*!
 * \brief   Helper template to control the behavior of ThrowOrTerminate::Execute<ExceptionType>.
 * \details throw_exceptions Execute will throw, otherwise call will terminate.
 * \tparam  throw_exceptions Flag for enabling/disabling exceptions.
 * \trace   CREQ-180467
 */
template <bool throw_exceptions>
class ThrowOrTerminateT;

/*!
 * \brief Overload when throw_exceptions ==  true.
 */
template <>
class ThrowOrTerminateT<true> {
 public:
  /*!
   * \brief  Throws the given error_code as the given Exception type.
   * \param  args Arguments passed to thse thrown exception.
   * \tparam ExceptionType Exception type to be thrown.
   * \return
   */
  template <class ExceptionType, typename... Args, bool with_exceptions = internal::kCompileWithExceptions,
            typename = std::enable_if_t<with_exceptions>,
            typename = std::enable_if_t<std::is_base_of<std::exception, ExceptionType>::value>>
  [[noreturn]] static auto Execute(Args&&... args) -> void {
    // VECTOR Next Line AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
#ifdef __EXCEPTIONS
    // VECTOR Next Line AutosarC++17_10-A15.1.1: MD_VAC_A15.1.1_onlyInstancesOfTypesDerivedFromStdExceptionShallbeThrown
    throw ExceptionType(std::forward<Args>(args)...);
#else
    // The always_false bool is here to assure the clang9 compiler that ThrowOrTerminateT<true> can not be accessed when
    // kCompileWithExceptions is false.
    static constexpr bool always_false{(sizeof...(args)) == -1};
    static_assert(always_false, "This cannot be compiled with -fno-exceptions.");

#endif
  }
};

/*!
 * \brief Overload when throw_exceptions == false.
 */
template <>
class ThrowOrTerminateT<false> {
 public:
  /*!
   * \brief Helper to cleanly discard parameter packs.
   */
  class ParameterSink final {
   public:
    /*!
     * \brief Default contsructor. Discards the parameter pack.
     * \param ... Input to construct the ParameterSink.
     */
    template <typename... Args>
    explicit ParameterSink(Args const&...) noexcept {}

    /*!
     * \brief Deleted copy constructor.
     */
    ParameterSink(ParameterSink const&) = delete;

    /*!
     * \brief Deleted move constructor.
     */
    ParameterSink(ParameterSink&&) = delete;

    /*!
     * \brief Deleted copy assignment operator.
     * \return
     */
    ParameterSink& operator=(ParameterSink const&) = delete;

    /*!
     * \brief Deleted move assignment operator.
     * \return
     */
    ParameterSink& operator=(ParameterSink&&) = delete;

    /*! \brief Default destructor. */
    ~ParameterSink() noexcept = default;
  };

  // VECTOR Next Construct AutosarC++17_10-A3.9.1: MD_VAC_A3.9.1_baseTypeRequiredBySpecification
  /*!
   * \brief  Calls Terminate() with the given message.
   * \tparam ExceptionType Is ignored.
   * \param  message Error message.
   * \return
   */
  template <class ExceptionType>
  [[noreturn]] static auto Execute(char const* message) noexcept -> void {
    Terminate(message);
  }

  /*!
   * \brief  Calls Terminate().
   * \tparam ExceptionType Is ignored.
   * \tparam Args Ignored arguments.
   * \param  args Ignored arguments.
   * \return
   */
  template <class ExceptionType, typename... Args>
  [[noreturn]] static auto Execute(Args&&... args) noexcept -> void {
    // Omitting parameter name is not possible since doxygen enforces parameter documentation for parameter pack.
    static_cast<void>(ParameterSink{args...});
    Terminate();
  }
};
}  // namespace internal

/*!
 * \brief  Throws the specified exception or terminates when excpeption are deactivated.
 * \tparam ExceptionType Exception type.
 * \tparam Args Argument types for exception construction.
 * \param  args Arguments for exception construction.
 */
template <class ExceptionType, typename... Args>
[[noreturn]] void ThrowOrTerminate(Args&&... args) {
  /*!
   * \brief An alias for internal::ThrowOrTerminateT<..>.
   */
  using ThrowOrTerminateType = internal::ThrowOrTerminateT<internal::kCompileWithExceptions>;
  ThrowOrTerminateType::Execute<ExceptionType>(std::forward<Args>(args)...);
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_THROW_OR_TERMINATE_H_
