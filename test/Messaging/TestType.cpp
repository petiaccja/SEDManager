#include <Messaging/Type.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("Type: identified types", "[Type]") {
    const Type type = IdentifiedType<IntegerType, 754_uid>(4, false);
    REQUIRE(type_isa<IntegerType>(type));
    REQUIRE(type_uid(type) == UID(754));
}


TEST_CASE("Type: type_cast", "[Type]") {
    const Type type = IdentifiedType<IntegerType, 754_uid>(4, false);
    REQUIRE(type_cast<IntegerType>(type).Width() == 4);
    REQUIRE(type_cast<IntegerType>(type).Signedness() == false);
}


TEST_CASE("Type: type_isa", "[Type]") {
    SECTION("IntegerType") {
        const Type type = IntegerType(4, false);
        REQUIRE(type_isa<IntegerType>(type));
    }
    SECTION("BytesType") {
        const Type type = BytesType(4, false);
        REQUIRE(type_isa<BytesType>(type));
    }
    SECTION("UnsignedIntType") {
        const Type type = UnsignedIntType(4);
        REQUIRE(type_isa<UnsignedIntType>(type));
    }
    SECTION("SignedIntType") {
        const Type type = SignedIntType(4);
        REQUIRE(type_isa<SignedIntType>(type));
    }
    SECTION("CappedBytesType") {
        const Type type = CappedBytesType(4);
        REQUIRE(type_isa<CappedBytesType>(type));
    }
    SECTION("FixedBytesType") {
        const Type type = FixedBytesType(4);
        REQUIRE(type_isa<FixedBytesType>(type));
    }
    SECTION("EnumerationType") {
        const Type type = EnumerationType(4, 7);
        REQUIRE(type_isa<EnumerationType>(type));
    }
    SECTION("AlternativeType") {
        const Type type = AlternativeType();
        REQUIRE(type_isa<AlternativeType>(type));
    }
    SECTION("ListType") {
        const Type type = ListType(IntegerType(4, false));
        REQUIRE(type_isa<ListType>(type));
    }
    SECTION("StructType") {
        const Type type = StructType();
        REQUIRE(type_isa<StructType>(type));
    }
    SECTION("SetType") {
        const Type type = SetType(3, 7);
        REQUIRE(type_isa<SetType>(type));
    }
    SECTION("RestrictedReferenceType") {
        const Type type = RestrictedReferenceType(0x01_uid);
        REQUIRE(type_isa<RestrictedReferenceType>(type));
    }
    SECTION("RestrictedByteReferenceType") {
        const Type type = RestrictedByteReferenceType(0x01_uid);
        REQUIRE(type_isa<RestrictedByteReferenceType>(type));
    }
    SECTION("RestrictedObjectReferenceType") {
        const Type type = RestrictedObjectReferenceType(0x01_uid);
        REQUIRE(type_isa<RestrictedObjectReferenceType>(type));
    }
    SECTION("GeneralReferenceType") {
        const Type type = GeneralReferenceType();
        REQUIRE(type_isa<GeneralReferenceType>(type));
    }
    SECTION("GeneralByteReferenceType") {
        const Type type = GeneralByteReferenceType();
        REQUIRE(type_isa<GeneralByteReferenceType>(type));
    }
    SECTION("GeneralObjectReferenceType") {
        const Type type = GeneralObjectReferenceType();
        REQUIRE(type_isa<GeneralObjectReferenceType>(type));
    }
    SECTION("GeneralTableReferenceType") {
        const Type type = GeneralTableReferenceType();
        REQUIRE(type_isa<GeneralTableReferenceType>(type));
    }
    SECTION("GeneralByteTableReferenceType") {
        const Type type = GeneralByteTableReferenceType();
        REQUIRE(type_isa<GeneralByteTableReferenceType>(type));
    }
    SECTION("GeneralObjectTableReferenceType") {
        const Type type = GeneralObjectTableReferenceType();
        REQUIRE(type_isa<GeneralObjectTableReferenceType>(type));
    }
    SECTION("NameValueUintegerType") {
        const Type type = NameValueUintegerType(0, IntegerType(4, false));
        REQUIRE(type_isa<NameValueUintegerType>(type));
    }
}
