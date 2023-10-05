#include "TokenStreamArchive.hpp"


uint8_t GetTag(size_t dataLength) {
    if (dataLength <= 15) {
        return SHORT_TAG;
    }
    else if (dataLength <= 2047) {
        return MEDIUM_TAG;
    }
    else if (dataLength <= 16'777'215) {
        return LONG_TAG;
    }
    else {
        throw std::invalid_argument("data length too large to fit into any atom");
    }
}


TokenStreamInputArchive::TokenStreamInputArchive(std::istream& stream)
    : cereal::InputArchive<TokenStreamInputArchive>(this),
      m_stream(stream) {}


TokenStreamOutputArchive::TokenStreamOutputArchive(std::ostream& stream)
    : cereal::OutputArchive<TokenStreamOutputArchive>(this),
      m_stream(stream) {}


void CEREAL_SAVE_FUNCTION_NAME(TokenStreamOutputArchive& ar, const Token& token) {
    ar.Insert(token);
}

void CEREAL_LOAD_FUNCTION_NAME(TokenStreamInputArchive& ar, Token& token) {
    ar.Extract(token);
}

void TokenStreamOutputArchive::Insert(const Token& token) {
    if (token.tag == TINY_TAG) {
        if (token.data.size() != 1) {
            throw std::invalid_argument("tiny atom must have a data length of 1 byte");
        }
        const uint8_t header = (token.isSigned << 6) | (token.data[0] & 0b0001'1111u) | ((token.data[0] & 0x80) >> 2);
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
    }
    else if (token.tag == SHORT_TAG) {
        const size_t length = token.data.size() & 0b1111u;
        const uint8_t header = token.tag
                               | (token.isByte << 5)
                               | (token.isSigned << 4)
                               | length;
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
        Insert(std::span<const uint8_t>(token.data));
    }
    else if (token.tag == MEDIUM_TAG) {
        const size_t length = token.data.size() & 0b111'1111'1111u;
        const uint8_t header = token.tag
                               | (token.isByte << 4)
                               | (token.isSigned << 3)
                               | (length >> 8);
        const uint8_t lengthLsb = uint8_t(length);
        Insert(std::span<const uint8_t, 1>{ &header, 1 });
        Insert(std::span<const uint8_t, 1>{ &lengthLsb, 1 });
        Insert(std::span<const uint8_t>(token.data));
    }
    else if (token.tag == LONG_TAG) {
        const size_t length = token.data.size() & 0x00FF'FFFFu;
        const uint8_t header = token.tag
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
        Insert(std::span<const uint8_t, 1>{ &token.tag, 1 });
        Insert(std::span<const uint8_t>(token.data));
    }
}


void TokenStreamInputArchive::Extract(Token& token) {
    uint8_t header;
    Extract(std::span<uint8_t, 1>(&header, 1));

    if ((header & TINY_MASK) == TINY_TAG) {
        bool isSigned = bool(header >> 6);
        const uint8_t bits = header & 0b0011'1111u;
        const auto signbit = ((bits >> 5) & 1);
        const uint8_t data = isSigned
                                 ? uint8_t(bits | signbit * 0b1110'0000u)
                                 : bits;
        token.tag = TINY_TAG;
        token.isSigned = isSigned;
        token.isByte = false;
        token.data = { data };
    }
    else if ((header & SHORT_MASK) == SHORT_TAG) {
        const uint8_t isByte = bool((header >> 5) & 1u);
        const uint8_t isSigned = bool((header >> 4) & 1u);
        const size_t length = header & 0b1111;
        token.tag = SHORT_TAG;
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(std::span{ token.data });
    }
    else if ((header & MEDIUM_MASK) == MEDIUM_TAG) {
        const uint8_t isByte = bool((header >> 4) & 1u);
        const uint8_t isSigned = bool((header >> 3) & 1u);
        const size_t lengthMsb = header & 0b0111;
        uint8_t lengthLsb = 0;
        Extract(std::span{ &lengthLsb, 1 });
        const size_t length = lengthMsb << 8 | lengthLsb;
        token.tag = MEDIUM_TAG;
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(std::span{ token.data });
    }
    else if ((header & LONG_MASK) == LONG_TAG) {
        const uint8_t isByte = bool((header >> 1) & 1u);
        const uint8_t isSigned = bool(header & 1u);
        std::array<uint8_t, 3> lengthBytes;
        Extract(std::span{ lengthBytes });
        const size_t length = (lengthBytes[0] << 16) | (lengthBytes[1] << 8) | lengthBytes[2];
        token.tag = LONG_TAG;
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(std::span{ token.data });
    }
    else {
        token.tag = header;
        token.data = {};
    }
}


uint8_t TokenStreamInputArchive::PeekTag() const {
    const uint8_t header = Peek();
    if ((header & TINY_MASK) == TINY_TAG) {
        return TINY_TAG;
    }
    else if ((header & SHORT_MASK) == SHORT_TAG) {
        return SHORT_TAG;
    }
    else if ((header & MEDIUM_MASK) == MEDIUM_TAG) {
        return MEDIUM_TAG;
    }
    else if ((header & LONG_MASK) == LONG_TAG) {
        return LONG_TAG;
    }
    else {
        return header;
    }
}