#include "SEDManager.hpp"

#include <Specification/Core/CoreModule.hpp>

#include <stdexcept>


SEDManager::SEDManager(std::string_view device) {
    m_device = std::make_shared<NvmeDevice>(device);
    m_tper = std::make_shared<TrustedPeripheral>(m_device);
    m_capabilities = m_tper->GetDesc();
    const auto comIdState = m_tper->VerifyComId();
    if (comIdState != eComIdState::ISSUED && comIdState != eComIdState::ASSOCIATED) {
        throw std::runtime_error("failed to acquire valid ComID");
    }
    m_sessionManager = std::make_shared<SessionManager>(m_tper);
}


const TPerDesc& SEDManager::GetCapabilities() const {
    return m_tper->GetDesc();
}


std::unordered_map<std::string, uint32_t> SEDManager::GetProperties() {
    const std::unordered_map<std::string, uint32_t> hostProperties = {
        {"MaxPackets",        1    },
        { "MaxSubpackets",    1    },
        { "MaxMethods",       1    },
        { "MaxComPacketSize", 65536},
        { "MaxIndTokenSize",  65536},
        { "MaxAggTokenSize",  65536},
        { "ContinuedTokens",  0    },
        { "SequenceNumbers",  0    },
        { "AckNAK",           0    },
        { "Asynchronous",     0    },
    };

    auto [tperProps, hostProps] = m_sessionManager->Properties(hostProperties);
    return tperProps;
}


std::vector<NamedObject> SEDManager::GetNamedRows(const Table& table) {
    std::vector<NamedObject> namedRows;
    for (const auto& row : table) {
        namedRows.emplace_back(row.Id(), GetModules().FindName(row.Id()).value_or(to_string(row.Id())));
    }
    return namedRows;
}


const TPerModules& SEDManager::GetModules() const {
    return m_tper->GetModules();
}


std::vector<NamedObject> SEDManager::GetSecurityProviders() {
    auto get = [this](std::shared_ptr<Session> session) {
        std::vector<NamedObject> securityProviders;

        Table sp = GetTable(core::eTable::SP);
        return GetNamedRows(sp);
    };

    if (m_session) {
        return get(m_session);
    }
    else {
        const auto maybeAdminSP = GetModules().FindUid("SP::Admin");
        if (!maybeAdminSP) {
            throw std::runtime_error("could not find the Admin SP");
        }
        const auto session = std::make_shared<Session>(m_sessionManager, *maybeAdminSP);
        return get(session);
    }
}


std::vector<NamedObject> SEDManager::GetAuthorities() {
    if (m_session) {
        Table auth = GetTable(core::eTable::Authority);
        return GetNamedRows(auth);
    }
    throw std::logic_error("a session must be active");
}


std::vector<NamedObject> SEDManager::GetTables() {
    if (m_session) {
        Table table = GetTable(core::eTable::Table);
        return GetNamedRows(table);
    }
    throw std::logic_error("a session must be active");
}


Table SEDManager::GetTable(Uid table) {
    if (m_session) {
        auto maybeTableDesc = GetModules().FindTable(table);
        if (!maybeTableDesc) {
            throw std::runtime_error("failed to get table description");
        }
        return Table(table, std::move(*maybeTableDesc), m_session);
    }
    throw std::logic_error("a session must be active");
}


Object SEDManager::GetObject(Uid table, Uid object) {
    if (m_session) {
        auto maybeTableDesc = GetModules().FindTable(table);
        if (!maybeTableDesc) {
            throw std::runtime_error("failed to get table description");
        }
        return Object(object, std::move(*maybeTableDesc), m_session);
    }
    throw std::logic_error("a session must be active");
}


void SEDManager::Start(Uid securityProvider) {
    if (m_session) {
        m_session = {};
    }
    m_session = std::make_shared<Session>(m_sessionManager, securityProvider);
}


void SEDManager::Authenticate(Uid authority, std::optional<std::span<const std::byte>> password) {
    if (m_session) {
        return m_session->base.Authenticate(authority, password);
    }
    throw std::logic_error("start a session on an SP to authenticate");
}


void SEDManager::End() {
    m_session = {};
}


void SEDManager::StackReset() {
    m_tper->StackReset();
    End();
}


void SEDManager::Reset() {
    m_tper->Reset();
    End();
}


