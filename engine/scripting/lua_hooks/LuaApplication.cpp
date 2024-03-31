#include "LuaApplication.h"
#include "core/Application.h"

int nimo_luafn_ApplicationClose(lua_State* L)
{
    // discard any extra arguments passed in
    lua_settop(L, 0);
    nimo::Application::Instance().Close();
    return 0;
}
