#include <Specification/Common/Utility.hpp>

#include <array>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("Specification: name sequence find UID", "[Specification]") {
    const NameSequence seq(106_uid, 6, 10, "name{}");

    SECTION("Out of bounds - low") {
        REQUIRE(!seq.Find(105_uid));
    }
    SECTION("Out of bounds - high") {
        REQUIRE(!seq.Find(116_uid));
    }
    SECTION("First") {
        const auto result = seq.Find(106_uid);
        REQUIRE(!!result);
        REQUIRE(*result == "name6");
    }
    SECTION("Last") {
        const auto result = seq.Find(115_uid);
        REQUIRE(!!result);
        REQUIRE(*result == "name15");
    }
}


TEST_CASE("Specification: name sequence find name", "[Specification]") {
    const NameSequence seq(106_uid, 6, 10, "name{}");

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
        {1_uid,  "1"},
        { 2_uid, "2"},
    };
    const std::initializer_list<NameSequence> sequences = {
        {10_uid,  0, 5, "s{}"},
        { 20_uid, 0, 5, "t{}"},
    };
    const NameAndUidFinder finder({ pairs }, sequences);

    SECTION("by uid") {
        REQUIRE(finder.Find(1_uid) == "1");
        REQUIRE(finder.Find(2_uid) == "2");
        REQUIRE(finder.Find(14_uid) == "s4");
        REQUIRE(finder.Find(21_uid) == "t1");
    }
    SECTION("by name") {
        REQUIRE(finder.Find("1") == 1_uid);
        REQUIRE(finder.Find("2") == 2_uid);
        REQUIRE(finder.Find("s4") == 14_uid);
        REQUIRE(finder.Find("t1") == 21_uid);
    }
}


TEST_CASE("Specification: finder UID collision", "[Specification]") {
    const std::initializer_list<std::pair<UID, std::string_view>> pairs = {
        {1_uid,  "1"},
        { 1_uid, "2"},
    };

    REQUIRE_THROWS(NameAndUidFinder({ pairs }, {}));
}


TEST_CASE("Specification: finder name collision", "[Specification]") {
    const std::initializer_list<std::pair<UID, std::string_view>> pairs = {
        {1_uid,  "1"},
        { 2_uid, "1"},
    };

    REQUIRE_THROWS(NameAndUidFinder({ pairs }, {}));
}


TEST_CASE("Specification: sp finder", "[Specification]") {
    SPNameAndUidFinder finder({
        {100_uid,  NameAndUidFinder({ { { 1_uid, "1" } } }, {})},
        { 101_uid, NameAndUidFinder({ { { 2_uid, "2" } } }, {})},
    });

    SECTION("by uid") {
        REQUIRE(finder.Find(1_uid, 100_uid) == "1");
        REQUIRE(!finder.Find(1_uid, 101_uid));
        REQUIRE(finder.Find(2_uid, 101_uid) == "2");
        REQUIRE(!finder.Find(2_uid, 100_uid));
    }
    SECTION("by name") {
        REQUIRE(finder.Find("1", 100_uid) == 1_uid);
        REQUIRE(!finder.Find("1", 101_uid));
        REQUIRE(finder.Find("2", 101_uid) == 2_uid);
        REQUIRE(!finder.Find("2", 100_uid));
    }
}