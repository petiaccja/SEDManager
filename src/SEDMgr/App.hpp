#pragma once

#include <TPerLib/NvmeDevice.hpp>
#include <TPerLib/SessionManager.hpp>
#include <TPerLib/TrustedPeripheral.hpp>


class App {
public:
    App(std::string_view device);

    const TPerDesc& GetCapabilities() const;
    std::unordered_map<std::string, uint32_t> GetProperties();

    std::vector<std::byte> GetMSIDPassword();
    void SetSIDPassword(std::span<const std::byte> sidPassword, std::span<const std::byte> newPassword);
    bool ActivateLocking(std::span<const std::byte> sidPassword);
    void ConfigureLockingRange(std::span<const std::byte> admin1Password,
                               Uid range,
                               bool readLockEnabled,
                               bool writeLockEnabled,
                               std::optional<uint64_t> startLba = std::nullopt,
                               std::optional<uint64_t> lengthLba = std::nullopt);
    void SetLockingRange(std::span<const std::byte> admin1Password,
                         Uid range,
                         bool readLocked,
                         bool writeLocked);
    void Revert(std::span<const std::byte> psidPassword);

private:
    std::shared_ptr<NvmeDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    TPerDesc m_capabilities;
};