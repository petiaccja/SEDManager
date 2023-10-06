#include "SessionManager.hpp"

#include "Serialization/RpcArchive.hpp"
#include "Serialization/RpcDebugArchive.hpp"
#include "Serialization/Utility.hpp"
#include "Structures/Packets.hpp"


SessionManager::SessionManager(std::shared_ptr<TrustedPeripheral> tper)
    : m_tper(tper){};


auto SessionManager::Properties(const std::optional<PropertyMap>& hostProperties)
    -> std::tuple<PropertyMap, std::optional<PropertyMap>> {
    using OutArgs = std::tuple<PropertyMap, std::optional<PropertyMap>>;
    return InvokeMethod<OutArgs>(0xFF01, hostProperties);
}


ComPacket SessionManager::CreatePacket(std::vector<uint8_t> payload) {
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


std::span<const uint8_t> SessionManager::UnwrapPacket(const ComPacket& packet) {
    constexpr auto errorNoResponse = "no response to packet";
    const auto& p = !packet.payload.empty() ? packet.payload[0] : throw std::runtime_error(errorNoResponse);
    const auto& s = !p.payload.empty() ? p.payload[0] : throw std::runtime_error(errorNoResponse);
    return s.payload;
}


Method SessionManager::InvokeMethod(const Method& method) {
    try {
        const RpcStream requestStream = SerializeMethod(INVOKING_ID, method);
        RpcDebugArchive debugAr(std::cout);

        std::stringstream requestSs;
        RpcOutputArchive requestAr(requestSs);
        save_strip_list(requestAr, requestStream);
        const auto requestTokens = BytesView(requestSs.view());
        std::cout << std::endl;
        const auto requestPacket = CreatePacket({ requestTokens.begin(), requestTokens.end() });
        const auto responsePacket = m_tper->SendPacket(PROTOCOL, requestPacket);
        const auto responseTokens = UnwrapPacket(responsePacket);

        RpcStream responseStream;
        FromTokens(responseTokens, responseStream);

        auto response = ParseMethod(responseStream);
        return response;
    }
    catch (std::exception& ex) {
        throw std::runtime_error(std::format("method invocation failed: {}", ex.what()));
    }
}
