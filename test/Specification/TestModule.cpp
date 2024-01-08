#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>
#include <Specification/PSID/PSIDModule.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("Specification: module find UID", "[Specification]") {
    const auto mod = CoreModule::Get();
    SECTION("valid") {
        const auto result = mod->FindUid("Table");
        REQUIRE(!!result);
        REQUIRE(*result == UID(core::eTable::Table));
    }
    SECTION("invalid") {
        REQUIRE(!mod->FindUid("INVALID_NAME"));
    }
}


TEST_CASE("Specification: module find name", "[Specification]") {
    const auto mod = CoreModule::Get();
    SECTION("valid") {
        const auto result = mod->FindName(UID(core::eTable::Table));
        REQUIRE(!!result);
        REQUIRE(*result == "Table");
    }
    SECTION("invalid") {
        REQUIRE(!mod->FindName(0xFFFF'FFFF'FFFF'CCCC_uid));
    }
}


TEST_CASE("Specification: CoreModule find table", "[Specification]") {
    const auto mod = CoreModule::Get();
    SECTION("valid") {
        const auto result = mod->FindTable(UID(core::eTable::Table));
        REQUIRE(!!result);
        REQUIRE(result.value().name == "Table");
    }
    SECTION("invalid") {
        REQUIRE(!mod->FindTable(0xFFFF'FFFF'FFFF'CCCC_uid));
    }
}

TEST_CASE("Specification: CoreModule find type", "[Specification]") {
    const auto mod = CoreModule::Get();
    SECTION("valid") {
        const auto result = mod->FindType(UID(core::eType::MethodID_object_ref));
        REQUIRE(!!result);
        REQUIRE(type_uid(result.value()) == UID(core::eType::MethodID_object_ref));
    }
    SECTION("invalid") {
        REQUIRE(!mod->FindTable(0xFFFF'FFFF'FFFF'CCCC_uid));
    }
}


TEST_CASE("Specification: CoreModule init", "[Specification]") {
    REQUIRE_NOTHROW(CoreModule::Get());
}


TEST_CASE("Specification: PSIDModule init", "[Specification]") {
    REQUIRE_NOTHROW(PSIDModule::Get());
}


TEST_CASE("Specification: OpalModule init", "[Specification]") {
    REQUIRE_NOTHROW(Opal1Module::Get());
    REQUIRE_NOTHROW(Opal2Module::Get());
}