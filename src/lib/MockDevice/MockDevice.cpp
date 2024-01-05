#include "MockDevice.hpp"

#include "MockPreconfig.hpp"
#include "MockSession.hpp"
#include "MockSetupLayer.hpp"

#include <Archive/Conversion.hpp>
#include <Messaging/Value.hpp>
#include <Error/Exception.hpp>
#include <Specification/Core/CoreModule.hpp>

namespace sedmgr {


MockDevice::MockDevice() {
    m_sps = std::make_shared<std::vector<MockSecurityProvider>>(GetMockPreconfig());

    AddOutputHandler(0x01, 0x0001, &MockDevice::Discovery);

    const auto baseSession = std::make_shared<MockSession>(m_sps, baseComId, 0x0000);
    AddInputHandler(0x01, baseComId, [baseSession](std::span<const std::byte> data) { baseSession->Input(data); });
    AddOutputHandler(0x01, baseComId, [baseSession](std::span<std::byte> data) { baseSession->Output(data); });

    const auto baseSetupLayer = std::make_shared<MockSetupLayer>(baseComId, 0x0000);
    AddInputHandler(0x02, baseComId, [baseSetupLayer](std::span<const std::byte> data) { baseSetupLayer->Input(data); });
    AddOutputHandler(0x02, baseComId, [baseSetupLayer](std::span<std::byte> data) { baseSetupLayer->Output(data); });
}


StorageDeviceDesc MockDevice::GetDesc() {
    return StorageDeviceDesc{
        .name = "Mock Device",
        .serial = "MOCK0001",
        .interface = eStorageDeviceInterface::OTHER,
    };
}


void MockDevice::AddInputHandler(uint8_t protocol, uint16_t comId, InputHandler handler) {
    const auto [it, fresh] = m_inputHandlers.insert({
        {protocol, comId},
        std::move(handler)
    });
    assert(fresh); // Bug in MockDevice's code.
}


void MockDevice::AddOutputHandler(uint8_t protocol, uint16_t comId, OutputHandler handler) {
    const auto [it, fresh] = m_outputHandlers.insert({
        {protocol, comId},
        std::move(handler)
    });
    assert(fresh); // Bug in MockDevice's code.
}


void MockDevice::RemoveInputHandler(uint8_t protocol, uint16_t comId) {
    const auto it = m_inputHandlers.find({ protocol, comId });
    assert(it != m_inputHandlers.end()); // Bug in MockDevice's code.
    m_inputHandlers.erase(it);
}


void MockDevice::RemoveOutputHandler(uint8_t protocol, uint16_t comId) {
    const auto it = m_outputHandlers.find({ protocol, comId });
    assert(it != m_outputHandlers.end()); // Bug in MockDevice's code.
    m_outputHandlers.erase(it);
}


bool MockDevice::HasInputHandler(uint8_t protocol, uint16_t comId) const {
    return m_inputHandlers.contains({ protocol, comId });
}


bool MockDevice::HasOutputHandler(uint8_t protocol, uint16_t comId) const {
    return m_outputHandlers.contains({ protocol, comId });
}


void MockDevice::SecuritySend(uint8_t securityProtocol,
                              std::span<const std::byte, 2> protocolSpecific,
                              std::span<const std::byte> data) {
    const uint16_t comId = uint16_t(protocolSpecific[0]) | uint16_t(protocolSpecific[1]) << 8;
    const auto it = m_inputHandlers.find({ securityProtocol, comId });
    if (it == m_inputHandlers.end()) {
        throw DeviceError(std::format("IF_SEND: invalid security protocol ({}) / ComID ({})", securityProtocol, comId));
    }
    it->second(data);
}


void MockDevice::SecurityReceive(uint8_t securityProtocol,
                                 std::span<const std::byte, 2> protocolSpecific,
                                 std::span<std::byte> data) {
    const uint16_t comId = uint16_t(protocolSpecific[0]) | uint16_t(protocolSpecific[1]) << 8;
    const auto it = m_outputHandlers.find({ securityProtocol, comId });
    if (it == m_outputHandlers.end()) {
        throw DeviceError(std::format("IF_RECV: invalid security protocol ({}) / ComID ({})", securityProtocol, comId));
    }
    it->second(data);
}


void MockDevice::Discovery(std::span<std::byte> data) {
    constexpr std::array discovery = {
        // Discovery header
        0_b, 0_b, 0_b, 52_b, // Length of data
        0_b, 0_b, // Version major
        0_b, 0_b, // Version minor
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // VU
        // TPer desc
        0x00_b, 0x01_b, // Feature code
        0x10_b, // Version
        0x0C_b, // Length
        0b0001'0001_b, // Bitmask
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, // Reserved
        // Locking desc
        0x00_b, 0x02_b, // Feature code
        0x10_b, // Version
        0x0C_b, // Length
        0b0000'0000_b, // Bitmask
        0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, // Reserved
        // Mock SSC desc == Opal v1 for now
        0x02_b, 0x00_b, // Feature code
        0x10_b, // Version | Reserved
        0x10_b, // Length
        std::byte(baseComId >> 8), std::byte(baseComId), // Base ComID
        0x00_b, 0x01_b, // Num ComIDs
        0x00_b, // Reserved
        0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, 0_b, // Reserved
        0_b, 0_b, 0_b, // Reserved
    };
    if (data.size() < discovery.size()) {
        throw DeviceError("receive buffer too small");
    }
    std::ranges::copy(discovery, data.begin());
}

} // namespace sedmgr