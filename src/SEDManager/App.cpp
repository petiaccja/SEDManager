#include "App.hpp"

#include <TPerLib/Core/Authorities.hpp>
#include <TPerLib/Core/SecurityProviders.hpp>
#include <TPerLib/Core/Tables.hpp>
#include <TPerLib/Session.hpp>


App::App(std::string_view device) {
    m_device = std::make_shared<NvmeDevice>(device);
    m_tper = std::make_shared<TrustedPeripheral>(m_device);
    m_capabilities = m_tper->GetDesc();
    const auto comIdState = m_tper->VerifyComId();
    if (comIdState != eComIdState::ISSUED && comIdState != eComIdState::ASSOCIATED) {
        throw std::runtime_error("failed to acquire valid ComID");
    }
    m_sessionManager = std::make_shared<SessionManager>(m_tper);
}


const TPerDesc& App::GetCapabilities() const {
    return m_tper->GetDesc();
}


std::unordered_map<std::string, uint32_t> App::GetProperties() {
    const std::unordered_map<std::string, uint32_t> hostProperties = {
        { "MaxPackets", 1 },
        { "MaxSubpackets", 1 },
        { "MaxMethods", 1 },
        { "MaxComPacketSize", 65536 },
        { "MaxIndTokenSize", 65536 },
        { "MaxAggTokenSize", 65536 },
        { "ContinuedTokens", 0 },
        { "SequenceNumbers", 0 },
        { "AckNAK", 0 },
        { "Asynchronous", 0 },
    };

    auto [tperProps, hostProps] = m_sessionManager->Properties(hostProperties);
    return tperProps;
}


std::vector<std::byte> App::GetMSIDPassword() {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin);
    return session.base.Get<std::vector<std::byte>>(opal::eRows_C_PIN::C_PIN_MSID, 3);
}


void App::SetSIDPassword(std::span<const std::byte> sidPassword, std::span<const std::byte> newPassword) {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin, sidPassword, eAuthority::SID);
    session.base.Set(opal::eRows_C_PIN::C_PIN_SID, 3, newPassword);
}


bool App::ActivateLocking(std::span<const std::byte> sidPassword) {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin, sidPassword, eAuthority::SID);
    const auto lifeCycle = session.base.Get<int>(opal::eSecurityProvider::Locking, 6);
    if (lifeCycle == int(opal::eSecurityProviderLifecycle::MANUFACTURED_INACTIVE)) {
        session.opal.Activate(opal::eSecurityProvider::Locking);
        return true;
    }
    return false;
}


void App::ConfigureLockingRange(std::span<const std::byte> admin1Password,
                                Uid range,
                                bool readLockEnabled,
                                bool writeLockEnabled,
                                std::optional<uint64_t> startLba,
                                std::optional<uint64_t> lengthLba) {
    Session session(m_sessionManager, opal::eSecurityProvider::Locking, admin1Password, opal::eAuthority::Admin1);
    const auto activeKey = session.base.Get<Uid>(range, uint32_t(eColumns_Locking::ActiveKey));
    if (startLba && lengthLba) {
        session.base.Set(range, uint32_t(eColumns_Locking::RangeStart), startLba.value());
        session.base.Set(range, uint32_t(eColumns_Locking::RangeLength), lengthLba.value());
    }
    session.base.Set(range, uint32_t(eColumns_Locking::ReadLockEnabled), readLockEnabled);
    session.base.Set(range, uint32_t(eColumns_Locking::WriteLockEnabled), writeLockEnabled);
    session.base.Set(range, uint32_t(eColumns_Locking::ReadLocked), readLockEnabled);
    session.base.Set(range, uint32_t(eColumns_Locking::WriteLocked), writeLockEnabled);
    session.base.GenKey(activeKey);
}


void App::SetLockingRange(std::span<const std::byte> admin1Password,
                          Uid range,
                          bool readLocked,
                          bool writeLocked) {
    Session session(m_sessionManager, opal::eSecurityProvider::Locking, admin1Password, opal::eAuthority::Admin1);
    session.base.Set(range, uint32_t(eColumns_Locking::ReadLocked), readLocked);
    session.base.Set(range, uint32_t(eColumns_Locking::WriteLocked), writeLocked);
}


void App::Revert(std::span<const std::byte> psidPassword) {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin, psidPassword, opal::eAuthority::PSID);
    session.opal.Revert(opal::eSecurityProvider::Admin);
}