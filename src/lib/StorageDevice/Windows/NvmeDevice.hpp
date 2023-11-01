#pragma once

#include "../Common/NvmeStructures.hpp"
#include "../Common/StorageDevice.hpp"

#include <cstdint>
#include <span>
#include <string_view>


namespace sedmgr {

class NvmeDevice : public StorageDevice {
public:
    NvmeDevice(std::string_view name);
    NvmeDevice(const NvmeDevice&) = delete;
    NvmeDevice& operator=(const NvmeDevice&) = delete;
    ~NvmeDevice();

    NvmeControllerIdentity IdentifyController();
    void SecuritySend(uint8_t securityProtocol,
                      std::span<const std::byte, 2> protocolSpecific,
                      std::span<const std::byte> data) override;
    void SecurityReceive(uint8_t securityProtocol,
                         std::span<const std::byte, 2> protocolSpecific,
                         std::span<std::byte> data) override;

private:
    void* m_handle;
};

} // namespace sedmgr