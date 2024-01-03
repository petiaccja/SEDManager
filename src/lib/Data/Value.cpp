#include "Value.hpp"


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
// Get specific
//------------------------------------------------------------------------------

std::span<const Value> Value::GetList() const {
    if (IsList()) {
        return std::get<List>(*m_storage);
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<List>());
}

std::vector<Value>& Value::GetList() {
    if (IsList()) {
        return std::get<List>(*m_storage);
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<List>());
}

std::vector<std::byte>& Value::GetBytes() {
    if (IsBytes()) {
        return std::get<Bytes>(*m_storage);
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<Bytes>());
}

std::span<const std::byte> Value::GetBytes() const {
    if (IsBytes()) {
        return std::get<Bytes>(*m_storage);
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<Bytes>());
}

const Named& Value::GetNamed() const {
    if (IsNamed()) {
        return std::get<Named>(*m_storage);
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<Named>());
}

Named& Value::GetNamed() {
    if (IsNamed()) {
        return std::get<Named>(*m_storage);
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<Named>());
}

eCommand Value::GetCommand() const {
    if (IsCommand()) {
        return std::get<eCommand>(*m_storage);
    }
    throw TypeConversionError(GetTypeStr(), GetTypeStr<eCommand>());
}


//------------------------------------------------------------------------------
// Query
//------------------------------------------------------------------------------

bool Value::IsInteger() const {
    if (!m_storage) {
        return false;
    }
    const auto visitor = []<class S>(const S&) { return std::is_integral_v<S>; };
    return std::visit(visitor, *m_storage);
}

bool Value::IsBytes() const {
    return m_storage && std::holds_alternative<Bytes>(*m_storage);
}

bool Value::IsList() const {
    return m_storage && std::holds_alternative<List>(*m_storage);
}

bool Value::IsNamed() const {
    return m_storage && std::holds_alternative<Named>(*m_storage);
}

bool Value::IsCommand() const {
    return m_storage && std::holds_alternative<eCommand>(*m_storage);
}

bool Value::HasValue() const {
    return m_storage != nullptr;
}

const std::type_info& Value::Type() const {
    if (!HasValue()) {
        return typeid(void);
    }
    const auto visitor = []<class S>(const S&) -> const std::type_info& { return typeid(S); };
    return std::visit(visitor, *m_storage);
}

std::string Value::GetTypeStr() const {
    if (!HasValue()) {
        return "<empty>";
    }
    const auto visitor = []<class S>(const S&) { return GetTypeStr<S>(); };
    return std::visit(visitor, *m_storage);
}

} // namespace sedmgr