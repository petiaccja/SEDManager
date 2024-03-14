#include "Value.hpp"

#include <regex>


namespace sedmgr {


//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

Value::Value(std::initializer_list<Value> values)
    : m_storage(std::make_shared<StorageType>(List(values))) {}


Value::Value(Named value)
    : m_storage(std::make_shared<StorageType>(std::move(value))) {}


Value::Value(eCommand command)
    : m_storage(std::make_shared<StorageType>(command)) {}


bool Value::operator==(const Value& rhs) const {
    if (HasValue() && rhs.HasValue()) {
        return *m_storage == *rhs.m_storage;
    }
    return !HasValue() && !rhs.HasValue();
}


//------------------------------------------------------------------------------
// Query
//------------------------------------------------------------------------------


bool Value::HasValue() const {
    return m_storage != nullptr;
}


bool Value::IsInteger() const {
    const auto visitor = []<class T>(const T&) {
        return std::is_integral_v<T>;
    };
    return m_storage ? std::visit(visitor, *m_storage) : false;
}


std::string Value::GetTypeStr() const {
    if (!HasValue()) {
        return "<empty>";
    }
    const auto visitor = []<class S>(const S&) { return GetTypeStr<S>(); };
    return std::visit(visitor, *m_storage);
}


std::vector<Token> Tokenize(const Bytes& value) {
    return {
        Token{
              .tag = Token::GetTag(value.size()),
              .isByte = true,
              .isSigned = false,
              .data = value,
              }
    };
}


std::pair<Bytes, std::span<const Token>> DeTokenize(Tokenized<Bytes> tokens) {
    if (tokens.tokens.empty()) {
        throw std::invalid_argument("expected at least 1 token");
    }
    const auto& myToken = tokens.tokens[0];
    if (!myToken.IsAtom() || !myToken.isByte || myToken.isSigned) {
        throw std::invalid_argument("expected a non-continued byte-token");
    }
    return { myToken.data, tokens.tokens.subspan(1) };
}


std::vector<Token> Tokenize(const eCommand& value) {
    return {
        Token{
              .tag = static_cast<eTag>(value),
              .isByte = false,
              .isSigned = false,
              }
    };
}


std::pair<eCommand, std::span<const Token>> DeTokenize(Tokenized<eCommand> tokens) {
    if (tokens.tokens.empty()) {
        throw std::invalid_argument("expected at least 1 token");
    }
    const auto& myToken = tokens.tokens[0];
    const auto rest = tokens.tokens.subspan(1);
    switch (myToken.tag) {
        case eTag::CALL: return { eCommand::CALL, rest };
        case eTag::END_OF_DATA: return { eCommand::END_OF_DATA, rest };
        case eTag::END_OF_SESSION: return { eCommand::END_OF_SESSION, rest };
        case eTag::START_TRANSACTION: return { eCommand::START_TRANSACTION, rest };
        case eTag::END_TRANSACTION: return { eCommand::END_TRANSACTION, rest };
        case eTag::EMPTY: return { eCommand::EMPTY, rest };
        default: throw std::invalid_argument("expected a valid command");
    }
}


std::vector<Token> Tokenize(const List& value) {
    std::vector<Token> tokens;
    tokens.push_back({ .tag = eTag::START_LIST });
    for (auto& item : value) {
        auto itemTokens = Tokenize(item);
        std::copy(std::make_move_iterator(itemTokens.begin()), std::make_move_iterator(itemTokens.end()), std::back_inserter(tokens));
    }
    tokens.push_back({ .tag = eTag::END_LIST });
    return tokens;
}


std::pair<List, std::span<const Token>> DeTokenize(Tokenized<List> tokens) {
    auto rest = tokens.tokens.subspan(0);
    if (rest.empty()) {
        throw std::invalid_argument("expected at least 2 tokens");
    }
    const auto& maybeStartList = rest.front();
    if (maybeStartList.tag != eTag::START_LIST) {
        throw std::invalid_argument("expected a start list token");
    }
    rest = rest.subspan(1);
    List list;
    while (true) {
        if (rest.empty()) {
            throw std::invalid_argument("list is not closed by an end list token");
        }
        const auto& maybeEndList = rest.front();
        if (maybeEndList.tag == eTag::END_LIST) {
            break;
        }
        auto [newValue, newRest] = DeTokenize(Tokenized<Value>{ rest });
        rest = newRest;
        list.push_back(std::move(newValue));
    }
    return { list, rest.subspan(1) };
}


std::vector<Token> Tokenize(const Named& value) {
    std::vector<Token> tokens;
    tokens.push_back({ .tag = eTag::START_NAME });

    auto nameTokens = Tokenize(value.name);
    std::copy(std::make_move_iterator(nameTokens.begin()), std::make_move_iterator(nameTokens.end()), std::back_inserter(tokens));

    auto valueTokens = Tokenize(value.value);
    std::copy(std::make_move_iterator(valueTokens.begin()), std::make_move_iterator(valueTokens.end()), std::back_inserter(tokens));

    tokens.push_back({ .tag = eTag::END_NAME });
    return tokens;
}


std::pair<Named, std::span<const Token>> DeTokenize(Tokenized<Named> tokens) {
    auto rest = tokens.tokens.subspan(0);
    if (rest.empty()) {
        throw std::invalid_argument("expected at least 4 tokens");
    }
    const auto& maybeStartList = rest.front();
    if (maybeStartList.tag != eTag::START_NAME) {
        throw std::invalid_argument("expected a start name token");
    }
    rest = rest.subspan(1);
    if (rest.empty()) {
        throw std::invalid_argument("expected a token for the named value's name");
    }
    auto [nameValue, newRest1] = DeTokenize(Tokenized<Value>{ rest });
    rest = newRest1;

    if (rest.empty()) {
        throw std::invalid_argument("expected a token for the named value's value");
    }
    auto [valueValue, newRest2] = DeTokenize(Tokenized<Value>{ rest });
    rest = newRest2;

    if (rest.empty()) {
        throw std::invalid_argument("expected end name token");
    }
    const auto& maybeEndList = rest.front();
    if (maybeEndList.tag != eTag::END_NAME) {
        throw std::invalid_argument("named value not terminated properly");
    }
    return {
        Named{std::move(nameValue), std::move(valueValue)},
        rest.subspan(1)
    };
}


std::vector<Token> Tokenize(const Value& value) {
    if (!value.HasValue()) {
        throw std::invalid_argument("cannot serialize empty Value");
    }
    const auto visitor = [](const auto& value) {
        return Tokenize(value);
    };
    return value.Visit(visitor);
}


std::pair<Value, std::span<const Token>> DeTokenize(Tokenized<Value> tokens) {
    if (tokens.tokens.empty()) {
        return { Value(), tokens.tokens };
    }
    const auto& leading = tokens.tokens.front();
    if (leading.IsAtom()) {
        if (leading.isByte) {
            return DeTokenize(Tokenized<Bytes>{ tokens.tokens });
        }
        else {
            if (leading.isSigned) {
                switch (leading.data.size()) {
                    case 1: return DeTokenize(Tokenized<int8_t>{ tokens.tokens });
                    case 2: return DeTokenize(Tokenized<int16_t>{ tokens.tokens });
                    case 3: [[fallthrough]];
                    case 4: return DeTokenize(Tokenized<int32_t>{ tokens.tokens });
                    case 5: [[fallthrough]];
                    case 6: [[fallthrough]];
                    case 7: [[fallthrough]];
                    case 8: return DeTokenize(Tokenized<int64_t>{ tokens.tokens });
                    default: throw std::invalid_argument("invalid integer type");
                }
            }
            else {
                switch (leading.data.size()) {
                    case 1: return DeTokenize(Tokenized<uint8_t>{ tokens.tokens });
                    case 2: return DeTokenize(Tokenized<uint16_t>{ tokens.tokens });
                    case 3: [[fallthrough]];
                    case 4: return DeTokenize(Tokenized<uint32_t>{ tokens.tokens });
                    case 5: [[fallthrough]];
                    case 6: [[fallthrough]];
                    case 7: [[fallthrough]];
                    case 8: return DeTokenize(Tokenized<uint64_t>{ tokens.tokens });
                    default: throw std::invalid_argument("invalid integer type");
                }
            }
        }
    }
    else if (leading.tag == eTag::START_LIST) {
        return DeTokenize(Tokenized<List>{ tokens.tokens });
    }
    else if (leading.tag == eTag::START_NAME) {
        return DeTokenize(Tokenized<Named>{ tokens.tokens });
    }
    else {
        return DeTokenize(Tokenized<eCommand>{ tokens.tokens });
    }
}


std::string Dump(const Value& value) {
    static constexpr auto indent = [](std::string_view str) {
        const auto re = std::regex(R"(\n)");
        std::string out = "  ";
        std::regex_replace(std::back_inserter(out), str.begin(), str.end(), re, "\n  ");
        return out;
    };

    if (value.Is<Bytes>()) {
        std::string str;
        str += "[ ";
        for (auto byte : value.Get<Bytes>()) {
            str += std::format("{:02x}", uint8_t(byte)) += " ";
        }
        str += "]";
        return str;
    }
    else if (value.Is<eCommand>()) {
        switch (value.Get<eCommand>()) {
            case eCommand::CALL: return "CALL";
            case eCommand::END_OF_DATA: return "END_OF_DATA";
            case eCommand::END_OF_SESSION: return "END_OF_SESSION";
            case eCommand::START_TRANSACTION: return "START_TRANSACTION";
            case eCommand::END_TRANSACTION: return "END_TRANSACTION";
            case eCommand::EMPTY: return "EMPTY";
        }
        return "ERROR:UNKNOWN_COMMAND";
    }
    else if (value.IsInteger()) {
        return std::to_string(value.Get<int64_t>());
    }
    else if (value.Is<List>()) {
        std::string str;
        str += "START_LIST {\n";
        for (const auto& item : value.Get<List>()) {
            str += indent(Dump(item)) += "\n";
        }
        str += "} END_LIST";
        return str;
    }
    else if (value.Is<Named>()) {
        std::string str;
        str += "START_NAME {\n";
        str += indent(Dump(value.Get<Named>().name)) += "\n";
        str += indent(Dump(value.Get<Named>().value)) += "\n";
        str += "} END_NAME";
        return str;
    }
    return "EMPTY";
}

} // namespace sedmgr