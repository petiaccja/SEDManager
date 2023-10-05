#include "RpcStream.hpp"


void foo() {
    RpcStream stream = {
        1,
        2,
        { "name", 6 },
    };
}


//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

// Lists.
RpcStream::RpcStream(std::initializer_list<RpcStream> values)
    : m_value(ListType(values)) {}


// Named.
RpcStream::RpcStream(std::string_view name, RpcStream value)
    : m_value(Named{ std::string{ name }, value }) {}

RpcStream::RpcStream(Named value)
    : m_value(std::move(value)) {}


// Commands.
RpcStream::RpcStream(eCommand command)
    : m_value(command) {}



//------------------------------------------------------------------------------
// Get specific
//------------------------------------------------------------------------------

// Lists.
std::span<const RpcStream> RpcStream::AsList() const {
    return std::any_cast<const ListType&>(m_value);
}

std::vector<RpcStream>& RpcStream::AsList() {
    return std::any_cast<std::vector<RpcStream>&>(m_value);
}

// Bytes
std::vector<uint8_t>& RpcStream::AsBytes() {
    return std::any_cast<std::vector<uint8_t>&>(m_value);
}

// Named.
const Named& RpcStream::AsNamed() const {
    return std::any_cast<const Named&>(m_value);
}

Named& RpcStream::AsNamed() {
    return std::any_cast<Named&>(m_value);
}

// Commands.
eCommand RpcStream::AsCommand() const {
    return std::any_cast<eCommand>(m_value);
}



//------------------------------------------------------------------------------
// Query
//------------------------------------------------------------------------------

bool RpcStream::IsInteger() const {
    const auto v = ForEachType<IntTypes>([this](auto* ptr) -> std::optional<bool> {
        if (m_value.type() == typeid(decltype(*ptr))) {
            return true;
        }
        return std::nullopt;
    });
    return v.has_value();
}

bool RpcStream::IsBytes() const {
    return typeid(BytesType) == m_value.type();
}

bool RpcStream::IsList() const {
    return typeid(ListType) == m_value.type();
}

bool RpcStream::IsNamed() const {
    return typeid(Named) == m_value.type();
}

bool RpcStream::IsCommand() const {
    return typeid(eCommand) == m_value.type();
}

const std::type_info& RpcStream::Type() const {
    return m_value.type();
}


//------------------------------------------------------------------------------
// Serialization
//------------------------------------------------------------------------------

RpcStream ConvertToData(const Token& token) {
    if (token.isByte) {
        if (token.isSigned) {
            throw std::invalid_argument("continued atoms are not supported");
        }
        return RpcStream(RpcStream::bytes, token.data);
    }
    else if (token.data.size() == 0) {
        return RpcStream(static_cast<eCommand>(token.tag));
    }
    else {
        uint64_t value = 0;
        for (auto& byte : token.data) {
            value <<= 8;
            value |= byte;
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


void InsertItem(RpcStream& target, RpcStream item) {
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