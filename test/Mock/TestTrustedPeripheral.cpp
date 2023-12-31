#include <async++/join.hpp>

#include <MockDevice/MockDevice.hpp>
#include <TrustedPeripheral/TrustedPeripheral.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("TrustedPeripheral: discovery", "[TrustedPeripheral]") {
    const auto device = std::make_shared<MockDevice>();
    TrustedPeripheral tper(device);
    const auto desc = tper.GetDesc();
    REQUIRE(desc.tperDesc);
    REQUIRE(desc.lockingDesc);
}


TEST_CASE("TrustedPeripheral: verify ComID valid", "[TrustedPeripheral]") {
    const auto device = std::make_shared<MockDevice>();
    TrustedPeripheral tper(device);
    REQUIRE(join(tper.VerifyComId()) == eComIdState::ISSUED);
}


TEST_CASE("TrustedPeripheral: stack reset", "[TrustedPeripheral]") {
    const auto device = std::make_shared<MockDevice>();
    TrustedPeripheral tper(device);
    REQUIRE_NOTHROW(join(tper.StackReset()));
}