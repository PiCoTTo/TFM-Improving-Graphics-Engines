set(INCLUDES
)
set(SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/RuntimeLayer.cpp
)
set(LIBS
nimo_engine
)
# Custom property for development.
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}../../../Nimo Projects/t>")

add_executable(nimo_runtime ${SOURCES})
set_target_properties(nimo_runtime PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(nimo_runtime PUBLIC ${INCLUDES})
target_link_libraries(nimo_runtime PUBLIC ${LIBS})
add_custom_command(
    TARGET nimo_runtime POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
    "${CMAKE_SOURCE_DIR}/third_party/assimp/v5.0.1/assimp-vc143-mt.dll"
    $<TARGET_FILE_DIR:nimo_runtime>)
#add_custom_command(
#    TARGET nimo_runtime POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${CMAKE_SOURCE_DIR}/res/ 
#    $<TARGET_FILE_DIR:nimo_runtime>)
add_custom_command(
    TARGET nimo_runtime POST_BUILD
    COMMAND xcopy
    ${CMAKE_SOURCE_DIR}/res 
    $<TARGET_FILE_DIR:nimo_runtime>
	/i /e /d /y)
	
# Copy only the modified shaders to the Assets directory of the project used for testing
add_custom_command(
    TARGET nimo_runtime POST_BUILD
    COMMAND xcopy
    ${CMAKE_SOURCE_DIR}/res/NewProjectAssets/Shaders 
    $<TARGET_FILE_DIR:nimo_runtime>/Assets/Shaders
	/i /d /y)