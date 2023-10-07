#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>

#include <cstdint>
#include <vector>


enum class eSubPacketKind {
    DATA = 0x0000,
    CREDIT_CONTROL = 0x8001,
};


struct SubPacket {
    uint16_t kind;
    std::vector<uint8_t> payload;

    static constexpr uint32_t HeaderLength() { return 12; }
    uint32_t PayloadLength() const { return uint32_t(payload.size()); }
    uint32_t PaddedPayloadLength() const { return uint32_t((payload.size() + 3) / 4 * 4); }

    std::strong_ordering operator<=>(const SubPacket&) const = default;
};


struct Packet {
    uint32_t tperSessionNumber;
    uint32_t hostSessionNumber;
    uint32_t sequenceNumber;
    uint16_t ackType;
    uint32_t acknowledgement;
    std::vector<SubPacket> payload;

    static constexpr uint32_t HeaderLength() { return 24; }
    uint32_t PayloadLength() const {
        uint32_t length = 0;
        for (auto& subPacket : payload) {
            length += subPacket.HeaderLength() + subPacket.PaddedPayloadLength();
        }
        return length;
    }

    std::strong_ordering operator<=>(const Packet&) const = default;
};


struct ComPacket {
    uint16_t comId = 0x0;
    uint16_t comIdExtension = 0x0;
    uint32_t outstandingData = 0;
    uint32_t minTransfer = 0;
    std::vector<Packet> payload;

    uint32_t PayloadLength() const {
        size_t length = 0;
        for (auto& packet : payload) {
            length += packet.HeaderLength() + packet.PayloadLength();
        }
        return length;
    }

    std::strong_ordering operator<=>(const ComPacket&) const = default;
};


template <class Archive>
void load(Archive& ar, ComPacket& obj) {
    uint32_t reserved = 0;
    uint32_t length;
    ar(reserved);
    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.outstandingData);
    ar(obj.minTransfer);
    ar(length);

    size_t offset = 0;
    obj.payload = {};
    while (offset < length) {
        Packet packet;
        ar(packet);
        offset += packet.HeaderLength() + packet.PayloadLength();
        obj.payload.push_back(std::move(packet));
    }
}


template <class Archive>
void load(Archive& ar, Packet& obj) {
    uint16_t reserved = 0;
    uint32_t length;
    ar(obj.tperSessionNumber);
    ar(obj.hostSessionNumber);
    ar(obj.sequenceNumber);
    ar(reserved);
    ar(obj.ackType);
    ar(obj.acknowledgement);
    ar(length);

    size_t offset = 0;
    obj.payload = {};
    while (offset < length) {
        SubPacket subPacket;
        ar(subPacket);
        offset += subPacket.HeaderLength() + subPacket.PaddedPayloadLength();
        obj.payload.push_back(std::move(subPacket));
    }
}


template <class Archive>
void load(Archive& ar, SubPacket& obj) {
    std::array<uint8_t, 6> reserved = { 0 };
    uint32_t length;
    ar(reserved);
    ar(obj.kind);
    ar(length);

    obj.payload.resize(length);
    for (auto& byte : obj.payload) {
        ar(byte);
    }
}


template <class Archive>
void save(Archive& ar, const ComPacket& obj) {
    uint32_t reserved = 0;
    ar(reserved);
    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.outstandingData);
    ar(obj.minTransfer);
    ar(obj.PayloadLength());
    for (auto& packet : obj.payload) {
        ar(packet);
    }
}


template <class Archive>
void save(Archive& ar, const Packet& obj) {
    uint16_t reserved = 0;
    ar(obj.tperSessionNumber);
    ar(obj.hostSessionNumber);
    ar(obj.sequenceNumber);
    ar(reserved);
    ar(obj.ackType);
    ar(obj.acknowledgement);
    ar(obj.PayloadLength());
    for (auto& subPacket : obj.payload) {
        ar(subPacket);
    }
}


template <class Archive>
void save(Archive& ar, const SubPacket& obj) {
    std::array<uint8_t, 6> reserved = { 0 };
    ar(reserved);
    ar(obj.kind);
    ar(obj.PayloadLength());
    for (auto& byte : obj.payload) {
        ar(byte);
    }
    for (size_t i = obj.PayloadLength(); i < obj.PaddedPayloadLength(); ++i) {
        ar(uint8_t(0));
    }
}