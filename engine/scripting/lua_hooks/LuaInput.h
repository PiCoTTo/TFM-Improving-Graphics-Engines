#pragma once
#include "lua.hpp"

int nimo_luafn_InputGetKey(lua_State* L);
int nimo_luafn_InputGetKeyPressed(lua_State* L);
int nimo_luafn_InputGetKeyReleased(lua_State* L);
int nimo_luafn_InputGetMouseButton(lua_State* L);
int nimo_luafn_InputGetMouseButtonPressed(lua_State* L);
int nimo_luafn_InputGetMouseButtonReleased(lua_State* L);
int nimo_luafn_InputGetMousePosition(lua_State* L);
int nimo_luafn_InputGetMouseScroll(lua_State* L);