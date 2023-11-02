#include <MockDevice/MockDevice.hpp>
#include <TrustedPeripheral/SessionManager.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("SessionManager: start / end session", "[SessionManager]") {
    const auto device = std::make_shared<MockDevice>();
    const auto tper = std::make_shared<TrustedPeripheral>(device);
    const auto sessionManager = std::make_shared<SessionManager>(tper);
    SessionManager::StartSessionResult sessionParams;
    REQUIRE_NOTHROW(sessionParams = sessionManager->StartSession(100, Uid(100), true));
    REQUIRE_NOTHROW(sessionManager->EndSession(sessionParams.spSessionId, sessionParams.hostSessionId));
}