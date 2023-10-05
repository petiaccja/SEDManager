#include "SessionManager.hpp"

#include "Serialization/RpcArchive.hpp"
#include "Serialization/RpcDebugArchive.hpp"
#include "Serialization/Utility.hpp"
#include "Structures/Packets.hpp"


SessionManager::SessionManager(std::shared_ptr<TrustedPeripheral> tper)
    : m_tper(tper){};


std::unordered_map<std::string, uint32_t> SessionManager::Properties(const std::optional<std::unordered_map<std::string, uint32_t>>& hostProperties) {
    constexpr uint64_t METHOD_ID = 0xFF01;

    const auto toNamed = [](const auto& prop) { return Named{ { RpcStream::bytes, prop.first }, prop.second }; };
    std::vector<RpcStream> args;
    if (hostProperties) {
        args = { Named{ 0, *hostProperties | std::views::transform(toNamed) } };
    }

    const Method result = InvokeMethod(Method{ .methodId = METHOD_ID, .args = std::move(args) });
    constexpr std::string_view baseError = "method call \"properties\" failed: {}";
    if (result.status != eMethodStatus::SUCCESS) {
        throw std::runtime_error(std::format(baseError, MethodStatusText(result.status)));
    }

    std::unordered_map<std::string, uint32_t> properties;
    if (result.args.empty()) {
        throw std::runtime_error(std::format(baseError, "expected at least one result"));
    }
    if (!result.args[0].IsList()) {
        throw std::runtime_error(std::format(baseError, "expected a list as first result"));
    }
    for (const auto& prop : result.args[0].AsList()) {
        if (!prop.IsNamed()) {
            throw std::runtime_error(std::format(baseError, "expected named value as property"));
        }
        const auto& named = prop.AsNamed();
        if (!named.value.IsInteger()) {
            throw std::runtime_error(std::format(baseError, "expected integer value as property"));
        }
        auto name = std::string(StringView(named.name.Get<std::span<const uint8_t>>()));
        const auto value = named.value.Get<uint32_t>();
        properties.insert_or_assign(std::move(name), value);
    }

    return properties;
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
