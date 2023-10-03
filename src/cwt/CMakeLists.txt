
set(target cwt-cucumber)

add_library(${target} STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/chunk.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/memory.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/debug.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/value.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/compiler.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/scanner.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/object.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/table.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/vm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/step_matcher.c
  ${CMAKE_CURRENT_SOURCE_DIR}/implementation/cucumber.c
)

if (STACK_TRACE)
  target_compile_definitions(${target} PUBLIC STACK_TRACE)
endif()