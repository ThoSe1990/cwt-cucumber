set(target unittests)

find_package(GTest)

if(GTest_FOUND)

  add_executable(${target} 
    ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/help_screen.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/report.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scenario_outline.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/table.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/util_filepath.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/value.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/compiler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/feature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/quiet.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_w_background.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline_w_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline_w_tags_2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline_w_examples_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline_w_examples_tags_2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline_and_scenario_w_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/ignore_descriptions.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/scenario_outline_strings.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/tables.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/tagged_feature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler/single_lines.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks/hooks.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks/hooks_with_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks/hooks_scenario_outline.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner/scanner.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner/set_langauge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner/english.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner/german.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner/spanish.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/vm.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/check_results.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/asserts.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/get_args.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/options_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/options_quiet.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/options_line.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/run_scenarios.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/run_w_background.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/tables.cc
  )
  target_link_libraries(${target} cucumber-no-main GTest::gtest_main)

endif()