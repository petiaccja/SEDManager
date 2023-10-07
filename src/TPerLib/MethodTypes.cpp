#include "MethodTypes.hpp"


RpcStream SerializeArg(const std::string& arg) {
    return SerializeArg(std::string_view(arg));
}


RpcStream SerializeArg(std::string_view arg) {
    return { RpcStream::bytes, arg };
}


void ParseArg(const RpcStream& stream, std::string& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    arg = std::string(stream.Get<std::string_view>());
}


void ParseArg(const RpcStream& stream, std::vector<std::byte>& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    auto bytes = stream.Get<std::span<const std::byte>>();
    arg = { bytes.begin(), bytes.end() };
}