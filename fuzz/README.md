# Fuzz testing `cwt-cucumber`

This is a beginner-friendly walkthrough for running and analyzing the
`fuzz-parser` libFuzzer harness locally. If you've never done fuzz testing
before, read this end to end before running anything.

For the short reference version (build flags, CMake option, CI details),
see the "Fuzz testing" section in [`AGENTS.md`](../AGENTS.md#fuzz-testing).

## What is fuzzing, in one sentence

Instead of you writing test cases by hand, a fuzzer automatically generates
and mutates random byte strings, feeds each one into your program, and
watches for it to crash. It's a robot trying millions of weird inputs per
minute to find the ones that break your code.

Here, the "program" is `fuzz-parser` — a tiny harness
(`fuzz/fuzz_parser.cpp`) that just does:

```cpp
cuke::parser p;
p.parse_script(the_random_bytes);
```

## Step 1 — Build it

Requires Clang (libFuzzer is a Clang-only feature). On macOS, use Homebrew
LLVM — Xcode's bundled Clang does not include the libFuzzer runtime.

```sh
cmake -S . -B build-fuzz \
  -DCMAKE_CXX_COMPILER=$(brew --prefix llvm)/bin/clang++ \
  -DCUCUMBER_BUILD_FUZZERS=ON -DCUCUMBER_BUILD_TESTS_AND_EXAMPLES=OFF
cmake --build build-fuzz --target fuzz-parser
```

This produces one executable: `build-fuzz/bin/fuzz-parser`. That binary
**is** the fuzzer — it has libFuzzer's engine linked in, plus our harness
code.

## Step 2 — Give it a starting point (the "corpus")

A fuzzer doesn't know anything about Gherkin syntax. Starting from empty or
random garbage, it's very unlikely to ever randomly type out the 8 specific
characters `Feature:`. So we help it by giving a few example inputs to
start mutating from.

This repo ships a ready-to-use seed corpus at `fuzz/corpus/parser/` — copies of
the real `.feature` files from `examples/features/` and
`stress-tests/features/` (covering scenarios, outlines, backgrounds, tags,
tables, doc strings, custom parameters, rules), plus a couple of small
handwritten files that reproduce previously-found crashes (see
`fuzz/corpus/parser/edge_*.feature`).

libFuzzer treats the directory you pass as both input **and** output: it
reads the existing seeds and also writes new, newly-discovered
"interesting" inputs into the same directory as it runs. **Never point it
directly at `fuzz/corpus/parser/`** — that would leave generated artifacts sitting
in a tracked directory, one `git add .` away from being committed by
accident. Instead, always copy it to a scratch directory first
(`parser-corpus-scratch/` is already gitignored, so anything written there is
automatically kept out of git):

```sh
cp -r fuzz/corpus/parser parser-corpus-scratch
./build-fuzz/bin/fuzz-parser -max_total_time=30 parser-corpus-scratch
```

If you'd rather build your own minimal corpus from scratch instead:

```sh
mkdir -p parser-corpus-scratch
printf 'Feature: demo\n' > parser-corpus-scratch/seed1
printf 'Feature:#\n' > parser-corpus-scratch/seed2
```

Think of `parser-corpus-scratch/` as "example homework" — the fuzzer takes these,
flips bytes, duplicates chunks, splices them together, etc., and keeps any
mutation that makes the program explore new code paths it hasn't seen
before.

## Step 3 — Run it

```sh
./build-fuzz/bin/fuzz-parser -max_total_time=30 parser-corpus-scratch
```

- `-max_total_time=30` — run for 30 seconds, then stop automatically.
- `corpus` — the directory to read seeds from AND write newly-discovered
  "interesting" inputs into (as new files) during the run.

While it runs, you'll see two kinds of output interleaved:

1. **Your own program's logs** — every time the parser rejects garbage
   input (which is almost always), it prints `Error while parsing script`
   etc. This is just noise; ignore it.
2. **libFuzzer's own status lines**, e.g.
   `#2097152 pulse cov: 2 ft: 2 corp: 1/1b ...` — periodic progress
   updates. `cov` = how many distinct code branches it has hit so far.
   Rising `cov` means it's exploring new paths; ignore these too, unless
   nothing rises for a long time (which means it's stuck and could use a
   better/bigger seed corpus).

## Step 4 — What a crash looks like

If the program does something illegal (uncaught exception, segfault,
sanitizer-detected memory bug), the whole thing stops immediately and you
see:

```
==PID== ERROR: libFuzzer: deadly signal
<stack trace>
...
Feature:#~                                   <- the actual bytes that crashed it
artifact_prefix='./'; Test unit written to ./crash-<hash>
```

Two important things happen at that moment:

1. It prints the **exact bytes** that caused the crash, both as hex and
   (if printable) as text.
2. It saves those exact bytes to a file **in your current working
   directory** (not in `corpus/`!) named `crash-<hash>`.

## Step 5 — How to read the stack trace

Ignore the first several frames — they're generic C++ runtime/signal
handling machinery (`__cxa_throw`, `abort`, `libc++abi`, ...). The
interesting part is the first frame that mentions your own code
(`cuke::something`), for example:

```
#12 std::__throw_length_error(...)              <- generic: "a string constructor failed"
#14 cuke::internal::parse_keyword_and_name(...)  <- YOUR code, this is where it happened
#15 cuke::internal::parse_feature(...)           <- who called it
#16 cuke::parser::parse_impl(...)                <- who called that
```

Read this like a phone-call chain: `parse_impl` called `parse_feature`,
which called `parse_keyword_and_name`, and *that* function tried to
construct a `std::string` with an invalid length, which threw, which
crashed the program. That tells you exactly which function to go open and
inspect.

## Step 6 — Reproduce it in isolation

```sh
./build-fuzz/bin/fuzz-parser crash-<hash>
```

No `-max_total_time`, no `corpus` — just point it at the single saved file.
This runs the parser **once** on those exact bytes, instantly reproducing
the same crash with the same stack trace, so you can debug it without
waiting for the fuzzer to find it again.

## Step 7 — Fix it, verify, clean up

1. Go to the function named in the stack trace, understand why it's wrong.
2. Fix it.
3. Rebuild (`cmake --build build-fuzz --target fuzz-parser`).
4. Re-run the same crash file — it should now execute cleanly with no
   crash.
5. Turn that same input into a permanent unit test (e.g. in
   `gtest/ast.cc`) so it's tested forever, not just when someone remembers
   to fuzz again.
6. Also copy the crashing input into `fuzz/corpus/parser/` as a small,
   descriptively-named `.feature` file (e.g. `edge_<what-it-tests>.feature`
   — see the existing `edge_*.feature` files for examples). This is a
   regression seed: it costs almost nothing to keep, and it means any
   future fuzzing run starts by immediately re-checking this exact case
   instead of relying on luck to rediscover it.
7. Delete `crash-*` files and the `build-fuzz/` directory (and any scratch
   corpus copy you made, e.g. `parser-corpus-scratch/`) — none of these should be
   committed (they're already gitignored). `fuzz/corpus/parser/` itself IS
   tracked and should be committed if you added a new regression seed to
   it in the previous step.

