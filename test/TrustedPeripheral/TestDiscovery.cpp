#include <Archive/Serialization.hpp>
#include <TrustedPeripheral/Discovery.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


std::vector<std::byte> MakeDiscoveryBytes(std::span<const std::byte> featureBytes) {
    std::vector discoveryBytes = {
        // Length of parameter data
        std::byte(uint8_t(featureBytes.size() >> 24)),
        std::byte(uint8_t(featureBytes.size() >> 16)),
        std::byte(uint8_t(featureBytes.size() >> 8)),
        std::byte(uint8_t(featureBytes.size())),
        // Data structure revision
        0_b, 0_b, 0_b, 1_b
    };
    for (size_t i = 8; i < 48; ++i) {
        discoveryBytes.push_back(0_b);
    }
    std::ranges::copy(featureBytes, std::back_inserter(discoveryBytes));
    return discoveryBytes;
}


TEST_CASE("Discovery: TPer features", "[Discovery]") {
    const std::array<std::byte, 16> featureBytes = {
        0x00_b, 0x01_b, // Feature code
        0x10_b, // Version | Reserved
        0x0C_b, // Length
        0b0101'0101_b, // Bitmask: Res | ComIDMgmt | Res | Stream | BufMgmt | ACKNAK | Async | Sync
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.tperDesc);
    REQUIRE(desc.tperDesc->comIdMgmtSupported == true);
    REQUIRE(desc.tperDesc->streamingSupported == true);
    REQUIRE(desc.tperDesc->bufferMgmtSupported == false);
    REQUIRE(desc.tperDesc->ackNakSupported == true);
    REQUIRE(desc.tperDesc->asyncSupported == false);
    REQUIRE(desc.tperDesc->syncSupported == true);
}


TEST_CASE("Discovery: Locking features", "[Discovery]") {
    const std::array<std::byte, 16> featureBytes = {
        0x00_b, 0x02_b, // Feature code
        0x10_b, // Version | Reserved
        0x0C_b, // Length
        0b0010'1010_b, // Bitmask: Res | Res | MBDone | MBEnabled | Encr | Locked | LE | LS
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.lockingDesc);
    REQUIRE(desc.lockingDesc->mbrDone == true);
    REQUIRE(desc.lockingDesc->mbrEnabled == false);
    REQUIRE(desc.lockingDesc->mediaEncryption == true);
    REQUIRE(desc.lockingDesc->locked == false);
    REQUIRE(desc.lockingDesc->lockingEnabled == true);
    REQUIRE(desc.lockingDesc->lockingSupported == false);
}


TEST_CASE("Discovery: Opal v1", "[Discovery]") {
    const std::array<std::byte, 20> featureBytes = {
        0x02_b, 0x00_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0x01_b, // Range crossing
        0_b, 0_b, 0_b, 0_b,
        0_b, 0_b, 0_b, 0_b,
        0_b, 0_b, 0_b
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<Opal1FeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
    REQUIRE(featureDesc.crossingRangeBehavior == 1);
}


TEST_CASE("Discovery: Opal v2", "[Discovery]") {
    const std::array<std::byte, 20> featureBytes = {
        0x02_b, 0x03_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0x01_b, // Range crossing
        1_b, 2_b, // Num locking SP Admins
        3_b, 4_b, // Num locking SP Users
        0x00_b, // Initial C_PIN
        0xFF_b, // Revert C_PIN
        0_b, 0_b, 0_b, 0_b, 0_b
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<Opal2FeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
    REQUIRE(featureDesc.crossingRangeBehavior == 1);
    REQUIRE(featureDesc.numAdminsSupported == 0x0102);
    REQUIRE(featureDesc.numUsersSupported == 0x0304);
    REQUIRE(featureDesc.initialCPinSidIndicator == false);
    REQUIRE(featureDesc.cPinSidRevertBehavior == true);
}


TEST_CASE("Discovery: Opalite", "[Discovery]") {
    const std::array<std::byte, 20> featureBytes = {
        0x03_b, 0x01_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0_b, 0_b, 0_b, 0_b, 0_b,
        0x00_b, // Initial C_PIN
        0xFF_b, // Revert C_PIN
        0_b, 0_b, 0_b, 0_b, 0_b
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<OpaliteFeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
    REQUIRE(featureDesc.initialCPinSidIndicator == false);
    REQUIRE(featureDesc.cPinSidRevertBehavior == true);
}


TEST_CASE("Discovery: Pyrite v1", "[Discovery]") {
    const std::array<std::byte, 20> featureBytes = {
        0x03_b, 0x02_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0_b, 0_b, 0_b, 0_b, 0_b,
        0x00_b, // Initial C_PIN
        0xFF_b, // Revert C_PIN
        0_b, 0_b, 0_b, 0_b, 0_b
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<Pyrite1FeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
    REQUIRE(featureDesc.initialCPinSidIndicator == false);
    REQUIRE(featureDesc.cPinSidRevertBehavior == true);
}


TEST_CASE("Discovery: Pyrite v2", "[Discovery]") {
    const std::array<std::byte, 20> featureBytes = {
        0x03_b, 0x03_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0_b, 0_b, 0_b, 0_b, 0_b,
        0x00_b, // Initial C_PIN
        0xFF_b, // Revert C_PIN
        0_b, 0_b, 0_b, 0_b, 0_b
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<Pyrite2FeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
    REQUIRE(featureDesc.initialCPinSidIndicator == false);
    REQUIRE(featureDesc.cPinSidRevertBehavior == true);
}


TEST_CASE("Discovery: Enterprise", "[Discovery]") {
    const std::array<std::byte, 20> featureBytes = {
        0x01_b, 0x00_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0x01_b, // Range crossing
        0_b, 0_b, 0_b, 0_b, 0_b,
        0_b, 0_b, 0_b, 0_b, 0_b,
        0_b
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<EnterpriseFeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
    REQUIRE(featureDesc.crossingRangeBehavior == 1);
}


TEST_CASE("Discovery: Ruby", "[Discovery]") {
    const std::array<std::byte, 20> featureBytes = {
        0x03_b, 0x04_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0x01_b, // Range crossing
        1_b, 3_b, // Num admins
        2_b, 5_b, // Num users
        0x00_b, // Initial C_PIN
        0xFF_b, // Revert C_PIN
        0_b, 0_b, 0_b, 0_b, 0_b
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<RubyFeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
    REQUIRE(featureDesc.crossingRangeBehavior == 1);
    REQUIRE(featureDesc.numAdminsSupported == 0x0103);
    REQUIRE(featureDesc.numUsersSupported == 0x0205);
    REQUIRE(featureDesc.initialCPinSidIndicator == false);
    REQUIRE(featureDesc.cPinSidRevertBehavior == true);
}


TEST_CASE("Discovery: KPIO", "[Discovery]") {
    const std::array<std::byte, 48> featureBytes = {
        0x03_b, 0x05_b, // Feature code
        0x10_b, // Version | Reserved
        44_b, // Length
        0xDE_b, 0xAD_b, // Base ComID
        0xBE_b, 0xEF_b, // Num ComIDs
        0xEE_b, 0xAD_b, // Protocol 0x03 Base ComID
        0xEE_b, 0xEF_b, // Protocol 0x03 Num ComIDs
        0x00_b, // Initial C_PIN
        0xFF_b, // Revert C_PIN
        1_b, 1_b, // Num KPIO SP Admins supported
        0xFF_b, // Bitmask
        1_b, 2_b, // Maximum supported key UID length
        0x01_b, // KMIP formatted key injection supported
        0_b, // Reserved,
        0xFF_b, // Bitmask
        0_b, // Reserved
        1_b, // AES-256 wrapping key supported,
        0_b, // Reserved
        0xFF_b, // Bitmask
        0_b, // Reserved
        0xFF_b, // Bitmask
        0_b, 0_b, 0_b, 0_b, // Reserved
        1_b, 2_b, 3_b, 4_b, // Number of encryption keys supported
        1_b, 2_b, 6_b, 4_b, // Total number of key tags supported
        1_b, 7_b, // Maximum number of key tags per namespace
        76_b, // None length
        0_b, 0_b, 0_b, 0_b, 0_b // Reserved
    };
    const auto discoveryBytes = MakeDiscoveryBytes(featureBytes);
    const auto desc = ParseTPerDesc(discoveryBytes);

    REQUIRE(desc.sscDescs.size() == 1);
    const auto& featureDesc = std::get<KeyPerIOFeatureDesc>(desc.sscDescs[0]);
    REQUIRE(featureDesc.baseComId == 0xDEAD);
    REQUIRE(featureDesc.numComIds == 0xBEEF);
}