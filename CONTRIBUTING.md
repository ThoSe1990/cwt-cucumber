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
cmake --build ./build -j$(nproc)
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
