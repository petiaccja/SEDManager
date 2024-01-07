#pragma once

#include <Messaging/Type.hpp>
#include <Messaging/Value.hpp>

#include <nlohmann/json.hpp>

#include <string>

namespace sedmgr {

std::string GetTypeStr(const Type& type);
nlohmann::json ValueToJSON(const Value& value, const Type& type, std::function<std::optional<std::string>(UID)> nameConverter = {});
Value JSONToValue(const nlohmann::json& value, const Type& type, std::function<std::optional<UID>(std::string_view)> nameConverter = {});

} // namespace sedmgr