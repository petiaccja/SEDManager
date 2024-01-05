#include "ValueToToken.hpp"

#include "../Conversion.hpp"

#include <cassert>


namespace sedmgr {

namespace impl {

    Value ConvertToData(const Token& token) {
        if (token.isByte) {
            if (token.isSigned) {
                throw std::invalid_argument("continued atoms are not supported");
            }
            return Value(token.data);
        }
        else if (token.data.size() == 0) {
            return Value(static_cast<eCommand>(token.tag));
        }
        else {
            uint64_t value = 0;
            for (auto& byte : token.data) {
                value <<= 8;
                value |= uint8_t(byte);
            }
            if (token.isSigned) {
                if (token.data.size() <= 1) {
                    return std::bit_cast<int8_t>(uint8_t(value));
                }
                if (token.data.size() <= 2) {
                    return std::bit_cast<int16_t>(uint16_t(value));
                }
                if (token.data.size() <= 4) {
                    return std::bit_cast<int32_t>(uint32_t(value));
                }
                return std::bit_cast<int64_t>(uint64_t(value));
            }
            else {
                if (token.data.size() <= 1) {
                    return uint8_t(value);
                }
                if (token.data.size() <= 2) {
                    return uint16_t(value);
                }
                if (token.data.size() <= 4) {
                    return uint32_t(value);
                }
                return uint64_t(value);
            }
        }
    }


    void InsertItem(Value& target, Value item) {
        if (target.Is<List>()) {
            target.Get<List>().push_back(std::move(item));
        }
        else if (target.Is<Named>()) {
            const bool doName = !target.Get<Named>().value.HasValue();
            if (doName) {
                target.Get<Named>().name = item;
                target.Get<Named>().value = eCommand::EMPTY;
            }
            else {
                if (!target.Get<Named>().value.Is<eCommand>() || target.Get<Named>().value.Get<eCommand>() != eCommand::EMPTY) {
                    throw std::invalid_argument("named items expect a single item as value");
                }
                target.Get<Named>().value = std::move(item);
            }
        }
        else {
            assert(false && "incorrect stack handling");
        }
    }


    void ValueToTokens(const Value& value, std::vector<Token>& out) {
        out = Tokenize(value);
    }

    Value TokensToValue(std::span<const Token> tokens) {
        return DeTokenize(Tokenized<Value>{tokens}).first;
    }

} // namespace impl


std::vector<Token> ValueToTokens(const Value& value) {
    std::vector<Token> tokens;
    impl::ValueToTokens(value, tokens);
    return tokens;
}


Value TokensToValue(std::span<const Token> tokens) {
    return impl::TokensToValue(tokens);
}

} // namespace sedmgr