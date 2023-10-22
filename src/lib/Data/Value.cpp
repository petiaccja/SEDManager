#include "Value.hpp"


//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

Value::Value(std::initializer_list<Value> values)
    : m_value(ListType(values)) {}


Value::Value(Named value)
    : m_value(std::move(value)) {}


Value::Value(eCommand command)
    : m_value(command) {}



bool Value::operator==(const Value& rhs) const {
    if (Type() != rhs.Type()) {
        return false;
    }
    if (IsInteger()) {
        return impl::ForEachType<IntTypes>([&]<class T>(T*) -> std::optional<bool> {
                   return Type() == typeid(T) ? std::optional(Get<T>() == rhs.Get<T>()) : std::nullopt;
               })
            .value_or(false);
    }
    else if (IsBytes()) {
        return std::ranges::equal(GetBytes(), rhs.GetBytes());
    }
    else if (IsCommand()) {
        return GetCommand() == rhs.GetCommand();
    }
    else if (IsList()) {
        return std::ranges::equal(GetList(), rhs.GetList());
    }
    else if (IsNamed()) {
        return GetNamed().name == rhs.GetNamed().name && GetNamed().value == rhs.GetNamed().value;
    }
    return false;
}


//------------------------------------------------------------------------------
// Get specific
//------------------------------------------------------------------------------

std::span<const Value> Value::GetList() const {
    try {
        return std::any_cast<const ListType&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "list");
    }
}

std::vector<Value>& Value::GetList() {
    try {
        return std::any_cast<std::vector<Value>&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "list");
    }
}

std::vector<std::byte>& Value::GetBytes() {
    try {
        return std::any_cast<BytesType&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "bytes");
    }
}

std::span<const std::byte> Value::GetBytes() const {
    try {
        return { std::any_cast<const BytesType&>(m_value) };
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "bytes");
    }
}

const Named& Value::GetNamed() const {
    try {
        return std::any_cast<const Named&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "named");
    }
}

Named& Value::GetNamed() {
    try {
        return std::any_cast<Named&>(m_value);
    }
    catch (std::bad_any_cast&) {
        throw TypeConversionError(GetTypeStr(), "named");
    }
}

eCommand Value::GetCommand() const {
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
    const auto v = impl::ForEachType<IntTypes>([this](auto* ptr) -> std::optional<bool> {
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
        const auto v = impl::ForEachType<IntTypes>([this](auto* ptr) -> std::optional<std::pair<size_t, bool>> {
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
