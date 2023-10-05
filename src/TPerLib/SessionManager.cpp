#include "SessionManager.hpp"

#include "Serialization/Utility.hpp"
#include "Structures/Packets.hpp"


SessionManager::SessionManager(std::shared_ptr<TrustedPeripheral> tper)
    : m_tper(tper){};


std::unordered_map<std::string, uint32_t> SessionManager::Properties(std::span<const std::pair<std::string_view, uint32_t>> hostProperties) {
    constexpr uint64_t METHOD_ID = 0xFF01;

    auto requestTokens = std::vector<uint8_t>{};

    const auto request = Packetize(std::move(requestTokens));
    const auto response = m_tper->SendPacket(PROTOCOL, request);

    constexpr auto errorNoResponse = "no response to packet";
    const auto& packet = !response.payload.empty() ? response.payload[0] : throw std::runtime_error(errorNoResponse);
    const auto& sub = !packet.payload.empty() ? packet.payload[0] : throw std::runtime_error(errorNoResponse);
    const auto& responseTokens = sub.payload;

    std::unordered_map<std::string, uint32_t> properties;
    return properties;
}


ComPacket SessionManager::Packetize(std::vector<uint8_t> payload) {
    SubPacket subPacket;
    subPacket.kind = static_cast<uint16_t>(eSubPacketKind::DATA);
    subPacket.payload = std::move(payload);

    Packet packet;
    packet.tperSessionNumber = 0;
    packet.hostSessionNumber = 0;
    packet.sequenceNumber = 0;
    packet.ackType = 0;
    packet.acknowledgement = 0;
    packet.payload.push_back(std::move(subPacket));

    ComPacket comPacket;
    comPacket.comId = m_tper->GetComId();
    comPacket.comIdExtension = m_tper->GetComIdExtension();
    comPacket.payload.push_back(std::move(packet));

    return comPacket;
}