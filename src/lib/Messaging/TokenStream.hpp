#pragma once

#include "Token.hpp"

#include <Archive/Serialization.hpp>


namespace sedmgr {

struct TokenStream {
    std::vector<Token> stream;
};

TokenStream SurroundWithList(TokenStream stream);
TokenStream UnSurroundWithList(TokenStream stream);

std::vector<std::byte> Serialize(const Token& token);
std::pair<Token, std::span<const std::byte>> DeSerialize(Serialized<Token> bytes);

std::vector<std::byte> Serialize(const TokenStream& tokenStream);
TokenStream DeSerialize(Serialized<TokenStream> bytes);

} // namespace sedmgr