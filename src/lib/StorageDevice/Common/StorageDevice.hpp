#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>


enum class eStorageDeviceInterface {
    NVME,
    SATA,
    SCSI,
    OTHER,
};


struct StorageDeviceLabel {
    std::string path;
    eStorageDeviceInterface interface;
};


std::vector<StorageDeviceLabel> EnumerateStorageDevices();


class StorageDevice {
public:
    virtual ~StorageDevice() {}

    virtual void SecuritySend(uint8_t securityProtocol,
                              std::span<const std::byte, 2> protocolSpecific,
                              std::span<const std::byte> data) = 0;
    virtual void SecurityReceive(uint8_t securityProtocol,
                                 std::span<const std::byte, 2> protocolSpecific,
                                 std::span<std::byte> data) = 0;
};
