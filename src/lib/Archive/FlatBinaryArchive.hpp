#pragma once

#include <cereal/cereal.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <format>
#include <iostream>
#include <ranges>
#include <span>


namespace sedmgr {

class FlatBinaryInputArchive : public cereal::InputArchive<FlatBinaryInputArchive> {
public:
    FlatBinaryInputArchive(std::istream& stream);
    FlatBinaryInputArchive(FlatBinaryInputArchive&&) = delete;
    FlatBinaryInputArchive(const FlatBinaryInputArchive&) = delete;
    FlatBinaryInputArchive& operator=(FlatBinaryInputArchive&&) = delete;
    FlatBinaryInputArchive& operator=(const FlatBinaryInputArchive&) = delete;

    template <size_t N>
    void Extract(std::span<std::byte, N> bytes) {
        const auto numExtracted = m_stream.rdbuf()->sgetn(reinterpret_cast<char*>(bytes.data()), bytes.size());
        if (numExtracted != bytes.size()) {
            throw std::invalid_argument(
                std::format("failed to extract {} bytes, extracted {}", bytes.size(), numExtracted));
        }
    }

private:
    std::istream& m_stream;
};


class FlatBinaryOutputArchive : public cereal::OutputArchive<FlatBinaryOutputArchive> {
public:
    FlatBinaryOutputArchive(std::ostream& stream);
    FlatBinaryOutputArchive(FlatBinaryOutputArchive&&) = delete;
    FlatBinaryOutputArchive(const FlatBinaryOutputArchive&) = delete;
    FlatBinaryOutputArchive& operator=(FlatBinaryOutputArchive&&) = delete;
    FlatBinaryOutputArchive& operator=(const FlatBinaryOutputArchive&) = delete;

    template <size_t N>
    void Insert(std::span<const std::byte, N> bytes) {
        const auto numWritten = m_stream.rdbuf()->sputn(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }

private:
    std::ostream& m_stream;
};


template <std::integral T>
auto ToFlatBinary(const T& t) {
    using UnsignedType = std::make_unsigned_t<T>;

    std::array<std::byte, sizeof(T)> bytes;
    auto us = std::bit_cast<UnsignedType>(t);
    for (auto& byte : std::views::reverse(bytes)) {
        byte = static_cast<std::byte>(uint8_t(us));
        constexpr auto shift = sizeof(T) > 1 ? 8 : 1; // To avoid warnings about shift being larger than type itself.
        us >>= shift;
    }

    return bytes;
}


template <std::integral T>
T FromFlatBinary(std::span<std::byte, sizeof(T)> bytes) {
    using UnsignedType = std::make_unsigned_t<T>;

    UnsignedType us = 0;
    for (auto& byte : bytes) {
        constexpr auto shift = sizeof(T) > 1 ? 8 : 1; // To avoid warnings about shift being larger than type itself.
        us <<= shift;
        us |= uint8_t(byte);
    }

    return std::bit_cast<T>(us);
}


template <>
inline auto ToFlatBinary<bool>(const bool& t) {
    return std::array{ static_cast<std::byte>(t) };
}


template <>
inline bool FromFlatBinary<bool>(std::span<std::byte, 1> bytes) {
    return static_cast<bool>(bytes[0]);
}


template <std::integral T>
void CEREAL_SAVE_FUNCTION_NAME(FlatBinaryOutputArchive& ar, const T& t) {
    const auto bytes = ToFlatBinary(t);
    ar.Insert(std::span<const std::byte, sizeof(T)>{ bytes });
}


template <std::integral T>
void CEREAL_LOAD_FUNCTION_NAME(FlatBinaryInputArchive& ar, T& t) {
    std::array<std::byte, sizeof(T)> bytes;
    ar.Extract(std::span<std::byte, sizeof(T)>{ bytes });
    t = FromFlatBinary<T>(bytes);
}


template <class T>
    requires std::is_enum_v<T>
void CEREAL_SAVE_FUNCTION_NAME(FlatBinaryOutputArchive& ar, const T& t) {
    CEREAL_SAVE_FUNCTION_NAME(ar, static_cast<std::underlying_type_t<T>>(t));
}


template <class T>
    requires std::is_enum_v<T>
void CEREAL_LOAD_FUNCTION_NAME(FlatBinaryInputArchive& ar, T& t) {
    std::underlying_type_t<T> value;
    CEREAL_LOAD_FUNCTION_NAME(ar, value);
    t = static_cast<T>(value);
}


template <std::floating_point T>
void CEREAL_SAVE_FUNCTION_NAME(FlatBinaryOutputArchive& ar, const T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(t) == 4, uint32_t, uint64_t>;
    const auto integral = std::bit_cast<UnsignedType>(t);
    CEREAL_SAVE_FUNCTION_NAME(ar, integral);
}


template <std::floating_point T>
void CEREAL_LOAD_FUNCTION_NAME(FlatBinaryInputArchive& ar, T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(T) == 4, uint32_t, uint64_t>;
    UnsignedType integral = 0;
    CEREAL_LOAD_FUNCTION_NAME(ar, integral);
    t = std::bit_cast<T>(integral);
}

} // namespace sedmgr