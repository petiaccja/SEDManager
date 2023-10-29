#include <Specification/Common/Utility.hpp>

#include <array>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Specification: table to descriptor", "[Specification]") {
    REQUIRE(TableToDescriptor(0x0000'BEEF'0000'0000) == Uid(0x0000'0001'0000'BEEF));
}


TEST_CASE("Specification: descriptor to table", "[Specification]") {
    REQUIRE(DescriptorToTable(0x0000'0001'0000'BEEF) == Uid(0x0000'BEEF'0000'0000));
}


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
        REQUIRE(*result == Uid(106));
    }
    SECTION("Last") {
        const auto result = seq.Find("name15");
        REQUIRE(!!result);
        REQUIRE(*result == Uid(115));
    }
}


TEST_CASE("Specification: finder", "[Specification]") {
    const std::initializer_list<std::pair<Uid, std::string_view>> pairs = {
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
        REQUIRE(finder.Find("1") == Uid(1));
        REQUIRE(finder.Find("2") == Uid(2));
        REQUIRE(finder.Find("s4") == Uid(14));
        REQUIRE(finder.Find("t1") == Uid(21));
    }
}


TEST_CASE("Specification: finder UID collision", "[Specification]") {
    const std::initializer_list<std::pair<Uid, std::string_view>> pairs = {
        {1,  "1"},
        { 1, "2"},
    };

    REQUIRE_THROWS(NameAndUidFinder({ pairs }, {}));
}


TEST_CASE("Specification: finder name collision", "[Specification]") {
    const std::initializer_list<std::pair<Uid, std::string_view>> pairs = {
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
        REQUIRE(finder.Find("1", 100) == Uid(1));
        REQUIRE(!finder.Find("1", 101));
        REQUIRE(finder.Find("2", 101) == Uid(2));
        REQUIRE(!finder.Find("2", 100));
    }
}


TEST_CASE("Specification: static column desc", "[Specification]") {
    const auto type = IntegerType(4, false);
    constexpr ColumnDescStatic staticDesc{ "name", true, type };
    const ColumnDesc desc = staticDesc;
    REQUIRE(desc.name == "name");
    REQUIRE(desc.isUnique == true);
    REQUIRE(type_isa<IntegerType>(desc.type));
}


TEST_CASE("Specification: static table desc", "[Specification]") {
    const auto type = IntegerType(4, false);
    constexpr std::array columns = {
        ColumnDescStatic{"name", true, type}
    };
    constexpr TableDescStatic staticDesc{ 1, "table", eTableKind::OBJECT, columns, 2 };
    const TableDesc desc = staticDesc;
    REQUIRE(desc.name == "table");
    REQUIRE(desc.kind == eTableKind::OBJECT);
    REQUIRE(desc.columns.size() == 1);
    REQUIRE(desc.singleRow == Uid(2));
}