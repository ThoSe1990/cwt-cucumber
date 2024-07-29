
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.template ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp)


set(cucumber-src 
  ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/options.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/compiler.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/tags.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/cucumber.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/feature.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/background.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/examples.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tags.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/vm.cpp
)

add_library(cucumber ${cucumber-src} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(cucumber-no-main ${cucumber-src})

install(
	TARGETS cucumber
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)
install(
	TARGETS cucumber-no-main
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

if (BUILD_SHARED_LIBS)
    set_target_properties(cucumber PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
                          )
    set_target_properties(cucumber-no-main PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
                          )
endif()
