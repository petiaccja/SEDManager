#include <Specification/Common/Utility.hpp>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Specification: find name sequence", "[Specification]") {
    NameSequence seq(106, 6, 10, "name{}");

    SECTION("Out of bounds - low") {
        REQUIRE(!FindNameSequence(105, seq));
    }
    SECTION("Out of bounds - high") {
        REQUIRE(!FindNameSequence(116, seq));
    }
    SECTION("First") {
        const auto result = FindNameSequence(106, seq);
        REQUIRE(!!result);
        REQUIRE(*result == "name6");
    }
    SECTION("Last") {
        const auto result = FindNameSequence(115, seq);
        REQUIRE(!!result);
        REQUIRE(*result == "name15");
    }
}


TEST_CASE("Specification: find UID sequence", "[Specification]") {
    NameSequence seq(106, 6, 10, "name{}");

    SECTION("Out of bounds - low") {
        REQUIRE(!FindUidSequence("name5", seq));
    }
    SECTION("Out of bounds - high") {
        REQUIRE(!FindUidSequence("name16", seq));
    }
    SECTION("First") {
        const auto result = FindUidSequence("name6", seq);
        REQUIRE(!!result);
        REQUIRE(*result == Uid(106));
    }
    SECTION("Last") {
        const auto result = FindUidSequence("name15", seq);
        REQUIRE(!!result);
        REQUIRE(*result == Uid(115));
    }
}


TEST_CASE("Specification: table to descriptor", "[Specification]") {
    REQUIRE(TableToDescriptor(0x0000'BEEF'0000'0000) == Uid(0x0000'0001'0000'BEEF));
}


TEST_CASE("Specification: descriptor to table", "[Specification]") {
    REQUIRE(DescriptorToTable(0x0000'0001'0000'BEEF) == Uid(0x0000'BEEF'0000'0000));
}