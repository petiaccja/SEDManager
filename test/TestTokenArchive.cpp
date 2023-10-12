#include <TPerLib/Serialization/Utility.hpp>

#include <catch2/catch_test_macros.hpp>



TEST_CASE("Save tiny atom positive", "[Token stream archive]") {
    Token token{
        .tag = eTag::TINY_ATOM,
        .isByte = false,
        .isSigned = true,
        .data = { 0b10'1010_b }
    };
    const auto bytes = ToTokens(token);
    const std::vector<std::byte> expected = { 0b0100'1010_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save tiny atom negative", "[Token stream archive]") {
    Token token{
        .tag = eTag::TINY_ATOM,
        .isByte = false,
        .isSigned = true,
        .data = { 0b1111'1010_b }
    };
    const auto bytes = ToTokens(token);
    const std::vector<std::byte> expected = { 0b0111'1010_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save short atom", "[Token stream archive]") {
    Token token{
        .tag = eTag::SHORT_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA_b }
    };
    const auto bytes = ToTokens(token);
    const std::vector<std::byte> expected = { 0b1011'0001_b, 0xEA_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save medium atom", "[Token stream archive]") {
    Token token{
        .tag = eTag::MEDIUM_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA_b }
    };
    const auto bytes = ToTokens(token);
    const std::vector<std::byte> expected = { 0b1101'1000_b, 0x01_b, 0xEA_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save long atom", "[Token stream archive]") {
    Token token{
        .tag = eTag::LONG_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA_b }
    };
    const auto bytes = ToTokens(token);
    const std::vector<std::byte> expected = { 0b1110'0011_b, 0x00_b, 0x00_b, 0x01_b, 0xEA_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Load tiny atom", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b0110'1010_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::TINY_ATOM);
    REQUIRE(token.isByte == false);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0b1110'1010_b });
}


TEST_CASE("Load short atom", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b1011'0001_b, 0xEA_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::SHORT_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("Load medium atom", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b1101'1000_b, 0x01_b, 0xEA_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::MEDIUM_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("Load long atom", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b1110'0011_b, 0x00_b, 0x00_b, 0x01_b, 0xEA_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::LONG_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("Save int8", "[Token stream archive]") {
    int8_t value = 0x0E;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1001'0001_b, 0x0E_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save uint8", "[Token stream archive]") {
    uint8_t value = 0x0E;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1000'0001_b, 0x0E_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save int32", "[Token stream archive]") {
    int32_t value = 0xDEADBEEF;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1001'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Save uint32", "[Token stream archive]") {
    uint32_t value = 0xDEADBEEF;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1000'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("Load int8", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b1001'0001_b, 0x0E_b };
    int8_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0x0E);
}


TEST_CASE("Load uint8", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b1000'0001_b, 0x0E_b };
    uint8_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0x0E);
}


TEST_CASE("Load int32", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b1001'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    int32_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0xDEADBEEF);
}


TEST_CASE("Load uint32", "[Token stream archive]") {
    const std::vector<std::byte> bytes = { 0b1000'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    uint32_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0xDEADBEEF);
}