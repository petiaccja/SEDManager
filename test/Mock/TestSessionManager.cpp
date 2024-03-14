#include <MockDevice/MockDevice.hpp>
#include <Specification/Opal/OpalModule.hpp>
#include <TrustedPeripheral/SessionManager.hpp>

#include <asyncpp/join.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;

const auto adminSp = *Opal1Module::Get()->FindUid("SP::Admin");


TEST_CASE("SessionManager: start / end session", "[SessionManager]") {
    const auto device = std::make_shared<MockDevice>();
    const auto tper = std::make_shared<TrustedPeripheral>(device);
    const auto sessionManager = std::make_shared<SessionManager>(tper);
    SessionManager::StartSessionResult sessionParams;
    REQUIRE_NOTHROW(sessionParams = join(sessionManager->StartSession(100, adminSp, true)));
    REQUIRE_NOTHROW(join(sessionManager->EndSession(sessionParams.spSessionId, sessionParams.hostSessionId)));
}


TEST_CASE("SessionManager: properties", "[SessionManager]") {
    const auto device = std::make_shared<MockDevice>();
    const auto tper = std::make_shared<TrustedPeripheral>(device);
    const auto sessionManager = std::make_shared<SessionManager>(tper);
    SessionManager::PropertiesResult properties;
    REQUIRE_NOTHROW(properties = join(sessionManager->Properties()));
    REQUIRE(properties.tperProperties.contains("MaxPackets"));
}