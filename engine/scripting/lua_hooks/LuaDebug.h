#pragma once
#include "lua.hpp"

int nimo_luafn_DebugLog(lua_State* L);
int nimo_luafn_WarningLog(lua_State* L);
int nimo_luafn_ErrorLog(lua_State* L);