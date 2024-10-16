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
/*!        \file  ara/core/instance_specifier.h
 *        \brief  SWS core type ara::core::InstanceSpecifier.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAC_INCLUDE_ARA_CORE_INSTANCE_SPECIFIER_H_
#define LIB_VAC_INCLUDE_ARA_CORE_INSTANCE_SPECIFIER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "ara/core/result.h"
#include "ara/core/string_view.h"

namespace ara {
namespace core {

// VECTOR Next Construct AutosarC++17_10-M3.4.1: MD_VAC_M3.4.1_symbolsCanBeDeclaredLocally
/*!
 * \brief Class representing an AUTOSAR Instance Specifier, which is basically an AUTOSAR shortname path wrapper.
 * \trace SPEC-7552586
 * \trace CREQ-219455
 * \vpublic
 */
class InstanceSpecifier final {
 public:
  /*!
   * \brief Class providing Construction Token
   * \vpublic
   */
  class ConstructionToken final {
   public:
    /*!
     * \brief Delete default constructor.
     */
    ConstructionToken() = delete;

    /*!
     * \brief Constructor
     *
     * \param metaModelIdentifier stringified meta model identifier
     */
    explicit ConstructionToken(StringView metaModelIdentifier) noexcept : token_id_{metaModelIdentifier} {}

    /*!
     * \brief Default Move Constructor.
     */
    ConstructionToken(ConstructionToken&&) noexcept = default;

    /*!
     * \brief Delete Copy Constructor.
     */
    ConstructionToken(ConstructionToken const&) = delete;

    /*!
     * \brief  Default Move assignment.
     * \return A reference to the assigned-to object.
     */
    ConstructionToken& operator=(ConstructionToken&&) & noexcept = default;

    /*!
     * \brief  Delete Copy assignment.
     * \return
     */
    ConstructionToken& operator=(ConstructionToken const&) & = delete;

    /*!
     * \brief Use default Destructor.
     */
    ~ConstructionToken() noexcept = default;

    /*!
     * \brief Gets the Construction token id for string field.
     * \return Stringfiled Construction token id.
     */
    StringView GetTokenId() const noexcept { return token_id_; }

   private:
    /*!
     * \brief stringified Construction token id
     */
    StringView token_id_;
  };

  /*!
   * \brief Default constructor deleted.
   * \vpublic
   */
  InstanceSpecifier() = delete;

  /*!
   * \brief Copy constructor.
   */
  InstanceSpecifier(InstanceSpecifier const&) noexcept = default;

  /*!
   * \brief Move constructor.
   */
  InstanceSpecifier(InstanceSpecifier&&) noexcept = default;

  /*!
   * \brief  Copy assignment operator.
   * \return A reference to the assigned-to object.
   */
  InstanceSpecifier& operator=(InstanceSpecifier const&) & noexcept = default;

  /*!
   * \brief  Move assignment operator.
   * \return A reference to the assigned-to object.
   */
  InstanceSpecifier& operator=(InstanceSpecifier&&) & noexcept = default;

  /*!
   * \brief Destructor.
   * \trace SPEC-7552589
   * \vpublic
   */
  ~InstanceSpecifier() noexcept = default;

  // VECTOR Disable AutosarC++17_10-M3.2.4: MD_VAC_M3.2.4_functionWithoutDefinition
  /*!
   * \brief     Throwing ctor from meta-model string.
   * \param     metaModelIdentifier Stringified meta model identifier (short name path) where path separator is '/'.
   * \exception InstanceSpecifierException, in case the given meta-model identifier isn't a valid meta-model
   *            identifier/short name path.
   * \trace     SPEC-7552587
   * \vpublic
   */
  explicit InstanceSpecifier(StringView metaModelIdentifier) noexcept(false);

  /*!
   * \brief  Pre-construct method for InstanceSpecifier.
   * \param  metaModelIdentifier meta model identifier (short name path) where path separator is '/'.
   * \return Result containing construction token from which an instance specifier can be
   *         constructed or an error code.
   * \trace  SPEC-7552590
   * \vpublic
   *
   */
  static Result<ConstructionToken> Preconstruct(StringView metaModelIdentifier) noexcept;

  // VECTOR Next Construct AutosarC++17_10-M7.1.2: MD_VAC_M7.1.2_parameterAsReferencePointerToConst
  /*!
   * \brief Non-throwing ctor for construction token pattern.
   * \param token ConstructionToken obtained via Pre-construct.
   * \trace SPEC-7552588
   * \vpublic
   */
  explicit InstanceSpecifier(ConstructionToken&& token) noexcept;

  /*!
   * \brief  Non-throwing helper method to create InstanceSpecifier from a string representation.
   * \param  metaModelIdentifier Stringfied form of InstanceSpecifier.
   * \return Result containing valid InstanceSpecifier or an ErrorCode.
   * \trace  SPEC-7552597
   * \vpublic
   */
  static Result<InstanceSpecifier> MakeInstanceSpecifier(StringView metaModelIdentifier) noexcept;

  /*!
   * \brief  Equal(==) operator to compare with other InstanceSpecifier instance.
   * \param  other InstanceSpecifier instance to compare this one with.
   * \return True in case both InstanceSpecifiers are denoting exactly the same model element, false else.
   * \trace  SPEC-7552591
   * \vpublic
   */
  bool operator==(InstanceSpecifier const& other) const noexcept;

  /*!
   * \brief  Equal(==) operator to compare with other InstanceSpecifier string representation.
   * \param  other String representation of model element to compare this one with.
   * \return True in case this InstanceSpecifiers is denoting exactly the same model element as other, false else.
   * \trace  SPEC-7552592
   * \vpublic
   */
  bool operator==(StringView other) const noexcept;

  /*!
   * \brief  Unequal(!=) operator to compare with other InstanceSpecifier instance.
   * \param  other InstanceSpecifier instance to compare this one with.
   * \return False in case both InstanceSpecifiers are denoting exactly the same model element, true else.
   * \trace  SPEC-7552593
   * \vpublic
   */
  bool operator!=(InstanceSpecifier const& other) const noexcept;

  /*!
   * \brief  Unequal(!=) operator to compare with other InstanceSpecifier string representation.
   * \param  other String representation to compare this one with.
   * \return False in case this InstanceSpecifiers is denoting exactly the same model element as other, true else.
   * \trace  SPEC-7552594
   * \vpublic
   */
  bool operator!=(StringView other) const noexcept;

  /*!
   * \brief  Lower than operator to compare with other InstanceSpecifier for ordering purposes (f.i. when
   *         collecting identifiers in maps).
   * \param  other InstanceSpecifier instance to compare this one with.
   * \return True in case this InstanceSpecifiers is lexically lower than other, false else.
   * \trace  SPEC-7552595
   * \vpublic
   */
  bool operator<(InstanceSpecifier const& other) const noexcept;

  /*!
   * \brief  Method to return the stringified form of InstanceSpecifier.
   * \return Stringified form of InstanceSpecifier.
   * \trace  SPEC-7552596
   * \vpublic
   */
  StringView ToString() const noexcept;
  // VECTOR Enable AutosarC++17_10-M3.2.4

 private:
  /*!
   * \brief Stringifed form of InstanceSpecifier.
   */
  StringView instance_specifier_;
};

}  // namespace core
}  // namespace ara

#endif  // LIB_VAC_INCLUDE_ARA_CORE_INSTANCE_SPECIFIER_H_
