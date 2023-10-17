#include <Archive/Conversion.hpp>
#include <UI/ValueToJSON.hpp>

#include <catch2/catch_test_macros.hpp>

constexpr uint64_t id_bytes_2 = 0x01;
constexpr uint64_t id_uinteger_4 = 0x02;
constexpr uint64_t id_optional_bytes_2 = 0x101;
constexpr uint64_t id_optional_uinteger_4 = 0x102;


const Type bytes_2 = IdentifiedType<BytesType, id_bytes_2>(2, true);
const Type uinteger_4 = IdentifiedType<IntegerType, id_uinteger_4>(4, false);
const Type optional_bytes_2 = IdentifiedType<NameValueUintegerType, id_optional_bytes_2>(0, bytes_2);
const Type optional_uinteger_4 = IdentifiedType<NameValueUintegerType, id_optional_uinteger_4>(1, uinteger_4);


TEST_CASE("JSON from IntegerType", "[ValueToJSON]") {
    const Type type = IntegerType(4, false);
    const Value value = 37;
    const auto json = ValueToJSON(value, type);
    REQUIRE(json == 37);
}


TEST_CASE("JSON from BytesType", "[ValueToJSON]") {
    const Type type = bytes_2;
    const Value value = std::vector{ 0xFF_b, 0x78_b };
    const auto json = ValueToJSON(value, type);
    REQUIRE(json == nlohmann::json("FF'78"));
}


TEST_CASE("JSON from ListType", "[ValueToJSON]") {
    const Type type = ListType(uinteger_4);
    const Value value = std::vector<unsigned>{ 2, 3 };
    const auto json = ValueToJSON(value, type);
    REQUIRE(json == nlohmann::json(std::vector<unsigned>{ 2, 3 }));
}


TEST_CASE("JSON from AlternativeType", "[ValueToJSON]") {
    const Type type = AlternativeType(uinteger_4, bytes_2);
    const Value value = Named(ToBytes(uint32_t(id_uinteger_4)), 37);
    const auto json = ValueToJSON(value, type);
    REQUIRE(json == nlohmann::json({
                {"type",   "00'00'00'02"},
                { "value", 37           }
    }));
}


TEST_CASE("JSON from StructType", "[ValueToJSON]") {
    const Type type = StructType(uinteger_4, bytes_2, optional_bytes_2, optional_uinteger_4);

    const unsigned field1 = 5;
    const std::vector field2{ 0x65_b, 0x84_b };
    const Named field3{
        0, std::vector{0x99_b, 0x72_b}
    };
    const Named field4{ 1, 99 };

    SECTION("No optionals") {
        const Value value = { field1, field2 };
        const auto json = ValueToJSON(value, type);
        REQUIRE(json == nlohmann::json({
                    field1,
                    "65'84",
                }));
    };
    SECTION("One optional") {
        const Value value = { field1, field2, field4 };
        const auto json = ValueToJSON(value, type);
        REQUIRE(json == nlohmann::json({
                    field1,
                    "65'84",
                    {{ "field", 1 }, { "value", 99 }},
        }));
    };
    SECTION("Both optionals") {
        const Value value = { field1, field2, field3, field4 };
        const auto json = ValueToJSON(value, type);
        REQUIRE(json == nlohmann::json({
                    field1,
                    "65'84",
                    {{ "field", 0 },  { "value", "99'72" }},
                    { { "field", 1 }, { "value", 99 }     },
        }));
    };
}


TEST_CASE("JSON from RestrictedReferenceType", "[ValueToJSON]") {
    const Type type = RestrictedReferenceType(0x201);
    const Value value = ToBytes(0x12'34'56'78'98'76'54'32);
    const auto json = ValueToJSON(value, type);
    REQUIRE(json == "ref:12'34'56'78'98'76'54'32");
}


TEST_CASE("JSON from GeneralReferenceType", "[ValueToJSON]") {
    const Type type = GeneralReferenceType();
    const Value value = ToBytes(0x12'34'56'78'98'76'54'32);
    const auto json = ValueToJSON(value, type);
    REQUIRE(json == "ref:12'34'56'78'98'76'54'32");
}