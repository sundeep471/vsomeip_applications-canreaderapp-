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
/*!        \file  ara/core/abort.h
 *        \brief  Explicit abnormal process termination.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_ABORT_H_
#define LIB_VAC_INCLUDE_ARA_CORE_ABORT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

namespace ara {
namespace core {

/*!
 * \brief   Convenience type alias for a pointer to an AbortHandler function.
 * \details Deviation from AUTOSAR 19-11: until C++17, noexcept may not be part of type alias.
 *          Be aware that the this type may change for future versions where only C++17 and later is supported.
 * \trace   CREQ-VaCommonLib-AbortHandling
 * \vpublic
 */
using AbortHandler = void (*)();

// VECTOR NC AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
/*!
 * \brief   Set the abort handler callback.
 * \details This function is thread-safe.
 *          According to AUTOSAR 19-11, handlers passed to this functions must be noexcept. However this cannot
 *          be enforced at compile time when using C++14. Be aware that the signature of this function may change
 *          in future versions where only support C++17 and later are supported.
 *          Calling SetAbortHandler inside the callback passed to this function has no effect.
 * \param   handler pointer to the callback to use. If set to nullptr, no custom abort handler will be used.
 * \return  If an abort handler was previously installed, a pointer to that handler is returned.
 *          Otherwise, nullptr is returned.
 * \trace   CREQ-VaCommonLib-AbortHandling
 * \vpublic
 */
AbortHandler SetAbortHandler(AbortHandler handler) noexcept;

// VECTOR NC AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
/*!
 * \brief   Terminate the current process abnormally.
 * \details Before terminating, a log message with FATAL severity is being output, which includes the text given as
 *          argument to this function.
 *          The text argument is expected to point to a null-terminated string with static storage duration.
 *          Also a user handler is executed if set via SetAbortHandler.
 *          This function will never return to its caller. The stack is not unwound: destructors of variables with
 *          automatic storage duration are not called.
 *          If ara::core::Abort is called within the user-provided handler, the process will terminate immediately
 *          without outputting a log message or calling the user handler again.
 * \note    The type of the text argument is a raw pointer (instead of a more "modern" type such as StringView) in order
 *          to increase the chances that the function call succeeds even in situations when e.g. the stack has been
 *          corrupted.
 * \param   text A custom text to include in the log message being output.
 * \trace   CREQ-VaCommonLib-AbortHandling
 * \vpublic
 */
[[noreturn]] void Abort(char const* text) noexcept {
    std::cerr << "Aborting: " << text << std::endl; // Log the message
    std::abort(); // Terminate the program
}

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_ABORT_H_
