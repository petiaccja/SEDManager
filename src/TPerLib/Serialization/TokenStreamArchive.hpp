#pragma once

#include "FlatBinaryArchive.hpp"

#include <cereal/cereal.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <format>
#include <iostream>
#include <ranges>
#include <span>


struct Token {
    uint8_t tag;
    bool isByte;
    bool isSigned;
    std::vector<uint8_t> data;
};


constexpr uint8_t TINY_TAG = 0b0000'0000u;
constexpr uint8_t TINY_MASK = 0b1000'0000u;
constexpr uint8_t SHORT_TAG = 0b1000'0000u;
constexpr uint8_t SHORT_MASK = 0b1100'0000u;
constexpr uint8_t MEDIUM_TAG = 0b1100'0000u;
constexpr uint8_t MEDIUM_MASK = 0b1110'0000u;
constexpr uint8_t LONG_TAG = 0b1110'0000u;
constexpr uint8_t LONG_MASK = 0b1111'1000u;


uint8_t GetTag(size_t dataLength);


class TokenStreamInputArchive : public cereal::InputArchive<TokenStreamInputArchive> {
public:
    TokenStreamInputArchive(std::istream& stream);
    TokenStreamInputArchive(TokenStreamInputArchive&&) = delete;
    TokenStreamInputArchive(const TokenStreamInputArchive&) = delete;
    TokenStreamInputArchive& operator=(TokenStreamInputArchive&&) = delete;
    TokenStreamInputArchive& operator=(const TokenStreamInputArchive&) = delete;

    void Extract(Token& token);
    uint8_t PeekTag() const;

protected:
    template <size_t N>
    void Extract(std::span<uint8_t, N> bytes) {
        size_t numExtracted = 0;
        for (auto& byte : bytes) {
            std::istream::char_type ch = 0;
            m_stream.get(ch);
            if (!m_stream.good()) {
                throw std::invalid_argument(
                    std::format("failed to extract {} bytes, extracted {}", bytes.size(), numExtracted));
            }
            byte = std::bit_cast<uint8_t>(ch);
            numExtracted++;
        }
    }

    uint8_t Peek() const {
        if (!m_stream.good()) {
            throw std::invalid_argument(
                std::format("failed to extract {} bytes, extracted {}", 1, 0));
        }
        return std::bit_cast<uint8_t>(std::istream::char_type(m_stream.peek()));
    }

private:
    std::istream& m_stream;
};


class TokenStreamOutputArchive : public cereal::OutputArchive<TokenStreamOutputArchive> {
public:
    TokenStreamOutputArchive(std::ostream& stream);
    TokenStreamOutputArchive(TokenStreamOutputArchive&&) = delete;
    TokenStreamOutputArchive(const TokenStreamOutputArchive&) = delete;
    TokenStreamOutputArchive& operator=(TokenStreamOutputArchive&&) = delete;
    TokenStreamOutputArchive& operator=(const TokenStreamOutputArchive&) = delete;

    void Insert(const Token& token);


protected:
    template <size_t N>
    void Insert(std::span<const uint8_t, N> bytes) {
        for (auto& byte : bytes) {
            m_stream.put(std::bit_cast<std::ostream::char_type>(byte));
        }
    }

private:
    std::ostream& m_stream;
};


void CEREAL_SAVE_FUNCTION_NAME(TokenStreamOutputArchive& ar, const Token& token);
void CEREAL_LOAD_FUNCTION_NAME(TokenStreamInputArchive& ar, Token& t);


template <std::integral T>
void CEREAL_SAVE_FUNCTION_NAME(TokenStreamOutputArchive& ar, const T& t) {
    // Store the value as a short atom.
    constexpr uint8_t tag = 0b1000'0000u;
    const uint8_t length = sizeof(T);
    constexpr uint8_t isByte = 0;
    constexpr uint8_t isSigned = std::is_signed_v<T>;
    const auto bytes = ToFlatBinary(t);

    CEREAL_SAVE_FUNCTION_NAME(ar, Token{ tag, isByte, isSigned, { bytes.begin(), bytes.end() } });
}


template <std::integral T>
void CEREAL_LOAD_FUNCTION_NAME(TokenStreamInputArchive& ar, T& t) {
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

    t = FromFlatBinary<T>(std::span<uint8_t, sizeof(T)>{ token.data });
}


template <std::floating_point T>
void CEREAL_SAVE_FUNCTION_NAME(TokenStreamOutputArchive& ar, const T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(t) == 4, uint32_t, uint64_t>;
    const auto integral = std::bit_cast<UnsignedType>(t);
    CEREAL_SAVE_FUNCTION_NAME(ar, integral);
}


template <std::floating_point T>
void CEREAL_LOAD_FUNCTION_NAME(TokenStreamInputArchive& ar, T& t) {
    static_assert(!std::is_floating_point<T>::value || (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                  "Portable binary only supports IEEE 754 standardized floating point");
    using UnsignedType = std::conditional<sizeof(T) == 4, uint32_t, uint64_t>;
    UnsignedType integral = 0;
    CEREAL_LOAD_FUNCTION_NAME(ar, integral);
    t = std::bit_cast<T>(integral);
}
