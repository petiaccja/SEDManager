#include "App.hpp"

#include <Specification/Authorities.hpp>
#include <Specification/Names.hpp>
#include <Specification/SecurityProviders.hpp>
#include <Specification/Tables.hpp>

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


std::vector<NamedObject> App::GetSecurityProviders() {
    auto get = [this](std::shared_ptr<Session> session) {
        std::vector<NamedObject> securityProviders;

        Table sp = Table(eTable::SP, session);
        for (const auto& row : sp) {
            std::optional<std::string> name = GetNameFromTable(row.Id(), uint32_t(eColumns_SP::Name));
            const auto id = row.Id();
            securityProviders.emplace_back(id, std::move(name));
        }

        return securityProviders;
    };

    if (m_session) {
        return get(m_session);
    }
    else {
        const auto session = std::make_shared<Session>(m_sessionManager, opal::eSecurityProvider::Admin);
        return get(session);
    }
}


std::vector<NamedObject> App::GetAuthorities() {
    if (m_session) {
        std::vector<NamedObject> authorities;
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


Table App::GetTable(Uid table) {
    if (m_session) {
        return Table(table, m_session);
    }
    throw std::logic_error("start a session to query tables");
}


void App::Start(Uid securityProvider) {
    m_session = std::make_shared<Session>(m_sessionManager, securityProvider);
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


Table::Table(Uid table, std::shared_ptr<Session> session)
    : m_table(table), m_session(session) {
    Uid descriptor = (1ull << 32) | (uint64_t(table) >> 32);
    m_numColumns = m_session->base.Get(descriptor, 20).Get<size_t>();
}


Uid Table::First() const {
    if (m_session) {
        return m_session->base.Next(m_table, {}).value_or(0);
    }
    return 0;
}