set(target fuzz-parser)

add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_parser.cpp)

target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${target} cucumber-no-main)

# -fsanitize=fuzzer provides main() (libFuzzer's driver) and the
# instrumentation feedback loop; address+undefined catch memory and UB
# bugs the fuzzer finds along the way. Clang-only: libFuzzer is not
# available with GCC.
target_compile_options(${target} PRIVATE
  -fsanitize=fuzzer,address,undefined -g -O1)
target_link_options(${target} PRIVATE
  -fsanitize=fuzzer,address,undefined)
