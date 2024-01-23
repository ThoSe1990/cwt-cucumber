set(target unittests)

find_package(GTest)

if(GTest_FOUND)

  add_executable(${target} 
    ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_feature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/value.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vm.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_set_langauge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_english.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_german.cc
  )
  target_link_libraries(${target} cucumber-no-main GTest::gtest_main)

endif()