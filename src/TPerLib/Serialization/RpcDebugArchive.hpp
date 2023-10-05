#pragma once

#include "../Structures/Rpc.hpp"
#include "FlatBinaryArchive.hpp"

#include <cereal/cereal.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <format>
#include <iostream>
#include <ranges>
#include <span>


class RpcDebugArchive : public cereal::OutputArchive<RpcDebugArchive> {
public:
    RpcDebugArchive(std::ostream& stream);
    RpcDebugArchive(RpcDebugArchive&&) = delete;
    RpcDebugArchive(const RpcDebugArchive&) = delete;
    RpcDebugArchive& operator=(RpcDebugArchive&&) = delete;
    RpcDebugArchive& operator=(const RpcDebugArchive&) = delete;

    void Insert(const Token& token);

private:
    std::ostream& m_stream;
    intptr_t indentation = 0;
    bool name = false;
};


void CEREAL_SAVE_FUNCTION_NAME(RpcDebugArchive& ar, const Token& token);


template <std::integral T>
void CEREAL_SAVE_FUNCTION_NAME(RpcDebugArchive& ar, const T& t) {
    // Store the value as a short atom.
    constexpr auto tag = eTag::SHORT_ATOM;
    const uint8_t length = sizeof(T);
    constexpr uint8_t isByte = 0;
    constexpr uint8_t isSigned = std::is_signed_v<T>;
    const auto bytes = ToFlatBinary(t);

    CEREAL_SAVE_FUNCTION_NAME(ar, Token{ tag, isByte, isSigned, { bytes.begin(), bytes.end() } });
}


template <std::floating_point T>
void CEREAL_SAVE_FUNCTION_NAME(RpcDebugArchive& ar, const T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(t) == 4, uint32_t, uint64_t>;
    const auto integral = std::bit_cast<UnsignedType>(t);
    CEREAL_SAVE_FUNCTION_NAME(ar, integral);
}
