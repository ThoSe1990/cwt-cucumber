# Contributing to cwt-cucumber

Thanks for your interest! This is a C++20 BDD/Cucumber interpreter. Below is
a quick guide for human contributors. For the full technical reference
(architecture, all APIs, extension points) see **[AGENTS.md](AGENTS.md)**.

---

## Getting started

```sh
# Install dependencies via Conan, configure and build
conan install . -of ./build --build missing
cmake -S . -B ./build \
  -DCMAKE_TOOLCHAIN_FILE=./build/conan_toolchain.cmake \
  -DCUCUMBER_UNDEFINED_STEPS_ARE_A_FAILURE=OFF
cmake --build ./build -j12
```

---

## Running the tests

```sh
# Unit tests (~500 tests, fast)
./build/bin/unittests

# Stress / edge-case tests
./build/bin/stress-tests ./stress-tests

# Smoke test (examples)
./build/bin/example ./examples --exclude-file 11_manual_fails.feature
```

## Benchmarking step matching

If you change `step_finder`/`step_definition` matching logic, run this
before and after your change on the same machine to sanity-check the
performance impact (not a precise, cross-platform benchmark — see
[Benchmarks](AGENTS.md#benchmarks) in AGENTS.md for details):

```sh
./build/bin/step-matching-benchmark
```

## Fuzz testing the parser

If you touch the scanner, lexer, or parser, it's worth running the
libFuzzer harness for a couple of minutes to check for crashes on
malformed input (requires Clang — on macOS use Homebrew LLVM, not Xcode's
bundled Clang; first time doing this? see [`fuzz/README.md`](fuzz/README.md)
for a full beginner walkthrough; see
[Fuzz testing](AGENTS.md#fuzz-testing) in AGENTS.md for the terse
reference):

```sh
cmake -S . -B build-fuzz -DCMAKE_CXX_COMPILER=clang++ -DCUCUMBER_BUILD_FUZZERS=ON
cmake --build build-fuzz --target fuzz-parser
cp -r fuzz/corpus/parser parser-corpus-scratch
./build-fuzz/bin/fuzz-parser -max_total_time=120 parser-corpus-scratch
```

`fuzz/corpus/parser/` is a tracked seed corpus of real and edge-case `.feature`
files — always fuzz with a copy of it (`parser-corpus-scratch/`, gitignored)
rather than from scratch or directly against `fuzz/corpus/parser/` itself: it
makes the fuzzer reach much deeper into the parser in the same amount of
time, and keeps libFuzzer's generated artifacts out of the tracked
directory. If you fix a crash found by fuzzing, add its input to
`fuzz/corpus/parser/` as a new
`edge_*.feature` file alongside a `gtest/ast.cc` regression test.

### Using a Mac? 

Its possible that XCode's Clang does not ship the fuzzing library, so install `llvm` with `brew` 
and set its compiler explicitly: 

```sh
# 1. Ensure Homebrew LLVM is installed (has the libFuzzer runtime; Xcode's clang doesn't)
brew install llvm

# 2. Configure a separate build dir pointing at Homebrew's clang++ by full path
cmake -S . -B build-fuzz \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ \
  -DCUCUMBER_BUILD_FUZZERS=ON \
  -DCUCUMBER_BUILD_TESTS_AND_EXAMPLES=OFF

# 3. Build just the fuzzer
cmake --build build-fuzz --target fuzz-parser

# 4. Copy the tracked seed corpus to a gitignored scratch dir, then run
#    (fuzzes for 2 minutes here, increase as needed)
cp -r fuzz/corpus/parser parser-corpus-scratch
./build-fuzz/bin/fuzz-parser -max_total_time=120 parser-corpus-scratch
```


---

## Making changes

| Task | Where to look in AGENTS.md |
|---|---|
| Understand the interpreter pipeline | [Architecture](AGENTS.md#architecture--interpreter-pipeline) |
| Add a step expression type | [Cucumber Expression System](AGENTS.md#cucumber-expression-system) |
| Add a CLI flag | [Adding a New CLI Flag](AGENTS.md#adding-a-new-cli-flag) |
| Add a keyword / language | [Adding Keywords](AGENTS.md#adding-keywords-to-the-interpreter) |
| Write a unit or stress test | [Writing Tests](AGENTS.md#writing-tests) |
| Release a new version | [Releasing a New Version](AGENTS.md#releasing-a-new-version) |

---

## Code style

- **Allman braces**, 2-space indent, max 80 columns
- Base style: Google (see `.clang-format`)
- Run before committing:
  ```sh
  clang-format -i src/*.hpp src/*.cpp gtest/*.cc
  ```
- CI will reject PRs that fail the format check.

Full conventions are in [Code Style](AGENTS.md#code-style).

---

## Submitting a PR

- Fill in the [PR template](.github/pull_request_template.md)
- All unit tests and stress tests must pass
- New behaviour needs a test (unit test for logic, stress test for Gherkin edge cases)
- No new mandatory dependencies — the library must build with zero external deps

---

## Reporting issues

Use the GitHub issue templates:

- 🐛 **Bug report** — unexpected interpreter behaviour
- 💡 **Feature request** — new expression type, CLI flag, language support, …
- ❓ **Question** — anything else
