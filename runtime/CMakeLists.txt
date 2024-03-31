set(INCLUDES 
)
set(SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/RuntimeLayer.cpp
)
set(LIBS
nimo_engine
)

add_executable(nimo_runtime ${SOURCES})
set_target_properties(nimo_runtime PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(nimo_runtime PUBLIC ${INCLUDES})
target_link_libraries(nimo_runtime PUBLIC ${LIBS})