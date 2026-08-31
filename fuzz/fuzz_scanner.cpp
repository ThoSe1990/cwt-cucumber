// libFuzzer harness for cuke::internal::scanner (src/scanner.cpp), the
// tokenizer that turns raw .feature text into tokens for the parser.
//
// This calls the scanner directly and repeatedly, bypassing the parser
// entirely. A scanner-only harness has a much shallower call stack than
// the full parser pipeline, so libFuzzer can explore its state machine
// (keyword/language detection, string/doc-string/comment skipping,
// end-of-input handling) far more densely per input. It also isolates
// scanner bugs from parser bugs when triage a crash.
//
// Build (Clang only, libFuzzer ships with Clang):
//   cmake -S . -B build-fuzz -DCMAKE_CXX_COMPILER=clang++ \
//     -DCUCUMBER_BUILD_FUZZERS=ON
//   cmake --build build-fuzz --target fuzz-scanner
//
// Run:
//   ./build-fuzz/bin/fuzz-scanner -max_total_time=120
//
// Any crash is written to a `crash-<hash>` file in the current directory;
// reproduce it with:
//   ./build-fuzz/bin/fuzz-scanner crash-<hash>

#include <cstdint>
#include <string_view>

#include "scanner.hpp"
#include "token.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  cuke::internal::scanner s(
      std::string_view(reinterpret_cast<const char*>(data), size));

  // Keep scanning tokens until eof or a generous cap, in case a bug causes
  // the scanner to never reach eof (infinite loop protection for the
  // fuzzer harness itself, not a fix for the library).
  constexpr std::size_t max_tokens = 100'000;
  for (std::size_t i = 0; i < max_tokens; ++i)
  {
    cuke::internal::token t = s.scan_token();
    if (t.type == cuke::internal::token_type::eof)
    {
      break;
    }
  }

  return 0;
}
