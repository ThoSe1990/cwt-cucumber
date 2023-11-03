
set(target box-c)

add_executable(${target} 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/box.c
  ${CMAKE_CURRENT_SOURCE_DIR}/step_definition.c
)
target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${target} cucumber)