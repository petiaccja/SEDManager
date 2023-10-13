#pragma once

#include <RPC/Packet.hpp>
#include <RPC/Value.hpp>

#include <optional>


void SetLogging(bool enabled);
void Log(std::string_view event, const ComPacket& request);
void Log(std::string_view event, const Value& request);