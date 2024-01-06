#pragma once

#include "MethodUtils.hpp"
#include "TrustedPeripheral.hpp"
#include <async++/task.hpp>

#include <Specification/Core/Defs/UIDs.hpp>

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

private:
    const ModuleCollection& GetModules() const;
    CallContext GetCallContext() const;

private:
    static constexpr Uid INVOKING_ID = 0xFF;
    static constexpr uint8_t PROTOCOL = 0x01;
    static constexpr auto propertiesMethod = Method<Uid(core::eMethod::Properties), 0, 1, 1, 1>{};
    static constexpr auto startSessionMethod = Method<Uid(core::eMethod::StartSession), 3, 9, 2, 6>{};

    std::shared_ptr<TrustedPeripheral> m_tper;
};


} // namespace sedmgr