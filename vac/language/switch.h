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
/*!        \file    vac/language/switch.h
 *         \brief   Implements a switch that works for non primitive types.
 *         \details Switch that works on complex data and may be used as expression.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_VAC_LANGUAGE_SWITCH_H_
#define LIB_VAC_INCLUDE_VAC_LANGUAGE_SWITCH_H_

/*********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <type_traits>
#include <utility>

#include "ara/core/optional.h"

namespace vac {
namespace language {

/*!
 * \brief   A switch 'expression' for complex data.
 * \details May be used on any type that has an operator== implementation.
 * \tparam  Out The output type for the switch expression.
 * \tparam  In The type on which the switch operates.
 */
template <typename Out, typename In>
class SwitchType {
 public:
  /*! \brief Alias for In const */
  using InConst = std::add_const_t<In>;

  /*!
   * \brief Constructs a switch 'expression' from the value to match on.
   * \param in The value to match on.
   */
  explicit SwitchType(InConst& in) noexcept : out_{}, in_{in} {}

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  A case 'expression' for the current switch.
   * \tparam SupplierType The function type for the value producer.
   * \param  input The case value.
   * \param  supplier Returns the data if the case matches.
   * \return The Switch itself to enable chaining.
   * \trace  CREQ-VaCommonLib-SwitchLambda
   */
  template <typename SupplierType, typename SupplierResult = std::result_of_t<SupplierType()>,
            std::enable_if_t<std::is_constructible<Out, SupplierResult>::value, std::int32_t> = 0>
      auto Case(InConst& input, SupplierType&& supplier) && noexcept(false) -> SwitchType&& {
    if ((!out_) && (in_.get() == input)) {
      out_.emplace(std::forward<SupplierResult>(supplier()));
    }
    return std::move(*this);
  }

  // VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  A case 'expression' for the current switch.
   * \param  input The case value.
   * \param  value Returns the data if the case matches.
   * \return The Switch itself to enable chaining.
   * \trace  CREQ-VaCommonLib-SwitchLiteral
   */
  template <typename OutputType, std::enable_if_t<std::is_constructible<Out, OutputType>::value, std::int32_t> = 0>
      auto Case(InConst& input, OutputType&& value) && noexcept(false) -> SwitchType&& {
    if ((!out_) && (in_.get() == input)) {
      out_.emplace(std::forward<OutputType>(value));
    }
    return std::move(*this);
  }

  /*!
   * \brief  A default case 'expression' for the current switch.
   * \tparam Supplier The function type for the value producer.
   * \param  supplier Returns the data if no other case matched.
   * \return Either the supplied value or a value produced before.
   * \trace  CREQ-VaCommonLib-SwitchLambda
   */
  template <typename Supplier, typename SupplierResult = std::result_of_t<Supplier()>,
            std::enable_if_t<std::is_constructible<Out, SupplierResult>::value, std::int32_t> = 0>
      auto Default(Supplier&& supplier) && noexcept(false) -> Out {
    return out_ ? *(std::move(this)->out_) : static_cast<Out>(std::forward<SupplierResult>(supplier()));
  }

  // VECTOR Next Construct AutosarC++17_10-A13.3.1: MD_VAC_A13.3.1_forwardingFunctionsShallNotBeOverloaded
  /*!
   * \brief  A default case 'expression' for the current switch.
   * \param  value Returns the data if no other case matched.
   * \return Either the supplied value or a value produced before.
   * \trace  CREQ-VaCommonLib-SwitchLiteral
   */
  template <typename Out1, std::enable_if_t<std::is_constructible<Out, Out1>::value, std::int32_t> = 0>
      auto Default(Out1&& value) && noexcept(false) -> Out {
    return out_ ? *(std::move(this)->out_) : static_cast<Out>(std::forward<Out1>(value));
  }

  /*!
   * \brief  Gets the unexhaustive result.
   * \return Either the produced value or an empty optional.
   * \trace  CREQ-VaCommonLib-SwitchOptional
   */
  auto Get() && -> ara::core::Optional<Out> { return std::move(this)->out_; }

 private:
  /*! \brief The container for the return value. */
  ara::core::Optional<Out> out_{};
  /*! \brief The reference for the switch value. */
  std::reference_wrapper<InConst> in_;
};

/*!
 * \brief   A switch 'statement' for complex data.
 * \details May be used on any type that has an operator== implementation.
 * \tparam  In The type on which the switch operates.
 */
template <typename In>
class SwitchType<void, In> {
 public:
  /*! \brief Alias for In const */
  using InConst = std::add_const_t<In>;

  /*!
   * \brief Constructs a switch 'statement' from the value to match on.
   * \param in The value to match on.
   */
  explicit SwitchType(InConst& in) noexcept : in_{in}, need_match_{true} {}

  // VECTOR Next Construct AutosarC++17_10-M5.14.1: MD_VAC_M5.14.1_operandHasNoSideEffect
  /*!
   * \brief  A case 'statement' for the current switch.
   * \tparam EffectType The function type that produces the side effect.
   * \param  input The case value.
   * \param  effect Performs the side effect if the case matches.
   * \return The Switch itself to enable chaining.
   * \trace  CREQ-VaCommonLib-SwitchSideEffect
   */
  template <typename EffectType>
      auto Case(InConst& input, EffectType&& effect) && noexcept(false) -> SwitchType&& {
    if (need_match_ && (in_.get() == input)) {
      need_match_ = false;
      effect();
    }
    return std::move(*this);
  }

  /*!
   * \brief  A default case 'statement' for the current switch.
   * \tparam Effect The function type that produces the side effect.
   * \param  effect Performs the side effect if no other case matched.
   * \return Either the supplied value or a value produced before.
   * \trace  CREQ-VaCommonLib-SwitchSideEffect
   */
  template <typename Effect>
      auto Default(Effect&& effect) && noexcept(false) -> void {
    if (need_match_) {
      need_match_ = false;
      effect();
    }
  }

 private:
  /*! \brief The reference for the switch value. */
  std::reference_wrapper<InConst> in_;
  /*! \brief Marker if further case statements need to be considered. */
  bool need_match_{true};
};

/*!
 * \brief   Constructs a switch 'expression'/'statement' from the value to match on.
 * \details May be used on any type that has an operator== implementation.
 * \tparam  Out The output type for the switch expression.
 * \tparam  In The type on which the switch operates.
 * \param   in The value to match on.
 * \return  A switch 'expression'/'statement'.
 * \trace   CREQ-VaCommonLib-SwitchComplexTypes
 */
template <typename Out = void, typename In>
auto Switch(In const& in) noexcept(false) -> SwitchType<Out, In> {
  return SwitchType<Out, In>{in};
}

}  // namespace language
}  // namespace vac

#endif  // LIB_VAC_INCLUDE_VAC_LANGUAGE_SWITCH_H_
