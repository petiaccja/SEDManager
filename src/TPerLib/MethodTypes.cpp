#include "MethodTypes.hpp"


RpcStream SerializeArg(const std::string& arg) {
    return { RpcStream::bytes, arg };
}


void ParseArg(const RpcStream& stream, std::string& arg) {
    if (!stream.IsBytes()) {
        throw std::invalid_argument("expected bytes");
    }
    arg = std::string(stream.Get<std::string_view>());
}