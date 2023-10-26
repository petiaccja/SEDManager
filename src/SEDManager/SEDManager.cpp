#include "SEDManager.hpp"

#include <Specification/Core/CoreModule.hpp>

#include <stdexcept>


SEDManager::SEDManager(std::shared_ptr<StorageDevice> device) : m_device(device) {
    LaunchStack();
}


const TPerDesc& SEDManager::GetDesc() const {
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


void SEDManager::GenMEK(Uid lockingRange) {
    m_session->base.GenKey(lockingRange);
}


void SEDManager::GenPIN(Uid credentialObject, uint32_t length) {
    m_session->base.GenKey(credentialObject, std::nullopt, length);
}


void SEDManager::Revert(Uid securityProvider) {
    m_session->opal.Revert(securityProvider);
    End();
    LaunchStack();
}


void SEDManager::Activate(Uid securityProvider) {
    m_session->opal.Activate(securityProvider);
}


void SEDManager::StackReset() {
    m_tper->StackReset();
    End();
    LaunchStack();
}


void SEDManager::Reset() {
    m_tper->Reset();
    End();
    LaunchStack();
}

void SEDManager::LaunchStack() {
    m_session = {};
    m_tper = std::make_shared<TrustedPeripheral>(m_device);
    const auto comIdState = m_tper->VerifyComId();
    if (comIdState != eComIdState::ISSUED && comIdState != eComIdState::ASSOCIATED) {
        throw std::runtime_error("failed to acquire valid ComID");
    }
    m_sessionManager = std::make_shared<SessionManager>(m_tper);
}