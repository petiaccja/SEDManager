#pragma once

#include <async++/stream.hpp>

#include <StorageDevice/NvmeDevice.hpp>
#include <TrustedPeripheral/Session.hpp>
#include <TrustedPeripheral/SessionManager.hpp>
#include <TrustedPeripheral/TrustedPeripheral.hpp>


namespace sedmgr {

class SimpleSession {
public:
    SimpleSession(std::shared_ptr<TrustedPeripheral> tper,
                  std::shared_ptr<Session> session,
                  UID securityProvider);

    SimpleSession(const SimpleSession&) = delete;
    SimpleSession& operator=(const SimpleSession&) = delete;
    SimpleSession(SimpleSession&&) = default;
    SimpleSession& operator=(SimpleSession&&) = default;
    ~SimpleSession();

    const ModuleCollection& GetModules() const;
    UID GetSecurityProvider() const;
    asyncpp::task<void> Authenticate(UID authority, std::optional<std::vector<std::byte>> password = {});
    asyncpp::task<void> End();

    asyncpp::stream<UID> GetTableRows(UID table);
    asyncpp::stream<Value> GetObjectColumns(UID object);
    asyncpp::task<Value> GetValue(UID object, uint32_t column);
    asyncpp::task<void> SetValue(UID object, uint32_t column, Value value);

    asyncpp::task<void> GenMEK(UID lockingRange);
    asyncpp::task<void> GenPIN(UID credentialObject, uint32_t length);
    asyncpp::task<void> Revert(UID securityProvider);
    asyncpp::task<void> Activate(UID securityProvider);

private:
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<Session> m_session;
    UID m_securityProvider;
};


class EncryptedDevice {
public:
    EncryptedDevice(std::shared_ptr<StorageDevice> device);
    EncryptedDevice(const EncryptedDevice&) = delete;
    EncryptedDevice& operator=(const EncryptedDevice&) = delete;
    EncryptedDevice(EncryptedDevice&&) = default;
    EncryptedDevice& operator=(EncryptedDevice&&) = default;

    static asyncpp::task<EncryptedDevice> Start(std::shared_ptr<StorageDevice> device);
    const TPerDesc& GetDesc() const;
    const ModuleCollection& GetModules() const;

    asyncpp::task<SimpleSession> Login(UID securityProvider);
    asyncpp::task<void> StackReset();
    asyncpp::task<void> Reset();

private:
    EncryptedDevice(std::shared_ptr<StorageDevice> device,
                    std::shared_ptr<TrustedPeripheral> tper,
                    std::shared_ptr<SessionManager> sessionManager);

private:
    std::shared_ptr<StorageDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
};

} // namespace sedmgr