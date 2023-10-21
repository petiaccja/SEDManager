#pragma once

#include <Data/Type.hpp>
#include <Data/Value.hpp>

#include <nlohmann/json.hpp>

#include <string>


std::string GetTypeStr(const Type& type);
nlohmann::json ValueToJSON(const Value& value, const Type& type);
Value JSONToValue(const nlohmann::json& value, const Type& type);