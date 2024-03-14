#include "EncryptedDevice.hpp"

#include <Specification/Core/CoreModule.hpp>

#include <asyncpp/join.hpp>

#include <stdexcept>


namespace sedmgr {

static const std::unordered_map<std::string, uint32_t> hostProperties = {
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


SimpleSession::SimpleSession(std::shared_ptr<TrustedPeripheral> tper,
                             std::shared_ptr<Session> session,
                             UID securityProvider)
    : m_tper(std::move(tper)),
      m_session(std::move(session)),
      m_securityProvider(securityProvider) {}


SimpleSession::~SimpleSession() {
    join(End());
}


const ModuleCollection& SimpleSession::GetModules() const {
    return m_tper->GetModules();
}

UID SimpleSession::GetSecurityProvider() const {
    return m_securityProvider;
}


asyncpp::task<void> SimpleSession::Authenticate(UID authority, std::optional<std::vector<std::byte>> password) {
    co_return co_await m_session->base.Authenticate(authority, password);
}


asyncpp::task<void> SimpleSession::End() {
    if (m_session) {
        co_await m_session->End();
    }
    m_session = nullptr;
}


asyncpp::stream<UID> SimpleSession::GetTableRows(UID table) {
    const auto desc = m_tper->GetModules().FindTable(table);
    if (!desc) {
        throw std::invalid_argument(std::format("could not find table description: {}", table.ToString()));
    }
    if (desc->singleRow) {
        co_yield *desc->singleRow;
    }
    else {
        std::optional<UID> lastUid = std::nullopt;
        while (const auto rowUid = co_await m_session->base.Next(table, lastUid)) {
            if (*rowUid != UID(0)) {
                co_yield *rowUid;
            }
            lastUid = *rowUid;
        }
    }
}


asyncpp::stream<Value> SimpleSession::GetObjectColumns(UID object) {
    const auto maybeTableDesc = m_tper->GetModules().FindTable(object.ContainingTable());
    if (!maybeTableDesc) {
        throw std::invalid_argument(std::format("could not find table description: {}", object.ToString()));
    }
    uint32_t index = 0;
    for (const auto& column : maybeTableDesc->columns) {
        co_yield co_await m_session->base.Get(object, index);
        ++index;
    }
}


asyncpp::task<Value> SimpleSession::GetValue(UID object, uint32_t column) {
    co_return co_await m_session->base.Get(object, column);
}


asyncpp::task<void> SimpleSession::SetValue(UID object, uint32_t column, Value value) {
    co_await m_session->base.Set(object, column, value);
}


asyncpp::task<void> SimpleSession::GenMEK(UID lockingRange) {
    co_await m_session->base.GenKey(lockingRange);
}


asyncpp::task<void> SimpleSession::GenPIN(UID credentialObject, uint32_t length) {
    co_await m_session->base.GenKey(credentialObject, std::nullopt, length);
}


asyncpp::task<void> SimpleSession::Revert(UID securityProvider) {
    co_await m_session->opal.Revert(securityProvider);
    co_await End();
}


asyncpp::task<void> SimpleSession::Activate(UID securityProvider) {
    co_await m_session->opal.Activate(securityProvider);
}


EncryptedDevice::EncryptedDevice(std::shared_ptr<StorageDevice> device)
    : EncryptedDevice(join(Start(device))) {}


EncryptedDevice::EncryptedDevice(std::shared_ptr<StorageDevice> device,
                                 std::shared_ptr<TrustedPeripheral> tper,
                                 std::shared_ptr<SessionManager> sessionManager)
    : m_device(device),
      m_tper(tper),
      m_sessionManager(sessionManager) {}


asyncpp::task<EncryptedDevice> EncryptedDevice::Start(std::shared_ptr<StorageDevice> device) {
    const auto tper = std::make_shared<TrustedPeripheral>(device);
    const auto comIdState = co_await tper->VerifyComId();
    if (comIdState != eComIdState::ISSUED && comIdState != eComIdState::ASSOCIATED) {
        throw std::runtime_error("failed to acquire valid ComID");
    }
    const auto sessionManager = std::make_shared<SessionManager>(tper);
    auto [tperProps, hostProps] = co_await sessionManager->Properties(hostProperties);
    co_return EncryptedDevice(device, tper, sessionManager);
}


const TPerDesc& EncryptedDevice::GetDesc() const {
    return m_tper->GetDesc();
}


const ModuleCollection& EncryptedDevice::GetModules() const {
    return m_tper->GetModules();
}


asyncpp::task<SimpleSession> EncryptedDevice::Login(UID securityProvider) {
    const auto session = std::make_shared<Session>(co_await Session::Start(m_sessionManager, securityProvider));
    co_return SimpleSession(m_tper, session, securityProvider);
}

asyncpp::task<void> EncryptedDevice::StackReset() {
    co_await m_tper->StackReset();
    *this = co_await Start(m_device);
}


asyncpp::task<void> EncryptedDevice::Reset() {
    co_await m_tper->Reset();
    *this = co_await Start(m_device);
}

} // namespace sedmgr