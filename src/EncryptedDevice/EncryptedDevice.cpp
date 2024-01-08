#include "EncryptedDevice.hpp"

#include <async++/join.hpp>

#include <Specification/Core/CoreModule.hpp>

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


EncryptedDevice::EncryptedDevice(std::shared_ptr<StorageDevice> device)
    : EncryptedDevice(join(Start(device))) {}


EncryptedDevice::EncryptedDevice(std::shared_ptr<StorageDevice> device,
                                 std::shared_ptr<TrustedPeripheral> tper,
                                 std::shared_ptr<SessionManager> sessionManager)
    : m_device(device),
      m_tper(tper),
      m_sessionManager(sessionManager) {}


EncryptedDevice::~EncryptedDevice() {
    join(End());
}


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


asyncpp::task<void> EncryptedDevice::Login(UID securityProvider) {
    m_session = std::make_shared<Session>(co_await Session::Start(m_sessionManager, securityProvider));
}


asyncpp::task<void> EncryptedDevice::Authenticate(UID authority, std::optional<std::span<const std::byte>> password) {
    if (m_session) {
        return m_session->base.Authenticate(authority, password);
    }
    throw std::logic_error("start a session on an SP to authenticate");
}


asyncpp::task<void> EncryptedDevice::End() {
    if (m_session) {
        co_await m_session->End();
    }
    m_session = nullptr;
}


asyncpp::stream<UID> EncryptedDevice::GetTableRows(UID table) {
    const auto desc = GetModules().FindTable(table);
    if (!desc) {
        throw std::invalid_argument("could not find table description");
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


asyncpp::stream<Value> EncryptedDevice::GetObjectColumns(UID object) {
    const auto maybeTableDesc = GetModules().FindTable(object.ContainingTable());
    if (!maybeTableDesc) {
        throw std::invalid_argument("could not find table description");
    }
    uint32_t index = 0;
    for (const auto& column : maybeTableDesc->columns) {
        co_yield co_await m_session->base.Get(object, index);
        ++index;
    }
}


asyncpp::task<Value> EncryptedDevice::GetObjectColumn(UID object, uint32_t column) {
    co_return co_await m_session->base.Get(object, column);
}


asyncpp::task<void> EncryptedDevice::SetObjectColumn(UID object, uint32_t column, Value value) {
    co_await m_session->base.Set(object, column, value);
}


asyncpp::task<void> EncryptedDevice::GenMEK(UID lockingRange) {
    co_await m_session->base.GenKey(lockingRange);
}


asyncpp::task<void> EncryptedDevice::GenPIN(UID credentialObject, uint32_t length) {
    co_await m_session->base.GenKey(credentialObject, std::nullopt, length);
}


asyncpp::task<void> EncryptedDevice::Revert(UID securityProvider) {
    co_await m_session->opal.Revert(securityProvider);
    co_await End();
}


asyncpp::task<void> EncryptedDevice::Activate(UID securityProvider) {
    co_await m_session->opal.Activate(securityProvider);
}


asyncpp::task<void> EncryptedDevice::StackReset() {
    co_await End();
    co_await m_tper->StackReset();
    *this = co_await Start(m_device);
}


asyncpp::task<void> EncryptedDevice::Reset() {
    co_await End();
    m_tper->Reset();
    *this = co_await Start(m_device);
}

} // namespace sedmgr