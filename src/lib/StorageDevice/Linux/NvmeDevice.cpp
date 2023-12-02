#include "NvmeDevice.hpp"

#include <Error/Exception.hpp>

#include <errno.h>
#include <fcntl.h>
#include <linux/nvme_ioctl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <format>


namespace sedmgr {

void SendCommand(int file, nvme_admin_cmd command);


NvmeDevice::NvmeDevice(std::string_view name) {
    const auto fd = open(name.data(), O_RDWR);
    if (fd < 0) {
        throw DeviceError{
            std::format("cannot open device '{}': {}", name, strerror(errno))
        };
    }
    m_file = fd;
}
NvmeDevice::~NvmeDevice() {
    close(m_file);
}


std::string_view Trim(std::string_view s) {
    auto it = std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace(c); });
    return { std::begin(s), it.base() };
}


NvmeControllerIdentity NvmeDevice::IdentifyController() {
    nvme_admin_cmd command;
    memset(&command, 0, sizeof(command));
    std::array<std::byte, 4096> data;
    command.opcode = static_cast<uint8_t>(eNvmeOpcode::IDENTIFY_CONTROLLER);
    command.addr = reinterpret_cast<size_t>(data.data());
    command.data_len = data.size();
    command.cdw10 = 0x0000'0001;
    SendCommand(m_file, command);

    NvmeControllerIdentity identity = {
        .vendorId = uint16_t(unsigned(data[0]) << 8 | unsigned(data[1])),
        .subsystemVendorId = uint16_t(unsigned(data[2]) << 8 | unsigned(data[3])),
        .serialNumber = std::string(Trim({ (const char*)data.data() + 4, (const char*)data.data() + 24 })),
        .modelNumber = std::string(Trim({ (const char*)data.data() + 24, (const char*)data.data() + 64 })),
        .firmwareRevision = std::string(Trim({ (const char*)data.data() + 64, (const char*)data.data() + 72 })),
        .recommendedArbitrationBurst = uint8_t(data[72]),
        .ieeeOuiIdentifier = uint16_t(unsigned(data[73]) << 16 | unsigned(data[74]) << 8 | unsigned(data[75])),
    };

    return identity;
}

StorageDeviceDesc NvmeDevice::GetDesc() {
    auto id = IdentifyController();
    return StorageDeviceDesc{
        .name = id.modelNumber,
        .serial = id.serialNumber,
        .interface = eStorageDeviceInterface::NVME,
    };
}

void NvmeDevice::SecuritySend(uint8_t securityProtocol,
                              std::span<const std::byte, 2> protocolSpecific,
                              std::span<const std::byte> data) {
    nvme_admin_cmd command;
    memset(&command, 0, sizeof(command));
    command.opcode = static_cast<uint8_t>(eNvmeOpcode::SECURITY_SEND);
    command.addr = reinterpret_cast<size_t>(data.data());
    command.data_len = data.size();
    command.cdw10 = (securityProtocol << 24)
                    | (uint32_t(protocolSpecific[1]) << 16)
                    | (uint32_t(protocolSpecific[0]) << 8);
    command.cdw11 = command.data_len;
    SendCommand(m_file, command);
}

void NvmeDevice::SecurityReceive(uint8_t securityProtocol,
                                 std::span<const std::byte, 2> protocolSpecific,
                                 std::span<std::byte> data) {
    nvme_admin_cmd command;
    memset(&command, 0, sizeof(command));
    command.opcode = static_cast<uint8_t>(eNvmeOpcode::SECURITY_RECV);
    command.addr = reinterpret_cast<size_t>(data.data());
    command.data_len = data.size();
    command.cdw10 = (securityProtocol << 24)
                    | (uint32_t(protocolSpecific[1]) << 16)
                    | (uint32_t(protocolSpecific[0]) << 8);
    command.cdw11 = command.data_len;
    SendCommand(m_file, command);
}

void SendCommand(int file, nvme_admin_cmd command) {
    const auto err = ioctl(file, NVME_IOCTL_ADMIN_CMD, &command);
    if (err != 0) {
        throw DeviceError{
            std::format("device returned an error: NVMe status = {}, command status = {}", err & 0x7FFu, command.result)
        };
    }
}

} // namespace sedmgr