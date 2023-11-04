#include "MockSetupLayer.hpp"

#include <Archive/Conversion.hpp>
#include <Data/SetupPackets.hpp>

namespace sedmgr {

MockSetupLayer::MockSetupLayer(uint16_t comId,
                               uint16_t comIdExt)
    : m_comId(comId), m_comIdExt(comIdExt) {}


void MockSetupLayer::Input(std::span<const std::byte> data) {
    if (data.size() < 8) {
        throw DeviceError("Mock: IF-SEND 0x02: send buffer too small");
    }
    uint16_t comId = 0;
    uint16_t comIdExt = 0;
    uint32_t requestCode = 0;
    FromBytes(data.subspan(0, 2), comId);
    FromBytes(data.subspan(2, 2), comIdExt);
    FromBytes(data.subspan(4, 4), requestCode);
    switch (requestCode) {
        case VerifyComIdValidRequest::requestCode: VerifyComIdValid(); break;
        case StackResetRequest::requestCode: StackReset(); break;
        default: throw DeviceError("Mock: IF-SEND 0x02: invalid request code");
    }
}


void MockSetupLayer::Output(std::span<std::byte> data) {
    if (m_responseQueue.empty()) {
        throw DeviceError("Mock: IF-RECV 0x02: no response available");
    }
    if (m_responseQueue.front().size() > data.size()) {
        throw DeviceError("Mock: IF-RECV 0x02: receive buffer too small");
    }
    std::ranges::copy(m_responseQueue.front(), data.begin());
}


void MockSetupLayer::VerifyComIdValid() {
    const VerifyComIdValidResponse response = {
        .comId = m_comId,
        .comIdExtension = m_comIdExt,
        .requestCode = VerifyComIdValidRequest::requestCode,
        .availableDataLength = 0x22,
        .comIdState = eComIdState::ISSUED,
        .timeOfAlloc = {},
        .timeOfExpiry = {},
        .timeCurrent = {},
    };
    EnqueueResponse(ToBytes(response));
}


void MockSetupLayer::StackReset() {
    const StackResetResponse response = {
        .comId = m_comId,
        .comIdExtension = m_comIdExt,
        .requestCode = StackResetRequest::requestCode,
        .availableDataLength = 4,
        .success = eStackResetStatus::SUCCESS,
    };
    EnqueueResponse(ToBytes(response));
}


void MockSetupLayer::EnqueueResponse(std::vector<std::byte> value) {
    m_responseQueue.push(std::move(value));
}

} // namespace sedmgr