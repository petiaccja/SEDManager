#pragma once

#include "SessionManager.hpp"

#include <memory>


namespace impl {

class Template {
public:
    Template() = default;
    Template(std::shared_ptr<SessionManager> sessionManager,
             uint32_t tperSessionNumber,
             uint32_t hostSessionNumber);

protected:
    MethodResult InvokeMethod(Uid invokingId, const Method& method);

    template <class OutArgs = std::tuple<>, class... InArgs>
    OutArgs InvokeMethod(Uid invokingId, Uid methodId, const InArgs&... inArgs);

private:
    ComPacket CreatePacket(std::vector<uint8_t> payload);
    std::span<const uint8_t> UnwrapPacket(const ComPacket& packet);

private:
    std::shared_ptr<SessionManager> m_sessionManager = nullptr;
    uint32_t m_tperSessionNumber = 0;
    uint32_t m_hostSessionNumber = 0;
    static constexpr Uid THIS_SP_ID = 0x0000'0000'0000'0001;
    static constexpr uint8_t PROTOCOL = 0x01;
};


class BaseTemplate : public Template {
public:
    using Template::Template;

    template <class T>
    T Get(Uid table, Uid row, uint32_t column);
    template <class T>
    T Get(Uid object, uint32_t column);

    template <class T>
    void Set(Uid table, Uid row, uint32_t column, const T& value);
    template <class T>
    void Set(Uid object, uint32_t column, const T& value);

    void GenKey(Uid credentialObject, std::optional<uint32_t> publicExponent = {}, std::optional<uint32_t> pinLength = {});

private:
    std::unordered_map<uint32_t, Value> Get(Uid objectOrTable, const CellBlock& block);
    void Set(Uid objectOrTable, std::optional<Uid> row, std::optional<std::unordered_map<uint32_t, Value>> values);
};


class OpalTemplate : public Template {
public:
    using Template::Template;

    void Revert(Uid securityProvider);
    void Activate(Uid securityProvider);
};


} // namespace impl



class Session {
public:
    Session(std::shared_ptr<SessionManager> sessionManager,
            Uid securityProvider,
            std::optional<std::span<const std::byte>> password = {},
            std::optional<Uid> authority = {});
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) = default;
    Session& operator=(Session&&);
    ~Session();

    uint32_t GetHostSessionNumber() const;
    uint32_t GetTPerSessionNumber() const;

private:
    static uint32_t NewHostSessionNumber();
    void End();

public:
    impl::BaseTemplate base;
    impl::OpalTemplate opal;

private:
    std::shared_ptr<SessionManager> m_sessionManager;
    uint32_t m_tperSessionNumber;
    uint32_t m_hostSessionNumber;
};


namespace impl {

template <class OutArgs, class... InArgs>
OutArgs Template::InvokeMethod(Uid invokingId, Uid methodId, const InArgs&... inArgs) {
    std::vector<Value> args = ArgsToValues(inArgs...);
    const Method method{ .methodId = methodId, .args = std::move(args) };

    const MethodResult result = InvokeMethod(invokingId, method);

    OutArgs outArgs;
    try {
        std::apply([&result](auto&... outArgs) { ArgsFromValues(result.values, outArgs...); }, outArgs);
    }
    catch (std::exception& ex) {
        throw std::runtime_error(std::format("call to method (id={:#010x}) returned unexpected values: {}", uint64_t(methodId), ex.what()));
    }
    return outArgs;
}


template <class T>
T BaseTemplate::Get(Uid table, Uid row, uint32_t column) {
    CellBlock block{
        .startRow = uint64_t(row),
        .startColumn = column,
        .endColumn = column,
    };
    auto values = Get(table, block);
    if (values.size() < 1) {
        throw std::runtime_error("device did not return any table values");
    }
    T result;
    FromValue(values.begin()->second, result);
    return result;
}

template <class T>
T BaseTemplate::Get(Uid object, uint32_t column) {
    CellBlock block{
        .startColumn = column,
        .endColumn = column,
    };
    auto values = Get(object, block);
    if (values.size() < 1) {
        throw std::runtime_error("device did not return any table values");
    }
    T result;
    FromValue(values.begin()->second, result);
    return result;
}

template <class T>
void BaseTemplate::Set(Uid table, Uid row, uint32_t column, const T& value) {
    Set(table, row, { { column, ToValue(value) } });
}

template <class T>
void BaseTemplate::Set(Uid object, uint32_t column, const T& value) {
    Set(object, std::nullopt, std::unordered_map<uint32_t, Value>{ { column, ToValue(value) } });
}

} // namespace impl