#pragma once

#include <Data/NativeTypes.hpp>

#include <SEDManager/SEDManager.hpp>

#include <bit>
#include <string_view>
#include <vector>


std::vector<std::byte> GetPassword(std::string_view prompt);

std::optional<Uid> FindOrParseUid(SEDManager& app, std::string_view nameOrUid);

std::vector<std::string_view> SplitName(std::string_view name);

std::string FormatTable(std::span<const std::string> columns, std::span<const std::vector<std::string>> rows);