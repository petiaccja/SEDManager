#include <Archive/Types/ValueToNative.hpp>

#include <algorithm>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("value_cast: Value", "[ValueToNative]") {
    const Value value = uint32_t(37);
    const Value native = uint32_t(37);
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: integer", "[ValueToNative]") {
    const Value value = uint32_t(37);
    const auto native = uint32_t(37);
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: string", "[ValueToNative]") {
    const Value value = std::vector{ 0x65_b, 0x66_b };
    const auto native = std::string{ 0x65, 0x66 };
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: string_view", "[ValueToNative]") {
    const Value value = std::vector{ 0x65_b, 0x66_b };
    const auto native = std::string_view{ "\x65\x66" };
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: Uid", "[ValueToNative]") {
    const Value value = std::array{ 0_b, 0_b, 0_b, 0_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    const auto native = Uid(0xDEADBEEF);
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: CellBlock", "[ValueToNative]") {
    const Value value = {
        Named{uint16_t(1),  std::array{ 0_b, 0_b, 0_b, 0_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b }},
        Named{ uint16_t(2), uint32_t(20)                                                    },
        Named{ uint16_t(3), uint32_t(30)                                                    },
        Named{ uint16_t(4), uint32_t(40)                                                    },
    };
    const auto native = CellBlock{
        .startRow = Uid(0xDEADBEEF),
        .endRow = 20,
        .startColumn = 30,
        .endColumn = 40,
    };
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: byte range", "[ValueToNative]") {
    const Value value = std::vector{ 0xBE_b, 0xEF_b };
    const auto native = std::vector{ 0xBE_b, 0xEF_b };
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: Value range", "[ValueToNative]") {
    const Value value = std::vector{ 4, 5 };
    const auto native = std::vector{ 4, 5 };
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        const auto conv = value_cast(native);
        REQUIRE(conv == value);
    }
}


TEST_CASE("value_cast: unordered_map", "[ValueToNative]") {
    const Value value = {
        Named{int32_t(4),  int32_t(5)},
        Named{ int32_t(6), int32_t(7)},
    };
    const auto native = std::unordered_map<int32_t, int32_t>{
        {4,  5},
        { 6, 7},
    };
    SECTION("Value to native") {
        const auto conv = value_cast<std::decay_t<decltype(native)>>(value);
        REQUIRE(conv == native);
    }
    SECTION("native to Value") {
        auto conv = value_cast(native);
        std::ranges::sort(conv.Get<List>(), [](const Value& lhs, const Value& rhs) {
            return lhs.Get<Named>().name.Get<uint16_t>() < rhs.Get<Named>().name.Get<uint16_t>();
        });
        REQUIRE(conv == value);
    }
}