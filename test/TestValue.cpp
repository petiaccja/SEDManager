#include <Archive/Conversion.hpp>
#include <Archive/TokenArchive.hpp>
#include <RPC/Value.hpp>

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
    REQUIRE_THROWS(s.Get<std::span<const std::byte>>());
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

TEST_CASE("Bytes set/get same", "[Value]") {
    std::array bytes{ 1_b, 2_b, 3_b };
    Value s = bytes;
    REQUIRE(s.Get<std::span<const std::byte>>()[0] == 1_b);
    REQUIRE(s.Get<std::span<const std::byte>>()[1] == 2_b);
    REQUIRE(s.Get<std::span<const std::byte>>()[2] == 3_b);
}

TEST_CASE("Bytes set/get fail", "[Value]") {
    std::array bytes{ 1_b, 2_b, 3_b };
    Value s = bytes;
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
    Value s = Named{ std::as_bytes(std::span("name"sv)), 1 };
    const auto& v = s.Get<Named>();
    REQUIRE(ToStringView(v.name.Get<std::span<const std::byte>>()) == "name");
    REQUIRE(v.value.Get<int>() == 1);
}

TEST_CASE("Named set/get fail", "[Value]") {
    using namespace std::string_view_literals;
    Value s = Named{ std::as_bytes(std::span("name"sv)), 1 };
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
    Value y = std::array{ 1_b };
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
    Value in = std::array{ 4_b, 5_b, 6_b };
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto r = out.Get<std::span<std::byte>>();
    REQUIRE(r[0] == 4_b);
    REQUIRE(r[1] == 5_b);
    REQUIRE(r[2] == 6_b);
}

TEMPLATE_TEST_CASE("Serialize list", "[Value]", JSONArchivePair, TokenArchivePair) {
    const auto values = std::array{ 4, 5 };
    Value in = values;
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    const auto& r = out.Get<std::span<Value>>();
    REQUIRE(r[0].Get<int>() == 4);
    REQUIRE(r[1].Get<int>() == 5);
}

TEMPLATE_TEST_CASE("Serialize named", "[Value]", JSONArchivePair, TokenArchivePair) {
    using namespace std::string_view_literals;
    std::vector name{ 0_b, 1_b, 2_b };
    Value in = Named{ name, 2 };
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    auto& r = out.Get<Named>();
    REQUIRE(r.name.Get<std::vector<std::byte>>() == name);
    REQUIRE(r.value.Get<int>() == 2);
}