#include <Archive/Conversion.hpp>
#include <Archive/TokenBinaryArchive.hpp>
#include <Archive/Types/ValueToToken.hpp>
#include <Data/Value.hpp>

#include <cereal/archives/json.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>



TEST_CASE("Value: integrals", "[Value]") {
    constexpr uint16_t raw = 76;
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.IsBytes() == false);
        REQUIRE(value.IsCommand() == false);
        REQUIRE(value.IsInteger() == true);
        REQUIRE(value.IsList() == false);
        REQUIRE(value.IsNamed() == false);
    }
    SECTION("Get success") {
        REQUIRE(value.Get<uint16_t>() == raw);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.GetNamed(), TypeConversionError);
    }
    SECTION("Get cast") {
        REQUIRE(value.Get<int64_t>() == raw);
    }
}


TEST_CASE("Value: list", "[Value]") {
    constexpr std::array<uint16_t, 3> raw = { 1, 7, 4 };
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.IsBytes() == false);
        REQUIRE(value.IsCommand() == false);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.IsList() == true);
        REQUIRE(value.IsNamed() == false);
    }
    SECTION("Get success") {
        REQUIRE(value.GetList().size() == 3);
        REQUIRE(value.GetList()[0].GetInt<uint16_t>() == raw[0]);
        REQUIRE(value.GetList()[1].GetInt<uint16_t>() == raw[1]);
        REQUIRE(value.GetList()[2].GetInt<uint16_t>() == raw[2]);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.GetNamed(), TypeConversionError);
    }
}


TEST_CASE("Value: bytes", "[Value]") {
    constexpr std::array raw = { 1_b, 7_b, 4_b };
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.IsBytes() == true);
        REQUIRE(value.IsCommand() == false);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.IsList() == false);
        REQUIRE(value.IsNamed() == false);
    }
    SECTION("Get success") {
        REQUIRE(std::ranges::equal(value.GetBytes(), raw));
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.GetNamed(), TypeConversionError);
    }
}


TEST_CASE("Value: command", "[Value]") {
    constexpr auto raw = eCommand::CALL;
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.IsBytes() == false);
        REQUIRE(value.IsCommand() == true);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.IsList() == false);
        REQUIRE(value.IsNamed() == false);
    }
    SECTION("Get success") {
        REQUIRE(value.GetCommand() == raw);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.GetNamed(), TypeConversionError);
    }
}


TEST_CASE("Value: named", "[Value]") {
    const auto raw = Named(1, 2);
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.IsBytes() == false);
        REQUIRE(value.IsCommand() == false);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.IsList() == false);
        REQUIRE(value.IsNamed() == true);
    }
    SECTION("Get success") {
        REQUIRE(value.GetNamed().name == 1);
        REQUIRE(value.GetNamed().value == 2);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.GetList(), TypeConversionError);
    }
}
