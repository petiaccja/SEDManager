#pragma once


#include "MockSecurityProvider.hpp"

#include <Data/ComPacket.hpp>
#include <StorageDevice/Common/StorageDevice.hpp>
#include <TrustedPeripheral/Method.hpp>

#include <bit>
#include <functional>
#include <map>
#include <queue>
#include <span>
#include <vector>


namespace sedmgr {

class MockDevice : public StorageDevice {
    struct SessionParams {
        uint32_t tsn;
        uint32_t hsn;
    };

public:
    MockDevice();

    StorageDeviceDesc GetDesc() override;
    void SecuritySend(uint8_t securityProtocol,
                      std::span<const std::byte, 2> protocolSpecific,
                      std::span<const std::byte> data) override;
    void SecurityReceive(uint8_t securityProtocol,
                         std::span<const std::byte, 2> protocolSpecific,
                         std::span<std::byte> data) override;

private:
    using InputHandler = std::function<void(std::span<const std::byte>)>;
    using OutputHandler = std::function<void(std::span<std::byte>)>;
    void AddInputHandler(uint8_t protocol, uint16_t comId, InputHandler handler);
    void AddOutputHandler(uint8_t protocol, uint16_t comId, OutputHandler handler);
    void RemoveInputHandler(uint8_t protocol, uint16_t comId);
    void RemoveOutputHandler(uint8_t protocol, uint16_t comId);
    bool HasInputHandler(uint8_t protocol, uint16_t comId) const;
    bool HasOutputHandler(uint8_t protocol, uint16_t comId) const;

    static void Discovery(std::span<std::byte> data);

private:
    static constexpr uint16_t baseComId = 0xBEEF;
    std::shared_ptr<std::vector<MockSecurityProvider>> m_sps;
    std::map<std::tuple<uint8_t, uint16_t>, InputHandler> m_inputHandlers;
    std::map<std::tuple<uint8_t, uint16_t>, OutputHandler> m_outputHandlers;
};

} // namespace sedmgr