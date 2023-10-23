#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>

#include <cstdint>
#include <optional>
#include <span>
#include <variant>


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


//------------------------------------------------------------------------------
// Device features
//------------------------------------------------------------------------------
struct TPerFeatureDesc {
    static constexpr uint16_t featureCode = 0x0001;

    bool comIdMgmtSupported;
    bool streamingSupported;
    bool bufferMgmtSupported;
    bool ackNakSupported;
    bool asyncSupported;
    bool syncSupported;
};


struct LockingFeatureDesc {
    static constexpr uint16_t featureCode = 0x0002;

    bool mbrSupported;
    bool mbrDone;
    bool mbrEnabled;
    bool mediaEncryption;
    bool locked;
    bool lockingEnabled;
    bool lockingSupported;
};


//------------------------------------------------------------------------------
// SSC features
//------------------------------------------------------------------------------

struct BasicSSCFeatureDesc {
    uint16_t baseComId;
    uint16_t numComIds;
};

struct EnterpriseFeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0100;
    static constexpr std::string_view featureName = "Enterprise";

    bool crossingRangeBehavior;
};


struct Opal1FeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0200;
    static constexpr std::string_view featureName = "Opal v1";

    bool crossingRangeBehavior;
};

struct Opal2FeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0203;
    static constexpr std::string_view featureName = "Opal v2";

    bool crossingRangeBehavior;
    uint16_t numAdminsSupported;
    uint16_t numUsersSupported;
    bool initialCPinSidIndicator;
    bool cPinSidRevertBehavior;
};

struct OpaliteFeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0301;
    static constexpr std::string_view featureName = "Opalite";

    bool initialCPinSidIndicator;
    bool cPinSidRevertBehavior;
};

struct Pyrite1FeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0302;
    static constexpr std::string_view featureName = "Pyrite v1";

    bool initialCPinSidIndicator;
    bool cPinSidRevertBehavior;
};

struct Pyrite2FeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0303;
    static constexpr std::string_view featureName = "Pyrite v2";

    bool initialCPinSidIndicator;
    bool cPinSidRevertBehavior;
};

struct RubyFeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0304;
    static constexpr std::string_view featureName = "Ruby";

    bool crossingRangeBehavior;
    uint16_t numAdminsSupported;
    uint16_t numUsersSupported;
    bool initialCPinSidIndicator;
    bool cPinSidRevertBehavior;
};

struct KeyPerIOFeatureDesc : BasicSSCFeatureDesc {
    static constexpr uint16_t featureCode = 0x0305;
    static constexpr std::string_view featureName = "Key Per I/O";
};


using SSCFeatureDesc = std::variant<
    EnterpriseFeatureDesc,
    Opal1FeatureDesc,
    Opal2FeatureDesc,
    OpaliteFeatureDesc,
    Pyrite1FeatureDesc,
    Pyrite2FeatureDesc,
    RubyFeatureDesc,
    KeyPerIOFeatureDesc>;


//------------------------------------------------------------------------------
// Device description
//------------------------------------------------------------------------------

struct TPerDesc {
    std::optional<TPerFeatureDesc> tperDesc;
    std::optional<LockingFeatureDesc> lockingDesc;
    std::vector<SSCFeatureDesc> sscDescs;
};


//------------------------------------------------------------------------------
// Serialization
//------------------------------------------------------------------------------

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


template <class Archive, std::derived_from<BasicSSCFeatureDesc> ConcreteSSCFeatureDesc>
void load(Archive& ar, ConcreteSSCFeatureDesc& obj) {
    // Always present
    ar(obj.baseComId);
    ar(obj.numComIds);

    // Key Per I/O has a completely different layout.
    if constexpr (!std::is_base_of_v<ConcreteSSCFeatureDesc, KeyPerIOFeatureDesc>) {
        uint8_t byte8;
        uint16_t numAdminsSupported;
        uint16_t numUsersSupported;
        uint8_t initialCPinSidIndicator;
        uint8_t cPinSidRevertBehavior;
        std::array<uint8_t, 5> reserved;

        ar(byte8);
        ar(numAdminsSupported);
        ar(numUsersSupported);
        ar(initialCPinSidIndicator);
        ar(cPinSidRevertBehavior);
        ar(reserved);

        if constexpr (requires { obj.crossingRangeBehavior; }) {
            obj.crossingRangeBehavior = byte8 & 0x01;
        }
        if constexpr (requires { obj.numAdminsSupported; }) {
            obj.numAdminsSupported = numAdminsSupported;
        }
        if constexpr (requires { obj.numUsersSupported; }) {
            obj.numUsersSupported = numUsersSupported;
        }
        if constexpr (requires { obj.initialCPinSidIndicator; }) {
            obj.initialCPinSidIndicator = 0x00 != initialCPinSidIndicator;
        }
        if constexpr (requires { obj.cPinSidRevertBehavior; }) {
            obj.cPinSidRevertBehavior = 0x00 != cPinSidRevertBehavior;
        }
    }
    else {
        // Key Per I/O is not supported beyond detecting it as a feature.
    }
}


TPerDesc ParseTPerDesc(std::span<const std::byte> bytes);