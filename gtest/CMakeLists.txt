set(target unittests)

find_package(GTest)

if(GTest_FOUND)

  add_executable(${target} 
    ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_feature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_scenario.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_scenario_outline.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_scenario_outline_w_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_scenario_outline_w_tags_2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_scenario_outline_w_examples_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_scenario_outline_w_examples_tags_2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_scenario_outline_and_scenario_w_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_ignore_descriptions.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_tagged_feature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks_with_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks_scenario_outline.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/util_filepath.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/value.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm_check_results.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm_asserts.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm_get_args.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_set_langauge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_english.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_german.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scenario_outline.cc
  )
  target_link_libraries(${target} cucumber-no-main GTest::gtest_main)

endif()