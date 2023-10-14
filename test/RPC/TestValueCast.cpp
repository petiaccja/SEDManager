#include <RPC/ValueCast.hpp>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("CellBlock value_cast", "[ValueCast]") {
    const CellBlock original = {
        .startRow = Uid(0xDEADBEEF),
        .endRow = 20,
        .startColumn = 30,
        .endColumn = 40,
    };

    const auto copy = value_cast<CellBlock>(value_cast(original));
    REQUIRE(original == copy);
}