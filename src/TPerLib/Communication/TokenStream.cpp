#include "TokenStream.hpp"


void foo() {
    TokenStream stream = {
        1,
        2,
        { "name", 6 },
    };
}


//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

// Lists.
TokenStream::TokenStream(std::initializer_list<TokenStream> values)
    : m_value(ListType(values)) {}


// Named.
TokenStream::TokenStream(std::string_view name, TokenStream value)
    : m_value(Named{ std::string{ name }, value }) {}

TokenStream::TokenStream(Named value)
    : m_value(std::move(value)) {}


// Commands.
TokenStream::TokenStream(eCommand command)
    : m_value(command) {}



//------------------------------------------------------------------------------
// Get specific
//------------------------------------------------------------------------------

// Lists.
std::span<const TokenStream> TokenStream::AsList() const {
    return std::any_cast<const ListType&>(m_value);
}

std::vector<TokenStream>& TokenStream::AsList() {
    return std::any_cast<std::vector<TokenStream>&>(m_value);
}

// Bytes
std::vector<uint8_t>& TokenStream::AsBytes() {
    return std::any_cast<std::vector<uint8_t>&>(m_value);
}

// Named.
const Named& TokenStream::AsNamed() const {
    return std::any_cast<const Named&>(m_value);
}

Named& TokenStream::AsNamed() {
    return std::any_cast<Named&>(m_value);
}

// Commands.
eCommand TokenStream::AsCommand() const {
    return std::any_cast<eCommand>(m_value);
}



//------------------------------------------------------------------------------
// Query
//------------------------------------------------------------------------------

bool TokenStream::IsInteger() const {
    const auto v = ForEachType<IntTypes>([this](auto* ptr) -> std::optional<bool> {
        if (m_value.type() == typeid(decltype(*ptr))) {
            return true;
        }
        return std::nullopt;
    });
    return v.has_value();
}

bool TokenStream::IsBytes() const {
    return typeid(BytesType) == m_value.type();
}

bool TokenStream::IsList() const {
    return typeid(ListType) == m_value.type();
}

bool TokenStream::IsNamed() const {
    return typeid(Named) == m_value.type();
}

bool TokenStream::IsCommand() const {
    return typeid(eCommand) == m_value.type();
}

const std::type_info& TokenStream::Type() const {
    return m_value.type();
}


//------------------------------------------------------------------------------
// Serialization
//------------------------------------------------------------------------------

TokenStream ConvertToData(const Token& token) {
    if (token.isByte) {
        if (token.isSigned) {
            throw std::invalid_argument("continued atoms are not supported");
        }
        return TokenStream(TokenStream::bytes, token.data);
    }
    else if (token.data.size() == 0) {
        return TokenStream(static_cast<eCommand>(token.tag));
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


void InsertItem(TokenStream& target, TokenStream item) {
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