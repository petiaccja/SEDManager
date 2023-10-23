#pragma once

#include <CLI/CLI.hpp>
#include <SEDManager/SEDManager.hpp>

#include <optional>
#include <unordered_set>


class Interactive {
public:
    Interactive(SEDManager& manager);
    Interactive(const Interactive&) = delete;
    Interactive(Interactive&&) = delete;
    Interactive& operator=(const Interactive&) = delete;
    Interactive& operator=(Interactive&&) = delete;

    std::optional<Uid> GetCurrentSecurityProvider() const;
    std::unordered_set<Uid> GetCurrentAuthorities() const;
    int Run();

private:
    void RegisterCallbackExit();
    void RegisterCallbackHelp();

    void RegisterCallbackStart();
    void RegisterCallbackAuthenticate();
    void RegisterCallbackEnd();

    void RegisterCallbackInfo();
    void RegisterCallbackFind();
    void RegisterCallbackRows();
    void RegisterCallbackColumns();
    void RegisterCallbackGet();
    void RegisterCallbackSet();
    void RegisterCallbackPasswd();
    void RegisterCallbackGenMEK();
    void RegisterCallbackGenPIN();
    void RegisterCallbackActivate();
    void RegisterCallbackRevert();

    void RegisterCallbackStackReset();
    void RegisterCallbackReset();

    void ClearCurrents();
    auto ParseGetSet(std::string rowName, int32_t column) const -> std::optional<std::tuple<Uid, Uid, int32_t>>;
    void PrintCaret() const;
    void PrintHelp(const std::string& command) const;

private:
    SEDManager& m_manager;
    CLI::App m_cli;
    std::optional<Uid> m_currentSecurityProvider;
    std::unordered_set<Uid> m_currentAuthorities;
    bool m_finished = false;
};