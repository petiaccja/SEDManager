#include "NvmeDevice.hpp"

#include <errno.h>
#include <fcntl.h>
#include <linux/nvme_ioctl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <format>


void SendCommand(int file, nvme_admin_cmd command);


NvmeDevice::NvmeDevice(std::string_view name) {
    const auto fd = open(name.data(), O_RDWR);
    if (fd < 0) {
        throw std::runtime_error{
            std::format("failed to open NVMe device '{}': {}", name, strerror(errno))
        };
    }
    m_file = fd;
}
NvmeDevice::~NvmeDevice() {
    close(m_file);
}

NvmeIdentifyController NvmeDevice::IdentifyController() {
    nvme_admin_cmd command;
    memset(&command, 0, sizeof(command));
    NvmeIdentifyController data;
    command.opcode = static_cast<uint8_t>(eNvmeOpcodes::IDENTIFY_CONTROLLER);
    command.addr = reinterpret_cast<size_t>(&data);
    command.data_len = sizeof(data);
    command.cdw10 = 0x0000'0001;
    SendCommand(m_file, command);
    return data;
}

void NvmeDevice::SecuritySend(uint8_t securityProtocol,
                              std::span<const uint8_t, 2> protocolSpecific,
                              std::span<const uint8_t> data) {
    nvme_admin_cmd command;
    memset(&command, 0, sizeof(command));
    command.opcode = static_cast<uint8_t>(eNvmeOpcodes::SECURITY_SEND);
    command.addr = reinterpret_cast<size_t>(data.data());
    command.data_len = data.size();
    command.cdw10 = (securityProtocol << 24)
                    | (protocolSpecific[1] << 16)
                    | (protocolSpecific[0] << 8);
    command.cdw11 = command.data_len;
    SendCommand(m_file, command);
}

void NvmeDevice::SecurityReceive(uint8_t securityProtocol,
                                 std::span<const uint8_t, 2> protocolSpecific,
                                 std::span<uint8_t> data) {
    nvme_admin_cmd command;
    memset(&command, 0, sizeof(command));
    command.opcode = static_cast<uint8_t>(eNvmeOpcodes::SECURITY_RECV);
    command.addr = reinterpret_cast<size_t>(data.data());
    command.data_len = data.size();
    command.cdw10 = (securityProtocol << 24)
                    | (protocolSpecific[1] << 16)
                    | (protocolSpecific[0] << 8);
    command.cdw11 = command.data_len;
    SendCommand(m_file, command);
}

void SendCommand(int file, nvme_admin_cmd command) {
    const auto err = ioctl(file, NVME_IOCTL_ADMIN_CMD, &command);
    if (err < 0) {
        throw std::runtime_error{
            std::format("error communicating with NVMe device: {}", strerror(err))
        };
    }
    if (err != 0) {
        throw std::runtime_error{
            std::format("NVMe device returned an error: {}", err)
        };
    }
}
