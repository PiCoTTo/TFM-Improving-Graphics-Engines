#include "LuaWindow.h"
#include "core/Application.h"

int nimo_luafn_WindowMaximize(lua_State* L)
{
    lua_settop(L, 0);
    nimo::Application::Instance().GetWindow().Maximize();
    return 0;
}
int nimo_luafn_WindowRestore(lua_State* L)
{
    lua_settop(L, 0);
    nimo::Application::Instance().GetWindow().Restore();
    return 0;
}
int nimo_luafn_WindowSetTitle(lua_State* L)
{
    lua_settop(L, 1);
    std::string title = luaL_checkstring(L, 1);
    nimo::Application::Instance().GetWindow().SetTitle(title);
    return 0;
}
int nimo_luafn_WindowGetTitle(lua_State* L)
{
    lua_settop(L, 0);
    lua_pushstring(L, nimo::Application::Instance().GetWindow().GetTitle().c_str());
    return 1;
}
int nimo_luafn_WindowGetSize(lua_State* L)
{
    lua_settop(L, 0);
    lua_pushinteger(L, nimo::Application::Instance().GetWindow().GetWidth());
    lua_pushinteger(L, nimo::Application::Instance().GetWindow().GetHeight());
    return 2;
}