#pragma once

#include "MockSecurityProvider.hpp"

#include <Messaging/NativeTypes.hpp>
#include <TrustedPeripheral/Method.hpp>

#include <cstdint>
#include <queue>
#include <span>


namespace sedmgr {

class MockSetupLayer {
public:
    MockSetupLayer(uint16_t comId,
                   uint16_t comIdExt);

    void Input(std::span<const std::byte> data);
    void Output(std::span<std::byte> data);

private:
    void VerifyComIdValid();
    void StackReset();
    void EnqueueResponse(std::vector<std::byte> value);

private:
    const uint16_t m_comId;
    const uint16_t m_comIdExt;
    std::queue<std::vector<std::byte>> m_responseQueue;
};

} // namespace sedmgr