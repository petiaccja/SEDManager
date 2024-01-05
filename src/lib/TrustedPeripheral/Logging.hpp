#pragma once

#include <Messaging/ComPacket.hpp>
#include <Messaging/Value.hpp>


namespace sedmgr {

void SetLogging(bool enabled);
void Log(std::string_view event, const ComPacket& request);
void Log(std::string_view event, const Value& request);

} // namespace sedmgr