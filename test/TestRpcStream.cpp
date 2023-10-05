#include <TPerLib/Serialization/RpcArchive.hpp>
#include <TPerLib/Serialization/RpcStream.hpp>

#include <cereal/archives/json.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

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

//------------------------------------------------------------------------------
// Query
//------------------------------------------------------------------------------


TEST_CASE("Query int", "[RpcStream]") {
    RpcStream y = 1;
    RpcStream n = "asd";
    REQUIRE(y.IsInteger());
    REQUIRE(!n.IsInteger());
}

TEST_CASE("Query bytes", "[RpcStream]") {
    RpcStream y = { RpcStream::bytes, "asd" };
    RpcStream n = "asd";
    REQUIRE(y.IsBytes());
    REQUIRE(!n.IsBytes());
}

TEST_CASE("Query named", "[RpcStream]") {
    RpcStream y = Named{ "asd", 1 };
    RpcStream n = "asd";
    REQUIRE(y.IsNamed());
    REQUIRE(!n.IsNamed());
}

TEST_CASE("Query list", "[RpcStream]") {
    RpcStream y = { 1, 2 };
    RpcStream n = 1;
    REQUIRE(y.IsList());
    REQUIRE(!n.IsList());
}

TEST_CASE("Query command", "[RpcStream]") {
    RpcStream y = eCommand::CALL;
    RpcStream n = 1;
    REQUIRE(y.IsCommand());
    REQUIRE(!n.IsCommand());
}


//------------------------------------------------------------------------------
// Serialization
//------------------------------------------------------------------------------

template <class ArchivePair>
static RpcStream CycleSerialization(const RpcStream& in) {
    using OutputArchive = std::tuple_element_t<0, ArchivePair>;
    using InputArchive = std::tuple_element_t<1, ArchivePair>;

    std::stringstream ss;
    {
        OutputArchive outAr(ss);
        outAr(in);
    }
    {
        InputArchive inAr(ss);
        RpcStream out;
        inAr(out);
        return out;
    }
}

static RpcStream GetFirstInList(const RpcStream& arg) {
    REQUIRE(arg.IsList());
    const auto& values = arg.AsList();
    REQUIRE(!values.empty());
    return values[0];
}

using JSONArchivePair = std::tuple<cereal::JSONOutputArchive, cereal::JSONInputArchive>;
using RPCArchivePair = std::tuple<RpcOutputArchive, RpcInputArchive>;


TEMPLATE_TEST_CASE("Serialize command", "[RpcStream]", JSONArchivePair, RPCArchivePair) {
    RpcStream in = eCommand::CALL;
    RpcStream out = GetFirstInList(CycleSerialization<TestType>(in));
    REQUIRE(out.AsCommand() == eCommand::CALL);
}

TEMPLATE_TEST_CASE("Serialize int16", "[RpcStream]", JSONArchivePair, RPCArchivePair) {
    RpcStream in = int16_t(1);
    RpcStream out = GetFirstInList(CycleSerialization<TestType>(in));
    REQUIRE(out.Type() == typeid(int16_t));
    REQUIRE(out.Get<int16_t>() == 1);
}

TEMPLATE_TEST_CASE("Serialize uint32", "[RpcStream]", JSONArchivePair, RPCArchivePair) {
    RpcStream in = uint32_t(1);
    RpcStream out = GetFirstInList(CycleSerialization<TestType>(in));
    REQUIRE(out.Type() == typeid(uint32_t));
    REQUIRE(out.Get<uint32_t>() == 1);
}

TEMPLATE_TEST_CASE("Serialize bytes", "[RpcStream]", JSONArchivePair, RPCArchivePair) {
    const auto bytes = std::array<uint8_t, 3>{ 4, 5, 6 };
    RpcStream in = { RpcStream::bytes, bytes };
    RpcStream out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto r = out.Get<std::span<uint8_t>>();
    REQUIRE(r[0] == 4);
    REQUIRE(r[1] == 5);
    REQUIRE(r[2] == 6);
}

TEMPLATE_TEST_CASE("Serialize list", "[RpcStream]", JSONArchivePair, RPCArchivePair) {
    const auto values = std::array<uint8_t, 3>{ 4, 5 };
    RpcStream in = values;
    RpcStream out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto& r = out.Get<std::span<RpcStream>>();
    REQUIRE(r[0].Get<int>() == 4);
    REQUIRE(r[1].Get<int>() == 5);
}

TEMPLATE_TEST_CASE("Serialize named", "[RpcStream]", JSONArchivePair, RPCArchivePair) {
    RpcStream in = Named{ "steve", 2 };
    RpcStream out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto& r = out.Get<Named>();
    REQUIRE(r.name == "steve");
    REQUIRE(r.value.Get<int>() == 2);
}