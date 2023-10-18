#include "TrustedPeripheral.hpp"

#include "Method.hpp"

#include <Error/Exception.hpp>
#include <Data/SetupPackets.hpp>

#include <Archive/Conversion.hpp>
#include <Data/ComPacket.hpp>

#include <array>
#include <stdexcept>
#include <thread>



std::pair<uint16_t, uint16_t> ExtractBaseComId(const auto& desc) {
    if constexpr (!std::is_convertible_v<decltype(desc), std::monostate>) {
        return { desc.baseComId, 0 };
    }
    throw std::invalid_argument("invalid SSC descriptor");
};



TrustedPeripheral::TrustedPeripheral(std::shared_ptr<StorageDevice> storageDevice) : m_storageDevice(std::move(storageDevice)) {
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
        std::array<std::byte, 46> responseBytes;
        std::ranges::fill(responseBytes, 0_b);
        SecurityReceive(0x02, m_comId, responseBytes);
        FromBytes(responseBytes, response);

        if (response.requestCode == 0) {
            throw NoResponseError("VERIFY_COMID_VALID");
        }
        if (response.availableDataLength == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    } while (response.availableDataLength == 0);

    return response.comIdState;
}


void TrustedPeripheral::Reset() {
    std::array payload{ std::byte(0) };
    SecuritySend(0x02, 0x0004, payload);
}


template <typename Rep, typename Period>
static void Sleep(std::chrono::duration<Rep, Period> time) {
    using namespace std::chrono;

    if (time < milliseconds(1)) {
        const auto start = high_resolution_clock::now();
        while (high_resolution_clock::now() - start < time) {
            // Busy wait loop.
        }
    }
    else {
        std::this_thread::sleep_for(time);
    }
}


ComPacket TrustedPeripheral::SendPacket(uint8_t protocol, const ComPacket& packet) {
    const auto request = ToBytes(packet);

    try {
        SecuritySend(protocol, GetComId(), request);
        auto packets = FlushResponses(protocol);
        if (packets.empty()) {
            throw ProtocolError("no response to IF-SEND");
        }
        return packets.back();
    }
    catch (std::exception& ex) {
        FlushResponses(protocol);
        throw;
    }
}


std::vector<ComPacket> TrustedPeripheral::FlushResponses(uint8_t protocol) {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    constexpr auto maxSleepTime = 20ms;
    constexpr size_t maxPacketSize = 1048576;

    bool more = true;
    std::vector<ComPacket> packets;
    std::vector<std::byte> bytes(2048, 0_b);
    auto currentSleepTime = 50us;
    do {
        ComPacket packet;
        SecurityReceive(protocol, GetComId(), bytes);
        FromBytes(bytes, packet);
        more = packet.outstandingData != 0;

        if (packet.minTransfer > bytes.size()) {
            size_t newSize = std::max(size_t(packet.minTransfer), size_t(maxPacketSize));
            bytes.resize(packet.minTransfer);
        }
        if (more) {
            Sleep(currentSleepTime);
            if (currentSleepTime * 2 < maxSleepTime) {
                currentSleepTime *= 2;
            }
        }
        packets.push_back(std::move(packet));
    } while (more);

    return packets;
}


void TrustedPeripheral::SecuritySend(uint8_t protocol, uint16_t comId, std::span<const std::byte> payload) {
    const std::array comIdBytes = { std::byte(comId >> 8), std::byte(comId & 0xFF) };
    const std::array comIdBytesFlip = { comIdBytes[1], comIdBytes[0] };
    m_storageDevice->SecuritySend(protocol, comIdBytesFlip, payload);
}


void TrustedPeripheral::SecurityReceive(uint8_t protocol, uint16_t comId, std::span<std::byte> response) {
    const std::array comIdBytes = { std::byte(comId >> 8), std::byte(comId & 0xFF) };
    const std::array comIdBytesFlip = { comIdBytes[1], comIdBytes[0] };
    m_storageDevice->SecurityReceive(protocol, comIdBytesFlip, response);
}


TPerDesc TrustedPeripheral::Discovery() {
    alignas(1024) std::array<std::byte, 2048> response;
    std::ranges::fill(response, 0_b);
    SecurityReceive(0x01, 0x0001, response);
    TPerDesc desc = ParseTPerDesc(response);
    return desc;
}


std::pair<uint16_t, uint16_t> TrustedPeripheral::RequestComId() {
    std::array<std::byte, 4> response;
    std::ranges::fill(response, 0xFF_b);
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
        std::array<std::byte, 20> responseBytes;
        std::ranges::fill(responseBytes, 0_b);
        SecurityReceive(0x02, m_comId, responseBytes);
        FromBytes(responseBytes, response);

        if (response.requestCode == 0) {
            throw NoResponseError("STACK_RESET");
        }
        if (response.availableDataLength == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    } while (response.availableDataLength == 0);

    if (response.success != eStackResetStatus::SUCCESS) {
        throw InvocationError("STACK_RESET", "failed");
    }
}
