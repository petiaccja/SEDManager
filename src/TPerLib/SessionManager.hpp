#pragma once

#include "Method.hpp"
#include "TrustedPeripheral.hpp"

#include <vector>


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

    PropertiesResult Properties(const std::optional<PropertyMap>& hostProperties = {});

    StartSessionResult StartSession(
        uint32_t hostSessionID,
        UID spId,
        bool write,
        std::optional<std::span<const std::byte>> hostChallenge = {},
        std::optional<UID> hostExchangeAuthority = {},
        std::optional<std::span<const std::byte>> hostExchangeCert = {},
        std::optional<UID> hostSigningAuthority = {},
        std::optional<std::span<const std::byte>> hostSigningCert = {},
        std::optional<uint32_t> sessionTimeout = {},
        std::optional<uint32_t> transTimeout = {},
        std::optional<uint32_t> initialCredit = {},
        std::optional<std::span<const std::byte>> signedHash = {});

    void EndSession(uint32_t tperSessionNumber, uint32_t hostSessionNumber);

private:
    ComPacket CreatePacket(std::vector<uint8_t> payload, uint32_t tperSessionNumber = 0, uint32_t hostSessionNumber = 0);

    std::span<const uint8_t> UnwrapPacket(const ComPacket& packet);

    Method InvokeMethod(const Method& method);

    template <class OutArgs, class... InArgs>
    OutArgs InvokeMethod(uint64_t methodId, const InArgs&... inArgs);

private:
    static constexpr uint64_t INVOKING_ID = 0xFF;
    static constexpr uint8_t PROTOCOL = 0x01;
    std::shared_ptr<TrustedPeripheral> m_tper;
};


template <class OutArgs, class... InArgs>
OutArgs SessionManager::InvokeMethod(uint64_t methodId, const InArgs&... inArgs) {
    constexpr uint64_t METHOD_ID = 0xFF01;

    std::vector<RpcStream> args = SerializeArgs(inArgs...);
    const Method result = InvokeMethod(Method{ .methodId = methodId, .args = std::move(args) });
    if (result.status != eMethodStatus::SUCCESS) {
        throw std::runtime_error(std::format("call to method (id={:#010x}) failed: {}", methodId, MethodStatusText(result.status)));
    }

    OutArgs outArgs;
    try {
        std::apply([&result](auto&... outArgs) { ParseArgs(result.args, outArgs...); }, outArgs);
    }
    catch (std::exception& ex) {
        throw std::runtime_error(std::format("call to method (id={:#010x}) returned unexpected values: {}", methodId, ex.what()));
    }
    return outArgs;
}