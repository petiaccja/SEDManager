#include "Session.hpp"

#include "Logging.hpp"
#include <async++/join.hpp>

#include <Archive/Conversion.hpp>
#include <Archive/TokenDebugArchive.hpp>
#include <Archive/Types/ValueToToken.hpp>
#include <Data/ComPacket.hpp>
#include <Error/Exception.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>

#include <atomic>


namespace sedmgr {

Session::Session(std::shared_ptr<SessionManager> sessionManager,
                 Uid securityProvider,
                 std::optional<std::span<const std::byte>> password,
                 std::optional<Uid> authority)

    : Session(join(Start(sessionManager, securityProvider, password, authority))) {
}


Session::Session(std::shared_ptr<SessionManager> sessionManager,
                 uint32_t tperSessionNumber,
                 uint32_t hostSessionNumber)
    : base{ sessionManager, tperSessionNumber, hostSessionNumber },
      opal{ sessionManager, tperSessionNumber, hostSessionNumber },
      m_sessionManager(sessionManager),
      m_tperSessionNumber(tperSessionNumber),
      m_hostSessionNumber(hostSessionNumber) {
}


Session& Session::operator=(Session&& rhs) {
    join(End());
    m_sessionManager = std::move(rhs.m_sessionManager);
    m_tperSessionNumber = rhs.m_tperSessionNumber;
    m_hostSessionNumber = rhs.m_hostSessionNumber;
    return *this;
}


Session::~Session() {
    join(End());
}


asyncpp::task<Session> Session::Start(std::shared_ptr<SessionManager> sessionManager,
                                      Uid securityProvider,
                                      std::optional<std::span<const std::byte>> password,
                                      std::optional<Uid> authority) {
    const auto hostSessionNumber = NewHostSessionNumber();
    const auto result = co_await sessionManager->StartSession(hostSessionNumber,
                                                              securityProvider,
                                                              true,
                                                              password,
                                                              std::nullopt,
                                                              std::nullopt,
                                                              authority);
    const auto tperSessionNumber = result.spSessionId;
    co_return Session(sessionManager, tperSessionNumber, hostSessionNumber);
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

asyncpp::task<void> Session::End() {
    try {
        if (m_sessionManager) {
            co_await m_sessionManager->EndSession(m_tperSessionNumber, m_hostSessionNumber);
            m_sessionManager = nullptr;
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


    const TPerModules& Template::GetModules() const {
        return m_sessionManager->GetTrustedPeripheral()->GetModules();
    }


    asyncpp::task<MethodResult> Template::InvokeMethod(Uid invokingId, const Method& method) {
        const std::string methodIdStr = GetModules().FindName(method.methodId).value_or(to_string(method.methodId));
        try {
            assert(m_sessionManager);

            const auto request = MethodToValue(invokingId, method);
            Log(std::format("Call '{}' [Session]", methodIdStr), request);
            std::stringstream requestSs(std::ios::binary | std::ios::out);
            TokenBinaryOutputArchive requestAr(requestSs);
            save_strip_list(requestAr, request);
            const auto requestBytes = std::as_bytes(std::span(requestSs.view()));
            const auto requestPacket = CreatePacket({ requestBytes.begin(), requestBytes.end() });
            const auto responsePacket = co_await m_sessionManager->GetTrustedPeripheral()->SendPacket(PROTOCOL, requestPacket);
            const auto responseBytes = UnwrapPacket(responsePacket);
            Value response;
            FromTokens(responseBytes, response);
            Log(std::format("Result '{}' [Session]", methodIdStr), response);

            MethodResult result = MethodResultFromValue(response);
            MethodStatusToException(methodIdStr, result.status);
            co_return result;
        }
        catch (InvocationError&) {
            throw;
        }
        catch (std::exception& ex) {
            throw InvocationError(methodIdStr, ex.what());
        }
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


    asyncpp::task<std::vector<Value>> BaseTemplate::Get(Uid object, uint32_t startColumn, uint32_t endColumn) {
        CellBlock cellBlock{
            .startColumn = startColumn,
            .endColumn = endColumn - 1,
        };
        auto [nameValuePairs] = co_await InvokeMethod<std::tuple<std::vector<Value>>>(object, core::eMethod::Get, cellBlock);
        std::vector<Value> values(endColumn - startColumn);
        for (auto& nvp : nameValuePairs) {
            const auto idx = nvp.GetNamed().name.Get<size_t>();
            if (size_t(idx - startColumn) > values.size()) {
                throw InvalidResponseError("Get", "too many columns");
            }
            values[idx - startColumn] = nvp.GetNamed().value;
        }
        std::ranges::transform(nameValuePairs, std::back_inserter(values), [](Value& value) {
            return std::move(value.GetNamed().value);
        });
        co_return values;
    }


    asyncpp::task<Value> BaseTemplate::Get(Uid object, uint32_t column) {
        auto values = co_await Get(object, column, column + 1);
        if (values.empty()) {
            throw InvalidResponseError("Get", "zero columns");
        }
        co_return std::move(values[0]);
    }


    asyncpp::task<void> BaseTemplate::Set(Uid object, std::span<const uint32_t> columns, std::span<const Value> values) {
        const std::optional<Uid> rowAddress = std::nullopt; // Must be omitted for objects.
        std::optional<std::vector<Value>> namedValuePairs = std::vector<Value>();
        for (auto [colIt, valIt] = std::tuple{ columns.begin(), values.begin() };
             colIt != columns.end() && valIt != values.end();
             ++colIt, ++valIt) {
            namedValuePairs.value().emplace_back(Named{ *colIt, *valIt });
        }
        co_await InvokeMethod(object, core::eMethod::Set, rowAddress, namedValuePairs);
    }


    asyncpp::task<void> BaseTemplate::Set(Uid object, uint32_t column, const Value& value) {
        co_await Set(object, std::span{ &column, 1 }, std::span{ &value, 1 });
    }


    asyncpp::task<std::vector<Uid>> BaseTemplate::Next(Uid table, std::optional<Uid> row, uint32_t count) {
        auto [nexts] = co_await InvokeMethod<std::tuple<std::vector<Uid>>>(table, core::eMethod::Next, row, std::optional(count));
        co_return nexts;
    }


    asyncpp::task<std::optional<Uid>> BaseTemplate::Next(Uid table, std::optional<Uid> row) {
        const auto nexts = co_await Next(table, row, 1);
        if (!nexts.empty()) {
            co_return nexts[0];
        }
        co_return {};
    }


    asyncpp::task<void> BaseTemplate::Authenticate(Uid authority, std::optional<std::span<const std::byte>> proof) {
        auto [result] = co_await InvokeMethod<std::tuple<Value>>(THIS_SP, core::eMethod::Authenticate, authority, proof);
        if (result.IsInteger()) {
            const auto success = result.Get<uint8_t>();
            if (!success) {
                throw PasswordError();
            }
        }
        else {
            throw NotImplementedError("challenge protocol for method 'Authenticate' is not implemented");
        }
    }


    asyncpp::task<void> BaseTemplate::GenKey(Uid object, std::optional<uint32_t> publicExponent, std::optional<uint32_t> pinLength) {
        co_await InvokeMethod(object, core::eMethod::GenKey, publicExponent, pinLength);
    }


    //------------------------------------------------------------------------------
    // Base template
    //------------------------------------------------------------------------------

    asyncpp::task<void> OpalTemplate::Revert(Uid securityProvider) {
        co_await InvokeMethod(securityProvider, opal::eMethod::Revert);
    }


    asyncpp::task<void> OpalTemplate::Activate(Uid securityProvider) {
        co_await InvokeMethod(securityProvider, opal::eMethod::Activate);
    }


} // namespace impl

} // namespace sedmgr