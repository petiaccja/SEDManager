#include <Archive/Conversion.hpp>
#include <Archive/TokenBinaryArchive.hpp>
#include <Archive/Types/ValueToToken.hpp>

#include <cereal/archives/json.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>


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
    const auto& values = arg.GetList();
    REQUIRE(!values.empty());
    return values[0];
}

using JSONArchivePair = std::tuple<cereal::JSONOutputArchive, cereal::JSONInputArchive>;
using TokenArchivePair = std::tuple<TokenBinaryOutputArchive, TokenBinaryInputArchive>;


TEMPLATE_TEST_CASE("Serialize command", "[Value]", JSONArchivePair, TokenArchivePair) {
    Value in = eCommand::CALL;
    Value out = GetFirstInList(CycleSerialization<TestType>(in));
    REQUIRE(out.GetCommand() == eCommand::CALL);
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