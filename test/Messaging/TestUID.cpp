#include <Messaging/Native.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


constexpr auto table = UID(0x0000'0001'0000'0000);
constexpr auto descriptor = UID(0x0000'0001'0000'0001);
constexpr auto object = UID(0x0000'0009'0000'0001);
constexpr auto containing = UID(0x0000'0009'0000'0000);
constexpr auto sessionManagerMethod = UID(0x0000'0000'0000'FF01);


TEST_CASE("UID: UID <=> str", "[UID]") {
    SECTION("to str") {
        REQUIRE("DEAD'BEEF'0123'4567" == UID(0xDEAD'BEEF'0123'4567).ToString());
    }
    SECTION("from str") {
        REQUIRE(UID::Parse("DEAD'BEEF'0123'4567") == UID(0xDEAD'BEEF'0123'4567));
    }
}


TEST_CASE("UID: query type", "[UID]") {
    REQUIRE(table.IsTable());
    REQUIRE(!table.IsDescriptor());
    REQUIRE(!table.IsObject());

    REQUIRE(!descriptor.IsTable());
    REQUIRE(descriptor.IsDescriptor());
    REQUIRE(!descriptor.IsObject());

    REQUIRE(!object.IsTable());
    REQUIRE(!object.IsDescriptor());
    REQUIRE(object.IsObject());

    REQUIRE(!sessionManagerMethod.IsTable());
    REQUIRE(!sessionManagerMethod.IsDescriptor());
    REQUIRE(sessionManagerMethod.IsObject());
}


TEST_CASE("UID: table <=> descriptor", "[UID]") {
    REQUIRE(table.ToDescriptor() == descriptor);
    REQUIRE(table == descriptor.ToTable());
}


TEST_CASE("UID: containing table", "[UID]") {
    REQUIRE(object.ContainingTable() == containing);
    REQUIRE(descriptor.ContainingTable() == table);
}