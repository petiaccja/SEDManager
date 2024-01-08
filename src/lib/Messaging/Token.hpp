#pragma once

#include <cereal/types/vector.hpp>

#include <concepts>
#include <cstdint>
#include <span>
#include <vector>


namespace sedmgr {

enum class eTag : uint8_t {
    TINY_ATOM = 0b0000'0000u,
    SHORT_ATOM = 0b1000'0000u,
    MEDIUM_ATOM = 0b1100'0000u,
    LONG_ATOM = 0b1110'0000u,
    START_LIST = 0xF0,
    END_LIST = 0xF1,
    START_NAME = 0xF2,
    END_NAME = 0xF3,
    CALL = 0xF8,
    END_OF_DATA = 0xF9,
    END_OF_SESSION = 0xFA,
    START_TRANSACTION = 0xFB,
    END_TRANSACTION = 0xFC,
    EMPTY = 0xFF,
};


enum class eMask : uint8_t {
    TINY_ATOM = 0b1000'0000u,
    SHORT_ATOM = 0b1100'0000u,
    MEDIUM_ATOM = 0b1110'0000u,
    LONG_ATOM = 0b1111'1000u,
};


struct Token {
    eTag tag = eTag::EMPTY;
    bool isByte = false;
    bool isSigned = false;
    std::vector<std::byte> data = {};

    auto operator<=>(const Token&) const noexcept = default;

    bool IsAtom() const;
    static eTag GetTag(size_t dataLength);
};

template <class T>
struct Tokenized {
    std::span<const Token> tokens;
};

std::vector<Token> Tokenize(const Token& token);
std::pair<Token, std::span<const Token>> DeTokenize(Tokenized<Token> tokens);


template <class T>
concept Tokenizable = requires(const T& value, Tokenized<T> tokens) {
    {
        Tokenize(value)
    } -> std::same_as<std::template vector<Token>>;
    {
        DeTokenize(tokens)
    } -> std::same_as<std::pair<T, std::span<const Token>>>;
};


static_assert(Tokenizable<Token>);


} // namespace sedmgr