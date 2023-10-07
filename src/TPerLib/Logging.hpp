#pragma once

#include "Communication/Packet.hpp"
#include "Communication/Value.hpp"

#include <optional>


void SetLogging(bool enabled);
void Log(std::string_view event, const ComPacket& request);
void Log(std::string_view event, const Value& request);