#pragma once

#include "MethodUtils.hpp"
#include "SessionManager.hpp"

#include <memory>

#include <Specification/Opal/OpalModule.hpp>


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
        CallContext GetCallContext(Uid invokingId) const;

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


        asyncpp::task<std::vector<Value>> Get(Uid object, uint32_t startColumn, uint32_t endColumn);
        asyncpp::task<Value> Get(Uid object, uint32_t column);
        asyncpp::task<void> Set(Uid object, std::span<const uint32_t> column, std::span<const Value> values);
        asyncpp::task<void> Set(Uid object, uint32_t columns, const Value& value);
        asyncpp::task<std::vector<Uid>> Next(Uid table, std::optional<Uid> row, uint32_t count);
        asyncpp::task<std::optional<Uid>> Next(Uid table, std::optional<Uid> row);
        asyncpp::task<void> Authenticate(Uid authority, std::optional<std::span<const std::byte>> proof);
        asyncpp::task<void> GenKey(Uid object, std::optional<uint32_t> publicExponent = {}, std::optional<uint32_t> pinLength = {});

    private:
        static constexpr auto getMethod = Method<Uid(core::eMethod::Get), 1, 0, 1, 0>{};
        static constexpr auto setMethod = Method<Uid(core::eMethod::Set), 0, 2, 0, 0>{};
        static constexpr auto nextMethod = Method<Uid(core::eMethod::Next), 0, 2, 1, 0>{};
        static constexpr auto authenticateMethod = Method<Uid(core::eMethod::Authenticate), 1, 1, 1, 0>{};
        static constexpr auto genKeyMethod = Method<Uid(core::eMethod::GenKey), 0, 2, 0, 0>{};
    };


    class OpalTemplate : public Template {
    public:
        using Template::Template;

        asyncpp::task<void> Revert(Uid securityProvider);
        asyncpp::task<void> Activate(Uid securityProvider);

    private:
        static constexpr auto revertMethod = Method<Uid(opal::eMethod::Revert), 0, 0, 0, 0>{};
        static constexpr auto activateMethod = Method<Uid(opal::eMethod::Activate), 0, 0, 0, 0>{};
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
    Session(Session&&) noexcept = default;
    Session& operator=(Session&&) noexcept;
    ~Session();

    static asyncpp::task<Session> Start(std::shared_ptr<SessionManager> sessionManager,
                                        Uid securityProvider,
                                        std::optional<std::span<const std::byte>> password = {},
                                        std::optional<Uid> authority = {});
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