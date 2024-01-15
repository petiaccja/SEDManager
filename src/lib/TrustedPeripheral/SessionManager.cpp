#include "SessionManager.hpp"

#include "Logging.hpp"


namespace sedmgr {

SessionManager::SessionManager(std::shared_ptr<TrustedPeripheral> tper)
    : m_tper(tper) {}


auto SessionManager::Properties(std::optional<PropertyMap> hostProperties)
    -> asyncpp::task<PropertiesResult> {
    const auto result = co_await propertiesMethod(GetCallContext(), ConvertArg(hostProperties));
    co_return ResultAs<PropertiesResult>(result);
}


auto SessionManager::StartSession(
    uint32_t hostSessionID,
    UID spId,
    bool write,
    std::optional<std::vector<std::byte>> hostChallenge,
    std::optional<UID> hostExchangeAuthority,
    std::optional<std::vector<std::byte>> hostExchangeCert,
    std::optional<UID> hostSigningAuthority,
    std::optional<std::vector<std::byte>> hostSigningCert,
    std::optional<uint32_t> sessionTimeout,
    std::optional<uint32_t> transTimeout,
    std::optional<uint32_t> initialCredit,
    std::optional<std::vector<std::byte>> signedHash)
    -> asyncpp::task<StartSessionResult> {
    const auto result = co_await startSessionMethod(GetCallContext(),
                                                    ConvertArg(hostSessionID),
                                                    ConvertArg(spId),
                                                    ConvertArg(write),
                                                    ConvertArg(hostChallenge),
                                                    ConvertArg(hostExchangeAuthority),
                                                    ConvertArg(hostExchangeCert),
                                                    ConvertArg(hostSigningAuthority),
                                                    ConvertArg(hostSigningCert),
                                                    ConvertArg(sessionTimeout),
                                                    ConvertArg(transTimeout),
                                                    ConvertArg(initialCredit),
                                                    ConvertArg(signedHash));
    co_return ResultAs<StartSessionResult>(result);
}


asyncpp::task<void> SessionManager::EndSession(uint32_t tperSessionNumber, uint32_t hostSessionNumber) {
    Value request = eCommand::END_OF_SESSION;
    Log("End session: request", request);
    const Value response = co_await SendPacketizedValue(m_tper, PROTOCOL, tperSessionNumber, hostSessionNumber, request, false);
    Log("End session: response", response);
}


std::shared_ptr<TrustedPeripheral> SessionManager::GetTrustedPeripheral() {
    return m_tper;
}


std::shared_ptr<const TrustedPeripheral> SessionManager::GetTrustedPeripheral() const {
    return m_tper;
}


const ModuleCollection& SessionManager::GetModules() const {
    return m_tper->GetModules();
}


CallContext SessionManager::GetCallContext() const {
    auto callRemoteMethod = [tper = m_tper](MethodCall call) -> asyncpp::task<MethodResult> {
        co_return co_await CallRemoteSessionMethod(tper, PROTOCOL, 0, 0, std::move(call));
    };
    auto getMethodName = [tper = m_tper](UID methodId) {
        const auto maybeMethodName = tper->GetModules().FindName(methodId);
        return maybeMethodName.value_or(methodId.ToString());
    };
    return CallContext{ .invokingId = INVOKING_ID, .callRemoteMethod = callRemoteMethod, .getMethodName = getMethodName };
}

} // namespace sedmgr