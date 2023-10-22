#include "ValueToToken.hpp"

#include <cassert>


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