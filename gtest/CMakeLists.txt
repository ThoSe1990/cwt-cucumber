set(target unittests)

find_package(GTest)

if(GTest_FOUND)

  set(FEATURES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cpp_test_features")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/paths.hpp.template ${CMAKE_CURRENT_SOURCE_DIR}/paths.hpp)

  add_executable(${target} 
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_errors.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/syntax_errors.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/run_scenarios.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/run_single_scenario.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/step_args.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp_bindings.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp_scenario_context.cc
  )
  target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/cwt/implementation)
  target_link_libraries(${target} cucumber-cpp-nomain GTest::gtest_main)

endif()