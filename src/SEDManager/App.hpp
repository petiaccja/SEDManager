#pragma once

#include <TPerLib/NvmeDevice.hpp>
#include <TPerLib/Session.hpp>
#include <TPerLib/SessionManager.hpp>
#include <TPerLib/TrustedPeripheral.hpp>



struct SecurityProvider {
    Uid uid;
    std::optional<std::string> name;
};


struct Authority {
    Uid uid;
    std::optional<std::string> name;
};


class App {
public:
    App(std::string_view device);

    const TPerDesc& GetCapabilities() const;
    std::unordered_map<std::string, uint32_t> GetProperties();

    std::vector<SecurityProvider> GetSecurityProviders();
    std::vector<Authority> GetAuthorities();

    void Start(Uid securityProvider);
    void Authenticate(Uid authority, std::optional<std::span<const std::byte>> password = {});
    void End();

    void StackReset();
    void Reset();
    void Revert(std::span<const std::byte> psidPassword);

private:
    std::optional<std::string> GetNameFromTable(Uid uid, std::optional<uint32_t> column = {});

private:
    std::shared_ptr<NvmeDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    std::optional<Session> m_session;
    TPerDesc m_capabilities;
};