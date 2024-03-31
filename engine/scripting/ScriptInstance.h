#pragma once
#include "Script.h"
#include "ScriptField.h"
#include "core/GUID.h"
#include <memory>
#include <map>
#include <string>

namespace nimo
{
class ScriptInstance
{
public:
    GUID owner;
    int stackReference;
    bool initialized = false;
    std::shared_ptr<Script> script;
    std::map<std::string, std::shared_ptr<ScriptField>> fields;
};
} // namespace nimo
