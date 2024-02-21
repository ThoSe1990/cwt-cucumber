
set(cucumber-src 
  ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/options.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/compiler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/tags.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/cucumber.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/feature.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/background.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/examples.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vm.cpp
)

add_library(cucumber ${cucumber-src} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(cucumber-no-main ${cucumber-src})

if (BUILD_SHARED_LIBS)
    set_target_properties(cucumber PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
                          )
    set_target_properties(cucumber-no-main PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
                          )
endif()