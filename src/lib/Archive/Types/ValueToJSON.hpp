#pragma once

#include <Data/Type.hpp>
#include <Data/Value.hpp>

#include <nlohmann/json.hpp>

#include <string>

namespace sedmgr {

std::string GetTypeStr(const Type& type);
nlohmann::json ValueToJSON(const Value& value, const Type& type, std::function<std::optional<std::string>(Uid)> nameConverter = {});
Value JSONToValue(const nlohmann::json& value, const Type& type, std::function<std::optional<Uid>(std::string_view)> nameConverter = {});

} // namespace sedmgr