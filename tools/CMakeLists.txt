set(INCLUDES 
${CMAKE_CURRENT_SOURCE_DIR}
)
set(SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/nimo_scene_multiplier.cpp 
)
set(LIBS
nimo_engine
)

add_executable(nimo_scene_multiplier ${SOURCES})
set_target_properties(nimo_scene_multiplier PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(nimo_scene_multiplier PUBLIC ${INCLUDES})
target_link_libraries(nimo_scene_multiplier PUBLIC ${LIBS})