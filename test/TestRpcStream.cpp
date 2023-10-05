#include <TPerLib/Serialization/RpcStream.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iostream>

//------------------------------------------------------------------------------
// Integrals
//------------------------------------------------------------------------------

TEST_CASE("Integral set/get same", "[RpcStream]") {
    RpcStream s = uint16_t(1);
    REQUIRE(s.Get<uint16_t>() == 1);
}

TEST_CASE("Integral set/get cast", "[RpcStream]") {
    RpcStream s = uint16_t(1);
    REQUIRE(s.Get<uint32_t>() == 1);
}

TEST_CASE("Integral set/get fail", "[RpcStream]") {
    RpcStream s = uint16_t(1);
    REQUIRE_THROWS(s.Get<std::span<const uint8_t>>());
}

TEST_CASE("Bool set/get same", "[RpcStream]") {
    RpcStream s = true;
    REQUIRE(s.Get<bool>() == true);
}

TEST_CASE("Char set/get same", "[RpcStream]") {
    RpcStream s = char(1);
    REQUIRE(s.Get<char>() == char(1));
}

//------------------------------------------------------------------------------
// Bytes
//------------------------------------------------------------------------------

TEST_CASE("Bytes span set/get same", "[RpcStream]") {
    std::array values{ 1, 2, 3 };
    RpcStream s = { RpcStream::bytes, values };
    REQUIRE(s.Get<std::span<const int>>()[0] == 1);
    REQUIRE(s.Get<std::span<const int>>()[1] == 2);
    REQUIRE(s.Get<std::span<const int>>()[2] == 3);
}

TEST_CASE("Bytes string set/get same", "[RpcStream]") {
    RpcStream s = { RpcStream::bytes, std::string_view("text") };
    const auto chars = s.Get<std::span<const char>>();
    REQUIRE(std::string_view(chars.begin(), chars.end()) == "text");
}

TEST_CASE("Bytes set/get fail", "[RpcStream]") {
    std::array values{ 1, 2, 3 };
    RpcStream s = { RpcStream::bytes, values };
    REQUIRE_THROWS(s.Get<int>());
}

//------------------------------------------------------------------------------
// Lists
//------------------------------------------------------------------------------

TEST_CASE("List set/get same", "[RpcStream]") {
    std::array values{ 1, 2, 3 };
    RpcStream s = values;
    const auto r = s.Get<std::span<const RpcStream>>();
    std::cout << s.Type().name() << std::endl;
    std::cout << r[0].Type().name() << std::endl;
    REQUIRE(r[0].Get<int>() == 1);
    REQUIRE(r[1].Get<int>() == 2);
    REQUIRE(r[2].Get<int>() == 3);
}

TEST_CASE("List set/get fail", "[RpcStream]") {
    std::array values{ 1, 2, 3 };
    RpcStream s = values;
    REQUIRE_THROWS(s.Get<int>());
}

//------------------------------------------------------------------------------
// Named
//------------------------------------------------------------------------------

TEST_CASE("Named set/get same", "[RpcStream]") {
    RpcStream s = Named{ "name", 1 };
    const auto& v = s.Get<Named>();
    REQUIRE(v.name == "name");
    REQUIRE(v.value.Get<int>() == 1);
}

TEST_CASE("Named set/get fail", "[RpcStream]") {
    RpcStream s = Named{ "name", 1 };
    REQUIRE_THROWS(s.Get<int>());
}

//------------------------------------------------------------------------------
// Commands
//------------------------------------------------------------------------------

TEST_CASE("Command set/get same", "[RpcStream]") {
    RpcStream s = eCommand::CALL;
    REQUIRE(s.Get<eCommand>() == eCommand::CALL);
}

TEST_CASE("Command set/get fail", "[RpcStream]") {
    RpcStream s = eCommand::CALL;
    REQUIRE_THROWS(s.Get<int>());
}
