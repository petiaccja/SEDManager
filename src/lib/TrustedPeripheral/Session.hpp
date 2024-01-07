#pragma once

#include "MethodUtils.hpp"
#include "SessionManager.hpp"

#include <Specification/Opal/OpalModule.hpp>

#include <memory>


namespace sedmgr {

namespace impl {

    class Template {
    public:
        Template() = default;
        Template(std::shared_ptr<SessionManager> sessionManager,
                 uint32_t tperSessionNumber,
                 uint32_t hostSessionNumber);

    protected:
        const ModuleCollection& GetModules() const;
        CallContext GetCallContext(UID invokingId) const;

    protected:
        static constexpr UID THIS_SP = 0x0000'0000'0000'0001;

    private:
        std::shared_ptr<SessionManager> m_sessionManager = nullptr;
        uint32_t m_tperSessionNumber = 0;
        uint32_t m_hostSessionNumber = 0;
        static constexpr uint8_t PROTOCOL = 0x01;
    };


    class BaseTemplate : public Template {
    public:
        using Template::Template;


        asyncpp::task<std::vector<Value>> Get(UID object, uint32_t startColumn, uint32_t endColumn);
        asyncpp::task<Value> Get(UID object, uint32_t column);
        asyncpp::task<void> Set(UID object, std::span<const uint32_t> column, std::span<const Value> values);
        asyncpp::task<void> Set(UID object, uint32_t columns, const Value& value);
        asyncpp::task<std::vector<UID>> Next(UID table, std::optional<UID> row, uint32_t count);
        asyncpp::task<std::optional<UID>> Next(UID table, std::optional<UID> row);
        asyncpp::task<void> Authenticate(UID authority, std::optional<std::span<const std::byte>> proof);
        asyncpp::task<void> GenKey(UID object, std::optional<uint32_t> publicExponent = {}, std::optional<uint32_t> pinLength = {});

    private:
        static constexpr auto getMethod = Method<UID(core::eMethod::Get), 1, 0, 1, 0>{};
        static constexpr auto setMethod = Method<UID(core::eMethod::Set), 0, 2, 0, 0>{};
        static constexpr auto nextMethod = Method<UID(core::eMethod::Next), 0, 2, 1, 0>{};
        static constexpr auto authenticateMethod = Method<UID(core::eMethod::Authenticate), 1, 1, 1, 0>{};
        static constexpr auto genKeyMethod = Method<UID(core::eMethod::GenKey), 0, 2, 0, 0>{};
    };


    class OpalTemplate : public Template {
    public:
        using Template::Template;

        asyncpp::task<void> Revert(UID securityProvider);
        asyncpp::task<void> Activate(UID securityProvider);

    private:
        static constexpr auto revertMethod = Method<UID(opal::eMethod::Revert), 0, 0, 0, 0>{};
        static constexpr auto activateMethod = Method<UID(opal::eMethod::Activate), 0, 0, 0, 0>{};
    };


} // namespace impl


class Session {
public:
    Session(std::shared_ptr<SessionManager> sessionManager,
            UID securityProvider,
            std::optional<std::span<const std::byte>> password = {},
            std::optional<UID> authority = {});
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) noexcept = default;
    Session& operator=(Session&&) noexcept;
    ~Session();

    static asyncpp::task<Session> Start(std::shared_ptr<SessionManager> sessionManager,
                                        UID securityProvider,
                                        std::optional<std::span<const std::byte>> password = {},
                                        std::optional<UID> authority = {});
    asyncpp::task<void> End();
    uint32_t GetHostSessionNumber() const;
    uint32_t GetTPerSessionNumber() const;

private:
    Session(std::shared_ptr<SessionManager> sessionManager,
            uint32_t tperSessionNumber,
            uint32_t hostSessionNumber);
    static uint32_t NewHostSessionNumber();

public:
    impl::BaseTemplate base;
    impl::OpalTemplate opal;

private:
    std::shared_ptr<SessionManager> m_sessionManager;
    uint32_t m_tperSessionNumber;
    uint32_t m_hostSessionNumber;
};


} // namespace sedmgr