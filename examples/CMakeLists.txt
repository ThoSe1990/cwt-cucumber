
set(target box)

add_executable(${target} 
  ${CMAKE_CURRENT_SOURCE_DIR}/step_definition.cpp
)

target_link_libraries(${target} cucumber)