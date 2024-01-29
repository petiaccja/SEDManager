#pragma once

#include "Discovery.hpp"
#include "ModuleCollection.hpp"
#include <async++/mutex.hpp>
#include <async++/task.hpp>

#include <Messaging/SetupPackets.hpp>
#include <StorageDevice/NvmeDevice.hpp>

#include <chrono>
#include <memory>


namespace sedmgr {

struct ComPacket;


class TrustedPeripheral {
public:
    TrustedPeripheral(std::shared_ptr<StorageDevice> storageDevice);
    ~TrustedPeripheral();

    const TPerDesc& GetDesc() const;
    const ModuleCollection& GetModules() const;

    uint16_t GetComId() const;
    uint16_t GetComIdExtension() const;
    asyncpp::task<eComIdState> VerifyComId();
    asyncpp::task<void> StackReset();
    asyncpp::task<void> Reset();

    asyncpp::task<ComPacket> SendPacket(uint8_t protocol, ComPacket packet);

private:
    static TPerDesc Discovery(std::shared_ptr<StorageDevice> storageDevice);
    static std::pair<uint16_t, uint16_t> RequestComId(std::shared_ptr<StorageDevice> storageDevice);

    asyncpp::task<void> Send(uint8_t protocol, uint16_t comId, std::span<const std::byte> payload);
    asyncpp::task<ComPacket> ExchangePacket(uint8_t protocol, ComPacket packet);
    template <class Reply, class Request>
    asyncpp::task<Reply> ExchangeStructure(uint8_t protocol, Request request);

    static std::array<std::byte, 2> SerializeComId(uint16_t comId);
    static void SecuritySend(StorageDevice& storageDevice, uint8_t protocol ,uint16_t comId, std::span<const std::byte> payload);
    static void SecurityReceive(StorageDevice& storageDevice, uint8_t protocol ,uint16_t comId, std::span<std::byte> payload);

private:
    std::shared_ptr<StorageDevice> m_storageDevice;
    TPerDesc m_desc;
    uint16_t m_comId;
    uint16_t m_comIdExtension;
    ModuleCollection m_modules;
    std::unique_ptr<asyncpp::mutex> m_sendRecvMutex;
};


namespace impl {

    struct ExponentialDelay {
        asyncpp::task<void> Delay();

        std::chrono::nanoseconds delay;
        std::chrono::nanoseconds maxDelay{ 0 };
        std::chrono::nanoseconds totalDelay{ 0 };
    };

} // namespace impl


template <class Reply, class Request>
asyncpp::task<Reply> TrustedPeripheral::ExchangeStructure(uint8_t protocol, Request request) {
    using namespace std::chrono_literals;

    const auto sendBuffer = Serialize(request);
    m_storageDevice->SecuritySend(protocol, SerializeComId(m_comId), sendBuffer);

    impl::ExponentialDelay delay{ 1us, 2000ms };
    do {
        std::array<std::byte, 256> responseBytes;
        std::ranges::fill(responseBytes, 0_b);
        m_storageDevice->SecurityReceive(protocol, SerializeComId(m_comId), responseBytes);
        auto reply = DeSerialize(Serialized<Reply>{ responseBytes });

        if (reply.requestCode == 0) {
            throw NoResponseError("no response available");
        }
        if (reply.availableDataLength == 0) {
            co_await delay.Delay();
            continue;
        }
        co_return reply;
    } while (true);
}

} // namespace sedmgr