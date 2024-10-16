
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.template ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp)


set(cucumber-src 
  ${CMAKE_CURRENT_SOURCE_DIR}/test_results.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/registry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cucumber.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/options.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tags.cpp
)

add_library(cucumber ${cucumber-src} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(cucumber-no-main ${cucumber-src} )

if (BUILD_SHARED_LIBS)
    set_target_properties(cucumber PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
    )
    set_target_properties(cucumber-no-main PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
    )
endif()

foreach( _target "cucumber" "cucumber-no-main")
    target_include_directories(
        ${_target}
        PUBLIC # where top-level project will look for the library's public headers
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            # where external projects will look for the library's public headers
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

endforeach()

include(CMakePackageConfigHelpers)

set(INCLUDE_INSTALL_DIR "include")
set(LIBRARY_INSTALL_DIR "lib")
set(LIBRARY_VERSION ${PROJECT_VERSION})
set(LIBRARY_SYSTEM_NAME "${CMAKE_HOST_SYSTEM_NAME}")

configure_package_config_file(
    cwtcucumber-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cwtcucumber-config.cmake
    INSTALL_DESTINATION lib/cwtcucumber/cmake
    PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR LIBRARY_VERSION LIBRARY_SYSTEM_NAME
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cwtcucumber-config-version.cmake
    VERSION ${LIBRARY_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    EXPORT "cwtcucumberTargets"
    FILE "cwtcucumberTargets${LIBRARY_SYSTEM_NAME}.cmake"
    NAMESPACE cwt::
    DESTINATION lib/cmake/cwtcucumber
    COMPONENT "cucumber"
)

install(
    TARGETS cucumber cucumber-no-main
    EXPORT cwtcucumberTargets
    DESTINATION "lib"
    COMPONENT "cucumber"
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION "include/cwtcucumber"
  COMPONENT cucumber
  FILES_MATCHING PATTERN "*.hpp"
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/cwtcucumber-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/cwtcucumber-config-version.cmake
    DESTINATION lib/cmake/cwtcucumber
    COMPONENT "cucumber"
)

add_library(cwt::cucumber ALIAS cucumber)
add_library(cwt::cucumber-no-main ALIAS cucumber-no-main)
