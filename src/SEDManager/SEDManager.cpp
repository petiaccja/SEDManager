#include "SEDManager.hpp"

#include <async++/join.hpp>

#include <Specification/Core/CoreModule.hpp>

#include <stdexcept>


namespace sedmgr {

SEDManager::SEDManager(std::shared_ptr<StorageDevice> device) : m_device(device) {
    join(LaunchStack());
}


const TPerDesc& SEDManager::GetDesc() const {
    return m_tper->GetDesc();
}


const TPerModules& SEDManager::GetModules() const {
    return m_tper->GetModules();
}


asyncpp::task<void> SEDManager::Start(Uid securityProvider) {
    if (m_session) {
        co_await m_session->End();
        m_session = {};
    }
    m_session = std::make_shared<Session>(co_await Session::Start(m_sessionManager, securityProvider));
}


asyncpp::task<void> SEDManager::Authenticate(Uid authority, std::optional<std::span<const std::byte>> password) {
    if (m_session) {
        return m_session->base.Authenticate(authority, password);
    }
    throw std::logic_error("start a session on an SP to authenticate");
}


asyncpp::task<void> SEDManager::End() {
    if (m_session) {
        co_await m_session->End();
        m_session = nullptr;
    }
}


asyncpp::stream<Uid> SEDManager::GetTableRows(Uid table) {
    while (const auto rowUid = co_await m_session->base.Next(table, std::nullopt)) {
        if (*rowUid != Uid(0)) {
            co_yield *rowUid;
        }
    }
}


asyncpp::stream<Value> SEDManager::GetObjectColumns(Uid table, Uid object) {
    const auto maybeTableDesc = GetModules().FindTable(table);
    if (!maybeTableDesc) {
        throw std::invalid_argument("could not find table description");
    }
    uint32_t index = 0;
    for (const auto& column : maybeTableDesc->columns) {
        co_yield co_await m_session->base.Get(object, index);
        ++index;
    }
}


asyncpp::task<Value> SEDManager::GetObjectColumn(Uid object, uint32_t column) {
    co_return co_await m_session->base.Get(object, column);
}


asyncpp::task<void> SEDManager::SetObjectColumn(Uid object, uint32_t column, Value value) {
    co_await m_session->base.Set(object, column, value);
}


asyncpp::task<void> SEDManager::GenMEK(Uid lockingRange) {
    co_await m_session->base.GenKey(lockingRange);
}


asyncpp::task<void> SEDManager::GenPIN(Uid credentialObject, uint32_t length) {
    co_await m_session->base.GenKey(credentialObject, std::nullopt, length);
}


asyncpp::task<void> SEDManager::Revert(Uid securityProvider) {
    co_await m_session->opal.Revert(securityProvider);
    join(End());
    co_await LaunchStack();
}


asyncpp::task<void> SEDManager::Activate(Uid securityProvider) {
    co_await m_session->opal.Activate(securityProvider);
}


asyncpp::task<void> SEDManager::StackReset() {
    co_await m_tper->StackReset();
    join(End());
    co_await LaunchStack();
}


asyncpp::task<void> SEDManager::Reset() {
    m_tper->Reset();
    join(End());
    co_await LaunchStack();
}

asyncpp::task<void> SEDManager::LaunchStack() {
    m_session = {};
    m_tper = std::make_shared<TrustedPeripheral>(m_device);
    const auto comIdState = co_await m_tper->VerifyComId();
    if (comIdState != eComIdState::ISSUED && comIdState != eComIdState::ASSOCIATED) {
        throw std::runtime_error("failed to acquire valid ComID");
    }
    m_sessionManager = std::make_shared<SessionManager>(m_tper);
    co_await ExchangeProperties();
}


asyncpp::task<std::unordered_map<std::string, uint32_t>> SEDManager::ExchangeProperties() {
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

    auto [tperProps, hostProps] = co_await m_sessionManager->Properties(hostProperties);
    co_return tperProps;
}

} // namespace sedmgr