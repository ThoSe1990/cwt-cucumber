set(target unittests)

find_package(GTest)

if(GTest_FOUND)
 
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/paths.template ${CMAKE_CURRENT_SOURCE_DIR}/paths.hpp)

  add_executable(${target} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ast.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ast_visitor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/file_io.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/help_screen.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/options.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/run_scenarios.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/run_scenario_hooks.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/registry.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_set_langauge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_english.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_german.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_spanish.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stdout_print.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/table.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/terminal_prints.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/test_results.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/util_filepath.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/value.cc
  )

  target_link_libraries(${target} cucumber-no-main GTest::gtest_main)
  install(
	TARGETS ${target}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
  )

endif()
