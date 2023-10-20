#pragma once

#include "Object.hpp"
#include "Table.hpp"

#include <StorageDevice/NvmeDevice.hpp>
#include <TrustedPeripheral/Session.hpp>
#include <TrustedPeripheral/SessionManager.hpp>
#include <TrustedPeripheral/TrustedPeripheral.hpp>


struct NamedObject {
    Uid uid = 0;
    std::optional<std::string> name = std::nullopt;
};


class SEDManager {
public:
    SEDManager(std::string_view device);

    const TPerDesc& GetCapabilities() const;
    std::unordered_map<std::string, uint32_t> GetProperties();

    std::vector<NamedObject> GetSecurityProviders();
    std::vector<NamedObject> GetAuthorities();
    std::vector<NamedObject> GetTables();

    Table GetTable(Uid table);
    Object GetObject(Uid table, Uid object);

    void Start(Uid securityProvider);
    void Authenticate(Uid authority, std::optional<std::span<const std::byte>> password = {});
    void End();

    void StackReset();
    void Reset();

    const TPerModules& GetModules() const;

private:
    std::vector<NamedObject> GetNamedRows(const Table& table);

private:
    std::shared_ptr<NvmeDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    std::shared_ptr<Session> m_session;
    TPerDesc m_capabilities;
};