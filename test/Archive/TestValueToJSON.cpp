#include <Archive/Conversion.hpp>
#include <Archive/Types/ValueToJSON.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


constexpr auto id_bytes_2 = 0x0000'0005'0000'0001_uid;
constexpr auto id_uinteger_4 = 0x0000'0005'0000'0002_uid;
constexpr auto id_optional_bytes_2 = 0x0000'0005'0000'0101_uid;
constexpr auto id_optional_uinteger_4 = 0x0000'0005'0000'0102_uid;


const Type bytes_2 = IdentifiedType<BytesType, id_bytes_2>(2, true);
const Type uinteger_4 = IdentifiedType<IntegerType, id_uinteger_4>(4, false);
const Type optional_bytes_2 = IdentifiedType<NameValueUintegerType, id_optional_bytes_2>(0, bytes_2);
const Type optional_uinteger_4 = IdentifiedType<NameValueUintegerType, id_optional_uinteger_4>(1, uinteger_4);


TEST_CASE("ValueToJSON: EnumerationType", "[ValueToJSON]") {
    const Type type = EnumerationType(0, 3, {
                                                {0,  "Zero" },
                                                { 3, "Three"}
    });
    const Value value1 = uint16_t(0);
    const Value value2 = uint16_t(1);
    const nlohmann::json json1 = "Zero";
    const nlohmann::json json2 = uint16_t(1);
    SECTION("Value to JSON") {
        const auto conv1 = ValueToJSON(value1, type);
        const auto conv2 = ValueToJSON(value2, type);
        REQUIRE(conv1 == json1);
        REQUIRE(conv2 == json2);
    }
    SECTION("JSON to Value") {
        const auto conv1 = JSONToValue(json1, type);
        const auto conv2 = JSONToValue(json2, type);
        REQUIRE(conv1 == value1);
        REQUIRE(conv2 == value2);
    }
}


TEST_CASE("ValueToJSON: IntegerType", "[ValueToJSON]") {
    const Type type = IntegerType(4, false);
    const Value value = uint32_t(37);
    const nlohmann::json json = uint32_t(37);
    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type);
        REQUIRE(conv == value);
    }
}


TEST_CASE("ValueToJSON: BytesType", "[ValueToJSON]") {
    const Type type = bytes_2;
    const Value value = std::vector{ 0xFF_b, 0x78_b };
    const auto json = nlohmann::json("FF'78");
    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type);
        REQUIRE(conv == value);
    }
}


TEST_CASE("ValueToJSON: ListType", "[ValueToJSON]") {
    const Type type = ListType(uinteger_4);
    const Value value = std::vector<unsigned>{ 2, 3 };
    const auto json = nlohmann::json(std::vector<unsigned>{ 2, 3 });
    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type);
        REQUIRE(conv == value);
    }
}


TEST_CASE("ValueToJSON: AlternativeType", "[ValueToJSON]") {
    const Type type = AlternativeType(uinteger_4, bytes_2);
    const Value value = Named(ToBytes(uint32_t(id_uinteger_4.value)), uint32_t(37));
    const auto json = nlohmann::json({
        {"ref:0000'0005'0000'0002", 37}
    });
    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type);
        REQUIRE(conv == value);
    }
}


TEST_CASE("ValueToJSON:StructType", "[ValueToJSON]") {
    const Type type = StructType(uinteger_4, bytes_2, optional_bytes_2, optional_uinteger_4);

    const unsigned field1 = uint32_t(5);
    const std::vector field2{ 0x65_b, 0x84_b };
    const Named field3{
        uint16_t(0), std::vector{0x99_b, 0x72_b}
    };
    const Named field4{ uint16_t(1), uint32_t(99) };

    SECTION("No optionals") {
        const Value value = { field1, field2 };
        const auto json = nlohmann::json({
            field1,
            "65'84",
        });
        SECTION("Value to JSON") {
            const auto conv = ValueToJSON(value, type);
            REQUIRE(conv == json);
        }
        SECTION("JSON to Value") {
            const auto conv = JSONToValue(json, type);
            REQUIRE(conv == value);
        }
    };
    SECTION("One optional") {
        const Value value = { field1, field2, field4 };
        const auto json = nlohmann::json({
            field1,
            "65'84",
            {{ "field", 1 }, { "value", 99 }},
        });
        SECTION("Value to JSON") {
            const auto conv = ValueToJSON(value, type);
            REQUIRE(conv == json);
        }
        SECTION("JSON to Value") {
            const auto conv = JSONToValue(json, type);
            REQUIRE(conv == value);
        }
    };
    SECTION("Both optionals") {
        const Value value = { field1, field2, field3, field4 };
        const auto json = nlohmann::json({
            field1,
            "65'84",
            {{ "field", 0 },  { "value", "99'72" }},
            { { "field", 1 }, { "value", 99 }     },
        });
        SECTION("Value to JSON") {
            const auto conv = ValueToJSON(value, type);
            REQUIRE(conv == json);
        }
        SECTION("JSON to Value") {
            const auto conv = JSONToValue(json, type);
            REQUIRE(conv == value);
        }
    };
}


TEST_CASE("ValueToJSON: ReferenceType names", "[ValueToJSON]") {
    const Type type = ReferenceType();
    const Value value = ToBytes(0x1234'5678'9876'5432);
    const nlohmann::json json = "ref:Macilaci";

    const auto uidConverter = [](UID uid) -> std::optional<std::string> {
        return uid == UID(0x1234'5678'9876'5432) ? std::optional<std::string>("Macilaci") : std::nullopt;
    };
    const auto nameConverter = [](std::string_view name) -> std::optional<UID> {
        return name == "Macilaci" ? std::optional(UID(0x1234'5678'9876'5432)) : std::nullopt;
    };

    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type, uidConverter);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type, nameConverter);
        REQUIRE(conv == value);
    }
}


TEST_CASE("ValueToJSON: RestrictedReferenceType", "[ValueToJSON]") {
    const Type type = RestrictedReferenceType(0x201);
    const Value value = ToBytes(0x1234'5678'9876'5432);
    const nlohmann::json json = "ref:1234'5678'9876'5432";

    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type);
        REQUIRE(conv == value);
    }
}


TEST_CASE("ValueToJSON:GeneralReferenceType", "[ValueToJSON]") {
    const Type type = GeneralReferenceType();
    const Value value = ToBytes(0x1234'5678'9876'5432);
    const nlohmann::json json = "ref:1234'5678'9876'5432";

    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type);
        REQUIRE(conv == value);
    }
}


TEST_CASE("ValueToJSON: NameValueUintegerType", "[ValueToJSON]") {
    const Type type = NameValueUintegerType(6, IntegerType(4, true));
    const Value value = Named(uint16_t(6), int32_t(748));
    const nlohmann::json json = {
        {"name",   6  },
        { "value", 748},
    };

    SECTION("Value to JSON") {
        const auto conv = ValueToJSON(value, type);
        REQUIRE(conv == json);
    }
    SECTION("JSON to Value") {
        const auto conv = JSONToValue(json, type);
        REQUIRE(conv == value);
    }
}