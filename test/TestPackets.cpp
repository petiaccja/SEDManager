#include <TPerLib/Serialization/Utility.hpp>
#include <TPerLib/Communication/Packet.hpp>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Serialize ComPacket empty", "[Packets]") {
    ComPacket packet{
        .comId = 0xABCD,
        .comIdExtension = 0x1234,
        .outstandingData = 0x12345678,
        .minTransfer = 0xDEADBEEF,
    };

    const std::vector<uint8_t> bytes = {
        0x00, 0x00, 0x00, 0x00, // Reserved
        0xAB, 0xCD, // ComID
        0x12, 0x34, // ComID extension
        0x12, 0x34, 0x56, 0x78, // Outstanding data
        0xDE, 0xAD, 0xBE, 0xEF, // Min transfer
        0x00, 0x00, 0x00, 0x00, // Length
    };

    SECTION("Serialize") {
        const auto ser = ToBytes(packet);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        ComPacket des;
        FromBytes(bytes, des);
        REQUIRE(des == packet);
    }
}


TEST_CASE("Serialize Packet empty", "[Packets]") {
    Packet packet{
        .tperSessionNumber = 0x12345678,
        .hostSessionNumber = 0x23456789,
        .sequenceNumber = 0x11223344,
        .ackType = 0xAABB,
        .acknowledgement = 0x22334455,
    };

    const std::vector<uint8_t> bytes = {
        0x12, 0x34, 0x56, 0x78, // TSN
        0x23, 0x45, 0x67, 0x89, // HSN
        0x11, 0x22, 0x33, 0x44, // Seq number
        0x00, 0x00, // Reserved
        0xAA, 0xBB, // Ack type
        0x22, 0x33, 0x44, 0x55, // Ack
        0x00, 0x00, 0x00, 0x00, // Length
    };

    SECTION("Serialize") {
        const auto ser = ToBytes(packet);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        Packet des;
        FromBytes(bytes, des);
        REQUIRE(des == packet);
    }
}


TEST_CASE("Serialize data SubPacket empty", "[Packets]") {
    SubPacket packet{
        .kind = 0xBEEF,
    };

    const std::vector<uint8_t> bytes = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Reserved
        0xBE, 0xEF, // Kind
        0x00, 0x00, 0x00, 0x00, // Length
    };

    SECTION("Serialize") {
        const auto ser = ToBytes(packet);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        SubPacket des;
        FromBytes(bytes, des);
        REQUIRE(des == packet);
    }
}


TEST_CASE("Serialize ComPacket", "[Packets]") {
    SubPacket sp{
        .kind = 0,
        .payload = { 0xBE, 0xEF },
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

    const std::vector<uint8_t> bytes = {
        // ComPacket
        0x00, 0x00, 0x00, 0x00, // Reserved
        0x00, 0x00, // ComID
        0x00, 0x00, // ComID extension
        0x00, 0x00, 0x00, 0x00, // Outstanding data
        0x00, 0x00, 0x00, 0x00, // Min transfer
        0x00, 0x00, 0x00, 40, // Length
        // Packet
        0x00, 0x00, 0x00, 0x00, // TSN
        0x00, 0x00, 0x00, 0x00, // HSN
        0x00, 0x00, 0x00, 0x00, // Seq number
        0x00, 0x00, // Reserved
        0x00, 0x00, // Ack type
        0x00, 0x00, 0x00, 0x00, // Ack
        0x00, 0x00, 0x00, 16, // Length
        // SubPacket
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Reserved
        0x00, 0x00, // Kind
        0x00, 0x00, 0x00, 2, // Length
        // Payload
        0xBE, 0xEF, // Data
        0x00, 0x00, // Padding
    };

    SECTION("Serialize") {
        const auto ser = ToBytes(cp);
        REQUIRE(ser == bytes);
    }
    SECTION("Deserialize") {
        ComPacket des;
        FromBytes(bytes, des);
        REQUIRE(des == cp);
    }
}