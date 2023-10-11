set(target unittests)

find_package(GTest REQUIRED)

add_executable(${target} 
  ${CMAKE_CURRENT_SOURCE_DIR}/syntax_errors.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/step_finder.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/step_args.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/tags.cc
)
target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src/cwt ${PROJECT_SOURCE_DIR}/src/cwt/implementation)
target_link_libraries(${target} cwt-cucumber GTest::gtest_main)