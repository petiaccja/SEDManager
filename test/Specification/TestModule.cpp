#include <Specification/Core/CoreModule.hpp>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("Specification: module find UID", "[Specification]") {
    const auto mod = CoreModule::Get();
    SECTION("valid") {
        const auto result = mod->FindUid("Table");
        REQUIRE(!!result);
        REQUIRE(*result == Uid(core::eTable::Table));
    }
    SECTION("invalid") {
        REQUIRE(!mod->FindUid("INVALID_NAME"));
    }
}


TEST_CASE("Specification: module find name", "[Specification]") {
    const auto mod = CoreModule::Get();
    SECTION("valid") {
        const auto result = mod->FindName(core::eTable::Table);
        REQUIRE(!!result);
        REQUIRE(*result == "Table");
    }
    SECTION("invalid") {
        REQUIRE(!mod->FindName(0xFFFF'FFFF'FFFF'CCCC));
    }
}