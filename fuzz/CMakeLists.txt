set(target fuzz-parser)

add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_parser.cpp)

target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# -fsanitize=fuzzer provides main() (libFuzzer's driver) and the
# instrumentation feedback loop; address+undefined catch memory and UB
# bugs the fuzzer finds along the way. Clang-only: libFuzzer is not
# available with GCC.
#
# Crucially, this must be built into the library code itself, not just
# the harness: link against the plain cucumber-no-main and the fuzzer
# would have zero coverage instrumentation inside the scanner/lexer/
# parser, so it could never tell when a mutation reached deeper code
# and would get stuck exploring only the first few bytes forever. So we
# compile a private, sanitizer-instrumented copy of the library sources
# scoped to this target only (cucumber-no-main itself stays unsanitized
# for gtest/benchmarks).
add_library(cucumber-fuzz-instrumented STATIC ${cucumber-src})
target_include_directories(cucumber-fuzz-instrumented PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_options(cucumber-fuzz-instrumented PUBLIC
  -fsanitize=fuzzer-no-link,address,undefined -g -O1)
target_link_options(cucumber-fuzz-instrumented PUBLIC
  -fsanitize=address,undefined)
if (nlohmann_json_FOUND)
  target_link_libraries(cucumber-fuzz-instrumented PUBLIC nlohmann_json::nlohmann_json)
  target_compile_definitions(cucumber-fuzz-instrumented PUBLIC WITH_JSON)
endif()

target_link_libraries(${target} cucumber-fuzz-instrumented)
target_compile_options(${target} PRIVATE
  -fsanitize=fuzzer,address,undefined -g -O1)
target_link_options(${target} PRIVATE
  -fsanitize=fuzzer,address,undefined)
