set(INCLUDES 
${CMAKE_CURRENT_SOURCE_DIR}
)
set(SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/core/Application.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/core/Entry.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/core/Window.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/core/GUID.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/core/Log.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/core/FileHandling.cpp
${CMAKE_CURRENT_SOURCE_DIR}/core/ExportedVariablesManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/assets/AssetIndex.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/assets/AssetManager.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/assets/AssetSerializer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/assets/AssetSettingsSerializer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/assets/AssetFileExtensions.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/stb_image.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/Renderer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/Context.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/Shader.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/IndexBuffer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/VertexArray.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/VertexBuffer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/FrameBuffer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/Texture.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/EnvironmentMap.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/Mesh.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/SceneRenderer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/DebugPass.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/DeferredPass.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/renderer/ForwardPass.cpp
${CMAKE_CURRENT_SOURCE_DIR}/renderer/PostprocessPass.cpp
${CMAKE_CURRENT_SOURCE_DIR}/renderer/GuiPass.cpp
${CMAKE_CURRENT_SOURCE_DIR}/scene/Scene.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scene/SceneSerializer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scene/SceneManager.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scene/Entity.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scene/Prefab.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scene/Components.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/project/Project.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/project/ProjectSerializer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/input/Input.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/ScriptManager.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/ScriptUtils.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/Script.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/lua_hooks/LuaDebug.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/lua_hooks/LuaInput.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/lua_hooks/LuaComponents.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/lua_hooks/LuaApplication.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/lua_hooks/LuaWindow.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/scripting/lua_hooks/LuaSceneManagement.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/audio/miniaudio.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/audio/AudioEngine.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/audio/AudioSource.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/audio/AudioSound.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/fonts/Font.cpp 
)
set(LIBS
glad
glfw
glm
stb
assimp
json
imgui
entt
spdlog
lua
miniaudio
freetype2
fsr2
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MT")
add_library(nimo_engine STATIC ${SOURCES})
target_include_directories(nimo_engine PUBLIC ${INCLUDES})
target_link_libraries(nimo_engine PUBLIC ${LIBS})