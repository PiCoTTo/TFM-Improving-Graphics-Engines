cmake_minimum_required(VERSION 3.1...3.22)

project(nimo VERSION 1.0)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/third_party/glad/include)

add_library(glfw INTERFACE IMPORTED GLOBAL)
target_include_directories(glfw INTERFACE ${CMAKE_SOURCE_DIR}/third_party/glfw/v3.3.7/include)
target_link_libraries(glfw INTERFACE ${CMAKE_SOURCE_DIR}/third_party/glfw/v3.3.7/lib-vc2022/glfw3.lib)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/third_party/glm/v0.9.9)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_SOURCE_DIR}/third_party/stb)

add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${CMAKE_SOURCE_DIR}/third_party/assimp/v5.0.1/include)
target_link_libraries(assimp INTERFACE ${CMAKE_SOURCE_DIR}/third_party/assimp/v5.0.1/assimp-vc143-mt.lib)

add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_SOURCE_DIR}/third_party/nlohmann_json/v3.11.2/)

set(ImGUISources
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/imgui.cpp
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/imgui_tables.cpp
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/imgui_draw.cpp
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/imgui_widgets.cpp
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/imgui_demo.cpp
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/backends/imgui_impl_glfw.cpp
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/backends/imgui_impl_opengl3.cpp
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/misc/cpp/imgui_stdlib.cpp
)
set(ImGUIIncludes
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking
${CMAKE_SOURCE_DIR}/third_party/imgui/imgui-docking/backends
${CMAKE_SOURCE_DIR}/third_party/glfw/v3.3.7/include
)
add_library(imgui ${ImGUISources})
target_include_directories(imgui PUBLIC ${ImGUIIncludes})

add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${CMAKE_SOURCE_DIR}/third_party/entt/v3.10.3/single_include)

set(NFDSources
${CMAKE_SOURCE_DIR}/third_party/nfd/v1.1.6/nfd_common.c
${CMAKE_SOURCE_DIR}/third_party/nfd/v1.1.6/nfd_win.cpp
)
set(NFDIncludes
${CMAKE_SOURCE_DIR}/third_party/nfd/v1.1.6/include
)
add_library(nfd ${NFDSources})
target_include_directories(nfd PUBLIC ${NFDIncludes})

add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE ${CMAKE_SOURCE_DIR}/third_party/spdlog/v1.11.0/include)

set(LUA_LIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lcode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lctype.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/ldebug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/ldo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/ldump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lfunc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lgc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/llex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lobject.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lopcodes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lparser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lstate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lstring.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/ltable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/ltm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lundump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lvm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lzio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lauxlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lbaselib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lcorolib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/ldblib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/liolib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lmathlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/loadlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/loslib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lstrlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/ltablib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/lutf8lib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src/linit.c
)
add_library(lua STATIC ${LUA_LIB_SRCS})
target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lua/v5.4.4/src)

add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE ${CMAKE_SOURCE_DIR}/third_party/miniaudio/)

add_library(freetype2 INTERFACE)
target_include_directories(freetype2 INTERFACE ${CMAKE_SOURCE_DIR}/third_party/freetype2/v2.13.0/include)
target_link_libraries(assimp INTERFACE ${CMAKE_SOURCE_DIR}/third_party/freetype2/v2.13.0/static/vs2015-2022/win64/freetype.lib)

add_library(fsr2 INTERFACE)
target_include_directories(fsr2 INTERFACE ${CMAKE_SOURCE_DIR}/third_party/fsr2/include)
# target_link_libraries(fsr2 INTERFACE ${CMAKE_SOURCE_DIR}/third_party/fsr2/ffx_fsr2_api_gl_x64.lib)
target_link_libraries(fsr2 INTERFACE optimized ${CMAKE_SOURCE_DIR}/third_party/fsr2/ffx_fsr2_api_x64.lib)
target_link_libraries(fsr2 INTERFACE optimized ${CMAKE_SOURCE_DIR}/third_party/fsr2/ffx_fsr2_api_gl_x64.lib)
target_link_libraries(fsr2 INTERFACE debug ${CMAKE_SOURCE_DIR}/third_party/fsr2/ffx_fsr2_api_x64d.lib)
target_link_libraries(fsr2 INTERFACE debug ${CMAKE_SOURCE_DIR}/third_party/fsr2/ffx_fsr2_api_gl_x64d.lib)

add_subdirectory(${CMAKE_SOURCE_DIR}/engine)
add_subdirectory(${CMAKE_SOURCE_DIR}/editor)
add_subdirectory(${CMAKE_SOURCE_DIR}/runtime)
add_subdirectory(${CMAKE_SOURCE_DIR}/tools)