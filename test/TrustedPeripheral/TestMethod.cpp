#include <Specification/Core/Defs/UIDs.hpp>
#include <TrustedPeripheral/Method.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


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