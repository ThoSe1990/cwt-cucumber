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
# compile a private, sanitizer-instrumented copy of the library sources,
# shared by every fuzz target below (cucumber-no-main itself stays
# unsanitized for gtest/benchmarks).
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

# add_fuzz_target(<name> <source.cpp>)
#
# Defines a libFuzzer executable `fuzz-<name>` from a single harness source
# file, linked against the shared instrumented library above.
function(add_fuzz_target name source)
  set(target fuzz-${name})

  add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/${source})
  target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(${target} cucumber-fuzz-instrumented)
  target_compile_options(${target} PRIVATE
    -fsanitize=fuzzer,address,undefined -g -O1)
  target_link_options(${target} PRIVATE
    -fsanitize=fuzzer,address,undefined)
endfunction()

add_fuzz_target(parser fuzz_parser.cpp)
add_fuzz_target(step-finder fuzz_step_finder.cpp)
add_fuzz_target(scanner fuzz_scanner.cpp)
add_fuzz_target(replace-variables fuzz_replace_variables.cpp)
