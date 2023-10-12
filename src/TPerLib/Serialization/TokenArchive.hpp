#pragma once

#include "../Communication/Token.hpp"
#include "FlatBinaryArchive.hpp"

#include <cereal/cereal.hpp>

#include <array>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <format>
#include <iostream>
#include <ranges>
#include <span>



class TokenInputArchive : public cereal::InputArchive<TokenInputArchive> {
public:
    TokenInputArchive(std::istream& stream);
    TokenInputArchive(TokenInputArchive&&) = delete;
    TokenInputArchive(const TokenInputArchive&) = delete;
    TokenInputArchive& operator=(TokenInputArchive&&) = delete;
    TokenInputArchive& operator=(const TokenInputArchive&) = delete;

    void Extract(Token& token);
    eTag PeekTag() const;

protected:
    template <size_t N>
    void Extract(std::span<std::byte, N> bytes) {
        const auto numExtracted = m_stream.rdbuf()->sgetn(reinterpret_cast<char*>(bytes.data()), bytes.size());
        if (numExtracted != bytes.size()) {
            throw std::invalid_argument(
                std::format("failed to extract {} bytes, extracted {}", bytes.size(), numExtracted));
        }
    }

    std::byte Peek() const {
        if (!m_stream.good()) {
            throw std::invalid_argument(
                std::format("failed to extract {} bytes, extracted {}", 1, 0));
        }
        return std::bit_cast<std::byte>(std::istream::char_type(m_stream.rdbuf()->sgetc()));
    }

private:
    std::istream& m_stream;
};


class TokenOutputArchive : public cereal::OutputArchive<TokenOutputArchive> {
public:
    TokenOutputArchive(std::ostream& stream);
    TokenOutputArchive(TokenOutputArchive&&) = delete;
    TokenOutputArchive(const TokenOutputArchive&) = delete;
    TokenOutputArchive& operator=(TokenOutputArchive&&) = delete;
    TokenOutputArchive& operator=(const TokenOutputArchive&) = delete;

    void Insert(const Token& token);


protected:
    template <size_t N>
    void Insert(std::span<const std::byte, N> bytes) {
        const auto numWritten = m_stream.rdbuf()->sputn(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        assert(numWritten == bytes.size());
    }

private:
    std::ostream& m_stream;
};


void CEREAL_SAVE_FUNCTION_NAME(TokenOutputArchive& ar, const Token& token);
void CEREAL_LOAD_FUNCTION_NAME(TokenInputArchive& ar, Token& t);


template <std::integral T>
void CEREAL_SAVE_FUNCTION_NAME(TokenOutputArchive& ar, const T& t) {
    // Store the value as a short atom.
    constexpr auto tag = eTag::SHORT_ATOM;
    const uint8_t length = sizeof(T);
    constexpr uint8_t isByte = 0;
    constexpr uint8_t isSigned = std::is_signed_v<T>;
    const auto bytes = ToFlatBinary(t);

    CEREAL_SAVE_FUNCTION_NAME(ar, Token{ tag, isByte, isSigned, { bytes.begin(), bytes.end() } });
}


template <std::integral T>
void CEREAL_LOAD_FUNCTION_NAME(TokenInputArchive& ar, T& t) {
    Token token;
    CEREAL_LOAD_FUNCTION_NAME(ar, token);

    if (token.isByte) {
        throw std::invalid_argument("token contains a byte stream, not an integer");
    }
    if (token.isSigned != std::is_signed_v<T>) {
        throw std::invalid_argument("token's signedness does not match integer's signedness");
    }
    if (token.data.size() != sizeof(T)) {
        throw std::invalid_argument("token does not contain the right number of bytes for integer");
    }

    t = FromFlatBinary<T>(std::span<std::byte, sizeof(T)>{ token.data });
}


template <std::floating_point T>
void CEREAL_SAVE_FUNCTION_NAME(TokenOutputArchive& ar, const T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(t) == 4, uint32_t, uint64_t>;
    const auto integral = std::bit_cast<UnsignedType>(t);
    CEREAL_SAVE_FUNCTION_NAME(ar, integral);
}


template <std::floating_point T>
void CEREAL_LOAD_FUNCTION_NAME(TokenInputArchive& ar, T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(T) == 4, uint32_t, uint64_t>;
    UnsignedType integral = 0;
    CEREAL_LOAD_FUNCTION_NAME(ar, integral);
    t = std::bit_cast<T>(integral);
}
