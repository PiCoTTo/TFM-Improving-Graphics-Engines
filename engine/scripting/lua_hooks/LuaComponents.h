#pragma once
#include "lua.hpp"

int nimo_luafn_HasEntityComponent(lua_State* L);
int nimo_luafn_GetEntityComponent(lua_State* L);
int nimo_luafn_SetEntityComponent(lua_State* L);
int nimo_luafn_EntityDestroy(lua_State* L);
int nimo_luafn_EntityInstantiate(lua_State* L);
int nimo_luafn_EntitySetActive(lua_State* L);
int nimo_luafn_EntityGetForward(lua_State* L);
int nimo_luafn_EntityGetRight(lua_State* L);
int nimo_luafn_EntityGetUp(lua_State* L);