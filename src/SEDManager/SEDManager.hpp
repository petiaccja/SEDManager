#pragma once

#include <async++/stream.hpp>

#include <StorageDevice/NvmeDevice.hpp>
#include <TrustedPeripheral/Session.hpp>
#include <TrustedPeripheral/SessionManager.hpp>
#include <TrustedPeripheral/TrustedPeripheral.hpp>


namespace sedmgr {

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
    const TPerModules& GetModules() const;

    asyncpp::task<void> Start(Uid securityProvider);
    asyncpp::task<void> Authenticate(Uid authority, std::optional<std::span<const std::byte>> password = {});
    asyncpp::task<void> End();

    asyncpp::stream<Uid> GetTableRows(Uid table);
    asyncpp::stream<Value> GetObjectColumns(Uid table, Uid object);
    asyncpp::task<Value> GetObjectColumn(Uid object, uint32_t column);
    asyncpp::task<void> SetObjectColumn(Uid object, uint32_t column, Value value);

    asyncpp::task<void> GenMEK(Uid lockingRange);
    asyncpp::task<void> GenPIN(Uid credentialObject, uint32_t length);
    asyncpp::task<void> Revert(Uid securityProvider);
    asyncpp::task<void> Activate(Uid securityProvider);

    asyncpp::task<void> StackReset();
    asyncpp::task<void> Reset();

private:
    asyncpp::task<std::unordered_map<std::string, uint32_t>> ExchangeProperties();
    asyncpp::task<void> LaunchStack();

private:
    std::shared_ptr<StorageDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    std::shared_ptr<Session> m_session;
};

} // namespace sedmgr