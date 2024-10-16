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
/*!        \file  test_adapter.h
 *        \brief  Encapsulation of friend declarations for unit testing.
 *
 *********************************************************************************************************************/
#ifndef LIB_VAC_INCLUDE_VAC_TESTING_TEST_ADAPTER_H_
#define LIB_VAC_INCLUDE_VAC_TESTING_TEST_ADAPTER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

// VECTOR Disable AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
// VECTOR Disable AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_macroExtendsAnotherMacro
// VECTOR Disable AutosarC++17_10-M16.3.1: MD_VAC_M16.3.1_onlyOneOrTwoHashesAllowedInMacroDefinition
// VECTOR Disable AutosarC++17_10-M16.3.2: MD_VAC_M16.3.2_the#And##OperatorsUsed
/*!
 * \brief   Macro to encapsulate friend declarations for unit testing.
 * \details The ifdef serves to disable our macro in case gtest is included (which defines the identical macro).
 * \trace   CREQ-158644
 */
#ifndef FRIEND_TEST
#define FRIEND_TEST(a, b) friend class a##_##b##_Test
#endif
// VECTOR Enable AutosarC++17_10-M16.3.2
// VECTOR Enable AutosarC++17_10-M16.3.1
// VECTOR Enable AutosarC++17_10-A16.0.1
// VECTOR Enable AutosarC++17_10-A16.0.1
#endif  // LIB_VAC_INCLUDE_VAC_TESTING_TEST_ADAPTER_H_
