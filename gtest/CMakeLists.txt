set(target unittests)

find_package(GTest REQUIRED)

add_executable(${target} 
${CMAKE_CURRENT_SOURCE_DIR}/compile_errors.cc
${CMAKE_CURRENT_SOURCE_DIR}/runtime_errors.cc
${CMAKE_CURRENT_SOURCE_DIR}/runtime_tags.cc
${CMAKE_CURRENT_SOURCE_DIR}/run_scenarios.cc
${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cc
${CMAKE_CURRENT_SOURCE_DIR}/step_args.cc
${CMAKE_CURRENT_SOURCE_DIR}/tags.cc
${CMAKE_CURRENT_SOURCE_DIR}/hooks.cc
${CMAKE_CURRENT_SOURCE_DIR}/background.cc
${CMAKE_CURRENT_SOURCE_DIR}/cpp_bindings.cc
${CMAKE_CURRENT_SOURCE_DIR}/cpp_scenario_context.cc
)
target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/cwt/implementation)
target_link_libraries(${target} cwt-cucumber GTest::gtest_main)