#include <Specification/Common/Utility.hpp>

#include <array>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("Specification: name sequence find UID", "[Specification]") {
    const NameSequence seq(106, 6, 10, "name{}");

    SECTION("Out of bounds - low") {
        REQUIRE(!seq.Find(105));
    }
    SECTION("Out of bounds - high") {
        REQUIRE(!seq.Find(116));
    }
    SECTION("First") {
        const auto result = seq.Find(106);
        REQUIRE(!!result);
        REQUIRE(*result == "name6");
    }
    SECTION("Last") {
        const auto result = seq.Find(115);
        REQUIRE(!!result);
        REQUIRE(*result == "name15");
    }
}


TEST_CASE("Specification: name sequence find name", "[Specification]") {
    const NameSequence seq(106, 6, 10, "name{}");

    SECTION("Out of bounds - low") {
        REQUIRE(!seq.Find("name5"));
    }
    SECTION("Out of bounds - high") {
        REQUIRE(!seq.Find("name16"));
    }
    SECTION("First") {
        const auto result = seq.Find("name6");
        REQUIRE(!!result);
        REQUIRE(*result == UID(106));
    }
    SECTION("Last") {
        const auto result = seq.Find("name15");
        REQUIRE(!!result);
        REQUIRE(*result == UID(115));
    }
}


TEST_CASE("Specification: finder", "[Specification]") {
    const std::initializer_list<std::pair<UID, std::string_view>> pairs = {
        {1,  "1"},
        { 2, "2"},
    };
    const std::initializer_list<NameSequence> sequences = {
        {10,  0, 5, "s{}"},
        { 20, 0, 5, "t{}"},
    };
    const NameAndUidFinder finder({ pairs }, sequences);

    SECTION("by uid") {
        REQUIRE(finder.Find(1) == "1");
        REQUIRE(finder.Find(2) == "2");
        REQUIRE(finder.Find(14) == "s4");
        REQUIRE(finder.Find(21) == "t1");
    }
    SECTION("by name") {
        REQUIRE(finder.Find("1") == UID(1));
        REQUIRE(finder.Find("2") == UID(2));
        REQUIRE(finder.Find("s4") == UID(14));
        REQUIRE(finder.Find("t1") == UID(21));
    }
}


TEST_CASE("Specification: finder UID collision", "[Specification]") {
    const std::initializer_list<std::pair<UID, std::string_view>> pairs = {
        {1,  "1"},
        { 1, "2"},
    };

    REQUIRE_THROWS(NameAndUidFinder({ pairs }, {}));
}


TEST_CASE("Specification: finder name collision", "[Specification]") {
    const std::initializer_list<std::pair<UID, std::string_view>> pairs = {
        {1,  "1"},
        { 2, "1"},
    };

    REQUIRE_THROWS(NameAndUidFinder({ pairs }, {}));
}


TEST_CASE("Specification: sp finder", "[Specification]") {
    SPNameAndUidFinder finder({
        {100,  NameAndUidFinder({ { { 1, "1" } } }, {})},
        { 101, NameAndUidFinder({ { { 2, "2" } } }, {})},
    });

    SECTION("by uid") {
        REQUIRE(finder.Find(1, 100) == "1");
        REQUIRE(!finder.Find(1, 101));
        REQUIRE(finder.Find(2, 101) == "2");
        REQUIRE(!finder.Find(2, 100));
    }
    SECTION("by name") {
        REQUIRE(finder.Find("1", 100) == UID(1));
        REQUIRE(!finder.Find("1", 101));
        REQUIRE(finder.Find("2", 101) == UID(2));
        REQUIRE(!finder.Find("2", 100));
    }
}