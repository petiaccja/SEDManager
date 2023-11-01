#include <Data/NativeTypes.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("NativeTypes: UID <=> str") {
    SECTION("to str") {
        REQUIRE("DEAD'BEEF'0123'4567" == to_string(Uid(0xDEAD'BEEF'0123'4567)));
    }
    SECTION("from str") {
        REQUIRE(stouid("DEAD'BEEF'0123'4567") == Uid(0xDEAD'BEEF'0123'4567));
    }
}