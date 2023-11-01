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
        if (target.IsList()) {
            target.GetList().push_back(std::move(item));
        }
        else if (target.IsNamed()) {
            const bool doName = !target.GetNamed().value.HasValue();
            if (doName) {
                target.GetNamed().name = item;
                target.GetNamed().value = eCommand::EMPTY;
            }
            else {
                if (!target.GetNamed().value.IsCommand() || target.GetNamed().value.Get<eCommand>() != eCommand::EMPTY) {
                    throw std::invalid_argument("named items expect a single item as value");
                }
                target.GetNamed().value = std::move(item);
            }
        }
        else {
            assert(false && "incorrect stack handling");
        }
    }


    void ValueToTokens(const Value& value, std::vector<Token>& out) {
        if (value.IsBytes()) {
            const auto& bytes = value.GetBytes();
            Token token = {
                .tag = GetTagForData(bytes.size()),
                .isByte = true,
                .isSigned = false,
                .data = {bytes.begin(), bytes.end()},
            };
            out.push_back(std::move(token));
        }
        else if (value.IsCommand()) {
            Token token = {
                .tag = static_cast<eTag>(value.GetCommand()),
                .isByte = false,
                .isSigned = false,
            };
            out.push_back(std::move(token));
        }
        else if (value.IsInteger()) {
            const auto result = ForEachType<Value::IntTypes>([&]<class T>(T*) -> std::optional<bool> {
                if (value.Type() == typeid(T)) {
                    Token token = {
                        .tag = eTag::SHORT_ATOM,
                        .isByte = false,
                        .isSigned = std::is_signed_v<T>,
                        .data = ToBytes(value.Get<T>()),
                    };
                    out.push_back(std::move(token));
                    return true;
                }
                return std::nullopt;
            });
            assert(result.has_value());
        }
        else if (value.IsList()) {
            out.push_back(Token{ .tag = eTag::START_LIST });
            for (const auto& item : value.GetList()) {
                ValueToTokens(item, out);
            }
            out.push_back(Token{ .tag = eTag::END_LIST });
        }
        else if (value.IsNamed()) {
            out.push_back(Token{ .tag = eTag::START_NAME });
            ValueToTokens(value.GetNamed().name, out);
            ValueToTokens(value.GetNamed().value, out);
            out.push_back(Token{ .tag = eTag::END_NAME });
        }
        else if (!value.HasValue()) {
            return;
        }
        else {
            assert(false);
        }
    }

    Value TokensToValue(std::span<const Token> tokens) {
        std::stack<Value> stack;
        stack.push(Value(std::vector<Value>{}));
        for (const auto& token : tokens) {
            if (token.tag == eTag::EMPTY) {
                continue;
            }
            else if (token.tag == eTag::START_LIST) {
                stack.push(Value(std::vector<Value>{}));
            }
            else if (token.tag == eTag::START_NAME) {
                stack.push(Value(Named{}));
            }
            else if (token.tag == eTag::END_LIST) {
                auto item = std::move(stack.top());
                if (stack.empty() || !item.IsList()) {
                    throw InvalidFormatError("invalid token stream: unexpected list close");
                }
                stack.pop();
                InsertItem(stack.top(), std::move(item));
            }
            else if (token.tag == eTag::END_NAME) {
                auto item = std::move(stack.top());
                if (stack.empty() || !item.IsNamed()) {
                    throw InvalidFormatError("invalid token stream: unexpected name close");
                }
                stack.pop();
                InsertItem(stack.top(), std::move(item));
            }
            else {
                stack.push(ConvertToData(token));
            }

            if (!stack.top().IsList() && !stack.top().IsNamed()) {
                auto item = std::move(stack.top());
                stack.pop();
                InsertItem(stack.top(), std::move(item));
            }
        }

        if (stack.size() != 1) {
            throw InvalidFormatError("invalid token stream: unclosed lists or named values");
        }

        return std::move(stack.top());
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