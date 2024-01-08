#include "Token.hpp"


namespace sedmgr {

bool Token::IsAtom() const {
    return tag == eTag::TINY_ATOM
           || tag == eTag::SHORT_ATOM
           || tag == eTag::MEDIUM_ATOM
           || tag == eTag::LONG_ATOM;
}


eTag Token::GetTag(size_t dataLength) {
    if (dataLength <= 15) {
        return eTag::SHORT_ATOM;
    }
    else if (dataLength <= 2047) {
        return eTag::MEDIUM_ATOM;
    }
    else if (dataLength <= 16'777'215) {
        return eTag::LONG_ATOM;
    }
    else {
        throw std::invalid_argument("data length too large to fit into any atom");
    }
}


std::vector<Token> Tokenize(const Token& token) {
    return { token };
}

std::pair<Token, std::span<Token>> DeTokenize(std::span<Token> tokens) {
    if (tokens.empty()) {
        throw std::invalid_argument("expected at least one token");
    }
    return { tokens[0], tokens.subspan(1) };
}

} // namespace sedmgr