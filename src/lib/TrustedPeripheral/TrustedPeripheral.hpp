#pragma once

#include "Discovery.hpp"
#include "TPerModules.hpp"
#include <async++/task.hpp>

#include <Data/SetupPackets.hpp>
#include <StorageDevice/NvmeDevice.hpp>

#include <chrono>
#include <memory>


namespace sedmgr {

struct ComPacket;


class TrustedPeripheral {
public:
    TrustedPeripheral(std::shared_ptr<StorageDevice> storageDevice);
    ~TrustedPeripheral();

    const TPerDesc& GetDesc() const;
    const TPerModules& GetModules() const;

    uint16_t GetComId() const;
    uint16_t GetComIdExtension() const;
    asyncpp::task<eComIdState> VerifyComId();
    asyncpp::task<void> StackReset();
    void Reset();

    asyncpp::task<ComPacket> SendPacket(uint8_t protocol, const ComPacket& packet);

private:
    void SecuritySend(uint8_t protocol, uint16_t comId, std::span<const std::byte> payload);
    void SecurityReceive(uint8_t protocol, uint16_t comId, std::span<std::byte> response);
    asyncpp::task<std::vector<ComPacket>> FlushResponses(uint8_t protocol);

    TPerDesc Discovery();
    std::pair<uint16_t, uint16_t> RequestComId();

private:
    std::shared_ptr<StorageDevice> m_storageDevice;
    uint16_t m_comId;
    uint16_t m_comIdExtension;
    TPerDesc m_desc;
    TPerModules m_modules;
};

} // namespace sedmgr