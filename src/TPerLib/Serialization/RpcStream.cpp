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
// Lookup
//------------------------------------------------------------------------------

// Lists.
std::span<const RpcStream> RpcStream::AsList() const {
    return std::any_cast<const ListType&>(m_value);
}

// Named.
const Named& RpcStream::AsNamed() const {
    return std::any_cast<const Named&>(m_value);
}

// Commands.
eCommand RpcStream::AsCommand() const {
    return std::any_cast<eCommand>(m_value);
}