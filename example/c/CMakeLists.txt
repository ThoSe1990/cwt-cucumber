
set(target example-c)

add_executable(${target} 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${target} cwt-cucumber)