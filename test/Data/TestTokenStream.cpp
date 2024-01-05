#include <Archive/Conversion.hpp>
#include <Data/TokenStream.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("TokenStream: Save tiny atom positive", "[TokenStream]") {
    Token token{
        .tag = eTag::TINY_ATOM,
        .isByte = false,
        .isSigned = true,
        .data = { 0b10'1010_b }
    };
    const auto bytes = Serialize(token);
    const std::vector<std::byte> expected = { 0b0100'1010_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenStream: Save tiny atom negative", "[TokenStream]") {
    Token token{
        .tag = eTag::TINY_ATOM,
        .isByte = false,
        .isSigned = true,
        .data = { 0b1111'1010_b }
    };
    const auto bytes = Serialize(token);
    const std::vector<std::byte> expected = { 0b0111'1010_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenStream: Save short atom", "[TokenStream]") {
    Token token{
        .tag = eTag::SHORT_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA_b }
    };
    const auto bytes = Serialize(token);
    const std::vector<std::byte> expected = { 0b1011'0001_b, 0xEA_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenStream: Save medium atom", "[TokenStream]") {
    Token token{
        .tag = eTag::MEDIUM_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA_b }
    };
    const auto bytes = Serialize(token);
    const std::vector<std::byte> expected = { 0b1101'1000_b, 0x01_b, 0xEA_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenStream: Save long atom", "[TokenStream]") {
    Token token{
        .tag = eTag::LONG_ATOM,
        .isByte = true,
        .isSigned = true,
        .data = { 0xEA_b }
    };
    const auto bytes = Serialize(token);
    const std::vector<std::byte> expected = { 0b1110'0011_b, 0x00_b, 0x00_b, 0x01_b, 0xEA_b };
    REQUIRE(bytes == expected);
}


TEST_CASE("TokenStream: Load tiny atom", "[TokenStream]") {
    const std::vector<std::byte> bytes = { 0b0110'1010_b };
    const Token token = DeSerialize(Serialized<Token>{ bytes }).first;
    REQUIRE(token.tag == eTag::TINY_ATOM);
    REQUIRE(token.isByte == false);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0b1110'1010_b });
}


TEST_CASE("TokenStream: Load short atom", "[TokenStream]") {
    const std::vector<std::byte> bytes = { 0b1011'0001_b, 0xEA_b };
    const Token token = DeSerialize(Serialized<Token>{ bytes }).first;
    REQUIRE(token.tag == eTag::SHORT_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("TokenStream: Load medium atom", "[TokenStream]") {
    const std::vector<std::byte> bytes = { 0b1101'1000_b, 0x01_b, 0xEA_b };
    const Token token = DeSerialize(Serialized<Token>{ bytes }).first;
    REQUIRE(token.tag == eTag::MEDIUM_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}


TEST_CASE("TokenStream: Load long atom", "[TokenStream]") {
    const std::vector<std::byte> bytes = { 0b1110'0011_b, 0x00_b, 0x00_b, 0x01_b, 0xEA_b };
    const Token token = DeSerialize(Serialized<Token>{ bytes }).first;
    REQUIRE(token.tag == eTag::LONG_ATOM);
    REQUIRE(token.isByte == true);
    REQUIRE(token.isSigned == true);
    REQUIRE(token.data == std::vector{ 0xEA_b });
}
