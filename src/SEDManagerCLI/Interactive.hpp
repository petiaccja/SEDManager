#pragma once

#include <CLI/CLI.hpp>
#include <SEDManager/SEDManager.hpp>


namespace interactive {

void AddCommands(SEDManager& app, CLI::App& cli);
std::optional<Uid> GetCurrentSP();
std::span<const Uid> GetCurrentAuthorities();

} // namespace interactive