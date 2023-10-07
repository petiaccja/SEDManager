#include "MethodArgs.hpp"


TokenStream SerializeArg(const std::string& arg) {
    return SerializeArg(std::string_view(arg));
}


TokenStream SerializeArg(std::string_view arg) {
    return { TokenStream::bytes, arg };
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