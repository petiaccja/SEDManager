#include "Session.hpp"

#include "Communication/Packet.hpp"
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
}

Session::~Session() {
    try {
        m_sessionManager->EndSession(m_tperSessionNumber, m_hostSessionNumber);
    }
    catch (std::exception& ex) {
        std::cerr << std::format("failed to end session (tsn={}, hsn={}): {}", m_tperSessionNumber, m_hostSessionNumber, ex.what()) << std::endl;
    }
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



MethodResult Template::InvokeMethod(const Method& method) {
    const auto reqestStream = SerializeMethod(INVOKING_ID, method);
    auto requestBytes = ToTokens(reqestStream);
    const auto requestPacket = CreatePacket(std::move(requestBytes));
    const auto responsePacket = m_sessionManager->GetTrustedPeripheral()->SendPacket(PROTOCOL, requestPacket);
    const auto responseBytes = UnwrapPacket(responsePacket);
    TokenStream responseStream;
    FromTokens(responseBytes, responseStream);

    MethodResult result = ParseMethodResult(responseStream);
    if (result.status != eMethodStatus::SUCCESS) {
        throw std::runtime_error(std::format("call to method (id={:#010x}) failed: {}", uint64_t(method.methodId), MethodStatusText(result.status)));
    }
    return result;
};


ComPacket Template::CreatePacket(std::vector<uint8_t> payload) {
    return m_sessionManager->CreatePacket(payload, m_tperSessionNumber, m_hostSessionNumber);
}


std::span<const uint8_t> Template::UnwrapPacket(const ComPacket& packet) {
    return m_sessionManager->UnwrapPacket(packet);
}


} // namespace impl