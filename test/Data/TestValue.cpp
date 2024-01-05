#include <Archive/Conversion.hpp>
#include <Data/Value.hpp>

#include <cereal/archives/json.hpp>

#include <algorithm>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("Value: integrals", "[Value]") {
    constexpr uint16_t raw = 76;
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.Is<Bytes>() == false);
        REQUIRE(value.Is<eCommand>() == false);
        REQUIRE(value.IsInteger() == true);
        REQUIRE(value.Is<List>() == false);
        REQUIRE(value.Is<Named>() == false);
    }
    SECTION("Get success") {
        REQUIRE(value.Get<uint16_t>() == raw);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.Get<Named>(), TypeConversionError);
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

        REQUIRE(value.Is<Bytes>() == false);
        REQUIRE(value.Is<eCommand>() == false);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.Is<List>() == true);
        REQUIRE(value.Is<Named>() == false);
    }
    SECTION("Get success") {
        REQUIRE(value.Get<List>().size() == 3);
        REQUIRE(value.Get<List>()[0].Get<uint16_t>() == raw[0]);
        REQUIRE(value.Get<List>()[1].Get<uint16_t>() == raw[1]);
        REQUIRE(value.Get<List>()[2].Get<uint16_t>() == raw[2]);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.Get<Named>(), TypeConversionError);
    }
}


TEST_CASE("Value: bytes", "[Value]") {
    constexpr std::array raw = { 1_b, 7_b, 4_b };
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.Is<Bytes>() == true);
        REQUIRE(value.Is<eCommand>() == false);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.Is<List>() == false);
        REQUIRE(value.Is<Named>() == false);
    }
    SECTION("Get success") {
        REQUIRE(std::ranges::equal(value.Get<Bytes>(), raw));
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.Get<Named>(), TypeConversionError);
    }
}


TEST_CASE("Value: command", "[Value]") {
    constexpr auto raw = eCommand::CALL;
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.Is<Bytes>() == false);
        REQUIRE(value.Is<eCommand>() == true);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.Is<List>() == false);
        REQUIRE(value.Is<Named>() == false);
    }
    SECTION("Get success") {
        REQUIRE(value.Get<eCommand>() == raw);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.Get<Named>(), TypeConversionError);
    }
}


TEST_CASE("Value: named", "[Value]") {
    const auto raw = Named(1, 2);
    const Value value = raw;

    SECTION("Query type") {
        REQUIRE(value.HasValue() == true);

        REQUIRE(value.Is<Bytes>() == false);
        REQUIRE(value.Is<eCommand>() == false);
        REQUIRE(value.IsInteger() == false);
        REQUIRE(value.Is<List>() == false);
        REQUIRE(value.Is<Named>() == true);
    }
    SECTION("Get success") {
        REQUIRE(value.Get<Named>().name == 1);
        REQUIRE(value.Get<Named>().value == 2);
    }
    SECTION("Get fail") {
        REQUIRE_THROWS_AS(value.Get<List>(), TypeConversionError);
    }
}


TEST_CASE("Value: tokenize command", "[Value]") {
    const auto value = eCommand::CALL;
    const std::vector<Token> tokens = {
        {.tag = eTag::CALL, .isByte = false, .isSigned = false},
    };
    SECTION("Value to native") {
        const auto conv = Tokenize(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto [conv, rest] = DeTokenize(Tokenized<eCommand>{ tokens });
        REQUIRE(conv == value);
    }
}


TEST_CASE("Value: tokenize integer", "[Value]") {
    const auto value = uint16_t(0xFECE);
    const std::vector<Token> tokens = {
        {.tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = false, .data = { 0xFE_b, 0xCE_b }},
    };
    SECTION("Value to native") {
        const auto conv = Tokenize(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto [conv, rest] = DeTokenize(Tokenized<uint16_t>{ tokens });
        REQUIRE(conv == value);
    }
}


TEST_CASE("Value: tokenize bytes", "[Value]") {
    const Bytes value = { 1_b, 2_b, 3_b, 4_b };
    const std::vector<Token> tokens = {
        {.tag = eTag::SHORT_ATOM, .isByte = true, .isSigned = false, .data = { 1_b, 2_b, 3_b, 4_b }},
    };
    SECTION("Value to native") {
        const auto conv = Tokenize(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto [conv, rest] = DeTokenize(Tokenized<Bytes>{ tokens });
        REQUIRE(conv == value);
    }
}


TEST_CASE("Value: tokenize named", "[Value]") {
    const Named value{ uint16_t(1), int16_t(3) };
    const std::vector<Token> tokens = {
        { .tag = eTag::START_NAME },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = false, .data = { 0_b, 1_b } },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 3_b } },
        { .tag = eTag::END_NAME },
    };
    SECTION("Value to native") {
        const auto conv = Tokenize(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto [conv, rest] = DeTokenize(Tokenized<Named>{ tokens });
        REQUIRE(conv.name == value.name);
        REQUIRE(conv.value == value.value);
    }
}


TEST_CASE("Value: tokenize list", "[Value]") {
    const List value = { int16_t(1), int16_t(2) };
    const std::vector<Token> tokens = {
        { .tag = eTag::START_LIST },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 1_b } },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 2_b } },
        { .tag = eTag::END_LIST },
    };
    SECTION("Value to native") {
        const auto conv = Tokenize(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto [conv, rest] = DeTokenize(Tokenized<List>{ tokens });
        REQUIRE(conv == value);
    }
}


TEST_CASE("Value: tokenize empty value", "[Value]") {
    Value value;
    const std::vector<Token> tokens = { };

    SECTION("Value to native") {
        const auto conv = Tokenize(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto [conv, rest] = DeTokenize(Tokenized<Value>{ tokens });
        REQUIRE(conv == value);
    }
}


TEST_CASE("Value: tokenize value", "[Value]") {
    Value value = {
        Value(Bytes{ 1_b, 2_b }),
        eCommand::END_OF_DATA,
        uint32_t(3),
        Value(List{ uint32_t(0), int32_t(1) }),
        Value(Named(uint32_t(1), int32_t(2))),
    };
    const std::vector<Token> tokens = {
        { .tag = eTag::START_LIST },
 // Bytes
        { .tag = eTag::SHORT_ATOM, .isByte = true, .isSigned = false, .data = { 1_b, 2_b } },
 // Command
        { .tag = eTag::END_OF_DATA },
 // Integer
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = false, .data = { 0_b, 0_b, 0_b, 3_b } },
 // List
        { .tag = eTag::START_LIST },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = false, .data = { 0_b, 0_b, 0_b, 0_b } },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 0_b, 0_b, 1_b } },
        { .tag = eTag::END_LIST },
 // Named
        { .tag = eTag::START_NAME },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = false, .data = { 0_b, 0_b, 0_b, 1_b } },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 0_b, 0_b, 2_b } },
        { .tag = eTag::END_NAME },
        { .tag = eTag::END_LIST },
    };

    SECTION("Value to native") {
        const auto conv = Tokenize(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto [conv, rest] = DeTokenize(Tokenized<Value>{ tokens });
        REQUIRE(conv == value);
    }
}