#pragma once

#include "FlatBinaryArchive.hpp"
#include "Token.hpp"

#include <cereal/cereal.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <format>
#include <iostream>
#include <ranges>
#include <span>


namespace sedmgr {

class TokenDebugArchive : public cereal::OutputArchive<TokenDebugArchive>, cereal::traits::TextArchive {
public:
    TokenDebugArchive(std::ostream& stream);
    TokenDebugArchive(TokenDebugArchive&&) = delete;
    TokenDebugArchive(const TokenDebugArchive&) = delete;
    TokenDebugArchive& operator=(TokenDebugArchive&&) = delete;
    TokenDebugArchive& operator=(const TokenDebugArchive&) = delete;

    void Insert(const Token& token);

private:
    std::ostream& m_stream;
    intptr_t indentation = 0;
    bool name = false;
};


void CEREAL_SAVE_FUNCTION_NAME(TokenDebugArchive& ar, const Token& token);


template <std::integral T>
void CEREAL_SAVE_FUNCTION_NAME(TokenDebugArchive& ar, const T& t) {
    // Store the value as a short atom.
    constexpr auto tag = eTag::SHORT_ATOM;
    const uint8_t length = sizeof(T);
    constexpr uint8_t isByte = 0;
    constexpr uint8_t isSigned = std::is_signed_v<T>;
    const auto bytes = ToFlatBinary(t);

    CEREAL_SAVE_FUNCTION_NAME(ar, Token{
                                      tag, isByte, isSigned, {bytes.begin(), bytes.end()}
    });
}


template <std::floating_point T>
void CEREAL_SAVE_FUNCTION_NAME(TokenDebugArchive& ar, const T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(t) == 4, uint32_t, uint64_t>;
    const auto integral = std::bit_cast<UnsignedType>(t);
    CEREAL_SAVE_FUNCTION_NAME(ar, integral);
}

} // namespace sedmgr