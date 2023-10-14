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
#include <stack>


class TokenTextArchive : public cereal::OutputArchive<TokenTextArchive>, cereal::traits::TextArchive {
public:
    TokenTextArchive(std::ostream& stream);
    TokenTextArchive(TokenTextArchive&&) = delete;
    TokenTextArchive(const TokenTextArchive&) = delete;
    TokenTextArchive& operator=(TokenTextArchive&&) = delete;
    TokenTextArchive& operator=(const TokenTextArchive&) = delete;

    void Insert(const Token& token);

private:
    std::ostream& m_stream;
    intptr_t m_indentation = 0;    
};


void CEREAL_SAVE_FUNCTION_NAME(TokenTextArchive& ar, const Token& token);


template <std::integral T>
void CEREAL_SAVE_FUNCTION_NAME(TokenTextArchive& ar, const T& t) {
    // Store the value as a short atom.
    constexpr auto tag = eTag::SHORT_ATOM;
    const uint8_t length = sizeof(T);
    constexpr uint8_t isByte = 0;
    constexpr uint8_t isSigned = std::is_signed_v<T>;
    const auto bytes = ToFlatBinary(t);

    CEREAL_SAVE_FUNCTION_NAME(ar, Token{ tag, isByte, isSigned, { bytes.begin(), bytes.end() } });
}


template <std::floating_point T>
void CEREAL_SAVE_FUNCTION_NAME(TokenTextArchive& ar, const T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(t) == 4, uint32_t, uint64_t>;
    const auto integral = std::bit_cast<UnsignedType>(t);
    CEREAL_SAVE_FUNCTION_NAME(ar, integral);
}
