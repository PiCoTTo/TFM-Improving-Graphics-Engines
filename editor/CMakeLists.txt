set(INCLUDES 
)
set(SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/EditorLayer.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/EditorPanel.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/GameViewPanel.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/SceneViewPanel.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/SceneContentsPanel.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/InspectorPanel.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/AssetExplorerPanel.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/UIHelpers.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/Preferences.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/PreferencesController.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/PreferencesWindow.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/RendererDebugPanel.cpp 
)
set(LIBS
nimo_engine
nfd
)

add_executable(nimo_editor ${SOURCES})
set_target_properties(nimo_editor PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(nimo_editor PUBLIC ${INCLUDES})
target_link_libraries(nimo_editor PUBLIC ${LIBS})
add_custom_command(
    TARGET nimo_editor POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
    "${CMAKE_SOURCE_DIR}/third_party/assimp/v5.0.1/assimp-vc143-mt.dll"
    $<TARGET_FILE_DIR:nimo_editor>)
add_custom_command(
    TARGET nimo_editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res/ 
    $<TARGET_FILE_DIR:nimo_editor>)