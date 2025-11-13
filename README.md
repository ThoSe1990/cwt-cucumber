[![Build & Test](https://github.com/ThoSe1990/cwt-cucumber/actions/workflows/unittests.yml/badge.svg)](https://github.com/ThoSe1990/cwt-cucumber/actions/workflows/unittests.yml)
[![Conan Center](https://img.shields.io/conan/v/cwt-cucumber)](https://conan.io/center/recipes/cwt-cucumber)

## Coding With Thomas Cucumber: A C++20 Cucumber Interpreter

<p align="center">
  <img src="docs/_static/readme-logo.png" width="180" alt="CWT-Cucumber Logo">
</p>

Welcome to **CWT-Cucumber**, a lightweight, modern **C++20 BDD testing framework** for native C++ projects.  

📚 **Full docs & examples:** [Read the docs](https://those1990.github.io/cwt-cucumber)  
🚀 **Available on Conan Center:** [cwt-cucumber](https://conan.io/center/recipes/cwt-cucumber)

CWT-Cucumber interprets **Gherkin feature files**, letting you write test cases in plain language and define behavior-driven development (BDD) scenarios in C++.

Tested compilers: GCC 13, Clang17, MSVC 19  

Thanks to [Jörg Kreuzberger](https://github.com/kreuzberger), who has contributed and tested a lot.  

---

### Quick Example

Feature file (`examples/features/1_first_scenario.feature`):

```gherkin
Feature: Putting apples into a box

  Scenario: Lets start with two apples
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 items
```
Step implementation:
```cpp
GIVEN(init_box, "An empty box")
{
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), 0);
}
WHEN(add_item, "I place {int} x {string} in it")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);

  cuke::context<box>().add_items(item, count);
}
THEN(box_items_count, "The box contains {int} item(s)")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}
```

## Disclaimer

This project has started as an educational project after reading "Crafting Interpreters" by Robert Nystorm. It has evolved into a stable production ready state and I'm happy to maintain it. All of the work here is done in my free time. 

## Found a Bug? Need a Feature?
Don't hesitate and open an Issue.  

Cheers 🍻  
  
2025 Coding with Thomas  
https://www.codingwiththomas.com/
