#include "TrustedPeripheral.hpp"

#include "Serialization/Utility.hpp"
#include "Communication/ComIdSetup.hpp"
#include "Communication/Packet.hpp"

#include <cereal/archives/portable_binary.hpp>

#include <array>
#include <stdexcept>
#include <thread>



std::pair<uint16_t, uint16_t> ExtractBaseComId(const auto& desc) {
    if constexpr (!std::is_convertible_v<decltype(desc), std::monostate>) {
        return { desc.baseComId, 0 };
    }
    throw std::invalid_argument("invalid SSC descriptor");
};



TrustedPeripheral::TrustedPeripheral(std::unique_ptr<NvmeDevice> storageDevice) : m_storageDevice(std::move(storageDevice)) {
    m_desc = Discovery();
    if (m_desc.tperDesc) {
        try {
            if (m_desc.tperDesc->comIdMgmtSupported) {
                std::tie(m_comId, m_comIdExtension) = RequestComId();
            }
            else {
                std::tie(m_comId, m_comIdExtension) = std::visit(
                    [](const auto& desc) { return ExtractBaseComId(desc); },
                    m_desc.sscDesc);
            }
        }
        catch (std::exception& ex) {
            throw std::runtime_error("could not acquire or determine ComID");
        }
    }
    else {
        throw std::runtime_error("no TPer description in level 0 discovery");
    }
}


TrustedPeripheral::~TrustedPeripheral() {
    try {
        StackReset();
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}


uint16_t TrustedPeripheral::GetComId() const {
    return m_comId;
}


uint16_t TrustedPeripheral::GetComIdExtension() const {
    return m_comIdExtension;
}


const TPerDesc& TrustedPeripheral::GetDesc() const {
    return m_desc;
}


eComIdState TrustedPeripheral::VerifyComId() {
    // Send VERIFY_COMID_VALID command.
    VerifyComIdValidRequest payload{
        .comId = m_comId,
        .comIdExtension = m_comIdExtension,
    };
    const auto payloadBytes = ToBytes(payload);
    SecuritySend(0x02, m_comId, payloadBytes);

    // Get VERIFY_COMID_VALID response.
    VerifyComIdValidResponse response;
    do {
        std::array<uint8_t, 46> responseBytes;
        std::ranges::fill(responseBytes, 0);
        SecurityReceive(0x02, m_comId, responseBytes);
        FromBytes(responseBytes, response);

        if (response.requestCode == 0) {
            throw std::runtime_error("verify comid valid failed: no response available");
        }
        if (response.availableDataLength == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    } while (response.availableDataLength == 0);

    return response.comIdState;
}


ComPacket TrustedPeripheral::SendPacket(uint8_t protocol, const ComPacket& packet) {
    const auto request = ToBytes(packet);

    SecuritySend(protocol, GetComId(), request);

    bool more = true;
    ComPacket result;
    std::vector<uint8_t> response(2048, 0);
    do {
        SecurityReceive(protocol, GetComId(), response);
        FromBytes(response, result);

        more = result.outstandingData != 0;
        if (result.minTransfer > response.size()) {
            size_t newSize = std::max(size_t(result.minTransfer), size_t(1048576));
            response.resize(result.minTransfer);
        }
        if (more) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    } while (more);

    return result;
}


void TrustedPeripheral::SecuritySend(uint8_t protocol, uint16_t comId, std::span<const uint8_t> payload) {
    const std::array comIdBytes = { uint8_t(comId >> 8), uint8_t(comId & 0xFF) };
    const std::array comIdBytesFlip = { comIdBytes[1], comIdBytes[0] };
    m_storageDevice->SecuritySend(protocol, comIdBytesFlip, payload);
}


void TrustedPeripheral::SecurityReceive(uint8_t protocol, uint16_t comId, std::span<uint8_t> response) {
    const std::array comIdBytes = { uint8_t(comId >> 8), uint8_t(comId & 0xFF) };
    const std::array comIdBytesFlip = { comIdBytes[1], comIdBytes[0] };
    m_storageDevice->SecurityReceive(protocol, comIdBytesFlip, response);
}


TPerDesc TrustedPeripheral::Discovery() {
    alignas(1024) std::array<uint8_t, 2048> response;
    std::ranges::fill(response, 0);
    SecurityReceive(0x01, 0x0001, response);
    TPerDesc desc = ParseTPerDesc(response);
    return desc;
}


std::pair<uint16_t, uint16_t> TrustedPeripheral::RequestComId() {
    std::array<uint8_t, 4> response;
    std::ranges::fill(response, 0xFF);
    SecurityReceive(0x02, 0x0000, response);
    uint16_t comId = 0;
    FromBytes(response, comId);
    uint16_t comIdExtension = 0;
    FromBytes(std::span{ response }.subspan(2), comId);
    return { comId, comIdExtension };
}


void TrustedPeripheral::StackReset() {
    // Send STACK_RESET command.
    StackResetRequest payload{
        .comId = m_comId,
        .comIdExtension = m_comIdExtension
    };
    const auto payloadBytes = ToBytes(payload);
    SecuritySend(0x02, m_comId, payloadBytes);

    // Get STACK_RESET response.
    StackResetResponse response;
    do {
        std::array<uint8_t, 20> responseBytes;
        std::ranges::fill(responseBytes, 0);
        SecurityReceive(0x02, m_comId, responseBytes);
        FromBytes(responseBytes, response);

        if (response.requestCode == 0) {
            throw std::runtime_error("stack reset failed: no response available");
        }
        if (response.availableDataLength == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    } while (response.availableDataLength == 0);

    if (response.success != eStackResetStatus::SUCCESS) {
        throw std::runtime_error("stack reset failed with failure code");
    }
}
