#pragma once

#include "Tables.hpp"
#include "Type.hpp"




struct ColumnType {
    
};






//------------------------------------------------------------------------------
// Integer types
//------------------------------------------------------------------------------

struct Integer1 final : IdentifiedType<SignedIntType, 0x0000'0005'0000'0210> {
    Integer1() : IdentifiedType(1) {}
};

struct Integer2 final : IdentifiedType<SignedIntType, 0x0000'0005'0000'0215> {
    Integer2() : IdentifiedType(2) {}
};

struct UInteger1 final : IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0211> {
    UInteger1() : IdentifiedType(1) {}
};

struct UInteger2 final : IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0215> {
    UInteger2() : IdentifiedType(2) {}
};

struct UInteger4 final : IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0220> {
    UInteger4() : IdentifiedType(4) {}
};

struct UInteger8 final : IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0225> {
    UInteger8() : IdentifiedType(8) {}
};


//------------------------------------------------------------------------------
// Uid type
//------------------------------------------------------------------------------

struct UidType final : IdentifiedType<FixedBytesType, 0x0000'0005'0000'0209> {
    UidType() : IdentifiedType(8) {}
};


//------------------------------------------------------------------------------
// Reference types
//------------------------------------------------------------------------------