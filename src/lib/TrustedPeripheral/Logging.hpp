#pragma once

#include <Data/ComPacket.hpp>
#include <Data/Value.hpp>


void SetLogging(bool enabled);
void Log(std::string_view event, const ComPacket& request);
void Log(std::string_view event, const Value& request);