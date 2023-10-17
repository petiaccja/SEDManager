#pragma once

#include <RPC/Value.hpp>
#include <Specification/Type.hpp>

#include <nlohmann/json.hpp>

std::string GetTypeStr(const Type& type);
nlohmann::json ValueToJSON(const Value& value, const Type& type);
Value JSONToValue(const nlohmann::json& value, const Type& type);