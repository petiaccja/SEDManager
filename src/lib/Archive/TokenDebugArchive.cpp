#include "TokenDebugArchive.hpp"


static std::string_view GetTagName(eTag tag) {
    switch (tag) {
        case eTag::TINY_ATOM: return "TINY_ATOM";
        case eTag::SHORT_ATOM: return "SHORT_ATOM";
        case eTag::MEDIUM_ATOM: return "MEDIUM_ATOM";
        case eTag::LONG_ATOM: return "LONG_ATOM";
        case eTag::START_LIST: return "START_LIST";
        case eTag::END_LIST: return "END_LIST";
        case eTag::START_NAME: return "START_NAME";
        case eTag::END_NAME: return "END_NAME";
        case eTag::CALL: return "CALL";
        case eTag::END_OF_DATA: return "END_OF_DATA";
        case eTag::END_OF_SESSION: return "END_OF_SESSION";
        case eTag::START_TRANSACTION: return "START_TRANSACTION";
        case eTag::END_TRANSACTION: return "END_TRANSACTION";
        case eTag::EMPTY: return "EMPTY";
    }
    return "EMPTY";
}


static std::string MakeIndentation(intptr_t indentation) {
    if (indentation >= 0) {
        return std::string(indentation, ' ');
    }
    else {
        return "- ";
    }
}

TokenDebugArchive::TokenDebugArchive(std::ostream& stream)
    : cereal::OutputArchive<TokenDebugArchive>(this), m_stream(stream) {}


void TokenDebugArchive::Insert(const Token& token) {
    if (token.tag == eTag::TINY_ATOM
        || token.tag == eTag::SHORT_ATOM
        || token.tag == eTag::MEDIUM_ATOM
        || token.tag == eTag::LONG_ATOM) {
        std::string dataRep;
        std::string dataType;
        if (token.isByte) {
            dataRep += "[ ";
            const size_t maxSize = name ? 60 : 32;
            for (size_t idx = 0; idx < std::min(maxSize, token.data.size()); ++idx) {
                name
                    ? dataRep += std::bit_cast<char>(token.data[idx])
                    : dataRep += std::format("{:02X} ", uint8_t(token.data[idx]));
            }
            if (token.data.size() > maxSize) {
                dataRep += "... ";
            }
            dataRep += " ]";
            dataType = std::format("b{}", token.data.size());
        }
        else {
            dataRep += "0x";
            for (auto byte : token.data) {
                dataRep += std::format("{:02x}", uint8_t(byte));
            }
            dataType = std::format("{}{}", token.isSigned ? "i" : "u", token.data.size() * 8);
        }
        m_stream << MakeIndentation(indentation)
                 << std::format("{} : {} : {}", GetTagName(token.tag), dataType, dataRep);
    }
    else if (token.tag == eTag::START_LIST || token.tag == eTag::START_NAME) {
        m_stream << MakeIndentation(indentation) << GetTagName(token.tag);
        indentation += 4;
    }
    else if (token.tag == eTag::END_LIST || token.tag == eTag::END_NAME) {
        indentation -= 4;
        m_stream << MakeIndentation(indentation) << GetTagName(token.tag);
    }
    else {
        m_stream << MakeIndentation(indentation) << GetTagName(token.tag);
    }
    m_stream << std::endl;
    name = token.tag == eTag::START_NAME;
}


void CEREAL_SAVE_FUNCTION_NAME(TokenDebugArchive& ar, const Token& token) {
    ar.Insert(token);
}