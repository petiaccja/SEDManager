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
        if (m_sessionManager) {
            m_sessionManager->EndSession(m_tperSessionNumber, m_hostSessionNumber);
        }
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
    const auto requestBytes = std::as_bytes(std::span(requestSs.view()));
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


ComPacket Template::CreatePacket(std::vector<std::byte> payload) {
    return m_sessionManager->CreatePacket(std::move(payload), m_tperSessionNumber, m_hostSessionNumber);
}


std::span<const std::byte> Template::UnwrapPacket(const ComPacket& packet) {
    return m_sessionManager->UnwrapPacket(packet);
}

//------------------------------------------------------------------------------
// Base template
//------------------------------------------------------------------------------


std::vector<Value> BaseTemplate::Get(Uid object, uint32_t startColumn, uint32_t endColumn) {
    CellBlock cellBlock{
        .startColumn = startColumn,
        .endColumn = endColumn - 1,
    };
    auto [nameValuePairs] = InvokeMethod<std::tuple<std::vector<Value>>>(object, eMethod::Get, cellBlock);
    std::vector<Value> values(endColumn - startColumn);
    for (auto& nvp : nameValuePairs) {
        const auto idx = nvp.AsNamed().name.Get<size_t>();
        if (size_t(idx - startColumn) > values.size()) {
            throw std::runtime_error("device returned unexpected columns");
        }
        values[idx - startColumn] = nvp.AsNamed().value;
    }
    std::ranges::transform(nameValuePairs, std::back_inserter(values), [](Value& value) {
        return std::move(value.AsNamed().value);
    });
    return values;
}


Value BaseTemplate::Get(Uid object, uint32_t column) {
    auto values = Get(object, column, column + 1);
    if (values.empty()) {
        throw std::runtime_error("device did not return any values");
    }
    return std::move(values[0]);
}


void BaseTemplate::Set(Uid object, std::span<const uint32_t> columns, std::span<const Value> values) {
    const std::optional<Uid> rowAddress = object;
    std::optional<std::vector<Value>> namedValuePairs = std::vector<Value>();
    for (auto [colIt, valIt] = std::tuple{ columns.begin(), values.begin() };
         colIt != columns.end() && valIt != values.end();
         ++colIt, ++valIt) {
        namedValuePairs.value().emplace_back(Named{ *colIt, *valIt });
    }
    InvokeMethod(object, eMethod::Set, rowAddress, namedValuePairs);
}


void BaseTemplate::Set(Uid object, uint32_t column, const Value& value) {
    Set(object, std::span{ &column, 1 }, std::span{ &value, 1 });
}


std::vector<Uid> BaseTemplate::Next(Uid table, std::optional<Uid> row, uint32_t count) {
    auto [nexts] = InvokeMethod<std::tuple<std::vector<Uid>>>(table, eMethod::Next, row, std::optional(count));
    return nexts;
}


std::optional<Uid> BaseTemplate::Next(Uid table, std::optional<Uid> row) {
    const auto nexts = Next(table, row, 1);
    if (!nexts.empty()) {
        return nexts[0];
    }
    return {};
}


void BaseTemplate::Authenticate(Uid authority, std::optional<std::span<const std::byte>> proof) {
    auto [result] = InvokeMethod<std::tuple<Value>>(THIS_SP, eMethod::Authenticate, authority, proof);
    if (result.IsInteger()) {
        const auto success = result.Get<uint8_t>();
        if (!success) {
            throw std::invalid_argument("authentication failed");
        }
    }
    else {
        throw std::invalid_argument("challenge protocol not implemented");
    }
}


void BaseTemplate::GenKey(Uid object, std::optional<uint32_t> publicExponent, std::optional<uint32_t> pinLength) {
    InvokeMethod(object, eMethod::GenKey, publicExponent, pinLength);
}


//------------------------------------------------------------------------------
// Base template
//------------------------------------------------------------------------------

void OpalTemplate::Revert(Uid securityProvider) {
    InvokeMethod(securityProvider, opal::eMethod::Revert);
}


void OpalTemplate::Activate(Uid securityProvider) {
    InvokeMethod(securityProvider, opal::eMethod::Activate);
}


} // namespace impl