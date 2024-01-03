#pragma once

#include "Method.hpp"
#include "TrustedPeripheral.hpp"
#include <async++/task.hpp>

#include <Error/Exception.hpp>

#include <vector>


namespace sedmgr {

class SessionManager {
public:
    using PropertyMap = std::unordered_map<std::string, uint32_t>;

    struct PropertiesResult {
        PropertyMap tperProperties;
        std::optional<PropertyMap> hostProperties;
    };

    struct StartSessionResult {
        uint32_t hostSessionId;
        uint32_t spSessionId;
        std::optional<std::vector<std::byte>> spChallenge;
        std::optional<std::vector<std::byte>> spExchangeCert;
        std::optional<std::vector<std::byte>> spSigningCert;
        std::optional<uint32_t> transTimeout;
        std::optional<uint32_t> initialCredit;
        std::optional<std::vector<std::byte>> signedHash;
    };

public:
    SessionManager(std::shared_ptr<TrustedPeripheral> tper);
    SessionManager(const SessionManager&) = delete;
    SessionManager(SessionManager&&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;
    SessionManager& operator=(SessionManager&&) = delete;

    asyncpp::task<PropertiesResult> Properties(const std::optional<PropertyMap>& hostProperties = {});

    asyncpp::task<StartSessionResult> StartSession(
        uint32_t hostSessionID,
        Uid spId,
        bool write,
        std::optional<std::span<const std::byte>> hostChallenge = {},
        std::optional<Uid> hostExchangeAuthority = {},
        std::optional<std::span<const std::byte>> hostExchangeCert = {},
        std::optional<Uid> hostSigningAuthority = {},
        std::optional<std::span<const std::byte>> hostSigningCert = {},
        std::optional<uint32_t> sessionTimeout = {},
        std::optional<uint32_t> transTimeout = {},
        std::optional<uint32_t> initialCredit = {},
        std::optional<std::span<const std::byte>> signedHash = {});

    asyncpp::task<void> EndSession(uint32_t tperSessionNumber, uint32_t hostSessionNumber);

    std::shared_ptr<TrustedPeripheral> GetTrustedPeripheral();
    std::shared_ptr<const TrustedPeripheral> GetTrustedPeripheral() const;

    ComPacket CreatePacket(std::vector<std::byte> payload, uint32_t tperSessionNumber = 0, uint32_t hostSessionNumber = 0);
    static std::span<const std::byte> UnwrapPacket(const ComPacket& packet);

private:
    asyncpp::task<Method> InvokeMethod(const Method& method);

    template <class OutArgs, class... InArgs>
    asyncpp::task<OutArgs> InvokeMethod(Uid methodId, const InArgs&... inArgs);

    const ModuleCollection& GetModules() const;

private:
    static constexpr Uid INVOKING_ID = 0xFF;
    static constexpr uint8_t PROTOCOL = 0x01;
    std::shared_ptr<TrustedPeripheral> m_tper;
};


template <class OutArgs, class... InArgs>
asyncpp::task<OutArgs> SessionManager::InvokeMethod(Uid methodId, const InArgs&... inArgs) {
    std::vector<Value> args = ArgsToValues(inArgs...);
    const Method result = co_await InvokeMethod(Method{ .methodId = methodId, .args = std::move(args) });

    OutArgs outArgs;
    try {
        std::apply([&result](auto&... outArgs) { ArgsFromValues(result.args, outArgs...); }, outArgs);
    }
    catch (std::exception& ex) {
        throw InvalidResponseError(GetModules().FindName(methodId).value_or(to_string(methodId)), ex.what());
    }
    co_return outArgs;
}

} // namespace sedmgr