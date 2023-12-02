#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>


namespace sedmgr {

enum class eStorageDeviceInterface {
    ATA,
    SATA,
    SCSI,
    NVME,
    SD,
    MMC,
    OTHER,
};


struct StorageDeviceLabel {
    std::string path;
    eStorageDeviceInterface interface;
};


struct StorageDeviceDesc {
    std::string name;
    std::string serial;
    eStorageDeviceInterface interface;
};


std::vector<StorageDeviceLabel> EnumerateStorageDevices();


class StorageDevice {
public:
    virtual ~StorageDevice() {}

    virtual StorageDeviceDesc GetDesc() = 0;
    virtual void SecuritySend(uint8_t securityProtocol,
                              std::span<const std::byte, 2> protocolSpecific,
                              std::span<const std::byte> data) = 0;
    virtual void SecurityReceive(uint8_t securityProtocol,
                                 std::span<const std::byte, 2> protocolSpecific,
                                 std::span<std::byte> data) = 0;
};

} // namespace sedmgr