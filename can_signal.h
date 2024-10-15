/**
**************************************************************************************
** \file can_signal.h
**
** \brief Definition of CAN signal class.
**
**************************************************************************************
*/
#ifndef INCLUDE_UTIL_CAN_SIGNAL_H_
#define INCLUDE_UTIL_CAN_SIGNAL_H_

/*
**====================================================================================
** Imported interfaces
**====================================================================================
*/
#include "ara/core/optional.h"
#include "ara/core/span.h"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>

//#include <optional> // for std::optional
//#include "ara_equivalent.h"


/*
**====================================================================================
** Named Namespaces
**====================================================================================
*/
namespace util {

/** ** \brief Definition of CAN signal base class.  ** ** \tparam T Desired raw value type.  */
template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
class CanSignalBase {
 public:
  /** ** \brief Alias to raw value type.  */
  using RawType = T;

  /** ** \brief Alias to bit count type.  */
  using BitCount = std::size_t;

  /** ** \brief Alias to constant data span type.  */
  using ConstDataSpan = ara::core::Span<const std::uint8_t>;

  /** ** \brief Alias to data span type.  */
  using DataSpan = ara::core::Span<std::uint8_t>;

  /** ** \brief Class constructor.  ** ** \param[in] start  Signal start bit.  ** \param[in] length Signal length in bits.  */
  constexpr CanSignalBase(BitCount start, BitCount length) : start_{start}, length_{length} {}

  /** ** \brief Gets raw signal value (overload for big endian / Motorola byte order).  ** ** \param[in] data Data containing the signal.  ** ** \return Raw signal value if successfully obtained.  */
  ara::core::Optional<RawType> GetValue(std::false_type /*unused*/, ConstDataSpan data) const {
    constexpr auto bits_per_byte = BitCount{CHAR_BIT};
    const auto bit_offset = start_ % bits_per_byte;
    const auto byte_offset = start_ / bits_per_byte;
    const auto byte_length = (length_ + bits_per_byte - 1) / bits_per_byte;

    if ((byte_offset + 1 < byte_length) || (data.size() < byte_offset)) {
      return ara::core::nullopt;
    }

    auto data_it = data.begin() + byte_offset;  // NOLINT
    auto result = RawType{0};

    auto fragment = *(data_it--) >> bit_offset;
    auto fragment_length = std::min(length_, bits_per_byte - bit_offset);

    for (auto offset = BitCount{0};;) {
      fragment &= GetBitMask(fragment_length);
      result |= static_cast<RawType>(static_cast<RawType>(fragment) << offset);
      offset += fragment_length;

      if (offset >= length_) {
        break;
      }

      fragment_length = std::min(length_ - offset, bits_per_byte);
      fragment = *(data_it--);
    }

    return FinalizeGet(result);
  }

  /**
  ** \brief Gets raw signal value (overload for little endian / Intel byte order).
  **
  ** \param[in] data Data containing the signal.
  **
  ** \return Raw signal value if successfully obtained.
  */
  ara::core::Optional<RawType> GetValue(std::true_type /*unused*/, ConstDataSpan data) const {
    constexpr auto bits_per_byte = BitCount{CHAR_BIT};
    const auto bit_offset = start_ % bits_per_byte;
    const auto byte_offset = start_ / bits_per_byte;
    const auto byte_length = (length_ + bits_per_byte - 1) / bits_per_byte;

    if (data.size() < byte_offset + byte_length) {
      return ara::core::nullopt;
    }

    auto data_it = data.begin() + byte_offset;  // NOLINT
    auto result = RawType{0};

    auto fragment = *(data_it++) >> bit_offset;
    auto fragment_length = std::min(length_, bits_per_byte - bit_offset);

    for (auto offset = BitCount{0};;) {
      fragment &= GetBitMask(fragment_length);
      result |= static_cast<RawType>(static_cast<RawType>(fragment) << offset);
      offset += fragment_length;

      if (offset >= length_) {
        break;
      }

      fragment_length = std::min(length_ - offset, bits_per_byte);
      fragment = *(data_it++);
    }

    return FinalizeGet(result);
  }

  /**
  ** \brief Sets raw signal value (overload for big endian / Motorola byte order).
  **
  ** \param[in] value Raw signal value.
  ** \param[in] data  Data to be set.
  **
  ** \return True if data was successfully set.
  */
  bool SetValue(std::false_type /*unused*/, RawType value, DataSpan data) const {
    static_cast<void>(value);
    static_cast<void>(data);

    return false;
  }

  /**
  ** \brief Sets raw signal value (overload for little endian / Intel byte order).
  **
  ** \param[in] value Raw signal value.
  ** \param[in] data  Data to be set.
  **
  ** \return True if data was successfully set.
  */
  bool SetValue(std::true_type /*unused*/, RawType value, DataSpan data) const {
    constexpr auto bits_per_byte = BitCount{CHAR_BIT};
    const auto bit_offset = start_ % bits_per_byte;
    const auto byte_offset = start_ / bits_per_byte;
    const auto byte_length = (length_ + bits_per_byte - 1) / bits_per_byte;

    if (data.size() < byte_offset + byte_length) {
      return false;
    }

    auto data_it = data.begin() + byte_offset;  // NOLINT

    auto fragment_length = std::min(length_, bits_per_byte - bit_offset);
    auto fragment_mask = GetBitMask(fragment_length) << bit_offset;
    auto fragment = static_cast<RawType>(value << bit_offset) & fragment_mask;

    for (auto offset = fragment_length;; offset += fragment_length, data_it++) {
      *data_it &= static_cast<DataSpan::value_type>(~fragment_mask);
      *data_it |= static_cast<DataSpan::value_type>(fragment);

      if (offset >= length_) {
        break;
      }

      fragment_length = std::min(length_ - offset, bits_per_byte);
      fragment_mask = GetBitMask(fragment_length);
      fragment = (value >> offset) & fragment_mask;
    }

    return true;
  }

 private:
  constexpr auto GetBitMask(BitCount bit_count) const {
    return (1U << bit_count) - 1U;
  }

  template <typename R = RawType>
  constexpr R FinalizeGet(std::enable_if_t<std::is_signed<R>::value, R> raw_value) const {
    const auto most_significant_bit = raw_value >> (length_ - 1);
    const auto leading_bits = std::numeric_limits<R>::max() << length_;

    return raw_value | static_cast<R>(most_significant_bit * leading_bits);
  }

  template <typename R = RawType>
  constexpr R FinalizeGet(std::enable_if_t<std::is_unsigned<R>::value, R> raw_value) const {
    return raw_value;
  }

  BitCount start_;
  BitCount length_;
};

/**
** \brief Declaration of CAN signal class.
**
** \tparam T            Desired raw value type.
** \tparam LittleEndian Indicates if signal has little endian byte order.
*/
template <typename T, bool LittleEndian, typename = void>
class CanSignal;

/**
** \brief Specialization of CAN signal for integral types.
**
** \tparam T            Desired raw value type.
** \tparam LittleEndian Indicates if signal has little endian byte order.
*/
template <typename T, bool LittleEndian>
class CanSignal<T, LittleEndian, typename std::enable_if_t<std::is_integral<T>::value>>
    : protected CanSignalBase<T> {
  using Base = CanSignalBase<T>;

 public:
  /**
  ** \brief Alias to physical value type.
  */
  using PhysicalType = double;

  /**
  ** \brief Forward raw value type.
  */
  using RawType = typename Base::RawType;

  /**
  ** \brief Forward bit count type.
  */
  using BitCount = typename Base::BitCount;

  /**
  ** \brief Forward constant data span type.
  */
  using ConstDataSpan = typename Base::ConstDataSpan;

  /**
  ** \brief Forward data span type.
  */
  using DataSpan = typename Base::DataSpan;

  /**
  ** \brief Class constructor.
  **
  ** \param[in] start  Signal start bit.
  ** \param[in] length Signal length in bits.
  */
  constexpr CanSignal(BitCount start, BitCount length) : Base{start, length} {}

  /**
  ** \brief Class constructor.
  **
  ** \param[in] start  Signal start bit.
  ** \param[in] length Signal length in bits.
  ** \param[in] factor Physical value scaling factor.
  ** \param[in] offset Physical value scaling offset.
  */
  constexpr CanSignal(BitCount start, BitCount length, PhysicalType factor, PhysicalType offset)
      : Base{start, length}, factor_{factor}, offset_{offset} {}

  /**
  ** \brief Class constructor.
  **
  ** \param[in] start  Signal start bit.
  ** \param[in] length Signal length in bits.
  ** \param[in] factor Physical value scaling factor.
  ** \param[in] offset Physical value scaling offset.
  ** \param[in] min    Minimum valid physical value.
  ** \param[in] max    Maximum valid physical value.
  */
  constexpr CanSignal(BitCount start, BitCount length, PhysicalType factor, PhysicalType offset,
                      PhysicalType min, PhysicalType max)
      : Base{start, length}, factor_{factor}, offset_{offset}, range_{min < max, min, max} {}

  /**
  ** \brief Gets raw signal value from message payload.
  **
  ** \param[in] data Message data containing the signal value.
  **
  ** \return Raw signal value if successfully obtained.
  */
  ara::core::Optional<RawType> GetRawValue(ConstDataSpan data) const {
    return Base::GetValue(std::integral_constant<bool, LittleEndian>{}, data);
  }

  /**
  ** \brief Gets raw signal value from physical signal value.
  **
  ** \param[in] physical_value Physical signal value.
  ** \param[in] validate       Indicates if physical signal value should be validated.
  **
  ** \return Raw signal value.
  */
  ara::core::Optional<RawType> GetRawValue(PhysicalType physical_value,
                                           bool validate = true) const {
    if (!validate || Validate(physical_value)) {
      return static_cast<RawType>((physical_value - offset_) / factor_);
    }

    return ara::core::nullopt;
  }

  /**
  ** \brief Gets physical signal value from message payload.
  **
  ** \param[in] data     Message data containing the signal value.
  ** \param[in] validate Indicates if physical signal value should be validated.
  **
  ** \return Physical signal value if successfully obtained.
  */
  ara::core::Optional<PhysicalType> GetPhysicalValue(ConstDataSpan data,
                                                     bool validate = true) const {
    auto raw_value = GetRawValue(data);

    return raw_value ? GetPhysicalValue(*raw_value, validate) : ara::core::nullopt;
  }

  /**
  ** \brief Gets physical signal value from raw signal value.
  **
  ** \param[in] raw_value Raw signal value.
  ** \param[in] validate  Indicates if physical signal value should be validated.
  **
  ** \return Physical signal value.
  */
  ara::core::Optional<PhysicalType> GetPhysicalValue(RawType raw_value,
                                                     bool validate = true) const {
    auto physical_value = (static_cast<PhysicalType>(raw_value) * factor_) + offset_;

    if (!validate || Validate(physical_value)) {
      return physical_value;
    }

    return ara::core::nullopt;
  }

  /**
  ** \brief Sets raw signal value in message payload.
  **
  ** \param[in] value Raw signal value.
  ** \param[in] data  Message data to be set.
  **
  ** \return True if data was successfully set.
  */
  bool SetRawValue(RawType value, DataSpan data) const {
    return Base::SetValue(std::integral_constant<bool, LittleEndian>{}, value, data);
  }

  /**
  ** \brief Sets physical signal value in message payload.
  **
  ** \param[in] value    Physical signal value.
  ** \param[in] data     Message data to be set.
  ** \param[in] validate Indicates if physical signal value should be validated.
  **
  ** \return True if data was successfully set.
  */
  bool SetPhysicalValue(PhysicalType value, DataSpan data, bool validate = true) const {
    auto raw_value = GetRawValue(value, validate);

    return raw_value && SetRawValue(*raw_value, data);
  }

  /**
  ** \brief Checks if given physical signal value is within operating range.
  **
  ** \param[in] physical_value Physical signal value.
  **
  ** \return True if physical value is within operating range.
  */
  constexpr bool Validate(PhysicalType physical_value) const {
    return !std::get<0>(range_) ||
           ((std::get<1>(range_) <= physical_value) && (std::get<2>(range_) >= physical_value));
  }

 private:
  using RangeType = std::tuple<bool, PhysicalType, PhysicalType>;

  PhysicalType factor_{1.0};
  PhysicalType offset_{0.0};
  RangeType range_{};
};

/**
** \brief Specialization of CAN signal for enumeration types.
**
** \tparam T            Desired raw value type.
** \tparam LittleEndian Indicates if signal has little endian byte order.
*/
template <typename T, bool LittleEndian>
class CanSignal<T, LittleEndian, typename std::enable_if_t<std::is_enum<T>::value>>
    : protected CanSignalBase<std::underlying_type_t<T>> {
  using Base = CanSignalBase<std::underlying_type_t<T>>;

 public:
  /**
  ** \brief Alias to enumeration value type.
  */
  using EnumType = T;

  /**
  ** \brief Forward raw value type.
  */
  using RawType = typename Base::RawType;

  /**
  ** \brief Forward bit count type.
  */
  using BitCount = typename Base::BitCount;

  /**
  ** \brief Forward constant data span type.
  */
  using ConstDataSpan = typename Base::ConstDataSpan;

  /**
  ** \brief Forward data span type.
  */
  using DataSpan = typename Base::DataSpan;

  /**
  ** \brief Class constructor.
  **
  ** \param[in] start  Signal start bit.
  ** \param[in] length Signal length in bits.
  */
  constexpr CanSignal(BitCount start, BitCount length) : Base{start, length} {}

  /**
  ** \brief Gets raw signal value.
  **
  ** \param[in] data Data containing the signal.
  **
  ** \return Raw signal value if successfully obtained.
  */
  ara::core::Optional<RawType> GetRawValue(ConstDataSpan data) const {
    return Base::GetValue(std::integral_constant<bool, LittleEndian>{}, data);
  }

  /**
  ** \brief Gets enumeration signal value.
  **
  ** \param[in] data Data containing the signal.
  **
  ** \return Enum signal value if successfully obtained.
  */
  ara::core::Optional<EnumType> GetEnumValue(ConstDataSpan data) const {
    auto raw_value = GetRawValue(data);

    if (raw_value) {
      return static_cast<EnumType>(*raw_value);
    }

    return ara::core::nullopt;
  }

  /**
  ** \brief Sets raw signal value in message payload.
  **
  ** \param[in] value Raw signal value.
  ** \param[in] data  Message data to be set.
  **
  ** \return True if data was successfully set.
  */
  bool SetRawValue(RawType value, DataSpan data) const {
    return Base::SetValue(std::integral_constant<bool, LittleEndian>{}, value, data);
  }

  /**
  ** \brief Sets enumeration signal value.
  **
  ** \param[in] value Enumeration signal value.
  ** \param[in] data  Message data to be set.
  **
  ** \return True if data was successfully set.
  */
  bool SetEnumValue(EnumType value, DataSpan data) const {
    return SetRawValue(static_cast<RawType>(value), data);
  }
};

/**
** \brief Alias to CAN signal with big endian byte order.
**
** \tparam T Desired raw value type.
*/
template <typename T>
using CanSignalBe = CanSignal<T, false>;

/**
** \brief Alias to CAN signal with little endian byte order.
**
** \tparam T Desired raw value type.
*/
template <typename T>
using CanSignalLe = CanSignal<T, true>;

}  // namespace util

#endif  // INCLUDE_UTIL_CAN_SIGNAL_H_

/************************************************************************************/
