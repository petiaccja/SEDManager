#pragma once

#include <cstdint>
#include <span>
#include <string_view>


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
