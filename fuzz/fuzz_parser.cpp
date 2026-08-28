// libFuzzer harness for cuke::parser::parse_script().
//
// Feeds arbitrary byte strings to the full Scanner -> Lexer -> Parser ->
// AST pipeline, looking for crashes, sanitizer violations (ASan/UBSan),
// and hangs (e.g. catastrophic regex backtracking triggered by malformed
// step text). This does not execute any step definitions - it only
// parses, so it is safe to run unattended for long stretches.
//
// Build (Clang only, libFuzzer ships with Clang):
//   cmake -S . -B build-fuzz -DCMAKE_CXX_COMPILER=clang++ \
//     -DCUCUMBER_BUILD_FUZZERS=ON
//   cmake --build build-fuzz --target fuzz-parser
//
// Run:
//   ./build-fuzz/bin/fuzz-parser -max_total_time=120
//
// Any crash is written to a `crash-<hash>` file in the current directory;
// reproduce it with:
//   ./build-fuzz/bin/fuzz-parser crash-<hash>

#include <cstdint>
#include <string_view>

#include "parser.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  cuke::parser p;
  p.parse_script(std::string_view(reinterpret_cast<const char*>(data), size));
  return 0;
}
