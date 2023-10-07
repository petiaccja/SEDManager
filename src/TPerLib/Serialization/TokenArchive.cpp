#include "TokenArchive.hpp"


eTag GetTagFromHeader(uint8_t header) {
    if ((header & uint8_t(eMask::TINY_ATOM)) == uint8_t(eTag::TINY_ATOM)) {
        return eTag::TINY_ATOM;
    }
    else if ((header & uint8_t(eMask::SHORT_ATOM)) == uint8_t(eTag::SHORT_ATOM)) {
        return eTag::SHORT_ATOM;
    }
    else if ((header & uint8_t(eMask::MEDIUM_ATOM)) == uint8_t(eTag::MEDIUM_ATOM)) {
        return eTag::MEDIUM_ATOM;
    }
    else if ((header & uint8_t(eMask::LONG_ATOM)) == uint8_t(eTag::LONG_ATOM)) {
        return eTag::LONG_ATOM;
    }
    else {
        return static_cast<eTag>(header);
    }
}


TokenInputArchive::TokenInputArchive(std::istream& stream)
    : cereal::InputArchive<TokenInputArchive>(this),
      m_stream(stream) {}


TokenOutputArchive::TokenOutputArchive(std::ostream& stream)
    : cereal::OutputArchive<TokenOutputArchive>(this),
      m_stream(stream) {}


void CEREAL_SAVE_FUNCTION_NAME(TokenOutputArchive& ar, const Token& token) {
    ar.Insert(token);
}

void CEREAL_LOAD_FUNCTION_NAME(TokenInputArchive& ar, Token& token) {
    ar.Extract(token);
}

void TokenOutputArchive::Insert(const Token& token) {
    if (token.tag == eTag::TINY_ATOM) {
        if (token.data.size() != 1) {
            throw std::invalid_argument("tiny atom must have a data length of 1 byte");
        }
        const uint8_t header = (token.isSigned << 6) | (token.data[0] & 0b0001'1111u) | ((token.data[0] & 0x80) >> 2);
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
    }
    else if (token.tag == eTag::SHORT_ATOM) {
        const size_t length = token.data.size() & 0b1111u;
        const uint8_t header = uint8_t(token.tag)
                               | (token.isByte << 5)
                               | (token.isSigned << 4)
                               | length;
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
        Insert(std::span<const uint8_t>(token.data));
    }
    else if (token.tag == eTag::MEDIUM_ATOM) {
        const size_t length = token.data.size() & 0b111'1111'1111u;
        const uint8_t header = uint8_t(token.tag)
                               | (token.isByte << 4)
                               | (token.isSigned << 3)
                               | (length >> 8);
        const uint8_t lengthLsb = uint8_t(length);
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
        Insert(std::span<const uint8_t, 1>{ &lengthLsb, 1 });
        Insert(std::span<const uint8_t>(token.data));
    }
    else if (token.tag == eTag::LONG_ATOM) {
        const size_t length = token.data.size() & 0x00FF'FFFFu;
        const uint8_t header = uint8_t(token.tag)
                               | (token.isByte << 1)
                               | (token.isSigned);
        const std::array<uint8_t, 3> lengthBytes = {
            uint8_t((length >> 16)),
            uint8_t((length >> 8)),
            uint8_t(length),
        };
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
        Insert(std::span{ lengthBytes });
        Insert(std::span<const uint8_t>(token.data));
    }
    else {
        const uint8_t header = uint8_t(token.tag);
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
        Insert(std::span<const uint8_t>(token.data));
    }
}


void TokenInputArchive::Extract(Token& token) {
    uint8_t header;
    Extract(std::span<uint8_t, 1>(&header, 1));
    eTag tag = GetTagFromHeader(header);
    token.tag = tag;

    if (tag == eTag::TINY_ATOM) {
        bool isSigned = bool(header >> 6);
        const uint8_t bits = header & 0b0011'1111u;
        const auto signbit = ((bits >> 5) & 1);
        const uint8_t data = isSigned
                                 ? uint8_t(bits | signbit * 0b1110'0000u)
                                 : bits;
        token.isSigned = isSigned;
        token.isByte = false;
        token.data = { data };
    }
    else if (tag == eTag::SHORT_ATOM) {
        const uint8_t isByte = bool((header >> 5) & 1u);
        const uint8_t isSigned = bool((header >> 4) & 1u);
        const size_t length = header & 0b1111;
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(std::span{ token.data });
    }
    else if (tag == eTag::MEDIUM_ATOM) {
        const uint8_t isByte = bool((header >> 4) & 1u);
        const uint8_t isSigned = bool((header >> 3) & 1u);
        const size_t lengthMsb = header & 0b0111;
        uint8_t lengthLsb = 0;
        Extract(std::span{ &lengthLsb, 1 });
        const size_t length = lengthMsb << 8 | lengthLsb;
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(std::span{ token.data });
    }
    else if (tag == eTag::LONG_ATOM) {
        const uint8_t isByte = bool((header >> 1) & 1u);
        const uint8_t isSigned = bool(header & 1u);
        std::array<uint8_t, 3> lengthBytes;
        Extract(std::span{ lengthBytes });
        const size_t length = (lengthBytes[0] << 16) | (lengthBytes[1] << 8) | lengthBytes[2];
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(std::span{ token.data });
    }
    else {
        token.data = {};
    }
}


eTag TokenInputArchive::PeekTag() const {
    return GetTagFromHeader(Peek());
}