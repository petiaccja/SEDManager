#include "Value.hpp"


void foo() {
    Value stream = {
        1,
        2,
        { "name", 6 },
    };
}


//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

// Lists.
Value::Value(std::initializer_list<Value> values)
    : m_value(ListType(values)) {}


// Named.
Value::Value(Named value)
    : m_value(std::move(value)) {}


// Commands.
Value::Value(eCommand command)
    : m_value(command) {}



//------------------------------------------------------------------------------
// Get specific
//------------------------------------------------------------------------------

// Lists.
std::span<const Value> Value::AsList() const {
    return std::any_cast<const ListType&>(m_value);
}

std::vector<Value>& Value::AsList() {
    return std::any_cast<std::vector<Value>&>(m_value);
}

// Bytes
std::vector<std::byte>& Value::AsBytes() {
    return std::any_cast<BytesType&>(m_value);
}

std::span<const std::byte> Value::AsBytes() const {
    return { std::any_cast<const BytesType&>(m_value) };
}

// Named.
const Named& Value::AsNamed() const {
    return std::any_cast<const Named&>(m_value);
}

Named& Value::AsNamed() {
    return std::any_cast<Named&>(m_value);
}

// Commands.
eCommand Value::AsCommand() const {
    return std::any_cast<eCommand>(m_value);
}



//------------------------------------------------------------------------------
// Query
//------------------------------------------------------------------------------

bool Value::IsInteger() const {
    const auto v = ForEachType<IntTypes>([this](auto* ptr) -> std::optional<bool> {
        if (m_value.type() == typeid(decltype(*ptr))) {
            return true;
        }
        return std::nullopt;
    });
    return v.has_value();
}

bool Value::IsBytes() const {
    return typeid(BytesType) == m_value.type();
}

bool Value::IsList() const {
    return typeid(ListType) == m_value.type();
}

bool Value::IsNamed() const {
    return typeid(Named) == m_value.type();
}

bool Value::IsCommand() const {
    return typeid(eCommand) == m_value.type();
}

const std::type_info& Value::Type() const {
    return m_value.type();
}


//------------------------------------------------------------------------------
// Serialization
//------------------------------------------------------------------------------

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
        target.AsList().push_back(std::move(item));
    }
    else if (target.IsNamed()) {
        const bool doName = !target.AsNamed().value.HasValue();
        if (doName) {
            target.AsNamed().name = item;
            target.AsNamed().value = eCommand::EMPTY;
        }
        else {
            if (!target.AsNamed().value.IsCommand() || target.AsNamed().value.Get<eCommand>() != eCommand::EMPTY) {
                throw std::invalid_argument("named items expect a single item as value");
            }
            target.AsNamed().value = std::move(item);
        }
    }
    else {
        assert(false && "incorrect stack handling");
    }
}