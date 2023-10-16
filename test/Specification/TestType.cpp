#include <Specification/ColumnTypes.hpp>
#include <Specification/Type.hpp>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Type integer", "[Type]") {
    UInteger1 type;
    REQUIRE(type.Width() == 1);
    REQUIRE(type.Signedness() == true);
    REQUIRE(type_uid(type) == 0x0000'0005'0000'0210);
}


TEST_CASE("Type cast", "[Type]") {
    Type source = UInteger1();
    IntegerType target = type_cast<IntegerType>(source);
    REQUIRE(target.Width() == 1);
    REQUIRE(target.Signedness() == true);
    REQUIRE(type_uid(target) == 0x0000'0005'0000'0210);
}