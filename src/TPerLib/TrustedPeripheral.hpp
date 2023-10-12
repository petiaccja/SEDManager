#pragma once

#include "Communication/ComIdSetup.hpp"
#include "Communication/Discovery.hpp"
#include "NvmeDevice.hpp"

#include <chrono>
#include <memory>

struct ComPacket;


class TrustedPeripheral {
public:
    TrustedPeripheral(std::shared_ptr<NvmeDevice> storageDevice);
    ~TrustedPeripheral();

    uint16_t GetComId() const;
    uint16_t GetComIdExtension() const;
    const TPerDesc& GetDesc() const;
    eComIdState VerifyComId();
    void StackReset();
    void Reset();

    ComPacket SendPacket(uint8_t protocol, const ComPacket& packet);

private:
    void SecuritySend(uint8_t protocol, uint16_t comId, std::span<const std::byte> payload);
    void SecurityReceive(uint8_t protocol, uint16_t comId, std::span<std::byte> response);

    TPerDesc Discovery();
    std::pair<uint16_t, uint16_t> RequestComId();

private:
    std::shared_ptr<NvmeDevice> m_storageDevice;
    uint16_t m_comId;
    uint16_t m_comIdExtension;
    TPerDesc m_desc;
};