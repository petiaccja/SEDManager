#include <Archive/Serialization.hpp>
#include <Messaging/SetupPackets.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("SetupPackets: VerifyComIdValidRequest to bytes", "[SetupPackets]") {
    VerifyComIdValidRequest input = { .comId = 0xDEAD, .comIdExtension = 0xBEEF };
    const std::vector expected = {
        0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b, // 0-3: Extended ComID
        0x00_b, 0x00_b, 0x00_b, 0x01_b, // 4-7: Request code
    };
    const auto& output = Serialize(input);
    REQUIRE(output == expected);
}


TEST_CASE("SetupPackets: StackResetRequest to bytes", "[SetupPackets]") {
    StackResetRequest input = { .comId = 0xDEAD, .comIdExtension = 0xBEEF };
    const std::vector expected = {
        0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b, // 0-3: Extended ComID
        0x00_b, 0x00_b, 0x00_b, 0x02_b, // 4-7: Request code
    };
    const auto& output = Serialize(input);
    REQUIRE(output == expected);
}


TEST_CASE("SetupPackets: bytes to VerifyComIdValidReponse", "[SetupPackets]") {
    const std::vector input = {
        0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b, // 0-3: Extended ComID
        0x00_b, 0x00_b, 0x00_b, 0x01_b, // 4-7: Request code
        0x00_b, 0x00_b, // 8-9: Reserved
        0x00_b, 0x22_b, // 10-11: Available data length
        0x00_b, 0x00_b, 0x00_b, 0x03_b, // 12-15: ComID state
        0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, // 16-25: Allocation time
        0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, // 26-35: Expiry time
        0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, // 36-45: Current time
    };

    const auto output = DeSerialize(Serialized<VerifyComIdValidResponse>{ input });
    REQUIRE(output.comId == 0xDEAD);
    REQUIRE(output.comIdExtension == 0xBEEF);
    REQUIRE(output.requestCode == 0x01);
    REQUIRE(output.availableDataLength == 0x22);
    REQUIRE(output.comIdState == eComIdState::ASSOCIATED);
}


TEST_CASE("SetupPackets: bytes to StackResetResponse", "[SetupPackets]") {
    const std::vector input = {
        0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b, // 0-3: Extended ComID
        0x00_b, 0x00_b, 0x00_b, 0x02_b, // 4-7: Request code
        0x00_b, 0x00_b, // 8-9: Reserved
        0x00_b, 0x04_b, // 10-11: Available data length
        0x00_b, 0x00_b, 0x00_b, 0x01_b, // 12-15: Success/Failure
    };

    StackResetResponse output = DeSerialize(Serialized<StackResetResponse>{ input });
    REQUIRE(output.comId == 0xDEAD);
    REQUIRE(output.comIdExtension == 0xBEEF);
    REQUIRE(output.requestCode == 0x02);
    REQUIRE(output.availableDataLength == 0x04);
    REQUIRE(output.success == eStackResetStatus::FAILURE);
}