// libFuzzer harness for cuke::internal::replace_variables()
// (src/util_regex.hpp), the Scenario Outline placeholder-substitution
// function. This is called on both step/scenario names (ignore_missing_key
// == false, throws-turned-log-error on unmatched keys) and doc strings
// (ignore_missing_key == true, always silent on unmatched keys), so both
// modes are exercised here.
//
// The fuzzed input is split on '\n' into up to three parts:
//   1. step text        - may contain <placeholder> tokens
//   2. examples column key   - the header cell name for the one-column table
//   3. examples column value - the corresponding cell value
// Missing parts default to empty strings, so a single-line (or empty)
// input still exercises the function with a placeholder that never
// matches any column.
//
// Build (Clang only, libFuzzer ships with Clang):
//   cmake -S . -B build-fuzz -DCMAKE_CXX_COMPILER=clang++ \
//     -DCUCUMBER_BUILD_FUZZERS=ON
//   cmake --build build-fuzz --target fuzz-replace-variables
//
// Run:
//   ./build-fuzz/bin/fuzz-replace-variables -max_total_time=120
//
// Any crash is written to a `crash-<hash>` file in the current directory;
// reproduce it with:
//   ./build-fuzz/bin/fuzz-replace-variables crash-<hash>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "table.hpp"
#include "util_regex.hpp"
#include "value.hpp"

namespace
{
std::vector<std::string> split_lines(std::string_view input,
                                     std::size_t max_parts)
{
  std::vector<std::string> parts;
  std::size_t start = 0;
  while (parts.size() + 1 < max_parts)
  {
    const std::size_t pos = input.find('\n', start);
    if (pos == std::string_view::npos)
    {
      break;
    }
    parts.emplace_back(input.substr(start, pos - start));
    start = pos + 1;
  }
  parts.emplace_back(input.substr(start));
  parts.resize(max_parts);
  return parts;
}
}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  const std::string_view input(reinterpret_cast<const char*>(data), size);
  const std::vector<std::string> parts = split_lines(input, 3);

  const std::string& step_text = parts[0];
  const std::string& key = parts[1];
  const std::string& value = parts[2];

  cuke::table t(cuke::value_array{cuke::value(key), cuke::value(value)}, 1);
  const cuke::table::row row = t.hash_row(0);

  // Exercise both the doc-string (ignore_missing_key = true) and the
  // step/scenario name (ignore_missing_key = false) code paths.
  [[maybe_unused]] auto doc_string_result =
      cuke::internal::replace_variables(step_text, row, true);
  [[maybe_unused]] auto step_text_result =
      cuke::internal::replace_variables(step_text, row, false);

  return 0;
}
