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