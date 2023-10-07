#include <TPerLib/Serialization/Utility.hpp>
#include <catch2/catch_test_macros.hpp>



TEST_CASE("Save tiny atom positive", "[Token stream archive]") {
    Token token{
        .tag = eTag::TINY_ATOM,
        .isByte = false,
        .isSigned = true,
        .data = { 0b10'1010 }
    };
    const auto bytes = ToTokens(token);
    const std::vector<uint8_t> expected = { 0b0100'1010 };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save tiny atom negative", "[Token stream archive]") {
    Token token{
        .tag = eTag::TINY_ATOM,
        .isByte = false,
        .isSigned = true,
        .data = { 0b1111'1010 }
    };
    const auto bytes = ToTokens(token);
    const std::vector<uint8_t> expected = { 0b0111'1010 };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save short atom", "[Token stream archive]") {
    Token token{
        .tag = eTag::SHORT_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA }
    };
    const auto bytes = ToTokens(token);
    const std::vector<uint8_t> expected = { 0b1011'0001, 0xEA };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save medium atom", "[Token stream archive]") {
    Token token{
        .tag = eTag::MEDIUM_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA }
    };
    const auto bytes = ToTokens(token);
    const std::vector<uint8_t> expected = { 0b1101'1000, 0x01, 0xEA };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save long atom", "[Token stream archive]") {
    Token token{
        .tag = eTag::LONG_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA }
    };
    const auto bytes = ToTokens(token);
    const std::vector<uint8_t> expected = { 0b1110'0011, 0x00, 0x00, 0x01, 0xEA };
    REQUIRE(bytes == expected);
}


TEST_CASE("Load tiny atom", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b0110'1010 };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::TINY_ATOM);
    REQUIRE(token.isByte == false);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector<uint8_t>{ 0b1110'1010 });
}


TEST_CASE("Load short atom", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b1011'0001, 0xEA };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::SHORT_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector<uint8_t>{ 0xEA });
}


TEST_CASE("Load medium atom", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b1101'1000, 0x01, 0xEA };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::MEDIUM_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector<uint8_t>{ 0xEA });
}


TEST_CASE("Load long atom", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b1110'0011, 0x00, 0x00, 0x01, 0xEA };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::LONG_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector<uint8_t>{ 0xEA });
}


TEST_CASE("Save int8", "[Token stream archive]") {
    int8_t value = 0x0E;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<uint8_t> expected = { 0b1001'0001, 0x0E };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save uint8", "[Token stream archive]") {
    uint8_t value = 0x0E;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<uint8_t> expected = { 0b1000'0001, 0x0E };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save int32", "[Token stream archive]") {
    int32_t value = 0xDEADBEEF;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<uint8_t> expected = { 0b1001'0100, 0xDE, 0xAD, 0xBE, 0xEF };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save uint32", "[Token stream archive]") {
    uint32_t value = 0xDEADBEEF;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<uint8_t> expected = { 0b1000'0100, 0xDE, 0xAD, 0xBE, 0xEF };
    REQUIRE(bytes == expected);
}


TEST_CASE("Load int8", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b1001'0001, 0x0E };
    int8_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0x0E);
}


TEST_CASE("Load uint8", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b1000'0001, 0x0E };
    uint8_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0x0E);
}


TEST_CASE("Load int32", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b1001'0100, 0xDE, 0xAD, 0xBE, 0xEF };
    int32_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0xDEADBEEF);
}


TEST_CASE("Load uint32", "[Token stream archive]") {
    const std::vector<uint8_t> bytes = { 0b1000'0100, 0xDE, 0xAD, 0xBE, 0xEF };
    uint32_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0xDEADBEEF);
}