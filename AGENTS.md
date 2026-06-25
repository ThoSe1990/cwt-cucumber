# AGENTS.md — AI & Contributor Reference for cwt-cucumber

This file is the authoritative quick-reference for AI coding agents and new
contributors. It covers build commands, project layout, architecture, APIs,
and conventions. Read it before touching code.

---

## Build & Test Commands

> All commands assume you are at the repository root.

### First-time setup (Conan + CMake)

```sh
# Install dependencies and generate the CMake toolchain
conan install . -of ./build --build missing

# Configure (no sanitizers — use for regular development)
cmake -S . -B ./build \
  -DCMAKE_TOOLCHAIN_FILE=./build/conan_toolchain.cmake \
  -DCUCUMBER_UNDEFINED_STEPS_ARE_A_FAILURE=OFF

# Build all targets
cmake --build ./build -j$(nproc)
```

### Incremental rebuild (after changing source files)

```sh
cmake --build ./build -j$(nproc)
```

### Running tests

```sh
# Unit tests (fast, ~500 tests, no feature files needed)
./build/bin/unittests

# Run a specific test suite or test by name filter
./build/bin/unittests --gtest_filter="step_finder*"
./build/bin/unittests --gtest_filter="step_finder.word_empty"

# Integration / stress tests (runs .feature files against the stress-test binary)
./build/bin/stress-tests ./stress-tests

# Example binary (also used as a smoke test in CI)
./build/bin/example ./examples --exclude-file 11_manual_fails.feature
```

### CMake build options

| Option | Default | Effect |
|---|---|---|
| `CUCUMBER_BUILD_TESTS_AND_EXAMPLES` | `ON` | Build `unittests`, `example`, `stress-tests` targets |
| `CUCUMBER_UNDEFINED_STEPS_ARE_A_FAILURE` | `ON` | Final result is FAILED if any step has no definition. Set `OFF` in CI and agent runs |

---

## Project Layout

```
cwt-cucumber/
├── src/                     # Library source — the interpreter itself
│   ├── scanner.cpp/.hpp     # Tokenises raw .feature file text → tokens
│   ├── lexer.cpp/.hpp       # Advances scanner, provides token stream to parser
│   ├── parser.hpp           # Recursive-descent parser → builds the AST
│   ├── ast.hpp/.cpp         # AST node types: feature, scenario, step, …
│   ├── registry.hpp/.cpp    # Global step registry + built-in cucumber expressions
│   ├── step_finder.hpp/.cpp # Matches feature step text against registered regex patterns
│   ├── test_runner.hpp/.cpp # AST visitor that executes matched steps
│   ├── util_regex.hpp       # create_regex_definition(), replace_variables() for outlines
│   ├── stub_generator.hpp   # --generate-steps: AST visitor that emits C++ stubs
│   ├── catalog.hpp/.cpp     # --steps-catalog: prints registered step definitions
│   ├── options.hpp/.cpp     # CLI argument parsing (program_args)
│   ├── cucumber.hpp/.cpp    # Entry point: cwt_cucumber class, entry_point()
│   ├── defines.hpp          # All public macros: GIVEN, WHEN, THEN, BEFORE, …
│   ├── asserts.hpp          # cuke::equal, cuke::is_true, cuke::less, …
│   ├── context.hpp          # cuke::context<T>: per-scenario shared state
│   ├── table.hpp/.cpp       # cuke::table: DataTable access
│   ├── value.hpp            # cuke::value: type-erased runtime value
│   ├── expression.hpp       # expression struct: {pattern, type_info, callback}
│   ├── param_info.hpp       # param_info: offset + type metadata for CUKE_ARG
│   └── …
├── gtest/                   # Google Test unit tests
│   ├── step_finder.cc       # Tests for pattern matching & value extraction
│   ├── run_scenarios.cc     # Tests that parse+run inline Gherkin scripts
│   ├── ast.cc               # Parser/AST tests
│   ├── registry.cc          # Expression registration tests
│   ├── scanner*.cc          # Scanner/language tests
│   └── CMakeLists.txt       # Adds each .cc file explicitly to the unittests target
├── stress-tests/
│   ├── features/            # .feature files exercising edge cases end-to-end
│   └── step_definition.cpp  # Step implementations for stress-test scenarios
├── examples/
│   ├── features/            # Documented example .feature files (1_…, 2_…, …)
│   ├── step_definition.cpp  # Example step implementations
│   ├── custom_parameters.cpp
│   └── hooks.cpp
├── .github/workflows/
│   ├── copilot-setup-steps.yml  # Pre-builds env for Copilot cloud agent
│   └── unittests.yml            # CI: Linux (GCC 13 + Clang 17), Windows, macOS
└── .clang-format            # Allman braces, 2-space indent, Google base style
```

---

## Architecture — Interpreter Pipeline

There are **two independent phases**: step registration (at program startup)
and feature execution (at runtime).

### Phase 1 — Step registration (static initialisation, before `main`)

Each `GIVEN` / `WHEN` / `THEN` / `STEP` macro defines a file-scope struct
whose constructor runs before `main()` and calls
`cuke::registry().push_step(step_definition(...))`.

Inside the `step_definition` constructor (`src/step.cpp`), the definition
string is **pre-processed once** and stored:

```
"I place {int} x {string} in it"
        │
        ▼
add_escape_chars()          (util_regex.hpp)
  Escapes regex special characters in the literal parts of the definition
  (e.g. "." → "\.", "[" → "\["). Cucumber expression tokens are not escaped.
        │
        ▼
create_regex_definition()   (util_regex.hpp)
  Replaces every {type} token with its registered regex pattern:
    {int}    → (-?\\d+)
    {string} → "([^"]*)"
    {word}   → ([^\\s]*)
    {}       → (.*)
    … etc.
  Wraps the whole result in  ^…$  anchors.
  Also builds the param_info vector (offset + type per capture group).
        │
        ▼
Stored in step_definition as:
  m_regex_definition  — the pre-compiled regex string  e.g. "^I place (-?\\d+) x \"([^\"]*)\" in it$"
  m_type_info         — vector<param_info> mapping CUKE_ARG index → registry callback
```

All step definitions sit in the **global registry** (`cuke::registry()`)
with their regex already built before any feature file is touched.

---

### Phase 2 — Feature execution (runtime)

A `.feature` file flows through this pipeline:

```
.feature text
    │
    ▼
Scanner (scanner.cpp)
  Reads raw text, recognises keywords (Feature:, Scenario:, Given, …),
  produces token_type tokens.
    │
    ▼
Lexer (lexer.cpp)
  Wraps Scanner, advances the token stream, provides lookahead to the Parser.
    │
    ▼
Parser (parser.hpp — header-only, recursive-descent)
  Builds an AST rooted at gherkin_document.
  Entry points:
    parser::parse_from_file(path)
    parser::parse_script(string_view)   ← used in unit tests
  Traversal:
    parser::for_each_scenario(node_visitor&)
    │
    ▼
AST nodes (ast.hpp)
  feature_node → scenario_node / scenario_outline_node → step_node
  Each step_node holds: keyword, name (step text), file, line,
                        doc_string, data_table.
  On construction, step_node calls step_finder::find() to locate a matching
  step_definition in the registry and sets m_has_step_definition.
    │
    ▼
step_finder (step_finder.cpp)
  step_finder::find(begin, end)
    — iterates every step_definition in the registry
    — calls step_matches(step_definition.regex_string()) for each
  step_finder::step_matches(pattern)
    — calls std::regex_match(feature_step_text, match, pattern)
    — the pattern is the pre-built m_regex_definition from Phase 1
    — on success, extracts captured groups into value_array
    │
    ▼
test_runner (test_runner.cpp) — implements node_visitor
  visit(scenario_node)         — runs hooks, background, then each step
  visit(scenario_outline_node) — expands Examples rows via replace_variables(),
                                 then runs each concrete scenario
  Step execution: step_node::run() calls the registered callback with
                  value_array, param_info vector, doc_string, table.
    │
    ▼
Results (test_results.hpp/cpp)
  Aggregates pass/fail/skip/undefined counts per step and scenario.
  Printed by cwt_cucumber::print_results().
```

---

### Scenario Outline variable substitution

For `Scenario Outline`, `replace_variables()` (`src/util_regex.hpp`) runs
**before** step matching. It substitutes `<placeholder>` tokens in the step
text with the current row's cell values. Empty cells are substituted with
`""` so that `{string}` / `{word}` / `{}` expressions still produce a
regex match (callbacks strip `""` back to an empty string).

---

## Cucumber Expression System

Built-in expressions live in `src/registry.hpp` in the `m_expressions.standard` map:

| Expression | Regex pattern | C++ type |
|---|---|---|
| `{int}` | `(-?\\d+)` | `int` |
| `{short}` | `(-?\\d+)` | `short` |
| `{long}` | `(-?\\d+)` | `long` |
| `{byte}` | `(-?\\d+)` | `char` |
| `{float}` | `(-?\\d*\\.?\\d+)` | `float` |
| `{double}` | `(-?\\d*\\.?\\d+)` | `double` |
| `{word}` | `([^\\s]*)` | `std::string` — empty cell → empty string |
| `{string}` | `"([^"]*)"` | `std::string` — value without quotes |
| `{}` | `(.*)` | `std::string` (anonymous, matches anything incl. empty) |

---

### Custom parameters — `CUSTOM_PARAMETER`

Custom expressions are registered with `CUSTOM_PARAMETER` and can span
**multiple capturing groups** in their regex pattern:

```cpp
// examples/custom_parameters.cpp
CUSTOM_PARAMETER(custom_parameter_date, "{date}",
                 R"((\d{4})-(\d{2})-(\d{2}))", "shipping date")
{
  date d;
  d.year  = CUKE_PARAM_ARG(1).as<int>();  // 1st capture group of the pattern
  d.month = CUKE_PARAM_ARG(2).as<int>();  // 2nd capture group
  d.day   = CUKE_PARAM_ARG(3).as<int>();  // 3rd capture group
  return d;
}

WHEN(ship_the_box, "The box gets shipped at {date}")
{
  date shipping_date = CUKE_ARG(1);  // framework calls the {date} callback
}
```

Inside the callback body, `CUKE_PARAM_ARG(n)` (1-based) addresses the *n*-th
capturing group of this expression's own pattern.  It receives an iterator
pointing to the first capture group of the match plus a `count` telling it
how many groups belong to it.

---

### `param_info` — how multi-group expressions stay in sync with `CUKE_ARG`

The key concern: if a custom expression uses **N capturing groups** instead
of 1, the regex match result has more entries than there are logical
parameters.  `CUKE_ARG(2)` must skip over those extra groups to land on the
correct value.

`create_regex_definition()` (`src/util_regex.hpp`) solves this by building
a `std::vector<param_info>` alongside the regex string:

```cpp
struct param_info {
  std::size_t offset;       // accumulated extra groups from all *preceding* params
  std::size_t param_count;  // number of capturing groups in *this* expression
  std::string key;          // e.g. "{date}"
  std::string description;
};
```

For each `{type}` token encountered left-to-right:
1. Look up the expression's regex pattern.
2. Count its capturing groups: `value_count = regex(pattern).mark_count()`.
3. Push `param_info{ current_offset, value_count, key, … }`.
4. Advance `offset += value_count - 1`  (extra groups beyond the 1 expected
   by a caller using `CUKE_ARG`).

**Concrete walk-through** — step `"Ships {date} and {int} items"`:

| # | Expression | Pattern | Groups | `offset` stored | Running offset |
|---|---|---|---|---|---|
| 1 | `{date}` | `(\d{4})-(\d{2})-(\d{2})` | 3 | 0 | 0 + (3-1) = **2** |
| 2 | `{int}` | `(-?\\d+)` | 1 | 2 | 2 + (1-1) = **2** |

Compiled regex: `^(\d{4})-(\d{2})-(\d{2}) and (-?\\d+) items$`

After a match the value array is `[year, month, day, n]` (indices 0-3).

```
CUKE_ARG(1)  →  zero_based=0, actual index = 0 + offset[0] = 0 + 0 = 0
                key = "{date}", count = 3
                → calls {date} callback with begin=values[0], count=3
                   CUKE_PARAM_ARG(1) = values[0]  (year)
                   CUKE_PARAM_ARG(2) = values[1]  (month)
                   CUKE_PARAM_ARG(3) = values[2]  (day)

CUKE_ARG(2)  →  zero_based=1, actual index = 1 + offset[1] = 1 + 2 = 3
                key = "{int}", count = 1
                → returns values[3]  (n)
```

So user-facing `CUKE_ARG` indices are always 1, 2, 3 … regardless of how
many internal groups each expression uses.

---

## Step Definition API

Include `src/cucumber.hpp` (or `cwt/cucumber.hpp` in installed form).

### Step macros

```cpp
GIVEN(unique_fn_name, "step definition text {int} with {string}")
{
  const int n      = CUKE_ARG(1);   // 1-based index
  const std::string s = CUKE_ARG(2);
  cuke::equal(n, 42);
}

WHEN(fn, "step text")  { /* … */ }
THEN(fn, "step text")  { /* … */ }
STEP(fn, "step text")  { /* … */ }  // keyword-agnostic alias
```

`unique_fn_name` is a C++ identifier — must be unique across the translation
unit. It has no runtime significance.

### Accessing step arguments

```cpp
const int n          = CUKE_ARG(1);   // auto-converts via expression callback
const std::string s  = CUKE_ARG(2);
const double d       = CUKE_ARG(3);
```

The type is resolved at the call site by the expression callback registered for
that `{type}`. C++ auto deduction does not work — always specify the type.

### DataTable

```cpp
WHEN(fn, "I have the following table:")
{
  const cuke::table& t = CUKE_TABLE();

  // Iterate all rows (returns table::row objects)
  for (const cuke::table::row& row : t.raw())
  {
    std::string val = row[0].to_string();
    int n           = row[1].as<int>();
  }

  // Named columns (first row is header)
  for (const cuke::table::row& row : t.hashes())
  {
    std::string item = row["ITEM"].to_string();
  }

  // Single key-value pair (two-column table, first row is key)
  cuke::table::pair kv = t.rows_hash();
  std::string item = kv["ITEM"].to_string();
}
```

### Doc string

```cpp
WHEN(fn, "I have a doc string:")
{
  const std::string doc    = CUKE_DOC_STRING();   // as single string
  const std::vector<std::string> lines = CUKE_DOC_STRING();  // as lines
}
```

### Shared state between steps — `cuke::context<T>`

`cuke::context<T>` is reset at the start of each scenario.

```cpp
// Construct/reset T with optional constructor args
cuke::context<MyType>();           // default-construct
cuke::context<MyType>(arg1, arg2); // forwarded to T's constructor

// Access existing instance (reference)
MyType& obj = cuke::context<MyType>();
const MyType& obj = cuke::context<MyType>();
```

### Assertion functions (`src/asserts.hpp`)

All functions accept an optional `std::string` message as last argument.

```cpp
cuke::equal(lhs, rhs);
cuke::not_equal(lhs, rhs);
cuke::greater(lhs, rhs);
cuke::greater_or_equal(lhs, rhs);
cuke::less(lhs, rhs);
cuke::less_or_equal(lhs, rhs);
cuke::is_true(condition);
cuke::is_false(condition);
```

A failing assertion marks the current step and scenario as **FAILED** but does
not throw — execution of the step body continues unless you return early.

### Hook macros

```cpp
BEFORE(fn)              { /* runs before every scenario */ }
AFTER(fn)               { /* runs after every scenario  */ }
BEFORE_T(fn, "@tag")    { /* conditional on tag expression */ }
AFTER_T(fn,  "@tag")    { /* conditional on tag expression */ }
BEFORE_STEP(fn)         { /* runs before every step  */ }
AFTER_STEP(fn)          { /* runs after every step   */ }
BEFORE_ALL(fn)          { /* once before any scenario */ }
AFTER_ALL(fn)           { /* once after all scenarios */ }

// Inside a hook body:
cuke::skip_scenario();           // mark scenario skipped, skip remaining steps
cuke::ignore_scenario();         // mark scenario ignored
cuke::fail_scenario("reason");   // immediately fail the scenario
```

---

## Adding a New CLI Flag

1. Add an enum value to `program_args::arg` in `src/options.hpp`.
2. Add a `definition` entry to the `defs[]` array in the same file.
3. Handle it in the appropriate method in `src/cucumber.cpp`
   (e.g. `export_catalog()` for output-only flags, or `run_tests()` for
   flags that affect the test run).
4. Add a unit test in `gtest/options.cc` (or the closest relevant file).

---

## Adding Keywords to the Interpreter

Keywords are the Gherkin tokens a scanner recognises: `Feature:`,
`Scenario:`, `Given`, `Examples:`, etc. Each language has its own
`identifier` subclass in `src/identifiers/`.

### How the keyword system works

1. `scanner` defaults to `english` as its identifier.
2. If the feature file begins with `# language: de` (or `es`, …) the scanner
   calls `set_language()` in `src/scanner.cpp`, which swaps the identifier
   to `german` / `spanish`.
3. At each position in the source the scanner calls
   `m_identifiers->get_token(str)`, which does a linear `starts_with` scan
   of that language's keyword table and returns the matching `token_type`.

### Token types available (`src/token.hpp`)

| `token_type` | Gherkin concept |
|---|---|
| `feature` | `Feature:` header |
| `scenario` | `Scenario:` / `Example:` |
| `scenario_outline` | `Scenario Outline:` / `Scenario Template:` |
| `background` | `Background:` |
| `rule` | `Rule:` |
| `examples` | `Examples:` / `Scenarios:` |
| `step` | `Given` / `When` / `Then` / `And` / `But` / `*` |

### Adding a keyword alias to an existing language

Open `src/identifiers/english.hpp` (or `german.hpp` / `spanish.hpp`):

1. Increment the `std::array` size by the number of new entries.
2. Add a `{std::string_view("New Keyword:"), token_type::target_type}` pair.
   Order does not matter for correctness, but put more specific keywords
   before shorter ones that share a prefix to avoid false matches.

**Example** — adding `Scenario Template:` and `Scenarios:` to English
(the exact change from PR #117):

```cpp
// Before
static constexpr std::array<std::pair<std::string_view, token_type>, 12>
    m_identifiers{{{std::string_view("Scenario Outline:"),
                    token_type::scenario_outline},
                   {std::string_view("Examples:"), token_type::examples},
                   /* … */}};

// After
static constexpr std::array<std::pair<std::string_view, token_type>, 14>
    m_identifiers{{{std::string_view("Scenario Outline:"),
                    token_type::scenario_outline},
                   {std::string_view("Scenario Template:"),   // ← new
                    token_type::scenario_outline},
                   {std::string_view("Examples:"), token_type::examples},
                   {std::string_view("Scenarios:"),            // ← new
                    token_type::examples},
                   /* … */}};
```

### Adding a new language

1. Create `src/identifiers/mylang.hpp`, subclassing `identifier`:

```cpp
#pragma once
#include <array>
#include "identifier.hpp"

namespace cuke::internal
{

class mylang : public identifier
{
  std::pair<token_type, std::size_t> get_token(
      std::string_view str) const override
  {
    for (const auto& element : m_identifiers)
    {
      if (str.starts_with(element.first))
      {
        return {element.second, element.first.length()};
      }
    }
    return {token_type::none, 0};
  }

 private:
  static constexpr std::array<std::pair<std::string_view, token_type>, N>
      m_identifiers{{
          {std::string_view("Feature:"), token_type::feature},
          {std::string_view("Scenario:"), token_type::scenario},
          // … all required keywords …
          {std::string_view("Given"), token_type::step},
          {std::string_view("When"),  token_type::step},
          {std::string_view("Then"),  token_type::step},
          {std::string_view("And"),   token_type::step},
          {std::string_view("But"),   token_type::step},
      }};
};

}  // namespace cuke::internal
```

2. Register it in `src/scanner.cpp`:

```cpp
#include "identifiers/mylang.hpp"

void scanner::set_language(std::string_view country)
{
  if (country == "de") { m_identifiers = std::make_shared<german>();  }
  else if (country == "es") { m_identifiers = std::make_shared<spanish>(); }
  else if (country == "xx") { m_identifiers = std::make_shared<mylang>(); }  // ← add
}
```

3. Add unit tests in a new file `gtest/scanner_mylang.cc` — one
   `TEST(mylang_keywords, …)` per keyword, using the `# language: xx` header:

```cpp
TEST(mylang_keywords, feature)
{
  const char* script = R"*(
  # language: xx
  <keyword here>
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
```

4. Register the new `.cc` file in `gtest/CMakeLists.txt` by adding it to the
   `add_executable` source list.

---

## Adding a New Built-in Cucumber Expression

Edit the `m_expressions.standard` map in `src/registry.hpp`:

```cpp
{"{mytype}", {"(regex_pattern)", "mytype", make_parameter_value<CppType>}},
```

If the C++ type needs custom conversion logic, supply a lambda instead of
`make_parameter_value<T>`:

```cpp
{"{mytype}", {"(regex_pattern)", "mytype",
  [](cuke::value_array::const_iterator begin, std::size_t count) -> any
  {
    return get_param_value(begin, count, 1).as<std::string>(); // example
  }}},
```

`mark_count()` of the regex pattern determines the offset arithmetic for
subsequent parameters (see the `param_info` section above).
**Built-in expressions must have exactly one capturing group** — this keeps
the offset contribution at zero and avoids surprising users.  If you need
multiple groups, define a `CUSTOM_PARAMETER` instead.

---

## Writing Tests

### Unit test (gtest/)

Tests that verify a self-contained piece of logic (parser, step_finder,
scanner, options, …).

**Pattern 1 — pure logic, no registry:**
```cpp
// gtest/step_finder.cc
TEST(step_finder, my_new_test)
{
  auto [pattern, types] = create_regex_definition("I have {int} items");
  step_finder sf("I have 5 items");
  ASSERT_TRUE(sf.step_matches(pattern));
  ASSERT_EQ(sf.values().size(), 1);
  EXPECT_EQ(sf.values().at(0).as<int>(), 5);
}
```

**Pattern 2 — full scenario execution with inline Gherkin:**
```cpp
// gtest/run_scenarios.cc  (use a TEST_F with a SetUp that clears + registers)
class my_test : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array& v, const auto&, const auto&, const auto&)
        { /* assert on v */ },
        "my step definition text"));
  }
};

TEST_F(my_test, scenario_passes)
{
  const char* script = R"*(
    Feature: f
    Scenario: s
    Given my step definition text
  )*";
  cuke::parser p;
  p.parse_script(script);
  cuke::test_runner runner;
  p.for_each_scenario(runner);
  EXPECT_EQ(cuke::results::test_results().scenarios_passed(), 1);
}
```

**Registering a new .cc file:** Add it to the `add_executable` list in
`gtest/CMakeLists.txt`.

### Stress / integration tests

The `stress-tests/` directory is the home for **edge-case and weird-structure
scenarios** — things that are valid Gherkin but exercise tricky paths in the
interpreter that unit tests don't easily reach.

#### When to add a stress test (vs. a unit test)

| Use a **unit test** (`gtest/`) when… | Use a **stress test** (`stress-tests/`) when… |
|---|---|
| Testing a single function / class in isolation | Testing the full pipeline end-to-end |
| No feature file involved | The bug manifests only when a `.feature` file is parsed and run |
| Fast, deterministic, easy to assert on return values | Edge-case Gherkin structures (empty cells, special chars, unusual formatting) |

#### Directory layout

```
stress-tests/
  CMakeLists.txt              — builds one executable linked to libcucumber
  features/
    stress-tests.feature      — all stress scenarios live here (one file)
  step_definition.cpp         — step implementations for stress scenarios
  hooks.cpp                   — hook examples (BEFORE/AFTER/BEFORE_STEP/etc.)
```

#### How to run

```sh
# build (from project root, after cmake configure)
cmake --build build --target stress-tests

# run all stress scenarios
./build/stress-tests/stress-tests ./stress-tests/features/stress-tests.feature
```

#### Adding a new stress scenario

1. **Add a `Scenario` or `Scenario Outline`** to
   `stress-tests/features/stress-tests.feature`.  
   Group related scenarios together and add a comment line if the intent
   is not obvious from the Gherkin alone.

2. **Implement any new steps** in `stress-tests/step_definition.cpp`.  
   Use `cuke::is_true` / `cuke::equal` to assert correctness inside steps —
   a failing assertion marks the scenario as failed.

3. **No CMake changes needed** — all `.cpp` files in the directory are
   already compiled into the `stress-tests` target.

4. **Run the binary** and verify all scenarios pass before opening a PR.

#### Example — adding a new edge case

```gherkin
# stress-tests/features/stress-tests.feature

  Scenario Outline: special characters in word expressions
    When The value is <val>
    Then It should equal <expected>

    Examples:
      | val       | expected   |
      | foo/bar   | "foo/bar"  |
      | café      | "café"     |
```

```cpp
// stress-tests/step_definition.cpp

WHEN(special_word_given, "The value is {word}")
{
  cuke::context<std::string>() = CUKE_ARG(1);
}

THEN(special_word_then, "It should equal {string}")
{
  std::string expected = CUKE_ARG(1);
  cuke::equal(expected, cuke::context<std::string>());
}
```

#### Coverage currently in `stress-tests.feature`

| Scenario | What it exercises |
|---|---|
| `Scenario Outline: a scenario outline` | `{word}` + `{}` with numeric/punctuation values in outline cells |
| `Scenario Outline: lets put the quotes in the step` | Inline `"…"` delimiters in step text (not in cells) |
| `Scenario: Doc string with quotes` | `"""…"""` doc string parsing |
| `Scenario: Doc string with backticks` | `` ```…``` `` doc string parsing |
| `Scenario: Doc string as vector` | `CUKE_DOC_STRING()` as `std::vector<std::string>` |
| `Scenario: Empty cells in data table` | `CUKE_TABLE()` with fully empty rows |
| `Scenario Outline: Empty cells in examples` | `{word}`, `{}`, `{string}` with empty outline cells (`""` sentinel) |

---

## Code Style

From `.clang-format`:

- Base style: Google
- **Allman braces** — opening brace on its own line for every block
- Indent: **2 spaces** (no tabs)
- `SortIncludes: false` — keep include order as written
- Max line length: 80 (Google default)

Run before committing:
```sh
clang-format -i src/*.hpp src/*.cpp gtest/*.cc
```

CI will reject PRs that fail the format check.

Additional conventions observed in the codebase:

- `[[nodiscard]]` on every function that returns a value that must be used
- `noexcept` on getters and functions with no failure path
- `static` on free functions that are file-local or header-only utilities
- `const` references for all read-only parameters
- No raw owning pointers — use `std::unique_ptr` or value semantics
- Prefer `std::string_view` for read-only string parameters; `std::string` for stored values
- Use `std::format` (C++20) for string formatting — do not use `printf` or `ostringstream`

---

## Releasing a New Version

A release requires exactly two file changes.

### 1. Bump the version in `CMakeLists.txt`

Line 2 of the root `CMakeLists.txt`:

```cmake
# Before
project(cwt-cucumber VERSION 2.8)

# After — example bump to 2.9
project(cwt-cucumber VERSION 2.9)
```

CMake automatically propagates `PROJECT_VERSION` to the docs `_version.py`
file via the `file(WRITE …)` call on line 4.

### 2. Promote `[Unreleased]` in `CHANGELOG.md`

Replace the `## [Unreleased]` heading with the new version and today's date
(`YYYY-MM-DD`), then add a fresh empty `## [Unreleased]` block above it:

```markdown
# Changelog

## [Unreleased]

## [2.9] 2026-06-25       ← was [Unreleased], now stamped

### Added

- …previous unreleased entries stay here…

### Fixed

- …
```

The date format is `YYYY-MM-DD`. The version number must match exactly what
is set in `CMakeLists.txt`.

---

## What NOT to Do

- **Do not** add mandatory dependencies. The library must remain buildable
  with no external deps (`nlohmann_json` is optional via `WITH_JSON`).
- **Do not** change `std::regex_match` back to `std::regex_search` in
  `step_finder.cpp` — the `while` loop with `(.*)` patterns causes an
  infinite loop on empty suffixes.
- **Do not** use `([^\s]+)` or `(.+)` for built-in expressions that must
  support empty values — use `([^\s]*)` / `(.*)` and strip the `""` sentinel
  in the callback.
- **Do not** add state to `step_finder` that persists across calls —
  it is constructed fresh per step lookup.
- **Do not** use `std::cout` / `std::cerr` directly in library code —
  route output through `cuke::log::info` / `cuke::log::error`.
