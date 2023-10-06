#include <TPerLib/Method.hpp>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Method - parse roundtrip", "[Method]") {
    const Method input = {
        .methodId = 0xDEADBEEF,
        .args = { 1, 2, 3 },
        .status = eMethodStatus::FAIL,
    };

    const auto stream = SerializeMethod(0xFF, input);
    const Method output = ParseMethod(stream);

    REQUIRE(output.methodId == input.methodId);
    REQUIRE(output.args.size() == 3);
    REQUIRE(output.args[0].Get<int>() == 1);
    REQUIRE(output.args[1].Get<int>() == 2);
    REQUIRE(output.args[2].Get<int>() == 3);
    REQUIRE(output.status == input.status);
}


TEST_CASE("Method - parse args roundtrip", "[Method]") {
    const auto streams = SerializeArgs(3, 4, std::optional<int>{}, std::optional<int>{ 5 }, std::optional<int>{});
    int arg0 = 0;
    int arg1 = 0;
    std::optional<int> arg2;
    std::optional<int> arg3;
    std::optional<int> arg4;
    ParseArgs(streams, arg0, arg1, arg2, arg3, arg4);
    REQUIRE(arg0 == 3);
    REQUIRE(arg1 == 4);
    REQUIRE(!arg2.has_value());
    REQUIRE(arg3.has_value());
    REQUIRE(arg3.value() == 5);
    REQUIRE(!arg4.has_value());
}