
set(target box)

add_executable(${target} 
  ${CMAKE_CURRENT_SOURCE_DIR}/step_definition.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/hooks.cpp
)
install(
	TARGETS ${target}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${target} cucumber)