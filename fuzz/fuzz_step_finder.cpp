// libFuzzer harness for the step-matching regex machinery:
// create_regex_definition() (src/util_regex.hpp) and
// step_finder::step_matches() (src/step_finder.cpp).
//
// This is the hand-rolled regex-building/matching layer that turns a step
// definition string like "I place {int} x {string} in it" into a compiled
// std::regex, and then matches feature step text against it. It is a
// prime spot for crashes (malformed {type} tokens, unbalanced braces or
// parens, unescaped regex metacharacters) and for hangs (catastrophic
// backtracking from pathological patterns), independent of the full
// parser pipeline.
//
// The fuzzed input is split into two halves on the first '\n':
//   - first half:  treated as a step *definition* (as if passed to a
//     GIVEN/WHEN/THEN/STEP macro) and run through create_regex_definition()
//   - second half: treated as feature step *text* and matched against the
//     resulting pattern via step_finder::step_matches()
// If there is no '\n', the whole input is used as both the definition and
// the text to match, so every input still exercises the full pipeline.
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
#include <string>
#include <string_view>

#include "step_finder.hpp"
#include "util_regex.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  std::string_view input(reinterpret_cast<const char*>(data), size);

  std::string definition_text;
  std::string feature_text;

  const std::size_t split = input.find('\n');
  if (split == std::string_view::npos)
  {
    definition_text = std::string(input);
    feature_text = std::string(input);
  }
  else
  {
    definition_text = std::string(input.substr(0, split));
    feature_text = std::string(input.substr(split + 1));
  }

  try
  {
    // Real step definitions are always escaped via add_escape_chars()
    // before create_regex_definition() (see src/step.cpp) - do the same
    // here so we fuzz the actual production code path, not a stricter
    // superset of it.
    auto [pattern, type_info] = cuke::internal::create_regex_definition(
        cuke::internal::add_escape_chars(definition_text));

    cuke::internal::step_finder finder(feature_text);
    [[maybe_unused]] bool matched = finder.step_matches(pattern);
  }
  catch (const std::exception&)
  {
    // std::regex may legitimately throw std::regex_error on malformed
    // patterns produced from fuzzed definition text; that is expected,
    // recoverable behavior, not a bug.
  }

  return 0;
}
