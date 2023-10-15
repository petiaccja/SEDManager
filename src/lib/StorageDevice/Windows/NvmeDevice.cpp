#include "NvmeDevice.hpp"

#include "../Exception.hpp"
#include "NvmeDevice.hpp"

#include <Windows.h>
#include <ntddscsi.h>
#include <nvme.h>
#include <winioctl.h>

#include <algorithm>
#include <cassert>
#include <format>
#include <vector>


void SendCommand(HANDLE handle,
                 uint8_t securityProtocol,
                 std::span<const std::byte, 2> protocolSpecific,
                 void* data,
                 size_t dataLength,
                 eNvmeOpcode opcode);


NvmeDevice::NvmeDevice(std::string_view name) {
    const auto handle = CreateFileA(name.data(),
                                    GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    nullptr,
                                    OPEN_EXISTING,
                                    0,
                                    nullptr);

    if (handle == INVALID_HANDLE_VALUE) {
        throw DeviceError{
            std::format("cannot open device '{}': {}", name, GetLastError())
        };
    }

    m_handle = handle;
}
NvmeDevice::~NvmeDevice() {
    CloseHandle(m_handle);
}

NvmeIdentifyController NvmeDevice::IdentifyController() {
    std::array protocolSpecific{ std::byte(0), std::byte(0) };
    NvmeIdentifyController data;
    SendCommand(m_handle, 0, protocolSpecific, &data, sizeof(data), eNvmeOpcode::IDENTIFY_CONTROLLER);
    return data;
}


void FillScsiPassthrough(SCSI_PASS_THROUGH_DIRECT& scsiCcmmand,
                         eNvmeOpcode opcode,
                         uint8_t securityProtocol,
                         std::span<const std::byte, 2> protocolSpecific,
                         size_t senseLength,
                         size_t senseOffset,
                         void* data,
                         size_t dataLength) {
    memset(&scsiCcmmand, 0, sizeof(SCSI_PASS_THROUGH_DIRECT));
    scsiCcmmand.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsiCcmmand.PathId = 0;
    scsiCcmmand.TargetId = 1;
    scsiCcmmand.Lun = 0;
    scsiCcmmand.SenseInfoLength = UCHAR(senseLength);

    scsiCcmmand.DataTransferLength = ULONG(dataLength);
    scsiCcmmand.TimeOutValue = 2;
    scsiCcmmand.DataBuffer = data;
    scsiCcmmand.SenseInfoOffset = ULONG(senseOffset);

    if (opcode == eNvmeOpcode::SECURITY_SEND || opcode == eNvmeOpcode::SECURITY_RECV) {
        scsiCcmmand.Cdb[1] = securityProtocol;
        scsiCcmmand.Cdb[2] = static_cast<UCHAR>(protocolSpecific[1]);
        scsiCcmmand.Cdb[3] = static_cast<UCHAR>(protocolSpecific[0]);
        scsiCcmmand.Cdb[6] = uint8_t(dataLength >> 24);
        scsiCcmmand.Cdb[7] = uint8_t(dataLength >> 16);
        scsiCcmmand.Cdb[8] = uint8_t(dataLength >> 8);
        scsiCcmmand.Cdb[9] = uint8_t(dataLength);
        scsiCcmmand.CdbLength = 12;
    }
    switch (opcode) {
        case eNvmeOpcode::IDENTIFY_CONTROLLER:
            scsiCcmmand.Cdb[0] = 0x12;
            scsiCcmmand.Cdb[4] = 0x60;
            scsiCcmmand.CdbLength = 6;
            scsiCcmmand.DataIn = SCSI_IOCTL_DATA_IN;
            break;
        case eNvmeOpcode::SECURITY_SEND:
            scsiCcmmand.Cdb[0] = 0xb5;
            scsiCcmmand.DataIn = SCSI_IOCTL_DATA_OUT;
            break;
        case eNvmeOpcode::SECURITY_RECV:
            scsiCcmmand.Cdb[0] = 0xA2;
            scsiCcmmand.DataIn = SCSI_IOCTL_DATA_IN;
            break;
        default: std::terminate();
    }
}


void NvmeDevice::SecuritySend(uint8_t securityProtocol,
                              std::span<const std::byte, 2> protocolSpecific,
                              std::span<const std::byte> data) {
    const auto buffer = const_cast<void*>(reinterpret_cast<const void*>(data.data()));
    SendCommand(m_handle, securityProtocol, protocolSpecific, buffer, data.size(), eNvmeOpcode::SECURITY_SEND);
}

void NvmeDevice::SecurityReceive(uint8_t securityProtocol,
                                 std::span<const std::byte, 2> protocolSpecific,
                                 std::span<std::byte> data) {
    SendCommand(m_handle, securityProtocol, protocolSpecific, data.data(), data.size(), eNvmeOpcode::SECURITY_RECV);
}

void SendCommand(HANDLE handle,
                 uint8_t securityProtocol,
                 std::span<const std::byte, 2> protocolSpecific,
                 void* data,
                 size_t dataLength,
                 eNvmeOpcode opcode) {
    constexpr size_t commandLength = sizeof(SCSI_PASS_THROUGH_DIRECT);
    constexpr size_t senseLength = 32;
    constexpr size_t senseOffset = (commandLength + sizeof(void*) - 1) / sizeof(void*) * sizeof(void*);
    alignas(sizeof(void*)) std::array<std::byte, senseOffset + senseLength> commandBuffer;
    auto scsiCommand = reinterpret_cast<SCSI_PASS_THROUGH_DIRECT*>(commandBuffer.data());

    FillScsiPassthrough(*scsiCommand,
                        opcode,
                        securityProtocol,
                        protocolSpecific,
                        senseLength,
                        senseOffset,
                        data,
                        dataLength);

    DWORD returnedLength = 0;
    const bool success = DeviceIoControl(handle,
                                         IOCTL_SCSI_PASS_THROUGH_DIRECT,
                                         commandBuffer.data(),
                                         commandBuffer.size(),
                                         commandBuffer.data(),
                                         commandBuffer.size(),
                                         &returnedLength,
                                         nullptr);
    if (!success || scsiCommand->ScsiStatus != 0) {
        throw std::format("device returned an error: NVMe status = {}", scsiCommand->ScsiStatus);
    }
}
