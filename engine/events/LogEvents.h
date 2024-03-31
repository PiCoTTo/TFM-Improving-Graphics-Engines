#pragma once

#include "Event.h"
#include "core/Log.h"
#include <string>

namespace nimo{
struct LogEvent: public Event
{
    LogEvent(Log::Level level, const std::string& msg) : level(level), msg(msg){}
    Log::Level level;
    std::string msg;
};
};