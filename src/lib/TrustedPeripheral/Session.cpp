#include "Session.hpp"

#include "Logging.hpp"
#include <async++/join.hpp>

#include <Archive/Conversion.hpp>
#include <Error/Exception.hpp>
#include <Messaging/ComPacket.hpp>
#include <Messaging/TokenStream.hpp>
#include <Specification/Core/CoreModule.hpp>
#include <Specification/Opal/OpalModule.hpp>

#include <atomic>


namespace sedmgr {

Session::Session(std::shared_ptr<SessionManager> sessionManager,
                 UID securityProvider,
                 std::optional<std::span<const std::byte>> password,
                 std::optional<UID> authority)

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


Session& Session::operator=(Session&& rhs) noexcept {
    try {
        join(End());
    }
    catch (std::exception& ex) {
        std::cerr << "error while closing session: " << ex.what() << std::endl;
    }
    m_sessionManager = std::move(rhs.m_sessionManager);
    m_tperSessionNumber = rhs.m_tperSessionNumber;
    m_hostSessionNumber = rhs.m_hostSessionNumber;
    return *this;
}


Session::~Session() {
    try {
        join(End());
    }
    catch (std::exception& ex) {
        std::cerr << "error while closing session: " << ex.what() << std::endl;
    }
}


asyncpp::task<Session> Session::Start(std::shared_ptr<SessionManager> sessionManager,
                                      UID securityProvider,
                                      std::optional<std::span<const std::byte>> password,
                                      std::optional<UID> authority) {
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


    const ModuleCollection& Template::GetModules() const {
        return m_sessionManager->GetTrustedPeripheral()->GetModules();
    }


    CallContext Template::GetCallContext(UID invokingId) const {
        auto callRemoteMethod = [tper = m_sessionManager->GetTrustedPeripheral(),
                                 tsn = m_tperSessionNumber,
                                 hsn = m_hostSessionNumber](MethodCall call) -> asyncpp::task<MethodResult> {
            co_return co_await CallRemoteMethod(tper, PROTOCOL, tsn, hsn, std::move(call));
        };
        auto getMethodName = [tper = m_sessionManager->GetTrustedPeripheral()](UID methodId) {
            const auto maybeMethodName = tper->GetModules().FindName(methodId);
            return maybeMethodName.value_or(methodId.ToString());
        };
        return CallContext{ .invokingId = invokingId, .callRemoteMethod = callRemoteMethod, .getMethodName = getMethodName };
    }


    //------------------------------------------------------------------------------
    // Base template
    //------------------------------------------------------------------------------


    asyncpp::task<std::vector<Value>> BaseTemplate::Get(UID object, uint32_t startColumn, uint32_t endColumn) {
        CellBlock cellBlock{
            .startColumn = startColumn,
            .endColumn = endColumn - 1,
        };
        auto [labeledValues] = co_await getMethod(GetCallContext(object), ConvertArg(cellBlock));
        std::vector<Value> values(endColumn - startColumn);
        for (auto& nvp : labeledValues.Get<List>()) {
            const auto idx = nvp.Get<Named>().name.Get<size_t>();
            if (size_t(idx - startColumn) > values.size()) {
                throw InvalidResponseError("Get", "too many columns");
            }
            values[idx - startColumn] = nvp.Get<Named>().value;
        }
        co_return values;
    }


    asyncpp::task<Value> BaseTemplate::Get(UID object, uint32_t column) {
        auto values = co_await Get(object, column, column + 1);
        if (values.empty()) {
            throw InvalidResponseError("Get", "zero columns");
        }
        co_return std::move(values[0]);
    }


    asyncpp::task<void> BaseTemplate::Set(UID object, std::span<const uint32_t> columns, std::span<const Value> values) {
        std::vector<Value> labeledValues;
        for (auto [colIt, valIt] = std::tuple{ columns.begin(), values.begin() };
             colIt != columns.end() && valIt != values.end();
             ++colIt, ++valIt) {
            labeledValues.emplace_back(Named{ *colIt, *valIt });
        }
        co_await setMethod(GetCallContext(object), std::nullopt, ConvertArg(labeledValues));
    }


    asyncpp::task<void> BaseTemplate::Set(UID object, uint32_t column, const Value& value) {
        co_await Set(object, std::span{ &column, 1 }, std::span{ &value, 1 });
    }


    asyncpp::task<std::vector<UID>> BaseTemplate::Next(UID table, std::optional<UID> row, uint32_t count) {
        auto [results] = co_await nextMethod(GetCallContext(table), ConvertArg(row), ConvertArg(count));
        std::vector<UID> nextUids;
        std::ranges::transform(results.Get<List>(), std::back_inserter(nextUids), [](const auto& value) -> UID {
            return ConvertResult(value);
        });
        co_return nextUids;
    }


    asyncpp::task<std::optional<UID>> BaseTemplate::Next(UID table, std::optional<UID> row) {
        const auto nexts = co_await Next(table, row, 1);
        if (!nexts.empty()) {
            co_return nexts[0];
        }
        co_return {};
    }


    asyncpp::task<void> BaseTemplate::Authenticate(UID authority, std::optional<std::span<const std::byte>> proof) {
        auto [result] = co_await authenticateMethod(GetCallContext(THIS_SP), ConvertArg(authority), ConvertArg(proof));
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


    asyncpp::task<void> BaseTemplate::GenKey(UID object, std::optional<uint32_t> publicExponent, std::optional<uint32_t> pinLength) {
        co_await genKeyMethod(GetCallContext(object), ConvertArg(publicExponent), ConvertArg(pinLength));
    }


    //------------------------------------------------------------------------------
    // Base template
    //------------------------------------------------------------------------------

    asyncpp::task<void> OpalTemplate::Revert(UID securityProvider) {
        co_await revertMethod(GetCallContext(securityProvider));
    }


    asyncpp::task<void> OpalTemplate::Activate(UID securityProvider) {
        co_await activateMethod(GetCallContext(securityProvider));
    }


} // namespace impl

} // namespace sedmgr