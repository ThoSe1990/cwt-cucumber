
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.template ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp)

find_package(nlohmann_json)


set(cucumber-src 
  ${CMAKE_CURRENT_SOURCE_DIR}/ast.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_results.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/registry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/catalog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cucumber.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/options.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/report.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/step.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tags.cpp
)

add_library(cucumber ${cucumber-src} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(cucumber-no-main ${cucumber-src} )

if (nlohmann_json_FOUND)
    target_link_libraries(cucumber nlohmann_json::nlohmann_json)
    target_link_libraries(cucumber-no-main nlohmann_json::nlohmann_json)
    add_compile_definitions(WITH_JSON)
endif()

if (BUILD_SHARED_LIBS)
    set_target_properties(cucumber PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
    )
    set_target_properties(cucumber-no-main PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
    )
endif()

include(GNUInstallDirs)

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
    cwt-cucumber-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cwt-cucumber-config.cmake
    INSTALL_DESTINATION lib/cwt-cucumber/cmake
    PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR LIBRARY_VERSION LIBRARY_SYSTEM_NAME
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cwt-cucumber-config-version.cmake
    VERSION ${LIBRARY_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    EXPORT "cwt-cucumber-Targets"
    FILE "cwt-cucumber-Targets${LIBRARY_SYSTEM_NAME}.cmake"
    NAMESPACE cwt::
    DESTINATION lib/cmake/cwt-cucumber
    COMPONENT "cucumber"
)

install(
    TARGETS cucumber cucumber-no-main
    EXPORT cwt-cucumber-Targets
    DESTINATION "lib"
    COMPONENT "cucumber"
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION "include/cwt-cucumber"
  COMPONENT cucumber
  FILES_MATCHING PATTERN "*.hpp"
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/cwt-cucumber-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/cwt-cucumber-config-version.cmake
    DESTINATION lib/cmake/cwt-cucumber
    COMPONENT "cucumber"
)

add_library(cwt::cucumber ALIAS cucumber)
add_library(cwt::cucumber-no-main ALIAS cucumber-no-main)
