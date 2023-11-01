#include <Archive/Conversion.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("TokenBinaryArchive: Save tiny atom positive", "[TokenBinaryArchive]") {
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


TEST_CASE("TokenBinaryArchive: Save tiny atom negative", "[TokenBinaryArchive]") {
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


TEST_CASE("TokenBinaryArchive: Save short atom", "[TokenBinaryArchive]") {
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


TEST_CASE("TokenBinaryArchive: Save medium atom", "[TokenBinaryArchive]") {
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


TEST_CASE("TokenBinaryArchive: Save long atom", "[TokenBinaryArchive]") {
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


TEST_CASE("TokenBinaryArchive: Load tiny atom", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b0110'1010_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::TINY_ATOM);
    REQUIRE(token.isByte == false);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0b1110'1010_b });
}


TEST_CASE("TokenBinaryArchive: Load short atom", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b1011'0001_b, 0xEA_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::SHORT_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("TokenBinaryArchive: Load medium atom", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b1101'1000_b, 0x01_b, 0xEA_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::MEDIUM_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("TokenBinaryArchive: Load long atom", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b1110'0011_b, 0x00_b, 0x00_b, 0x01_b, 0xEA_b };
    Token token;
    FromTokens(bytes, token);
    REQUIRE(token.tag == eTag::LONG_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("TokenBinaryArchive: Save int8", "[TokenBinaryArchive]") {
    int8_t value = 0x0E;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1001'0001_b, 0x0E_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenBinaryArchive: Save uint8", "[TokenBinaryArchive]") {
    uint8_t value = 0x0E;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1000'0001_b, 0x0E_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenBinaryArchive: Save int32", "[TokenBinaryArchive]") {
    int32_t value = 0xDEADBEEF;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1001'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenBinaryArchive: Save uint32", "[TokenBinaryArchive]") {
    uint32_t value = 0xDEADBEEF;
    const auto bytes = ToTokens(value);
    // Short atom.
    const std::vector<std::byte> expected = { 0b1000'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenBinaryArchive: Load int8", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b1001'0001_b, 0x0E_b };
    int8_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0x0E);
}


TEST_CASE("TokenBinaryArchive: Load uint8", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b1000'0001_b, 0x0E_b };
    uint8_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0x0E);
}


TEST_CASE("TokenBinaryArchive: Load int32", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b1001'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    int32_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0xDEADBEEF);
}


TEST_CASE("TokenBinaryArchive: Load uint32", "[TokenBinaryArchive]") {
    const std::vector<std::byte> bytes = { 0b1000'0100_b, 0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b };
    uint32_t value = 0;
    FromTokens(bytes, value);
    REQUIRE(value == 0xDEADBEEF);
}