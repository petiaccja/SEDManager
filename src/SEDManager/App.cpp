#include "App.hpp"

#include <TPerLib/Core/Authorities.hpp>
#include <TPerLib/Core/Names.hpp>
#include <TPerLib/Core/SecurityProviders.hpp>
#include <TPerLib/Core/Tables.hpp>
#include <TPerLib/Session.hpp>

#include <stdexcept>


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


std::optional<std::string> App::GetNameFromTable(Uid uid, std::optional<uint32_t> column) {
    const auto name = GetName(uid);
    if (name) {
        return std::string(*name);
    }
    else if (column) {
        try {
            auto name = value_cast<std::string>(m_session->base.Get(uid, *column));
            return { std::move(name) };
        }
        catch (std::exception&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}


std::vector<SecurityProvider> App::GetSecurityProviders() {
    auto get = [this](Session& session) {
        std::vector<SecurityProvider> securityProviders;
        auto uid = session.base.Next(eTable::SP, std::nullopt);
        while (uid) {
            std::optional<std::string> name = GetNameFromTable(*uid, uint32_t(eColumns_SP::Name));
            securityProviders.emplace_back(*uid, std::move(name));
            uid = session.base.Next(eTable::SP, *uid);
        }
        return securityProviders;
    };

    if (m_session) {
        return get(*m_session);
    }
    else {
        Session session(m_sessionManager, opal::eSecurityProvider::Admin);
        return get(session);
    }
}


std::vector<Authority> App::GetAuthorities() {
    if (m_session) {
        std::vector<Authority> authorities;
        auto uid = m_session->base.Next(eTable::Authority, std::nullopt);
        while (uid) {
            std::optional<std::string> name = GetNameFromTable(*uid, uint32_t(eColumns_Authority::Name));
            authorities.emplace_back(*uid, std::move(name));
            uid = m_session->base.Next(eTable::Authority, *uid);
        }
        return authorities;
    }
    throw std::logic_error("start a session to query authorities");
}


void App::Start(Uid securityProvider) {
    m_session = Session(m_sessionManager, securityProvider);
}


void App::Authenticate(Uid authority, std::optional<std::span<const std::byte>> password) {
    if (m_session) {
        return m_session->base.Authenticate(authority, password);
    }
    throw std::logic_error("start a session on an SP to authenticate");
}


void App::End() {
    m_session = {};
}


void App::StackReset() {
    m_tper->StackReset();
    End();
}


void App::Reset() {
    m_tper->Reset();
    End();
}


void App::Revert(std::span<const std::byte> psidPassword) {
    Session session(m_sessionManager, opal::eSecurityProvider::Admin, psidPassword, opal::eAuthority::PSID);
    session.opal.Revert(opal::eSecurityProvider::Admin);
    End();
}