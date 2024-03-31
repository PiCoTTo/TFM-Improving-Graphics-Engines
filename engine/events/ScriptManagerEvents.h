#pragma once

#include "Event.h"
#include "scripting/Script.h"
#include <memory>

namespace nimo{
struct ScriptInstanceCreatedEvent: public Event
{
    ScriptInstanceCreatedEvent(GUID owner, std::shared_ptr<Script> script) : owner(owner), script(script){}
    GUID owner;
    std::shared_ptr<Script> script;
};
struct ScriptInstanceDestroyedEvent: public Event
{
    ScriptInstanceDestroyedEvent(GUID owner, std::shared_ptr<Script> script) : owner(owner), script(script){}
    GUID owner;
    std::shared_ptr<Script> script;
};
};