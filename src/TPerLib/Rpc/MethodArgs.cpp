#include "MethodArgs.hpp"

#include "../Serialization/Utility.hpp"


TokenStream SerializeArg(const std::string& arg) {
    return SerializeArg(std::string_view(arg));
}


TokenStream SerializeArg(std::string_view arg) {
    return { TokenStream::bytes, arg };
}


TokenStream SerializeArg(const Uid& arg) {
    return { TokenStream::bytes, ToBytes(arg.value) };
}


void ParseArg(const TokenStream& stream, Uid& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    const auto bytes = stream.Get<std::span<const uint8_t>>();
    if (bytes.size() != 8) {
        throw std::invalid_argument("a UID is 8 bytes");
    }
    uint64_t value;
    FromBytes(bytes, value);
    arg = value;
}


void ParseArg(const TokenStream& stream, std::string& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    arg = std::string(stream.Get<std::string_view>());
}


void ParseArg(const TokenStream& stream, std::vector<std::byte>& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    auto bytes = stream.Get<std::span<const std::byte>>();
    arg = { bytes.begin(), bytes.end() };
}