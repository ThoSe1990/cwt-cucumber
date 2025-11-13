cmake_minimum_required(VERSION 3.16)
project(cwt-cucumber VERSION 2.7)

set(version_content "version = \"${PROJECT_VERSION}\"\n")
file(WRITE "${CMAKE_SOURCE_DIR}/docs/_version.py" "${version_content}")

option(CUCUMBER_UNDEFINED_STEPS_ARE_A_FAILURE "The overall result will fail if the tests contain undefined steps" ON)
if(CUCUMBER_UNDEFINED_STEPS_ARE_A_FAILURE)
    add_definitions(-DUNDEFINED_STEPS_ARE_A_FAILURE)
endif()

set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)

add_subdirectory(${PROJECT_SOURCE_DIR}/gtest)
add_subdirectory(${PROJECT_SOURCE_DIR}/stress-tests)
