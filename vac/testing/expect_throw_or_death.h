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
/*!        \file expect_throw_or_death.h
 *        \brief Contains macros to be used instead of the gtest macro EXPECT_THROW and its relatives.
 *
 *      \details The idea is to have macros which can be a drop-in replacement of EXPECT_THROW and its relatives to
 *               easier be able to write tests for both exceptionless code and code with exceptions.
 *               Note: Do *not* use this to replace the gtest macro EXPECT_DEATH.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_TESTING_EXPECT_THROW_OR_DEATH_H_
#define LIB_VAC_INCLUDE_VAC_TESTING_EXPECT_THROW_OR_DEATH_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <gtest/gtest.h>

namespace vac {
namespace testing {
namespace internal {
/*! \brief Generic message regex to be used in EXPECT_DEATH. */
constexpr char const* death_message_regex{"^.*$"};
}  // namespace internal
}  // namespace testing
}  // namespace vac

// VECTOR NL AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
#ifdef __EXCEPTIONS

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest EXPECT_THROW.
 * \trace CREQ-VaCommonLib-ExpectThrowOrDeath
 * \vprivate Product private
 */
#define EXPECT_THROW_OR_DEATH(statement, exception_type) EXPECT_THROW(statement, exception_type)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest ASSERT_THROW.
 * \trace CREQ-VaCommonLib-AssertThrowOrDeath
 * \vprivate Product private
 */
#define ASSERT_THROW_OR_DEATH(statement, exception_type) ASSERT_THROW(statement, exception_type)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest EXPECT_ANY_THROW.
 * \trace CREQ-VaCommonLib-ExpectAnyThrowOrDeath
 * \vprivate Product private
 */
#define EXPECT_ANY_THROW_OR_DEATH(statement) EXPECT_ANY_THROW(statement)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest ASSERT_ANY_THROW.
 * \trace CREQ-VaCommonLib-AssertAnyThrowOrDeath
 * \vprivate Product private
 */
#define ASSERT_ANY_THROW_OR_DEATH(statement) ASSERT_ANY_THROW(statement)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest EXPECT_NO_THROW.
 * \trace CREQ-VaCommonLib-ExpectNoThrowOrDeath
 * \vprivate Product private
 */
#define EXPECT_NO_THROW_OR_DEATH(statement) EXPECT_NO_THROW(statement)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest ASSERT_NO_THROW.
 * \trace CREQ-VaCommonLib-AssertNoThrowOrDeath
 * \vprivate Product private
 */
#define ASSERT_NO_THROW_OR_DEATH(statement) ASSERT_NO_THROW(statement)

#else

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest EXPECT_DEATH, discards the second argument.
 * \trace CREQ-VaCommonLib-ExpectThrowOrDeath
 * \vprivate Product private
 */
#define EXPECT_THROW_OR_DEATH(statement, ...) EXPECT_DEATH(statement, vac::testing::internal::death_message_regex)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest ASSERT_DEATH, discards the second argument.
 * \trace CREQ-VaCommonLib-AssertThrowOrDeath
 * \vprivate Product private
 */
#define ASSERT_THROW_OR_DEATH(statement, ...) ASSERT_DEATH(statement, vac::testing::internal::death_message_regex)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest EXPECT_DEATH.
 * \trace CREQ-VaCommonLib-ExpectAnyThrowOrDeath
 * \vprivate Product private
 */
#define EXPECT_ANY_THROW_OR_DEATH(statement) EXPECT_DEATH(statement, vac::testing::internal::death_message_regex)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Alias for gtest ASSERT_DEATH.
 * \trace CREQ-VaCommonLib-AssertAnyThrowOrDeath
 * \vprivate Product private
 */
#define ASSERT_ANY_THROW_OR_DEATH(statement) ASSERT_DEATH(statement, vac::testing::internal::death_message_regex)

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief   Does nothing extra, executes statement provided.
 * \details While EXPECT_NO_THROW can handle an unexpected throw and still continue, there is no EXPECT_NO_DEATH that
 *          can do the same. If a death occurs, then the test will terminate and not continue. A more accurate name
 *          would be EXPECT_NO_THROW_OR_ASSERT_NO_DEATH, but this is inconveniently long.
 * \trace   CREQ-VaCommonLib-ExpectNoThrowOrDeath
 * \vprivate Product private
 */
#define EXPECT_NO_THROW_OR_DEATH(statement) statement

// VECTOR Next Construct AutosarC++17_10-A16.0.1: MD_VAC_A16.0.1_conditionalCompilationIsOnlyAllowedForIncludeGuards
/*!
 * \brief Does nothing extra, executes statement provided.
 * \trace CREQ-VaCommonLib-AssertNoThrowOrDeath
 * \vprivate Product private
 */
#define ASSERT_NO_THROW_OR_DEATH(statement) statement

#endif

#endif  // LIB_VAC_INCLUDE_VAC_TESTING_EXPECT_THROW_OR_DEATH_H_
