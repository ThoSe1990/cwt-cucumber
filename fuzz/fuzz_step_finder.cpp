// libFuzzer harness for create_regex_definition() (src/util_regex.hpp).
//
// The input is a step definition (first line if there is a '\n'), escaped
// like production (see src/step.cpp), then compiled. Matching is not
// fuzzed: std::regex_match can hang on chained "(.*)" from anonymous
// "{}"; that path stays in the unit tests.
//
// Build (Clang only, libFuzzer ships with Clang):
//   cmake -S . -B build-fuzz -DCMAKE_CXX_COMPILER=clang++ \
//     -DCUCUMBER_BUILD_FUZZERS=ON
//   cmake --build build-fuzz --target fuzz-step-finder
//
// Run:
//   ./build-fuzz/bin/fuzz-step-finder -max_total_time=120
//
// Any crash is written to a `crash-<hash>` file in the current directory;
// reproduce it with:
//   ./build-fuzz/bin/fuzz-step-finder crash-<hash>

#include <cstdint>
#include <regex>
#include <string>
#include <string_view>

#include "util_regex.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data,
                                      size_t size)  // NOLINT
{
  std::string_view input(reinterpret_cast<const char*>(data), size);
  const std::size_t split = input.find('\n');
  const std::string definition_text = split == std::string_view::npos
                                          ? std::string(input)
                                          : std::string(input.substr(0, split));

  try
  {
    [[maybe_unused]] const auto compiled =
        cuke::internal::create_regex_definition(
            cuke::internal::add_escape_chars(definition_text));
  }
  catch (const std::regex_error&)
  {
    // std::regex may legitimately throw std::regex_error on malformed
    // patterns produced from fuzzed definition text; that is expected,
    // recoverable behavior, not a bug. Deliberately narrower than
    // std::exception: other exceptions (e.g. std::bad_alloc from runaway
    // allocation on a pathological pattern) must propagate to libFuzzer
    // instead of being silently treated as a successful run.
  }

  return 0;
}
