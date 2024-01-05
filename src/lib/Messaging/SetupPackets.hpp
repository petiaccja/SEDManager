#pragma once

#include <cereal/types/array.hpp>

#include <array>
#include <cstdint>


namespace sedmgr {

enum class eComIdState : uint32_t {
    INVALID = 0x00,
    INACTIVE = 0x01,
    ISSUED = 0x02,
    ASSOCIATED = 0x03,
};

enum class eStackResetStatus : uint32_t {
    SUCCESS = 0,
    FAILURE = 1,
};


struct VerifyComIdValidRequest {
    uint16_t comId;
    uint16_t comIdExtension;
    static constexpr uint32_t requestCode = 0x00'00'00'01;
};


struct StackResetRequest {
    uint16_t comId;
    uint16_t comIdExtension;
    static constexpr uint32_t requestCode = 0x00'00'00'02;
};


struct VerifyComIdValidResponse {
    uint16_t comId;
    uint16_t comIdExtension;
    uint32_t requestCode;
    uint16_t availableDataLength;
    eComIdState comIdState;
    std::array<uint8_t, 10> timeOfAlloc;
    std::array<uint8_t, 10> timeOfExpiry;
    std::array<uint8_t, 10> timeCurrent;
};


struct StackResetResponse {
    uint16_t comId;
    uint16_t comIdExtension;
    uint32_t requestCode;
    uint16_t availableDataLength;
    eStackResetStatus success;
};


template <class Archive>
void save(Archive& ar, const StackResetRequest& obj) {
    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.requestCode);
}


template <class Archive>
void save(Archive& ar, const VerifyComIdValidRequest& obj) {
    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.requestCode);
}


template <class Archive>
void load(Archive& ar, StackResetResponse& obj) {
    uint16_t reserved1 = 0;
    uint32_t success;

    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.requestCode);
    ar(reserved1);
    ar(obj.availableDataLength);
    ar(success);
    obj.success = static_cast<eStackResetStatus>(success);
}


template <class Archive>
void save(Archive& ar, const StackResetResponse& obj) {
    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.requestCode);
    ar(uint16_t(0));
    ar(obj.availableDataLength);
    ar(static_cast<uint32_t>(obj.success));
}


template <class Archive>
void load(Archive& ar, VerifyComIdValidResponse& obj) {
    uint16_t reserved;
    uint32_t comIdState;

    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.requestCode);
    ar(reserved);
    ar(obj.availableDataLength);
    ar(comIdState);
    obj.comIdState = static_cast<eComIdState>(comIdState);
    ar(obj.timeOfAlloc);
    ar(obj.timeOfExpiry);
    ar(obj.timeCurrent);
}


template <class Archive>
void save(Archive& ar, const VerifyComIdValidResponse& obj) {
    ar(obj.comId);
    ar(obj.comIdExtension);
    ar(obj.requestCode);
    ar(uint16_t(0));
    ar(obj.availableDataLength);
    ar(static_cast<uint32_t>(obj.comIdState));
    ar(obj.timeOfAlloc);
    ar(obj.timeOfExpiry);
    ar(obj.timeCurrent);
}

} // namespace sedmgr