#include <Archive/Conversion.hpp>
#include <Archive/TokenBinaryArchive.hpp>
#include <Archive/Types/ValueToToken.hpp>

#include <cereal/archives/json.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("ValueToToken: empty", "[ValueToToken]") {
    const Value value;
    const std::vector<Token> tokens = {};
    SECTION("Value to native") {
        const auto conv = ValueToTokens(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto conv = TokensToValue(tokens);
        REQUIRE(conv.GetList().empty());
    }
}


TEST_CASE("ValueToToken: integer", "[ValueToToken]") {
    const Value value = uint32_t(1);
    const std::vector<Token> tokens = {
        {.tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = false, .data = { 0_b, 0_b, 0_b, 1_b }},
    };
    SECTION("Value to native") {
        const auto conv = ValueToTokens(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto conv = TokensToValue(tokens);
        REQUIRE(conv.GetList()[0] == value);
    }
}


TEST_CASE("ValueToToken: command", "[ValueToToken]") {
    const Value value = eCommand::CALL;
    const std::vector<Token> tokens = {
        {.tag = eTag::CALL, .isByte = false, .isSigned = false},
    };
    SECTION("Value to native") {
        const auto conv = ValueToTokens(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto conv = TokensToValue(tokens);
        REQUIRE(conv.GetList()[0] == value);
    }
}


TEST_CASE("ValueToToken: bytes", "[ValueToToken]") {
    const Value value = std::array{ 1_b, 2_b, 3_b, 4_b };
    const std::vector<Token> tokens = {
        {.tag = eTag::SHORT_ATOM, .isByte = true, .isSigned = false, .data = { 1_b, 2_b, 3_b, 4_b }},
    };
    SECTION("Value to native") {
        const auto conv = ValueToTokens(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto conv = TokensToValue(tokens);
        REQUIRE(conv.GetList()[0] == value);
    }
}


TEST_CASE("ValueToToken: named", "[ValueToToken]") {
    const Value value = Named{ uint16_t(1), int16_t(3) };
    const std::vector<Token> tokens = {
        { .tag = eTag::START_NAME },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = false, .data = { 0_b, 1_b } },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 3_b } },
        { .tag = eTag::END_NAME },
    };
    SECTION("Value to native") {
        const auto conv = ValueToTokens(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto conv = TokensToValue(tokens);
        REQUIRE(conv.GetList()[0] == value);
    }
}


TEST_CASE("ValueToToken: list", "[ValueToToken]") {
    const Value value = { int16_t(1), int16_t(2) };
    const std::vector<Token> tokens = {
        { .tag = eTag::START_LIST },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 1_b } },
        { .tag = eTag::SHORT_ATOM, .isByte = false, .isSigned = true, .data = { 0_b, 2_b } },
        { .tag = eTag::END_LIST },
    };
    SECTION("Value to native") {
        const auto conv = ValueToTokens(value);
        REQUIRE(conv == tokens);
    }
    SECTION("native to Value") {
        const auto conv = TokensToValue(tokens);
        REQUIRE(conv.GetList()[0] == value);
    }
}


TEST_CASE("ValueToToken: serialization", "[ValueToToken]") {
    std::stringstream ss(std::ios::binary | std::ios::out | std::ios::in);

    Value value = {
        { Named(1, 2) },
        eCommand::END_OF_DATA,
        { 0, 0, 0 },
    };
    Value reconstructued;

    {
        TokenBinaryOutputArchive outAr(ss);
        outAr(value);
    }
    {
        TokenBinaryInputArchive inAr(ss);
        inAr(reconstructued);
    }
    REQUIRE(reconstructued.GetList()[0] == value);
}