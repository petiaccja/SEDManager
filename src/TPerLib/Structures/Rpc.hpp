#pragma once

#include <cereal/types/vector.hpp>

#include <cstdint>
#include <vector>


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
    eTag tag;
    bool isByte;
    bool isSigned;
    std::vector<uint8_t> data;
};


eTag GetTagForData(size_t dataLength);


template <class Archive>
void save(Archive& ar, const Token& token) {
    ar(cereal::make_nvp("tag", static_cast<uint8_t>(token.tag)));
    ar(cereal::make_nvp("is_bytes", token.isByte));
    ar(cereal::make_nvp("is_signed", token.isSigned));
    ar(cereal::make_nvp("data", token.data));
}


template <class Archive>
void load(Archive& ar, Token& token) {
    uint8_t tag = 0;
    ar(cereal::make_nvp("tag", tag));
    token.tag = static_cast<eTag>(tag);
    ar(cereal::make_nvp("is_bytes", token.isByte));
    ar(cereal::make_nvp("is_signed", token.isSigned));
    ar(cereal::make_nvp("data", token.data));
}