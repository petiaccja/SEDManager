#include "TokenTextArchive.hpp"


static std::string_view GetCmdName(eTag cmd) {
    switch (cmd) {
        case eTag::TINY_ATOM: return "tiny";
        case eTag::SHORT_ATOM: return "short";
        case eTag::MEDIUM_ATOM: return "medium";
        case eTag::LONG_ATOM: return "long";
        case eTag::START_LIST: return "sl";
        case eTag::END_LIST: return "el";
        case eTag::START_NAME: return "sn";
        case eTag::END_NAME: return "en";
        case eTag::CALL: return "call";
        case eTag::END_OF_DATA: return "eod";
        case eTag::END_OF_SESSION: return "eos";
        case eTag::START_TRANSACTION: return "st";
        case eTag::END_TRANSACTION: return "et";
        case eTag::EMPTY: return "empty";
    }
    return "empty";
}


static std::string MakeIndentation(intptr_t indentation) {
    if (indentation >= 0) {
        return std::string(indentation, ' ');
    }
    else {
        return "- ";
    }
}

TokenTextArchive::TokenTextArchive(std::ostream& stream)
    : cereal::OutputArchive<TokenTextArchive>(this), m_stream(stream) {}


static void WriteData(std::ostream& os, const Token& token) {
    os << (token.isByte ? 'b' : (token.isSigned ? 'i' : 'u'));
    os << '"';
    for (auto byte : token.data) {
        os << std::format("{:02x}", uint8_t(byte));
    }
    os << '"';
}


void TokenTextArchive::Insert(const Token& token) {
    if (token.tag == eTag::TINY_ATOM
        || token.tag == eTag::SHORT_ATOM
        || token.tag == eTag::MEDIUM_ATOM
        || token.tag == eTag::LONG_ATOM) {
        m_stream << MakeIndentation(m_indentation);
        WriteData(m_stream, token);
        m_stream << std::endl;
    }
    else if (token.tag == eTag::START_LIST) {
        m_stream << MakeIndentation(m_indentation) << "[" << std::endl;
        m_indentation += 4;
    }
    else if (token.tag == eTag::END_LIST) {
        m_indentation -= 4;
        m_stream << MakeIndentation(m_indentation) << "]" << std::endl;
    }
    else if (token.tag == eTag::START_NAME) {
        m_stream << MakeIndentation(m_indentation) << "{" << std::endl;
        m_indentation += 4;
    }
    else if (token.tag == eTag::END_NAME) {
        m_indentation -= 4;
        m_stream << MakeIndentation(m_indentation) << "}" << std::endl;
    }
    else {
        m_stream << MakeIndentation(m_indentation) << "c\"" << GetCmdName(token.tag) << '"';
    }
}


void CEREAL_SAVE_FUNCTION_NAME(TokenTextArchive& ar, const Token& token) {
    ar.Insert(token);
}