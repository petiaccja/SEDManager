#include "Value.hpp"


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



bool Value::operator==(const Value& rhs) const {
    if (Type() != rhs.Type()) {
        return false;
    }
    if (IsInteger()) {
        return ForEachType<IntTypes>([&]<class T>(T*) -> std::optional<bool> {
                   return Type() == typeid(T) ? std::optional(Get<T>() == rhs.Get<T>()) : std::nullopt;
               })
            .value_or(false);
    }
    else if (IsBytes()) {
        return std::ranges::equal(AsBytes(), rhs.AsBytes());
    }
    else if (IsCommand()) {
        return AsCommand() == rhs.AsCommand();
    }
    else if (IsList()) {
        return std::ranges::equal(AsList(), rhs.AsList());
    }
    else if (IsNamed()) {
        return AsNamed().name == rhs.AsNamed().name && AsNamed().value == rhs.AsNamed().value;
    }
    return false;
}


//------------------------------------------------------------------------------
// Get specific
//------------------------------------------------------------------------------

// Lists.
std::span<const Value> Value::AsList() const {
    try {
        return std::any_cast<const ListType&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "list");
    }
}

std::vector<Value>& Value::AsList() {
    try {
        return std::any_cast<std::vector<Value>&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "list");
    }
}

// Bytes
std::vector<std::byte>& Value::AsBytes() {
    try {
        return std::any_cast<BytesType&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "bytes");
    }
}

std::span<const std::byte> Value::AsBytes() const {
    try {
        return { std::any_cast<const BytesType&>(m_value) };
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "bytes");
    }
}

// Named.
const Named& Value::AsNamed() const {
    try {
        return std::any_cast<const Named&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "named");
    }
}

Named& Value::AsNamed() {
    try {
        return std::any_cast<Named&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "named");
    }
}

// Commands.
eCommand Value::AsCommand() const {
    try {
        return std::any_cast<eCommand>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "command");
    }
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

std::string Value::GetTypeStr() const {
    if (IsInteger()) {
        const auto v = ForEachType<IntTypes>([this](auto* ptr) -> std::optional<std::pair<size_t, bool>> {
            using Q = std::decay_t<decltype(*ptr)>;
            if (m_value.type() == typeid(Q)) {
                return std::pair{ sizeof(Q), std::is_signed_v<Q> };
            }
            return std::nullopt;
        });
        return std::format("{}int{}", v->second ? "" : "u", v->first * 8);
    }
    else if (IsCommand()) {
        return "command";
    }
    else if (IsList()) {
        return "list";
    }
    else if (IsNamed()) {
        return "named";
    }
    else if (IsBytes()) {
        return "bytes";
    }
    return "<empty>";
}
