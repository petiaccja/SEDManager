#pragma once

#include <cstdint>
#include <string>


// Documentation:
// https://nvmexpress.org/wp-content/uploads/NVM-Express-Base-Specification-2_0-2021.06.02-Ratified-5.pdf


namespace sedmgr {

enum class eNvmeOpcode {
    IDENTIFY_CONTROLLER = 0x06,
    SECURITY_SEND = 0x81,
    SECURITY_RECV = 0x82,
};

struct NvmeControllerIdentity {
    uint16_t vendorId = 0;
    uint16_t subsystemVendorId = 0;
    std::string serialNumber;
    std::string modelNumber;
    std::string firmwareRevision;
    uint8_t recommendedArbitrationBurst = 0;
    unsigned int ieeeOuiIdentifier = 0;
};

} // namespace sedmgr