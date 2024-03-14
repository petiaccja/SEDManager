#include "TrustedPeripheral.hpp"

#include "Logging.hpp"
#include "MethodUtils.hpp"

#include <Archive/Serialization.hpp>
#include <Error/Exception.hpp>
#include <Messaging/ComPacket.hpp>
#include <Messaging/SetupPackets.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>

#include <asyncpp/join.hpp>
#include <asyncpp/sleep.hpp>

#include <array>
#include <stdexcept>


namespace sedmgr {

void LoadModules(const TPerDesc& desc, ModuleCollection& modules) {
    if (desc.tperDesc && desc.lockingDesc) {
        const auto coreModule = CoreModule::Get();
        modules.Load(coreModule);
    }
    for (const auto& sscDesc : desc.sscDescs) {
        const auto featureCode = std::visit([](auto& desc) { return desc.featureCode; }, sscDesc);
        switch (featureCode) {
            case Opal1FeatureDesc::featureCode: modules.Load(Opal1Module::Get()); break;
            case Opal2FeatureDesc::featureCode: modules.Load(Opal2Module::Get()); break;
        }
    }
}


TrustedPeripheral::TrustedPeripheral(std::shared_ptr<StorageDevice> storageDevice)
    : m_storageDevice(storageDevice), m_sendRecvMutex(std::make_unique<asyncpp::mutex>()) {
    m_desc = Discovery(storageDevice);

    if (m_desc.tperDesc) {
        if (m_desc.tperDesc->comIdMgmtSupported) {
            try {
                std::tie(m_comId, m_comIdExtension) = RequestComId(storageDevice);
            }
            catch (std::exception& ex) {
                throw std::runtime_error(std::format("dynamically allocating ComID failed: {}", ex.what()));
            }
        }
        else {
            if (!m_desc.sscDescs.empty()) {
                std::tie(m_comId, m_comIdExtension) = std::visit(
                    [](const auto& desc) { return std::pair{ desc.baseComId, uint16_t(0) }; },
                    m_desc.sscDescs[0]);
            }
            else {
                throw std::runtime_error("no statically allocated ComIDs available");
            }
        }
    }
    else {
        throw std::runtime_error("level 0 discovery did not return a TPer description -- not a TCG-compliant device?");
    }
    LoadModules(m_desc, m_modules);
}


TrustedPeripheral::~TrustedPeripheral() {
    try {
        join(StackReset());
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


const ModuleCollection& TrustedPeripheral::GetModules() const {
    return m_modules;
}


asyncpp::task<eComIdState> TrustedPeripheral::VerifyComId() {
    const VerifyComIdValidRequest request{
        .comId = m_comId,
        .comIdExtension = m_comIdExtension
    };

    const auto reply = co_await ExchangeStructure<VerifyComIdValidResponse>(0x02, request);

    co_return reply.comIdState;
}


asyncpp::task<void> TrustedPeripheral::StackReset() {
    const StackResetRequest request{
        .comId = m_comId,
        .comIdExtension = m_comIdExtension
    };

    const auto reply = co_await ExchangeStructure<StackResetResponse>(0x02, request);

    if (reply.success != eStackResetStatus::SUCCESS) {
        throw InvocationError("STACK_RESET", "failed");
    }
}


asyncpp::task<void> TrustedPeripheral::Reset() {
    std::array payload{ std::byte(0) };
    co_await Send(0x02, 0x0004, payload);
}


asyncpp::task<ComPacket> TrustedPeripheral::SendPacket(uint8_t protocol, ComPacket packet) {
    co_return co_await ExchangePacket(protocol, std::move(packet));
}


TPerDesc TrustedPeripheral::Discovery(std::shared_ptr<StorageDevice> storageDevice) {
    std::array<std::byte, 4096> response;
    std::ranges::fill(response, 0_b);
    SecurityReceive(*storageDevice, 0x01, 0x0001, response);
    TPerDesc desc = ParseTPerDesc(response);
    return desc;
}


std::pair<uint16_t, uint16_t> TrustedPeripheral::RequestComId(std::shared_ptr<StorageDevice> storageDevice) {
    std::array<std::byte, 4> response;
    std::ranges::fill(response, 0xFF_b);
    SecurityReceive(*storageDevice, 0x02, 0x0000, response);
    const auto comId = DeSerialize(Serialized<uint16_t>{ response });
    const auto comIdExtension = DeSerialize(Serialized<uint16_t>{ std::span{ response }.subspan(2) });
    return { comId, comIdExtension };
}


asyncpp::task<void> TrustedPeripheral::Send(uint8_t protocol, uint16_t comId, std::span<const std::byte> payload) {
    asyncpp::unique_lock lk = co_await *m_sendRecvMutex;
    SecuritySend(*m_storageDevice, protocol, comId, payload);
}


asyncpp::task<void> impl::ExponentialDelay::Delay() {
    using namespace std::chrono_literals;

    if (maxDelay != 0ns && totalDelay > maxDelay) {
        throw NoResponseError("timed out");
    }
    co_await asyncpp::sleep_for(delay);
    totalDelay += delay;
    delay *= 2;
}


asyncpp::task<ComPacket> TrustedPeripheral::ExchangePacket(uint8_t protocol, ComPacket packet) {
    using namespace std::chrono_literals;

    const asyncpp::unique_lock lk = co_await *m_sendRecvMutex;

    const auto sendBuffer = Serialize(packet);
    SecuritySend(*m_storageDevice, protocol, packet.comId, sendBuffer);

    std::vector<ComPacket> receivedPackets;
    std::vector<std::byte> receiveBuffer(2048);
    impl::ExponentialDelay delay{ 1us, 2000ms };
    do {
        SecurityReceive(*m_storageDevice, protocol, packet.comId, receiveBuffer);
        auto receivedPacket = DeSerialize(Serialized<ComPacket>(receiveBuffer));

        // Device wants to send data larger than the receive buffer.
        if (receivedPacket.minTransfer > receiveBuffer.size()) {
            if (receivedPacket.minTransfer < 1048576) {
                receiveBuffer.resize(receivedPacket.minTransfer);
                continue;
            }
            throw ProtocolError("response too large");
        }

        const bool receivedData = !receivedPacket.payload.empty();
        const auto outstandingData = receivedPacket.outstandingData;

        // Current packet contains useful data.
        if (receivedData) {
            receivedPackets.push_back(std::move(receivedPacket));
        }

        // If device does not intend to send more data, exit loop.
        if (outstandingData == 0) {
            break;
        }
        // If device intends to send more data, but it's not ready yet, wait a bit.
        if (outstandingData == 1) {
            co_await delay.Delay();
        }
    } while (true);

    if (receivedPackets.empty()) {
        throw NoResponseError("empty packet received");
    }
    if (receivedPackets.size() > 1) {
        throw ProtocolError("multiple packets sent by the device, expected only one");
    }
    co_return std::move(receivedPackets.back());
}


std::array<std::byte, 2> TrustedPeripheral::SerializeComId(uint16_t comId) {
    return { std::byte(comId & 0xFF), std::byte(comId >> 8) };
}


void TrustedPeripheral::SecuritySend(StorageDevice& storageDevice, uint8_t protocol, uint16_t comId, std::span<const std::byte> payload) {
    try {
        storageDevice.SecuritySend(protocol, SerializeComId(comId), payload);
        Log(std::format("IF-SEND: Protocol={}, ComID={}, payload: {} bytes", protocol, comId, payload.size()));
    }
    catch (std::exception& ex) {
        Log(std::format("IF-SEND FAILED: Protocol={}, ComID={}, payload: {} bytes --- {}", protocol, comId, payload.size(), ex.what()));
        throw;
    }
}


void TrustedPeripheral::SecurityReceive(StorageDevice& storageDevice, uint8_t protocol, uint16_t comId, std::span<std::byte> payload) {
    try {
        storageDevice.SecurityReceive(protocol, SerializeComId(comId), payload);
        Log(std::format("IF-RECV: Protocol={}, ComID={}, payload: {} bytes", protocol, comId, payload.size()));
    }
    catch (std::exception& ex) {
        Log(std::format("IF-RECV FAILED: Protocol={}, ComID={}, payload: {} bytes --- {}", protocol, comId, payload.size(), ex.what()));
        throw;
    }
}

} // namespace sedmgr