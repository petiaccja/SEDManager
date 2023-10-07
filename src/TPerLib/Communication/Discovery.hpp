#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>

#include <cstdint>
#include <optional>
#include <span>
#include <variant>


enum class eDiscoveryFeatureCode {
    TPER = 0x0001,
    LOCKING = 0x0002,
    OPAL2 = 0x0203,
    OPALITE = 0x0301,
    PYRITE1 = 0x302,
    PYRITE2 = 0x0303,
};


struct DiscoveryHeader {
    uint32_t lengthOfData;
    uint16_t versionMajor;
    uint16_t versionMinor;
    std::array<uint8_t, 48 - 16> vendorUnique;
};


struct FeatureDescHeader {
    uint16_t featureCode;
    uint8_t version;
    uint8_t length;
};


struct TPerFeatureDesc {
    bool comIdMgmtSupported;
    bool streamingSupported;
    bool bufferMgmtSupported;
    bool ackNakSupported;
    bool asyncSupported;
    bool syncSupported;
};


struct LockingFeatureDesc {
    bool mbrSupported;
    bool mbrDone;
    bool mbrEnabled;
    bool mediaEncryption;
    bool locked;
    bool lockingEnabled;
    bool lockingSupported;
};


struct Opal2FeatureDesc {
    uint16_t baseComId;
    uint16_t numComIds;
    bool crossingRangeBehavior;
    uint16_t numAdminsSupported;
    uint16_t numUsersSupported;
    uint8_t initialCPinSidIndicator;
    uint8_t cPinSidRevertBehavior;
};


struct OpaliteFeatureDesc {
    uint16_t baseComId;
    uint16_t numComIds;
    uint8_t initialCPinSidIndicator;
    uint8_t cPinSidRevertBehavior;
};


struct Pyrite1FeatureDesc {
    uint16_t baseComId;
    uint16_t numComIds;
    uint8_t initialCPinSidIndicator;
    uint8_t cPinSidRevertBehavior;
};


struct Pyrite2FeatureDesc {
    uint16_t baseComId;
    uint16_t numComIds;
    uint8_t initialCPinSidIndicator;
    uint8_t cPinSidRevertBehavior;
};


using StorageClassFeatureDesc = std::variant<std::monostate,
                                             Opal2FeatureDesc,
                                             OpaliteFeatureDesc,
                                             Pyrite1FeatureDesc,
                                             Pyrite2FeatureDesc>;


struct TPerDesc {
    std::optional<TPerFeatureDesc> tperDesc;
    std::optional<LockingFeatureDesc> lockingDesc;
    StorageClassFeatureDesc sscDesc;
};


template <class Archive>
void load(Archive& ar, DiscoveryHeader& obj) {
    ar(obj.lengthOfData);
    ar(obj.versionMajor);
    ar(obj.versionMinor);
    std::array<uint8_t, 16 - 8> reserved = { 0 };
    ar(reserved);
    ar(obj.vendorUnique);
}


template <class Archive>
void load(Archive& ar, FeatureDescHeader& obj) {
    uint8_t versionReserved = obj.version << 4;
    ar(obj.featureCode);
    ar(versionReserved);
    obj.version = versionReserved >> 4;
    ar(obj.length);
}


template <class Archive>
void load(Archive& ar, TPerFeatureDesc& obj) {
    uint8_t bits = uint8_t(
        (unsigned(obj.comIdMgmtSupported) << 6)
        | (unsigned(obj.streamingSupported) << 4)
        | (unsigned(obj.bufferMgmtSupported) << 3)
        | (unsigned(obj.ackNakSupported) << 2)
        | (unsigned(obj.asyncSupported) << 1)
        | (unsigned(obj.syncSupported) << 0));
    std::array<uint8_t, 15 - 5> reserved;

    ar(bits);
    ar(reserved);

    obj.comIdMgmtSupported = bool((bits >> 6) & 1);
    obj.streamingSupported = bool((bits >> 4) & 1);
    obj.bufferMgmtSupported = bool((bits >> 3) & 1);
    obj.ackNakSupported = bool((bits >> 2) & 1);
    obj.asyncSupported = bool((bits >> 1) & 1);
    obj.syncSupported = bool((bits >> 0) & 1);
}


template <class Archive>
void load(Archive& ar, LockingFeatureDesc& obj) {
    uint8_t bits = uint8_t(
        !(unsigned(obj.mbrSupported) << 6)
        | (unsigned(obj.mbrDone) << 5)
        | (unsigned(obj.mbrEnabled) << 4)
        | (unsigned(obj.mediaEncryption) << 3)
        | (unsigned(obj.locked) << 2)
        | (unsigned(obj.lockingEnabled) << 1)
        | (unsigned(obj.lockingSupported) << 0));
    std::array<uint8_t, 15 - 5> reserved;

    ar(bits);
    ar(reserved);

    obj.mbrSupported = !bool((bits >> 6) & 1);
    obj.mbrDone = bool((bits >> 5) & 1);
    obj.mbrEnabled = bool((bits >> 4) & 1);
    obj.mediaEncryption = bool((bits >> 3) & 1);
    obj.locked = bool((bits >> 2) & 1);
    obj.lockingEnabled = bool((bits >> 1) & 1);
    obj.lockingSupported = bool((bits >> 0) & 1);
}


template <class Archive>
void load(Archive& ar, Opal2FeatureDesc& obj) {
    uint8_t reserved1 = 0;
    std::array<uint8_t, 5> reserved2 = { 0 };

    ar(obj.baseComId);
    ar(obj.numComIds);
    ar(reserved1);
    ar(obj.numAdminsSupported);
    ar(obj.numUsersSupported);
    ar(obj.initialCPinSidIndicator);
    ar(obj.cPinSidRevertBehavior);
    ar(reserved2);
}


template <class Archive>
void load(Archive& ar, OpaliteFeatureDesc& obj) {
    std::array<uint8_t, 5> reserved1 = { 0 };
    std::array<uint8_t, 5> reserved2 = { 0 };

    ar(obj.baseComId);
    ar(obj.numComIds);
    ar(reserved1);
    ar(obj.initialCPinSidIndicator);
    ar(obj.cPinSidRevertBehavior);
    ar(reserved2);
}


template <class Archive>
void load(Archive& ar, Pyrite1FeatureDesc& obj) {
    std::array<uint8_t, 5> reserved1 = { 0 };
    std::array<uint8_t, 5> reserved2 = { 0 };

    ar(obj.baseComId);
    ar(obj.numComIds);
    ar(reserved1);
    ar(obj.initialCPinSidIndicator);
    ar(obj.cPinSidRevertBehavior);
    ar(reserved2);
}


template <class Archive>
void load(Archive& ar, Pyrite2FeatureDesc& obj) {
    std::array<uint8_t, 5> reserved1 = { 0 };
    std::array<uint8_t, 5> reserved2 = { 0 };

    ar(obj.baseComId);
    ar(obj.numComIds);
    ar(reserved1);
    ar(obj.initialCPinSidIndicator);
    ar(obj.cPinSidRevertBehavior);
    ar(reserved2);
}


TPerDesc ParseTPerDesc(std::span<const uint8_t> bytes);