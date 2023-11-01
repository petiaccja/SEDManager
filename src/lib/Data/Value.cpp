#include "Value.hpp"


namespace sedmgr {

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
    if (IsInteger() && rhs.IsInteger()) {
        if (Type() != rhs.Type()) {
            return false;
        }
        const auto cmpResult = impl::ForEachType<IntTypes>([&]<class T>(T*) -> std::optional<bool> {
            if (Type() == typeid(T)) {
                const auto lhsv = Get<T>();
                const auto rhsv = rhs.Get<T>();
                return lhsv == rhsv;
            }
            return std::nullopt;
        });
        return cmpResult.value_or(false);
    }
    else if (IsBytes() && rhs.IsBytes()) {
        return std::ranges::equal(GetBytes(), rhs.GetBytes());
    }
    else if (IsCommand() && rhs.IsCommand()) {
        return GetCommand() == rhs.GetCommand();
    }
    else if (IsList() && rhs.IsList()) {
        return std::ranges::equal(GetList(), rhs.GetList());
    }
    else if (IsNamed() && rhs.IsNamed()) {
        return GetNamed().name == rhs.GetNamed().name && GetNamed().value == rhs.GetNamed().value;
    }
    else if (!HasValue() && !rhs.HasValue()) {
        return true;
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
// Querystd::optional(Get<T>() == )
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

} // namespace sedmgr