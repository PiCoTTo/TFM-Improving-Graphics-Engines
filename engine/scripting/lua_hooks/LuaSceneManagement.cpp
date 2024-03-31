#include "LuaSceneManagement.h"
#include "scene/SceneManager.h"

int nimo_luafn_SceneManagementLoadScene(lua_State* L)
{
    lua_settop(L, 1);
    std::string name = luaL_checkstring(L, 1);
    nimo::SceneManager::AddLoadRequest(name);
    return 0;
}
