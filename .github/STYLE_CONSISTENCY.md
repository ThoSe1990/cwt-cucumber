
# STYLE_CONSISTENCY.md

## Purpose

This document defines repository-specific conventions that are not covered by clang-format, clang-tidy, or compiler warnings.

Review for consistency with existing repository patterns rather than personal preferences.

Do not comment on formatting issues already enforced by tooling.

---

# Core Philosophy

This repository favors:

* Modern C++20
* Readability over cleverness
* Small focused abstractions
* Explicit intent
* Minimal dependencies
* Standard library solutions before custom implementations
* Value semantics where practical

When reviewing code, prefer consistency with existing implementations over introducing new patterns.

---

# Naming Conventions

## Types

Classes, structs, enums and concepts use snake_case.

Examples:

```cpp
cwt_cucumber
context_value
feature_file
```

When adding new domain objects, follow existing naming patterns.

Avoid introducing synonyms for established concepts.

Examples:

Preferred:

```cpp
feature
scenario
step
hook
context
```

Avoid:

```cpp
test_case
workflow
action
execution_unit
```

unless there is a strong domain reason.

---

## Functions

Function names should clearly describe behavior.

Prefer:

```cpp
parse_feature()
execute_step()
register_hook()
```

Avoid vague names:

```cpp
process()
handle()
run()
do_work()
```

unless the abstraction genuinely requires it.

---

## Variables

Prefer descriptive names.

Good:

```cpp
scenario_name
feature_path
items_count
```

Avoid:

```cpp
tmp
x
data
value
```

when a more specific name exists.

---

# Type Usage

## Explicit Types

Prefer explicit types when they improve readability.

Example:

```cpp
const std::size_t count = CUKE_ARG(1);
const std::string item = CUKE_ARG(2);
```

Do not introduce auto unless the type is obvious from the right-hand side.

Good:

```cpp
auto iter = values.find(key);
```

Less preferred:

```cpp
auto value = CUKE_ARG(1);
```

---

## Const Correctness

Apply const wherever possible.

Prefer:

```cpp
void execute() const;
const box& my_box = context<box>();
```

Reviewers should flag mutable state that appears unnecessary.

---

## References vs Pointers

Non-owning relationships should prefer references.

Preferred:

```cpp
feature& feature_ref
const scenario& scenario
```

Use pointers only when:

* null is meaningful
* ownership is optional

Review any introduction of raw pointers carefully.

---

# Class Design

## Small Focused Classes

Classes should have a single responsibility.

If a class begins handling:

* parsing
* execution
* reporting
* storage

simultaneously, suggest decomposition.

---

## Data Ownership

Favor RAII and automatic lifetime management.

Prefer:

```cpp
std::vector
std::string
std::optional
std::unique_ptr
```

Avoid manual memory management.

---

# API Design

## Express Intent

Function signatures should communicate behavior.

Preferred:

```cpp
[[nodiscard]]
std::optional<scenario> find_scenario(...);
```

over:

```cpp
scenario get(...);
```

when absence is expected.

---

## Attributes

Use attributes consistently.

Prefer:

```cpp
[[nodiscard]]
```

for return values that should not be ignored.

Use:

```cpp
noexcept
```

when failure is impossible or intentionally not propagated.

---

# Error Handling

Follow the repository's established error-handling style.

Before introducing:

* exceptions
* error codes
* expected<T>

check how neighboring code handles failures.

Prefer consistency over introducing a new strategy.

---

# STL Usage

Prefer standard library facilities before custom implementations.

Examples:

```cpp
std::vector
std::unordered_map
std::optional
std::variant
std::filesystem
```

Avoid custom containers or utilities unless there is a demonstrated need.

---

# Repository Architecture

## Reuse Existing Concepts

Before introducing:

```cpp
step_registry
feature_loader
hook_manager
context_store
```

check whether a similar abstraction already exists.

Reviewer should flag duplicate concepts.

---

## Domain Vocabulary

Use existing domain language consistently.

Preferred terms:

* feature
* scenario
* step
* hook
* context
* table
* parameter

Avoid introducing alternate terminology for the same concepts.

---

# Review Priorities

High Severity

* Duplicates existing abstraction
* Violates ownership model
* Introduces inconsistent error handling
* Breaks established domain vocabulary

Medium Severity

* Inconsistent naming
* Different API style than neighboring code
* Missing const correctness

Low Severity

* Local readability improvements
* Minor simplifications

---

# Explicitly Ignore

The reviewer should not comment on:

* Formatting
* Include ordering
* Whitespace
* Braces
* Naming already enforced by automated checks
* Issues already reported by clang-tidy
* Issues already reported by clang-format

Focus only on repository-specific consistency.
