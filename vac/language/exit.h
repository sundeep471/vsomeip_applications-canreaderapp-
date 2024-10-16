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
/*!        \file  vac/language/exit.h
 *        \brief  Exit for handling of "Corruptions".
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_EXIT_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_EXIT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>
#include <cstdlib>

namespace vac {
namespace language {

// VECTOR NC AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
/*!
 * \brief   Fucntion for handling of corruptions.
 * \trace   CREQ-VaCommonLib-Exit
 * \param   exit_code Exit status of the program.
 * \vpublic
 */
[[noreturn]] void Exit(std::uint8_t exit_code) noexcept;

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_EXIT_H_
