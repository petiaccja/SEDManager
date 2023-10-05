#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string_view>

// Documentation:
// https://nvmexpress.org/wp-content/uploads/NVM-Express-Base-Specification-2_0-2021.06.02-Ratified-5.pdf

enum class eNvmeOpcodes {
    IDENTIFY_CONTROLLER = 0x06,
    SECURITY_SEND = 0x81,
    SECURITY_RECV = 0x82,
};

struct NvmeIdentifyController {
    uint16_t vendorId;
    uint16_t subsystemVendorId;
    std::array<char, 20> serialNumber;
    std::array<char, 40> modelNumber;
    std::array<char, 8> firmwareRevision;
    uint8_t recommendedArbitrationBurst;
    unsigned int ieeeOuiIdentifier : 24;
    std::array<char, 4020> reserved;
};


class NvmeDevice {
public:
    NvmeDevice(std::string_view name);
    NvmeDevice(const NvmeDevice&) = delete;
    NvmeDevice& operator=(const NvmeDevice&) = delete;
    ~NvmeDevice();

    NvmeIdentifyController IdentifyController();
    void SecuritySend(uint8_t securityProtocol,
                      std::span<const uint8_t, 2> protocolSpecific,
                      std::span<const uint8_t> data);
    void SecurityReceive(uint8_t securityProtocol,
                         std::span<const uint8_t, 2> protocolSpecific,
                         std::span<uint8_t> data);

private:
    int m_file;
};
