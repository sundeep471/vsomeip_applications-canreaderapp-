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
/**     \file       lifecycle.h
 *      \brief      Initialization/Deinitialization functions for VAC
 *      \details
 *
 *********************************************************************************************************************/
#ifndef LIB_VAC_INCLUDE_VAC_INTERNAL_LIFECYCLE_H_
#define LIB_VAC_INCLUDE_VAC_INTERNAL_LIFECYCLE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "ara/core/result.h"

namespace vac {
namespace internal {

// VECTOR NC AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
/*!
 * \brief Initializes the VaCommonLib module for application usage.
 * \returns Nothing on success
 *
 * \trace CREQ-VaCommonLib-Initialization
 * \vprivate Product private.
 */
auto InitializeComponent() noexcept -> ara::core::Result<void>;

// VECTOR NC AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
/*!
 * \brief Deinitializes the VaCommonLib module.
 * \return Nothing on success
 *
 * \trace CREQ-VaCommonLib-Initialization
 * \vprivate Product private.
 */
auto DeinitializeComponent() noexcept -> ara::core::Result<void>;

// VECTOR NC AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
/*!
 * \brief Gets the initialization state of the VaCommonLib module.
 * \return True in case the component is initialized and ready for use.
 *
 * \trace CREQ-VaCommonLib-Initialization
 * \vprivate Product private.
 */
auto IsComponentInitialized() noexcept -> bool;
}  // namespace internal
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_INTERNAL_LIFECYCLE_H_
