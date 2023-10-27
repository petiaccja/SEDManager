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
    SEDManager(std::shared_ptr<StorageDevice> device);
    SEDManager(const SEDManager&) = delete;
    SEDManager& operator=(const SEDManager&) = delete;
    SEDManager(SEDManager&&) = default;
    SEDManager& operator=(SEDManager&&) = default;

    const TPerDesc& GetDesc() const;
    std::unordered_map<std::string, uint32_t> GetProperties();
    const TPerModules& GetModules() const;

    void Start(Uid securityProvider);
    void Authenticate(Uid authority, std::optional<std::span<const std::byte>> password = {});
    void End();

    Table GetTable(Uid table);
    Object GetObject(Uid table, Uid object);
    void GenMEK(Uid lockingRange);
    void GenPIN(Uid credentialObject, uint32_t length);
    void Revert(Uid securityProvider);
    void Activate(Uid securityProvider);

    void StackReset();
    void Reset();

private:
    std::vector<NamedObject> GetNamedRows(const Table& table);
    void LaunchStack();

private:
    std::shared_ptr<StorageDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    std::shared_ptr<Session> m_session;
};