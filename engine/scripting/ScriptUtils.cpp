#include "ScriptUtils.h"
#include "core/Log.h"
#include "lua.hpp"

void nimo::ScriptUtils::PrintLuaStack(lua_State* L)
{
    NIMO_DEBUG("Lua stack:");
    int top=lua_gettop(L);
    for (int i=1; i <= top; i++) {
        PrintStackItem(L,i);
    }
}
void nimo::ScriptUtils::PrintLuaTable(lua_State *L, int index)
{
    // Push another reference to the table on top of the stack (so we know
    // where it is, and this function can work for negative, positive and
    // pseudo indices
    lua_pushvalue(L, index);
    // stack now contains: -1 => table
    lua_pushnil(L);
    // stack now contains: -1 => nil; -2 => table
    while (lua_next(L, -2))
    {
        // stack now contains: -1 => value; -2 => key; -3 => table
        // copy the key so that lua_tostring does not modify the original
        lua_pushvalue(L, -2);
        // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
        // const char *key = lua_tostring(L, -1);
        // const char *value = lua_tostring(L, -2);
        NIMO_DEBUG("Table item key - value:");
        PrintStackItem(L, -1);
        PrintStackItem(L, -2);

        // pop value + copy of key, leaving original key
        lua_pop(L, 2);
        // stack now contains: -1 => key; -2 => table
    }
    // stack now contains: -1 => table (when lua_next returns 0 it pops the key
    // but does not push anything.)
    // Pop table
    lua_pop(L, 1);
    // Stack is now the same as it was on entry to this function
}

void nimo::ScriptUtils::PrintStackItem(lua_State *L, int i)
{
    switch (lua_type(L, i)) {
    case LUA_TNUMBER:
        NIMO_DEBUG("{}\t{}\t{}", i, luaL_typename(L,i), lua_tonumber(L,i));
        break;
    case LUA_TSTRING:
        NIMO_DEBUG("{}\t{}\t{}", i, luaL_typename(L,i), lua_tostring(L,i));
        break;
    case LUA_TBOOLEAN:
        NIMO_DEBUG("{}\t{}\t{}", i, luaL_typename(L,i), (lua_toboolean(L, i) ? "true" : "false"));
        break;
    case LUA_TFUNCTION:
        NIMO_DEBUG("{}\t{}\t{}", i, luaL_typename(L,i), "function");
        break;
    case LUA_TNIL:
        NIMO_DEBUG("{}\t{}\t{}", i, luaL_typename(L,i), "nil");
        break;
    case LUA_TTABLE:
        NIMO_DEBUG("{}\t{}\t{}", i, luaL_typename(L,i), lua_topointer(L,i));
        // PrintLuaTable(L, i);
        break;
    default:
        NIMO_DEBUG("{}\t{}\t{}", i, luaL_typename(L,i), lua_topointer(L,i));
        break;
    }
}

