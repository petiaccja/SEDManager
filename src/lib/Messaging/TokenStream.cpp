#include "TokenStream.hpp"

#include <format>
#include <array>

namespace sedmgr {

TokenStream SurroundWithList(TokenStream stream) {
    stream.stream.insert(stream.stream.begin(), Token{ .tag = eTag::START_LIST });
    stream.stream.insert(stream.stream.end(), Token{ .tag = eTag::END_LIST });
    return stream;
}


TokenStream UnSurroundWithList(TokenStream stream) {
    if (!stream.stream.empty()
        && stream.stream.front().tag == eTag::START_LIST
        && stream.stream.back().tag == eTag::END_LIST) {
        stream.stream.erase(stream.stream.end() - 1);
        stream.stream.erase(stream.stream.begin());
        return stream;
    }
    throw std::invalid_argument("stream must be a list");
}


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


template <size_t N>
void Insert(std::vector<std::byte>& stream, std::span<const std::byte, N> bytes) {
    stream.insert(stream.end(), bytes.begin(), bytes.end());
}


template <size_t N>
void Extract(std::span<const std::byte>& stream, std::span<std::byte, N> bytes) {
    if (stream.size() < bytes.size()) {
        throw std::invalid_argument(
            std::format("failed to extract {} bytes, extracted {}", bytes.size(), stream.size()));
    }
    std::copy_n(stream.begin(), bytes.size(), bytes.begin());
    stream = stream.subspan(bytes.size());
}


std::vector<std::byte> Serialize(const Token& token) {
    std::vector<std::byte> stream;

    if (token.tag == eTag::TINY_ATOM) {
        if (token.data.size() != 1) {
            throw std::invalid_argument("tiny atom must have a data length of 1 byte");
        }
        const uint8_t header = (token.isSigned << 6) | (uint8_t(token.data[0]) & 0b0001'1111u) | ((uint8_t(token.data[0]) & 0x80) >> 2);
        Insert(stream, std::span{ reinterpret_cast<const std::byte*>(&header), 1 });
    }
    else if (token.tag == eTag::SHORT_ATOM) {
        const size_t length = token.data.size() & 0b1111u;
        const uint8_t header = uint8_t(token.tag)
                               | (token.isByte << 5)
                               | (token.isSigned << 4)
                               | length;
        Insert(stream, std::span{ reinterpret_cast<const std::byte*>(&header), 1 });
        Insert(stream, std::span(token.data));
    }
    else if (token.tag == eTag::MEDIUM_ATOM) {
        const size_t length = token.data.size() & 0b111'1111'1111u;
        const uint8_t header = uint8_t(token.tag)
                               | (token.isByte << 4)
                               | (token.isSigned << 3)
                               | (length >> 8);
        const uint8_t lengthLsb = uint8_t(length);
        Insert(stream, std::span{ reinterpret_cast<const std::byte*>(&header), 1 });
        Insert(stream, std::span{ reinterpret_cast<const std::byte*>(&lengthLsb), 1 });
        Insert(stream, std::span(token.data));
    }
    else if (token.tag == eTag::LONG_ATOM) {
        const size_t length = token.data.size() & 0x00FF'FFFFu;
        const uint8_t header = int(token.tag)
                               | int(token.isByte << 1)
                               | int(token.isSigned);
        const std::array<std::byte, 3> lengthBytes = {
            std::byte(uint8_t(((length >> 16)))),
            std::byte(uint8_t(((length >> 8)))),
            std::byte(uint8_t((length))),
        };
        Insert(stream, std::span{ reinterpret_cast<const std::byte*>(&header), 1 });
        Insert(stream, std::span{ lengthBytes });
        Insert(stream, std::span(token.data));
    }
    else {
        const uint8_t header = uint8_t(token.tag);
        Insert(stream, std::span{ reinterpret_cast<const std::byte*>(&header), 1 });
        Insert(stream, std::span(token.data));
    }

    return stream;
}


std::pair<Token, std::span<const std::byte>> DeSerialize(Serialized<Token> bytes) {
    auto stream = bytes.bytes;

    Token token;
    uint8_t header;
    Extract(stream, std::span(reinterpret_cast<std::byte*>(&header), 1));
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
        token.data = { std::byte(data) };
    }
    else if (tag == eTag::SHORT_ATOM) {
        const uint8_t isByte = bool((header >> 5) & 1u);
        const uint8_t isSigned = bool((header >> 4) & 1u);
        const size_t length = header & 0b1111;
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(stream, std::span{ token.data });
    }
    else if (tag == eTag::MEDIUM_ATOM) {
        const uint8_t isByte = bool((header >> 4) & 1u);
        const uint8_t isSigned = bool((header >> 3) & 1u);
        const size_t lengthMsb = header & 0b0111;
        std::byte lengthLsb;
        Extract(stream, std::span{ &lengthLsb, 1 });
        const size_t length = lengthMsb << 8 | uint8_t(lengthLsb);
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(stream, std::span{ token.data });
    }
    else if (tag == eTag::LONG_ATOM) {
        const uint8_t isByte = bool((header >> 1) & 1u);
        const uint8_t isSigned = bool(header & 1u);
        std::array<std::byte, 3> lengthBytes;
        Extract(stream, std::span{ lengthBytes });
        const size_t length = (uint8_t(lengthBytes[0]) << 16) | (uint8_t(lengthBytes[1]) << 8) | uint8_t(lengthBytes[2]);
        token.isSigned = isSigned;
        token.isByte = isByte;
        token.data.resize(length);
        Extract(stream, std::span{ token.data });
    }
    else {
        token.data = {};
    }

    return { token, stream };
}


std::vector<std::byte> Serialize(const TokenStream& tokenStream) {
    std::vector<std::byte> bytes;
    for (const auto& token : tokenStream.stream) {
        auto tokenBytes = Serialize(token);
        std::copy(std::make_move_iterator(tokenBytes.begin()), std::make_move_iterator(tokenBytes.end()), std::back_inserter(bytes));
    }
    return bytes;
}


TokenStream DeSerialize(Serialized<TokenStream> bytes) {
    auto stream = bytes.bytes;
    std::vector<Token> tokens;
    while (!stream.empty()) {
        auto [token, newStream] = DeSerialize(Serialized<Token>{ stream });
        tokens.push_back(std::move(token));
        stream = newStream;
    }
    return TokenStream{ std::move(tokens) };
}


} // namespace sedmgr