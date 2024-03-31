#pragma once
#include "lua.hpp"

int nimo_luafn_WindowMaximize(lua_State* L);
int nimo_luafn_WindowRestore(lua_State* L);
int nimo_luafn_WindowSetTitle(lua_State* L);
int nimo_luafn_WindowGetTitle(lua_State* L);
int nimo_luafn_WindowGetSize(lua_State* L);