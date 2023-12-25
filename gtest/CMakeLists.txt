set(target unittests)

find_package(GTest)

if(GTest_FOUND)

  add_executable(${target} 
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner_english_keywords.cc
  )
  target_link_libraries(${target} cucumber-no-main GTest::gtest_main)

endif()