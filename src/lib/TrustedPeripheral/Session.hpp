#pragma once

#include "Method.hpp"
#include "SessionManager.hpp"

#include <Error/Exception.hpp>

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

    const TPerModules& GetModules() const;

private:
    ComPacket CreatePacket(std::vector<std::byte> payload);
    std::span<const std::byte> UnwrapPacket(const ComPacket& packet);

protected:
    static constexpr Uid THIS_SP = 0x0000'0000'0000'0001;

private:
    std::shared_ptr<SessionManager> m_sessionManager = nullptr;
    uint32_t m_tperSessionNumber = 0;
    uint32_t m_hostSessionNumber = 0;
    static constexpr uint8_t PROTOCOL = 0x01;
};


class BaseTemplate : public Template {
public:
    using Template::Template;


    std::vector<Value> Get(Uid object, uint32_t startColumn, uint32_t endColumn);
    Value Get(Uid object, uint32_t column);
    void Set(Uid object, std::span<const uint32_t> column, std::span<const Value> values);
    void Set(Uid object, uint32_t columns, const Value& value);
    std::vector<Uid> Next(Uid table, std::optional<Uid> row, uint32_t count);
    std::optional<Uid> Next(Uid table, std::optional<Uid> row);

    void Authenticate(Uid authority, std::optional<std::span<const std::byte>> proof);

    void GenKey(Uid object, std::optional<uint32_t> publicExponent = {}, std::optional<uint32_t> pinLength = {});
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
        throw InvalidResponseError(GetModules().FindName(methodId).value_or(to_string(methodId)), ex.what());
    }
    return outArgs;
}


} // namespace impl