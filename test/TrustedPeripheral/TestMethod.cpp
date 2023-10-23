#include <TrustedPeripheral/Method.hpp>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Method: parse method roundtrip", "[Method]") {
    const Method input = {
        .methodId = 0xDEADBEEF,
        .args = {1, 2, 3},
        .status = eMethodStatus::FAIL,
    };

    const auto value = MethodToValue(0xFF, input);
    const Method output = MethodFromValue(value);

    REQUIRE(output.methodId == input.methodId);
    REQUIRE(output.args.size() == 3);
    REQUIRE(output.args[0].Get<int>() == 1);
    REQUIRE(output.args[1].Get<int>() == 2);
    REQUIRE(output.args[2].Get<int>() == 3);
    REQUIRE(output.status == input.status);
}


TEST_CASE("Method: parse results", "[Method]") {
    const Value value = {
        {1,                                          2,          3         },
        eCommand::END_OF_DATA,
        { static_cast<uint8_t>(eMethodStatus::FAIL), uint8_t(0), uint8_t(0)},
    };
    const MethodResult expected = {
        .values = {1, 2, 3},
        .status = eMethodStatus::FAIL,
    };

    const MethodResult output = MethodResultFromValue(value);

    REQUIRE(output.values.size() == 3);
    REQUIRE(output.values[0].Get<int>() == 1);
    REQUIRE(output.values[1].Get<int>() == 2);
    REQUIRE(output.values[2].Get<int>() == 3);
    REQUIRE(output.status == eMethodStatus::FAIL);
}


TEST_CASE("Method: parse args roundtrip", "[Method]") {
    const auto streams = ArgsToValues(3, 4, std::optional<int>{}, std::optional<int>{ 5 }, std::optional<int>{});
    int arg0 = 0;
    int arg1 = 0;
    std::optional<int> arg2;
    std::optional<int> arg3;
    std::optional<int> arg4;
    ArgsFromValues(streams, arg0, arg1, arg2, arg3, arg4);
    REQUIRE(arg0 == 3);
    REQUIRE(arg1 == 4);
    REQUIRE(!arg2.has_value());
    REQUIRE(arg3.has_value());
    REQUIRE(arg3.value() == 5);
    REQUIRE(!arg4.has_value());
}