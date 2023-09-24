
set(target example)

add_library(cwt-cucumber-nomain STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/chunk.c
  ${CMAKE_CURRENT_SOURCE_DIR}/memory.c
  ${CMAKE_CURRENT_SOURCE_DIR}/debug.c
  ${CMAKE_CURRENT_SOURCE_DIR}/value.c
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler.c
  ${CMAKE_CURRENT_SOURCE_DIR}/scanner.c
  ${CMAKE_CURRENT_SOURCE_DIR}/object.c
  ${CMAKE_CURRENT_SOURCE_DIR}/table.c
  ${CMAKE_CURRENT_SOURCE_DIR}/vm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/step_matcher.c
  ${CMAKE_CURRENT_SOURCE_DIR}/cwt_cucumber.c
)

add_executable(${target} 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_link_libraries(${target} cwt-cucumber-nomain)