#include <Archive/Serialization.hpp>
#include <Messaging/ComPacket.hpp>

#include <catch2/catch_test_macros.hpp>


using namespace sedmgr;


TEST_CASE("ComPacket: ComPacket empty", "[ComPacket]") {
    ComPacket packet{
        .comId = 0xABCD,
        .comIdExtension = 0x1234,
        .outstandingData = 0x12345678,
        .minTransfer = 0xDEADBEEF,
    };

    const std::vector<std::byte> bytes = {
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Reserved
        0xAB_b, 0xCD_b, // ComID
        0x12_b, 0x34_b, // ComID extension
        0x12_b, 0x34_b, 0x56_b, 0x78_b, // Outstanding data
        0xDE_b, 0xAD_b, 0xBE_b, 0xEF_b, // Min transfer
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Length
    };

    SECTION("Serialize") {
        const auto ser = Serialize(packet);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        const auto des = DeSerialize(Serialized<ComPacket>{ bytes });
        REQUIRE(des == packet);
    }
}


TEST_CASE("ComPacket: Packet empty", "[ComPacket]") {
    Packet packet{
        .tperSessionNumber = 0x12345678,
        .hostSessionNumber = 0x23456789,
        .sequenceNumber = 0x11223344,
        .ackType = 0xAABB,
        .acknowledgement = 0x22334455,
    };

    const std::vector<std::byte> bytes = {
        0x12_b, 0x34_b, 0x56_b, 0x78_b, // TSN
        0x23_b, 0x45_b, 0x67_b, 0x89_b, // HSN
        0x11_b, 0x22_b, 0x33_b, 0x44_b, // Seq number
        0x00_b, 0x00_b, // Reserved
        0xAA_b, 0xBB_b, // Ack type
        0x22_b, 0x33_b, 0x44_b, 0x55_b, // Ack
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Length
    };

    SECTION("Serialize") {
        const auto ser = Serialize(packet);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        const auto des = DeSerialize(Serialized<Packet>{ bytes });
        REQUIRE(des == packet);
    }
}


TEST_CASE("ComPacket: SubPacket empty", "[ComPacket]") {
    SubPacket packet{
        .kind = 0xBEEF,
    };

    const std::vector<std::byte> bytes = {
        0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, // Reserved
        0xBE_b, 0xEF_b, // Kind
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Length
    };

    SECTION("Serialize") {
        const auto ser = Serialize(packet);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        const auto des = DeSerialize(Serialized<SubPacket>{ bytes });
        REQUIRE(des == packet);
    }
}


TEST_CASE("ComPacket: filled", "[ComPacket]") {
    SubPacket sp{
        .kind = 0,
        .payload = {0xBE_b, 0xEF_b},
    };

    Packet p{
        .tperSessionNumber = 0,
        .hostSessionNumber = 0,
        .sequenceNumber = 0,
        .ackType = 0,
        .acknowledgement = 0,
        .payload = { sp },
    };

    ComPacket cp{
        .comId = 0,
        .comIdExtension = 0,
        .outstandingData = 0,
        .minTransfer = 0,
        .payload = { p },
    };

    const std::vector<std::byte> bytes = {
        // ComPacket
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Reserved
        0x00_b, 0x00_b, // ComID
        0x00_b, 0x00_b, // ComID extension
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Outstanding data
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Min transfer
        0x00_b, 0x00_b, 0x00_b, 40_b, // Length
        // Packet
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // TSN
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // HSN
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Seq number
        0x00_b, 0x00_b, // Reserved
        0x00_b, 0x00_b, // Ack type
        0x00_b, 0x00_b, 0x00_b, 0x00_b, // Ack
        0x00_b, 0x00_b, 0x00_b, 16_b, // Length
        // SubPacket
        0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, // Reserved
        0x00_b, 0x00_b, // Kind
        0x00_b, 0x00_b, 0x00_b, 2_b, // Length
        // Payload
        0xBE_b, 0xEF_b, // Data
        0x00_b, 0x00_b, // Padding
    };

    SECTION("Serialize") {
        const auto ser = Serialize(cp);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        const auto des = DeSerialize(Serialized<ComPacket>{ bytes });
        REQUIRE(des == cp);
    }
}