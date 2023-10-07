#include <TPerLib/Serialization/TokenArchive.hpp>
#include <TPerLib/Communication/Value.hpp>

#include <cereal/archives/json.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

//------------------------------------------------------------------------------
// Integrals
//------------------------------------------------------------------------------

TEST_CASE("Integral set/get same", "[Value]") {
    Value s = uint16_t(1);
    REQUIRE(s.Get<uint16_t>() == 1);
}

TEST_CASE("Integral set/get cast", "[Value]") {
    Value s = uint16_t(1);
    REQUIRE(s.Get<uint32_t>() == 1);
}

TEST_CASE("Integral set/get fail", "[Value]") {
    Value s = uint16_t(1);
    REQUIRE_THROWS(s.Get<std::span<const uint8_t>>());
}

TEST_CASE("Bool set/get same", "[Value]") {
    Value s = true;
    REQUIRE(s.Get<bool>() == true);
}

TEST_CASE("Char set/get same", "[Value]") {
    Value s = char(1);
    REQUIRE(s.Get<char>() == char(1));
}

//------------------------------------------------------------------------------
// Bytes
//------------------------------------------------------------------------------

TEST_CASE("Bytes span set/get same", "[Value]") {
    std::array values{ 1, 2, 3 };
    Value s = { Value::bytes, values };
    REQUIRE(s.Get<std::span<const int>>()[0] == 1);
    REQUIRE(s.Get<std::span<const int>>()[1] == 2);
    REQUIRE(s.Get<std::span<const int>>()[2] == 3);
}

TEST_CASE("Bytes string set/get same", "[Value]") {
    Value s = { Value::bytes, std::string_view("text") };
    const auto chars = s.Get<std::span<const char>>();
    REQUIRE(std::string_view(chars.begin(), chars.end()) == "text");
}

TEST_CASE("Bytes set/get fail", "[Value]") {
    std::array values{ 1, 2, 3 };
    Value s = { Value::bytes, values };
    REQUIRE_THROWS(s.Get<int>());
}

//------------------------------------------------------------------------------
// Lists
//------------------------------------------------------------------------------

TEST_CASE("List set/get same", "[Value]") {
    std::array values{ 1, 2, 3 };
    Value s = values;
    const auto r = s.Get<std::span<const Value>>();
    std::cout << s.Type().name() << std::endl;
    std::cout << r[0].Type().name() << std::endl;
    REQUIRE(r[0].Get<int>() == 1);
    REQUIRE(r[1].Get<int>() == 2);
    REQUIRE(r[2].Get<int>() == 3);
}

TEST_CASE("List set/get fail", "[Value]") {
    std::array values{ 1, 2, 3 };
    Value s = values;
    REQUIRE_THROWS(s.Get<int>());
}

//------------------------------------------------------------------------------
// Named
//------------------------------------------------------------------------------

TEST_CASE("Named set/get same", "[Value]") {
    using namespace std::string_view_literals;
    Value s = Named{ { Value::bytes, "name"sv }, 1 };
    const auto& v = s.Get<Named>();
    REQUIRE(v.name.Get<std::string_view>() == "name");
    REQUIRE(v.value.Get<int>() == 1);
}

TEST_CASE("Named set/get fail", "[Value]") {
    using namespace std::string_view_literals;
    Value s = Named{ { Value::bytes, "name"sv }, 1 };
    REQUIRE_THROWS(s.Get<int>());
}

//------------------------------------------------------------------------------
// Commands
//------------------------------------------------------------------------------

TEST_CASE("Command set/get same", "[Value]") {
    Value s = eCommand::CALL;
    REQUIRE(s.Get<eCommand>() == eCommand::CALL);
}

TEST_CASE("Command set/get fail", "[Value]") {
    Value s = eCommand::CALL;
    REQUIRE_THROWS(s.Get<int>());
}

//------------------------------------------------------------------------------
// Query
//------------------------------------------------------------------------------


TEST_CASE("Query int", "[Value]") {
    Value y = 1;
    Value n = "asd";
    REQUIRE(y.IsInteger());
    REQUIRE(!n.IsInteger());
}

TEST_CASE("Query bytes", "[Value]") {
    Value y = { Value::bytes, "asd" };
    Value n = "asd";
    REQUIRE(y.IsBytes());
    REQUIRE(!n.IsBytes());
}

TEST_CASE("Query named", "[Value]") {
    Value y = Named{ "asd", 1 };
    Value n = "asd";
    REQUIRE(y.IsNamed());
    REQUIRE(!n.IsNamed());
}

TEST_CASE("Query list", "[Value]") {
    Value y = { 1, 2 };
    Value n = 1;
    REQUIRE(y.IsList());
    REQUIRE(!n.IsList());
}

TEST_CASE("Query command", "[Value]") {
    Value y = eCommand::CALL;
    Value n = 1;
    REQUIRE(y.IsCommand());
    REQUIRE(!n.IsCommand());
}


//------------------------------------------------------------------------------
// Serialization
//------------------------------------------------------------------------------

template <class ArchivePair>
static Value CycleSerialization(const Value& in) {
    using OutputArchive = std::tuple_element_t<0, ArchivePair>;
    using InputArchive = std::tuple_element_t<1, ArchivePair>;

    constexpr bool isBinary = !std::is_base_of_v<cereal::traits::TextArchive, OutputArchive>;
    std::stringstream ss = isBinary
                               ? std::stringstream(std::ios::binary | std::ios::out | std::ios::in)
                               : std::stringstream{ std::ios::out | std::ios::in };
    {
        OutputArchive outAr(ss);
        outAr(in);
    }
    {
        InputArchive inAr(ss);
        Value out;
        inAr(out);
        return out;
    }
}

static Value GetFirstInList(const Value& arg) {
    REQUIRE(arg.IsList());
    const auto& values = arg.AsList();
    REQUIRE(!values.empty());
    return values[0];
}

using JSONArchivePair = std::tuple<cereal::JSONOutputArchive, cereal::JSONInputArchive>;
using TokenArchivePair = std::tuple<TokenOutputArchive, TokenInputArchive>;


TEMPLATE_TEST_CASE("Serialize command", "[Value]", JSONArchivePair, TokenArchivePair) {
    Value in = eCommand::CALL;
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    REQUIRE(out.AsCommand() == eCommand::CALL);
}

TEMPLATE_TEST_CASE("Serialize int16", "[Value]", JSONArchivePair, TokenArchivePair) {
    Value in = int16_t(1);
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    REQUIRE(out.Type() == typeid(int16_t));
    REQUIRE(out.Get<int16_t>() == 1);
}

TEMPLATE_TEST_CASE("Serialize uint32", "[Value]", JSONArchivePair, TokenArchivePair) {
    Value in = uint32_t(1);
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    REQUIRE(out.Type() == typeid(uint32_t));
    REQUIRE(out.Get<uint32_t>() == 1);
}

TEMPLATE_TEST_CASE("Serialize bytes", "[Value]", JSONArchivePair, TokenArchivePair) {
    const auto bytes = std::array<uint8_t, 3>{ 4, 5, 6 };
    Value in = { Value::bytes, bytes };
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto r = out.Get<std::span<uint8_t>>();
    REQUIRE(r[0] == 4);
    REQUIRE(r[1] == 5);
    REQUIRE(r[2] == 6);
}

TEMPLATE_TEST_CASE("Serialize list", "[Value]", JSONArchivePair, TokenArchivePair) {
    const auto values = std::array<uint8_t, 3>{ 4, 5 };
    Value in = values;
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto& r = out.Get<std::span<Value>>();
    REQUIRE(r[0].Get<int>() == 4);
    REQUIRE(r[1].Get<int>() == 5);
}

TEMPLATE_TEST_CASE("Serialize named", "[Value]", JSONArchivePair, TokenArchivePair) {
    using namespace std::string_view_literals;
    Value in = Named{ { Value::bytes, "steve"sv }, 2 };
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto& r = out.Get<Named>();
    REQUIRE(r.name.Get<std::string_view>() == "steve");
    REQUIRE(r.value.Get<int>() == 2);
}