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


class EncryptedDevice {
public:
    EncryptedDevice(std::shared_ptr<StorageDevice> device);
    EncryptedDevice(const EncryptedDevice&) = delete;
    EncryptedDevice& operator=(const EncryptedDevice&) = delete;
    EncryptedDevice(EncryptedDevice&&) = default;
    EncryptedDevice& operator=(EncryptedDevice&&) = default;
    ~EncryptedDevice();

    static asyncpp::task<EncryptedDevice> Start(std::shared_ptr<StorageDevice> device);
    const TPerDesc& GetDesc() const;
    const ModuleCollection& GetModules() const;

    asyncpp::task<void> Login(Uid securityProvider);
    asyncpp::task<void> Authenticate(Uid authority, std::optional<std::span<const std::byte>> password = {});
    asyncpp::task<void> StackReset();
    asyncpp::task<void> Reset();
    asyncpp::task<void> End();

    asyncpp::stream<Uid> GetTableRows(Uid table);
    asyncpp::stream<Value> GetObjectColumns(Uid table, Uid object);
    asyncpp::task<Value> GetObjectColumn(Uid object, uint32_t column);
    asyncpp::task<void> SetObjectColumn(Uid object, uint32_t column, Value value);

    asyncpp::task<void> GenMEK(Uid lockingRange);
    asyncpp::task<void> GenPIN(Uid credentialObject, uint32_t length);
    asyncpp::task<void> Revert(Uid securityProvider);
    asyncpp::task<void> Activate(Uid securityProvider);

private:
    EncryptedDevice(std::shared_ptr<StorageDevice> device,
                    std::shared_ptr<TrustedPeripheral> tper,
                    std::shared_ptr<SessionManager> sessionManager);

private:
    std::shared_ptr<StorageDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    std::shared_ptr<Session> m_session;
};

} // namespace sedmgr