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