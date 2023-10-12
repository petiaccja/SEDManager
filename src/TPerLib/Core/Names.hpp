#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include "../Rpc/Types.hpp"


std::optional<std::string_view> GetName(Uid uid);
std::vector<Uid> GetUid(std::string_view name);