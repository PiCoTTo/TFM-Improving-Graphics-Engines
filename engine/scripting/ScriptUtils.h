#pragma once

struct lua_State;
namespace nimo
{
namespace ScriptUtils
{
    void PrintLuaStack(lua_State* L);
    void PrintLuaTable(lua_State *L, int index);
    void PrintStackItem(lua_State *L, int i);
};
};