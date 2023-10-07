#include "Session.hpp"

#include "Communication/Packet.hpp"
#include "Logging.hpp"
#include "Serialization/TokenDebugArchive.hpp"
#include "Serialization/Utility.hpp"

#include <atomic>



Session::Session(std::shared_ptr<SessionManager> sessionManager,
                 Uid securityProvider,
                 std::optional<std::span<const std::byte>> password,
                 std::optional<Uid> authority)

    : m_sessionManager(sessionManager) {
    m_hostSessionNumber = NewHostSessionNumber();
    const auto result = sessionManager->StartSession(m_hostSessionNumber,
                                                     securityProvider,
                                                     true,
                                                     password,
                                                     std::nullopt,
                                                     std::nullopt,
                                                     authority);
    m_tperSessionNumber = result.spSessionId;
    base = impl::BaseTemplate{ m_sessionManager, m_tperSessionNumber, m_hostSessionNumber };
    opal = impl::OpalTemplate{ m_sessionManager, m_tperSessionNumber, m_hostSessionNumber };
}


Session& Session::operator=(Session&& rhs) {
    End();
    m_sessionManager = std::move(rhs.m_sessionManager);
    m_tperSessionNumber = rhs.m_tperSessionNumber;
    m_hostSessionNumber = rhs.m_hostSessionNumber;
    return *this;
}


Session::~Session() {
    End();
}


uint32_t Session::GetHostSessionNumber() const {
    return m_hostSessionNumber;
}

uint32_t Session::GetTPerSessionNumber() const {
    return m_tperSessionNumber;
}

uint32_t Session::NewHostSessionNumber() {
    static std::atomic_uint32_t hsn = 1;
    return hsn.fetch_add(1);
}

void Session::End() {
    try {
        m_sessionManager->EndSession(m_tperSessionNumber, m_hostSessionNumber);
    }
    catch (std::exception& ex) {
        std::cerr << std::format("failed to end session (tsn={}, hsn={}): {}", m_tperSessionNumber, m_hostSessionNumber, ex.what()) << std::endl;
    }
}

//------------------------------------------------------------------------------
// Templates
//------------------------------------------------------------------------------


namespace impl {
Template::Template(std::shared_ptr<SessionManager> sessionManager,
                   uint32_t tperSessionNumber,
                   uint32_t hostSessionNumber)
    : m_sessionManager(sessionManager),
      m_tperSessionNumber(tperSessionNumber),
      m_hostSessionNumber(hostSessionNumber) {}



MethodResult Template::InvokeMethod(Uid invokingId, const Method& method) {
    assert(m_sessionManager);

    const auto request = MethodToValue(invokingId, method);
    Log("Session Method - Call", request);
    std::stringstream requestSs(std::ios::binary | std::ios::out);
    TokenOutputArchive requestAr(requestSs);
    save_strip_list(requestAr, request);
    const auto requestBytes = BytesView(requestSs.view());
    const auto requestPacket = CreatePacket({ requestBytes.begin(), requestBytes.end() });
    const auto responsePacket = m_sessionManager->GetTrustedPeripheral()->SendPacket(PROTOCOL, requestPacket);
    const auto responseBytes = UnwrapPacket(responsePacket);
    Value response;
    FromTokens(responseBytes, response);
    Log("Session Method - Result", response);

    MethodResult result = MethodResultFromValue(response);
    if (result.status != eMethodStatus::SUCCESS) {
        throw std::runtime_error(std::format("call to method (id={:#010x}) failed: {}", uint64_t(method.methodId), MethodStatusText(result.status)));
    }
    return result;
};


ComPacket Template::CreatePacket(std::vector<uint8_t> payload) {
    return m_sessionManager->CreatePacket(std::move(payload), m_tperSessionNumber, m_hostSessionNumber);
}


std::span<const uint8_t> Template::UnwrapPacket(const ComPacket& packet) {
    return m_sessionManager->UnwrapPacket(packet);
}

//------------------------------------------------------------------------------
// Base template
//------------------------------------------------------------------------------

Value BaseTemplate::Get(Uid table, Uid row, uint32_t column) {
    CellBlock block{
        .startRow = uint64_t(row),
        .startColumn = column,
        .endColumn = column,
    };
    auto&& [values] = InvokeMethod<std::tuple<std::unordered_map<uint32_t, Value>>>(table, eMethodId::Get, block);
    if (values.size() < 1) {
        throw std::runtime_error("device did not return any table values");
    }
    return std::move(values.begin()->second);
}

Value BaseTemplate::Get(Uid object, uint32_t column) {
    CellBlock block{
        .startColumn = column,
        .endColumn = column,
    };
    auto&& [values] = InvokeMethod<std::tuple<std::unordered_map<uint32_t, Value>>>(object, eMethodId::Get, block);
    if (values.size() < 1) {
        throw std::runtime_error("device did not return any table values");
    }
    return std::move(values.begin()->second);
}


void BaseTemplate::Set(Uid objectOrTable, std::optional<Uid> row, std::optional<std::unordered_map<uint32_t, Value>> rowValues) {
    InvokeMethod(objectOrTable, eMethodId::Set, rowValues);
}


//------------------------------------------------------------------------------
// Base template
//------------------------------------------------------------------------------

void OpalTemplate::Revert(Uid securityProvider) {
    InvokeMethod(securityProvider, eMethodId::Opal_Revert);
}


} // namespace impl