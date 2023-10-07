#include "SessionManager.hpp"

#include "Serialization/TokenArchive.hpp"
#include "Serialization/TokenDebugArchive.hpp"
#include "Serialization/Utility.hpp"
#include "Communication/Packet.hpp"


template <class Struct, class... Args>
Struct FromTuple(std::tuple<Args...> args) {
    return std::apply([]<class... T>(T&&... args) { return Struct{ std::forward<Args>(args)... }; }, std::move(args));
}


SessionManager::SessionManager(std::shared_ptr<TrustedPeripheral> tper)
    : m_tper(tper){};


auto SessionManager::Properties(const std::optional<PropertyMap>& hostProperties)
    -> PropertiesResult {
    using OutArgs = std::tuple<PropertyMap, std::optional<PropertyMap>>;
    return FromTuple<PropertiesResult>(InvokeMethod<OutArgs>(0xFF01, hostProperties));
}


auto SessionManager::StartSession(
    uint32_t hostSessionID,
    UID spId,
    bool write,
    std::optional<std::span<const std::byte>> hostChallenge,
    std::optional<UID> hostExchangeAuthority,
    std::optional<std::span<const std::byte>> hostExchangeCert,
    std::optional<UID> hostSigningAuthority,
    std::optional<std::span<const std::byte>> hostSigningCert,
    std::optional<uint32_t> sessionTimeout,
    std::optional<uint32_t> transTimeout,
    std::optional<uint32_t> initialCredit,
    std::optional<std::span<const std::byte>> signedHash)
    -> StartSessionResult {
    using OutArgs = std::tuple<
        uint32_t,
        uint32_t,
        std::optional<std::vector<std::byte>>,
        std::optional<std::vector<std::byte>>,
        std::optional<std::vector<std::byte>>,
        std::optional<uint32_t>,
        std::optional<uint32_t>,
        std::optional<std::vector<std::byte>>>;
    auto results = InvokeMethod<OutArgs>(0xFF02,
                                         hostSessionID,
                                         spId,
                                         write,
                                         hostChallenge,
                                         hostExchangeAuthority,
                                         hostExchangeCert,
                                         hostSigningAuthority,
                                         hostSigningCert,
                                         sessionTimeout,
                                         transTimeout,
                                         initialCredit,
                                         signedHash);
    return FromTuple<StartSessionResult>(std::move(results));
}


void SessionManager::EndSession(uint32_t tperSessionNumber, uint32_t hostSessionNumber) {
    auto payload = ToTokens(TokenStream(eCommand::END_OF_SESSION));
    const auto packet = CreatePacket(std::move(payload), tperSessionNumber, hostSessionNumber);
    m_tper->SendPacket(PROTOCOL, packet);
}


ComPacket SessionManager::CreatePacket(std::vector<uint8_t> payload, uint32_t tperSessionNumber, uint32_t hostSessionNumber) {
    SubPacket subPacket;
    subPacket.kind = static_cast<uint16_t>(eSubPacketKind::DATA);
    subPacket.payload = std::move(payload);

    Packet packet;
    packet.tperSessionNumber = tperSessionNumber;
    packet.hostSessionNumber = hostSessionNumber;
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
        const TokenStream requestStream = SerializeMethod(INVOKING_ID, method);
        TokenDebugArchive debugAr(std::cout);

        std::stringstream requestSs;
        TokenOutputArchive requestAr(requestSs);
        save_strip_list(requestAr, requestStream);
        const auto requestTokens = BytesView(requestSs.view());
        const auto requestPacket = CreatePacket({ requestTokens.begin(), requestTokens.end() });
        const auto responsePacket = m_tper->SendPacket(PROTOCOL, requestPacket);
        const auto responseTokens = UnwrapPacket(responsePacket);

        TokenStream responseStream;
        FromTokens(responseTokens, responseStream);

        auto response = ParseMethod(responseStream);
        return response;
    }
    catch (std::exception& ex) {
        throw std::runtime_error(std::format("method invocation failed: {}", ex.what()));
    }
}
