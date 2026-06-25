## Summary
<!-- What does this PR do? One or two sentences. -->


## Motivation
<!-- Why is this change needed? Link to the issue it fixes if applicable. -->

Closes #


## Changes
<!-- List the files/areas changed and what was done. Be specific. -->

- 


## How to Test
<!-- Exact commands to verify the change. Copy-paste ready. -->

```sh
cmake --build ./build -j$(nproc)

# Unit tests
./build/bin/unittests

# Integration tests
./build/bin/stress-tests ./stress-tests
```

<!-- If you added new tests, name them here: -->
New tests:
- 


## Checklist

- [ ] Code follows the Allman brace style and passes `clang-format`
- [ ] New or changed behaviour is covered by a unit test in `gtest/` or a scenario in `stress-tests/`
- [ ] No mandatory new dependencies introduced
- [ ] `AGENTS.md` updated if new APIs, conventions, or CLI flags were added
