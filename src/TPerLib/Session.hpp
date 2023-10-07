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
    MethodResult InvokeMethod(const Method& method);

    template <class OutArgs, class... InArgs>
    OutArgs InvokeMethod(Uid methodId, const InArgs&... inArgs);

private:
    ComPacket CreatePacket(std::vector<uint8_t> payload);
    std::span<const uint8_t> UnwrapPacket(const ComPacket& packet);

private:
    std::shared_ptr<SessionManager> m_sessionManager;
    uint32_t m_tperSessionNumber;
    uint32_t m_hostSessionNumber;
    static constexpr Uid INVOKING_ID = 0x0000'0000'0000'0001;
    static constexpr uint8_t PROTOCOL = 0x01;
};


template <class OutArgs, class... InArgs>
OutArgs Template::InvokeMethod(Uid methodId, const InArgs&... inArgs) {
    std::vector<TokenStream> args = SerializeArgs(inArgs...);
    const Method method{ .methodId = methodId, .args = std::move(args) };

    const MethodResult result = InvokeMethod(method);

    OutArgs outArgs;
    try {
        std::apply([&result](auto&... outArgs) { ParseArgs(result.values, outArgs...); }, outArgs);
    }
    catch (std::exception& ex) {
        throw std::runtime_error(std::format("call to method (id={:#010x}) returned unexpected values: {}", uint64_t(methodId), ex.what()));
    }
    return outArgs;
}


class BaseTemplate : public Template {
public:
    using Template::Template;

    void GetTableValue(Uid row, uint32_t column);
};


} // namespace impl



class Session {
public:
    Session(std::shared_ptr<SessionManager> sessionManager,
            Uid securityProvider,
            std::optional<std::span<const std::byte>> password = {},
            std::optional<Uid> authority = {});
    Session(const Session&) = delete;
    Session(Session&&) = delete;
    Session& operator=(const Session&) = delete;
    Session& operator=(Session&&) = delete;
    ~Session();

    uint32_t GetHostSessionNumber() const;
    uint32_t GetTPerSessionNumber() const;

private:
    static uint32_t NewHostSessionNumber();

private:
    std::shared_ptr<SessionManager> m_sessionManager;
    uint32_t m_tperSessionNumber;
    uint32_t m_hostSessionNumber;
};