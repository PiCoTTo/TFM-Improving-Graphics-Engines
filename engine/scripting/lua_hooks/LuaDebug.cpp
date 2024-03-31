#include "LuaDebug.h"
#include "core/Log.h"

int nimo_luafn_DebugLog(lua_State* L)
{
    std::string trace = lua_tostring(L, 1);
    NIMO_DEBUG("[LUA] {}", trace);
    return 0;
}
int nimo_luafn_WarningLog(lua_State* L)
{
    
    std::string trace = luaL_checkstring(L, 1);
    NIMO_WARN("[LUA] {}", trace);
    return 0;
}
int nimo_luafn_ErrorLog(lua_State* L)
{
    std::string trace = luaL_checkstring(L, 1);
    NIMO_ERROR("[LUA] {}", trace);
    return 0;
}