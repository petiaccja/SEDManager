#pragma once

#include "NativeTypes.hpp"

#include <optional>
#include <string_view>


std::optional<std::string_view> GetName(Uid uid);
std::string GetNameOrUid(Uid uid);
std::optional<Uid> GetUid(std::string_view name);
std::optional<Uid> GetUidOrHex(std::string_view name);
