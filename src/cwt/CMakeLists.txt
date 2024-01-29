
set(cucumber-src 
  ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/compiler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/cucumber.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/feature.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/examples.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vm.cpp
)

add_library(cucumber STATIC ${cucumber-src})
add_library(cucumber-no-main STATIC ${cucumber-src})
