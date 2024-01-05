#include "SessionManager.hpp"

#include "Logging.hpp"

#include <Archive/Conversion.hpp>
#include <Data/ComPacket.hpp>
#include <Data/TokenStream.hpp>
#include <Error/Exception.hpp>
#include <Specification/Core/CoreModule.hpp>


namespace sedmgr {

template <class Struct, class... Args>
Struct FromTuple(std::tuple<Args...> args) {
    return std::apply([]<class... T>(T&&... args) { return Struct{ std::forward<Args>(args)... }; }, std::move(args));
}


SessionManager::SessionManager(std::shared_ptr<TrustedPeripheral> tper)
    : m_tper(tper) {}


auto SessionManager::Properties(const std::optional<PropertyMap>& hostProperties)
    -> asyncpp::task<PropertiesResult> {
    using OutArgs = std::tuple<PropertyMap, std::optional<PropertyMap>>;
    co_return FromTuple<PropertiesResult>(co_await InvokeMethod<OutArgs>(core::eMethod::Properties, hostProperties));
}


auto SessionManager::StartSession(
    uint32_t hostSessionID,
    Uid spId,
    bool write,
    std::optional<std::span<const std::byte>> hostChallenge,
    std::optional<Uid> hostExchangeAuthority,
    std::optional<std::span<const std::byte>> hostExchangeCert,
    std::optional<Uid> hostSigningAuthority,
    std::optional<std::span<const std::byte>> hostSigningCert,
    std::optional<uint32_t> sessionTimeout,
    std::optional<uint32_t> transTimeout,
    std::optional<uint32_t> initialCredit,
    std::optional<std::span<const std::byte>> signedHash)
    -> asyncpp::task<StartSessionResult> {
    using OutArgs = std::tuple<
        uint32_t,
        uint32_t,
        std::optional<std::vector<std::byte>>,
        std::optional<std::vector<std::byte>>,
        std::optional<std::vector<std::byte>>,
        std::optional<uint32_t>,
        std::optional<uint32_t>,
        std::optional<std::vector<std::byte>>>;
    auto results = co_await InvokeMethod<OutArgs>(core::eMethod::StartSession,
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
    co_return FromTuple<StartSessionResult>(std::move(results));
}


asyncpp::task<void> SessionManager::EndSession(uint32_t tperSessionNumber, uint32_t hostSessionNumber) {
    Value request = eCommand::END_OF_SESSION;
    Log("Close session", request);
    const auto requestStream = TokenStream{ Tokenize(request) };
    const auto payload = Serialize(requestStream);
    const auto packet = CreatePacket(std::move(payload), tperSessionNumber, hostSessionNumber);
    const auto result = co_await m_tper->SendPacket(PROTOCOL, packet);
    try {
        const auto resultBytes = UnwrapPacket(result);
        Value resultValue;
        const auto responseStream = SurroundWithList(DeSerialize(Serialized<TokenStream>{ resultBytes }));
        const Value response = DeTokenize(Tokenized<Value>{ responseStream.stream }).first;
        if (resultValue.HasValue()) {
            Log("Close session: response", resultValue);
        }
    }
    catch (...) {
    }
}


std::shared_ptr<TrustedPeripheral> SessionManager::GetTrustedPeripheral() {
    return m_tper;
}


std::shared_ptr<const TrustedPeripheral> SessionManager::GetTrustedPeripheral() const {
    return m_tper;
}


ComPacket SessionManager::CreatePacket(std::vector<std::byte> payload, uint32_t tperSessionNumber, uint32_t hostSessionNumber) {
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


std::span<const std::byte> SessionManager::UnwrapPacket(const ComPacket& packet) {
    constexpr auto errorNoResponse = "received no response to packet";
    const auto& p = !packet.payload.empty() ? packet.payload[0] : throw NoResponseError();
    const auto& s = !p.payload.empty() ? p.payload[0] : throw NoResponseError();
    return s.payload;
}


asyncpp::task<Method> SessionManager::InvokeMethod(const Method& method) {
    const std::string methodIdStr = GetModules().FindName(method.methodId).value_or(to_string(method.methodId));
    try {
        const Value request = MethodToValue(INVOKING_ID, method);
        Log(std::format("Call '{}' [SessionManager]", methodIdStr), request);
        const auto requestStream = UnSurroundWithList(TokenStream{ Tokenize(request) });
        const auto requestBytes = Serialize(requestStream);
        const auto requestPacket = CreatePacket(requestBytes);
        const auto responsePacket = co_await m_tper->SendPacket(PROTOCOL, requestPacket);
        const auto responseBytes = UnwrapPacket(responsePacket);
        const auto responseStream = SurroundWithList(DeSerialize(Serialized<TokenStream>{ responseBytes }));
        const Value response = DeTokenize(Tokenized<Value>{ responseStream.stream }).first;
        Log(std::format("Result '{}' [SessionManager]", methodIdStr), response);

        auto responseMethod = MethodFromValue(response);
        MethodStatusToException(methodIdStr, responseMethod.status);
        co_return responseMethod;
    }
    catch (InvocationError&) {
        throw;
    }
    catch (std::exception& ex) {
        throw InvocationError(methodIdStr, ex.what());
    }
}


const ModuleCollection& SessionManager::GetModules() const {
    return m_tper->GetModules();
}

} // namespace sedmgr