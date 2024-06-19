[![CI](https://github.com/ThoSe1990/cucumber-cpp/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/ThoSe1990/cucumber-cpp/actions/workflows/main.yml) [![Build Status](https://dev.azure.com/thomassedlmair/cwt-cucumber/_apis/build/status%2FThoSe1990.cwt-cucumber?branchName=main)](https://dev.azure.com/thomassedlmair/cwt-cucumber/_build/latest?definitionId=14&branchName=main)


## Coding With Thomas Cucumber: A C++ Cucumber Interpreter

👋 Hello and Welcome to my C++20 Cucumber interpreter 🥒🚀✨

## Table of Contents

1. [Getting Started](#getting-started)
1. [Implementing Steps](#implementing-steps)
    1. [Steps and Value Access](#steps-and-value-access)
    1. [Scenario Context](#scenario-context)
    1. [Scenario Outline](#scenario-outline)
    1. [Background](#background)
    1. [Doc Strings](#doc-strings)
    1. [Tables](#tables)
        1. [Option 1: Raw Access](#option-1-raw-access)
        1. [Option 2: Hashes](#option-2-hashes)
        1. [Option 3: Key/Value Pairs or Rows Hash](#option-3-keyvalue-pairs-or-rows-hash)
    1. [Tags](#tags)
    1. [Hooks](#hooks)
    1. [Tagged Hooks](#tagged-hooks)
    1. [Run Single Scenarios / Directories](#run-single-scenarios--directories)
1. [Disclaimer](#disclaimer)
1. [Found A Bug? Need A Feature?](#found-a-bug-need-a-feature)


Tested compilers: GCC 13, Clang17 and MSVC 19
Full documentation: https://those1990.github.io/cwt-cucumber-docs
Conan Recipe: https://github.com/ThoSe1990/cwt-cucumber-conan

## Getting Started

Let us start with Cucumber. First of all build the project on your machine with CMake (you can use the Docker from this directory if you want): 

```shell
cmake -S . -B ./build
cmake --build ./build
```

Now, consider a simple box where we can put items. To run Cucumber tests, we create a scenario: 

```gherkin
Feature: My first feature
  This is my cucumber-cpp hello world

  Scenario: First Scenario
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 item(s)
```
Now execute the first example from the example directory:

```shell
./build/bin/box ./examples/features/1_first_scenario.feature

Feature: My first feature  ./examples/features/1_first_scenario.feature:2

Scenario: First Scenario  ./examples/features/1_first_scenario.feature:5
[   PASSED    ] An empty box  ./examples/features/1_first_scenario.feature:6
[   PASSED    ] I place 2 x "apple" in it  ./examples/features/1_first_scenario.feature:7
[   PASSED    ] The box contains 2 item(s)  ./examples/features/1_first_scenario.feature:8

1 Scenarios (1 passed)
3 Steps (3 passed)
```

## Implementing Steps

Include `cucumber.hpp` and start implementing your steps. In this example, we will use a simple box class, which should be self-explanatory:


```cpp 
#include "cucumber.hpp"
#include "box.hpp"


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

THEN(check_box_size, "The box contains {int} item(s)")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}
```

### Steps and Value Access

To implement a step, use a define with a function name and a step name. There is no technical difference between these definitions:
- `GIVEN(function_name, "your step goes here")` 
- `WHEN(function_name, "your step goes here")` 
- `THEN(function_name, "your step goes here")` 
- Alternatively just use `STEP(function_name, "your step goes here")` 

Use `CUKE_ARG(..)` to access a value in a step. Start with index `1` for the first value. I have overloaded the underlying function with the implicit conversion operator. So always declare a variable to access values:

```cpp
STEP(your_function, "A {string}, an {int} and a {float}")
{
  std::string str = CUKE_ARG(1);
  int i = CUKE_ARG(2);
  float f = CUKE_ARG(3);
  // ...
}
```

The values are implemted as [Cucumber expressions](https://github.com/cucumber/cucumber-expressions) and currently supported are: `{byte}`, `{short}`, `{int}`, `{long}`, `{float}`, `{double}`, `{string}`. 

### Scenario Context

The scenario context or `cuke::context` stores an object for the duration of a scenario. After the scenario is finished, the object is destroyed. One instance of each type can be stored in the scenario context.

Recall these steps from the first example:

```cpp
WHEN(add_item, "I place {int} x {string} in it")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);

  cuke::context<box>().add_items(item, count);
}

THEN(check_box_size, "The box contains {int} item(s)")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}
```

When initializing your object, you can pass all arguments to the context: 

```cpp
// forwards 1,2,3 to your object: 
cuke::context<some_object>(1,2,3);
// access or default initialize your object: 
cuke::context<some_object>();
```



### Scenario Outline
In a scenario outline you can define variables and run a scenario with different values:

```gherkin
Feature: My first feature

  Scenario Outline: First Scenario Outline
    Given An empty box
    When I place <count> x <item> in it
    Then The box contains <count> item(s)

    Examples:
      | count | item      |
      | 1     | "apple"   |
      | 2     | "bananas" |
```

Which gives this output:

```shell
./build/bin/box ./examples/features/2_scenario_outline.feature

Feature: My first feature  ./examples/features/2_scenario_outline.feature:2

Scenario Outline: First Scenario Outline  ./examples/features/2_scenario_outline.feature:5
[   PASSED    ] An empty box  ./examples/features/2_scenario_outline.feature:6
[   PASSED    ] I place <count> x <item> in it  ./examples/features/2_scenario_outline.feature:7
[   PASSED    ] The box contains <count> item(s)  ./examples/features/2_scenario_outline.feature:8
  With Examples:
  | count | item      |
  | 1     | "apple"   |

Scenario Outline: First Scenario Outline  ./examples/features/2_scenario_outline.feature:5
[   PASSED    ] An empty box  ./examples/features/2_scenario_outline.feature:6
[   PASSED    ] I place <count> x <item> in it  ./examples/features/2_scenario_outline.feature:7
[   PASSED    ] The box contains <count> item(s)  ./examples/features/2_scenario_outline.feature:8
  With Examples:
  | count | item      |
  | 2     | "bananas" |


2 Scenarios (2 passed)
6 Steps (6 passed)
```

### Background
A background is a set of steps (or a single step) which are the first steps of every `Scenario` in a `Feature`. After the feature definition add `Background`, see `./examples/features/3_background.feature`:

```gherkin
Feature: We always need apples!

  Background: Add an apple 
    Given An empty box
    When I place 1 x "apple" in it

  Scenario: Apples Apples Apples
    When I place 1 x "apple" in it
    Then The box contains 2 item(s)

  Scenario: Apples and Bananas
    When I place 1 x "apple" in it
    And I place 1 x "banana" in it
    Then The box contains 3 item(s)
```

### Doc Strings

Append a doc string to a defined step in your feature file: 

```gherkin
  Scenario: Doc string with quotes
    When There is a doc string:
    """
    This is a docstring with quotes
    after a step
    """
```

There is no parameter in the step definition needed. 
  
To access a doc string use ``CUKE_DOC_STRING()``: 

```cpp 
// There is no parameter needed in your step
WHEN(doc_string, "There is a doc string:")
{
  // and now you can use it here: 
  const std::string& str = CUKE_DOC_STRING();
  // .. 
}
```

### Tables

Similar to doc strings, you can append tables to a defined step. Then there are three different options to access the values. To create a table in your step definition use: 
- `const cuke::table& t = CUKE_TABLE();` or as copy
- `cuke::table t = CUKE_TABLE();`

You can directly access the elements with the `operator[]`. But the underlying value is a `cuke::value` which you have to cast accordingly with `as` or `to _string`.

```cpp 
const cuke::table& t = CUKE_TABLE();
t[0][0].to_string();
t[0][1].as<int>();
// ... 
```

#### Option 1: Raw Access

First we look at a raw table. This means there is no header line or identifiers for the given values:

```gherkin
Scenario: Adding items with raw
  Given An empty box
  When I add all items with raw():
    | apple      | 2 |
    | strawberry | 3 |
    | banana     | 5 |
  Then The box contains 10 item(s)
```

You can iterate over this table with `raw()`:

```cpp
WHEN(add_table_raw, "I add all items with raw():")
{
  // create a table 
  const cuke::table& t = CUKE_TABLE();

  // with raw() you iterate over all rows 
  for (const auto& row : t.raw())
  {
    // and with the operator[] you get access to each cell in each row
    cuke::context<box>().add_items(row[0].to_string(), row[1].copy_as<long>());
  }
}
```


#### Option 2: Hashes

With an additional header in the table we can make this table more descriptive:

```gherkin
Scenario: Adding items with hashes
  Given An empty box
  When I add all items with hashes():
    | ITEM   | QUANTITY |
    | apple  | 3        |
    | banana | 6        |
  Then The box contains 9 item(s)
```

You can now iterate over the table using `hashes()` and  access the elements with string literals:

```cpp 
WHEN(add_table_hashes, "I add all items with hashes():")
{
  const cuke::table& t = CUKE_TABLE();
  for (const auto& row : t.hashes())
  {
    cuke::context<box>().add_items(row["ITEM"].to_string(), row["QUANTITY"].as<long>());
  }
}
```


#### Option 3: Key/Value Pairs or Rows Hash

Another more descriptive way works for key value pairs, or rows hash. The first column describes the element, the second holds the element:

```gherkin
Scenario: Adding items with rows_hash
  Given An empty box
  When I add the following item with rows_hash():
    | ITEM     | really good apples |
    | QUANTITY | 3                  |
  Then The box contains 3 item(s)
```

And with `cuke::table::pair hash_rows = t.rows_hash();` you can create this hash map. The access to each element is again by the string literal. 

```cpp 
WHEN(add_table_rows_hash, "I add the following item with rows_hash():") 
{
  const cuke::table& t = CUKE_TABLE();
  // cuke::table::pair is just an alias for a std::unordered_map which gets created in rows.hash()
  cuke::table::pair hash_rows = t.rows_hash();

  cuke::context<box>().add_items(hash_rows["ITEM"].to_string(), hash_rows["QUANTITY"].as<long>());
}
```


### Tags 

Use the terminal option `-t` or `--tags` to provide tags. This will then check the given condition with tagged scenario and execute them accordingly. Consider this feature file: 


```gherkin
Feature: Scenarios with tags

  @apples
  Scenario: Apple
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 item(s)

  @apples @bananas
  Scenario: Apples and Bananas
    Given An empty box
    When I place 2 x "apple" in it
    And I place 2 x "banana" in it
    Then The box contains 4 item(s)
```

And when we run this with tags, we can control which scenarios are executed.

This executes both scenarios:
```shell
./build/bin/box ./examples/features/4_tags.feature -t "@apples or @bananas"
```
And this would just execute the second scenario due to the `and` condition:
```shell
./build/bin/box ./examples/features/4_tags.feature -t "@apples and @bananas"
```

Note: Tags can be applied to `Feature:`, `Scenario:`, `Scenario Outline:` and `Examples:`.

### Hooks 

Hooks are executed before and after each scenario or step. The implementation is pretty straightforward. You can have multiple hooks of the same type. All of them are executed at their time. 

```cpp
// ./examples/hooks.cpp: 
BEFORE(before)
{
  // this runs before every scenario
}
AFTER(after)
{
  // this runs after every scenario
}
BEFORE_STEP(before_step)
{
  // this runs before every step
}
AFTER_STEP(after_step)
{
  // this runs after every step
}
```


### Tagged Hooks

You can add a tag expression to your hook. Use  
- `BEFORE_T(name, "tags come here")` for a tagged hook before a scenrio
- `AFTER_T(name, "tags come here")` for a tagged hook after a scenario
  
This means a tagged hook is executed, when a scenario fulfills the given condition. You can pass in any logical expression to a tagged hook:

```cpp
// ./examples/hooks.cpp

AFTER_T(dispatch_box, "@ship or @important")
{
  std::cout << "The box is shipped!" << std::endl;
}
```

```gherkin
Feature: Scenarios with tags

  @ship 
  Scenario: We want to ship cucumbers
    Given An empty box
    When I place 1 x "cucumber" in it
    Then The box contains 1 item(s)

  @important
  Scenario: Important items must be shipped immediately
    Given An empty box
    When I place 2 x "important items" in it
    Then The box contains 2 item(s)
```

And now we can see that our box was shipped:

```shell
./build/bin/box ./examples/features/5_tagged_hooks.feature

Feature: Scenarios with tags  ./examples/features/5_tagged_hooks.feature:1

Scenario: We want to ship cucumbers  ./examples/features/5_tagged_hooks.feature:4
[   PASSED    ] An empty box  ./examples/features/5_tagged_hooks.feature:5
[   PASSED    ] I place 1 x "cucumber" in it  ./examples/features/5_tagged_hooks.feature:6
[   PASSED    ] The box contains 1 item(s)  ./examples/features/5_tagged_hooks.feature:7
The box is shipped!

Scenario: Important items must be shipped immediately  ./examples/features/5_tagged_hooks.feature:10
[   PASSED    ] An empty box  ./examples/features/5_tagged_hooks.feature:11
[   PASSED    ] I place 2 x "important items" in it  ./examples/features/5_tagged_hooks.feature:12
[   PASSED    ] The box contains 2 item(s)  ./examples/features/5_tagged_hooks.feature:13
The box is shipped!


2 Scenarios (2 passed)
6 Steps (6 passed)
```


### Run Single Scenarios / Directories

If you only want to run single scenarios, you can append the appropriate line to the feature file:

This runs a Scenario in Line 6:
```shell
./build/bin/box ./examples/features/box.feature:6
```
This runs each Scenario in line 6, 11, 14:
```shell
./build/bin/box ./examples/features/box.feature:6:11:14
```

If you want to execute all feature files in a directory (and subdirectory), just pass the directory as argument:
```shell
./build/bin/box ./examples/features
```

## Disclaimer

This is a fun/educational project for me. After reading "Crafting Interpreters" by Robert Nystorm, I wanted a meaningful, not too complex example. Since I really like and use Cucumber, this seems like a perfect project to me. I'm not getting paid for this and I do all the implementation in my free time aside from work. 

## Found A Bug? Need A Feature?
Don't hesitate and open an Issue.  

  
    

Cheers 🍻  
  
2024 Coding with Thomas  
https://www.codingwiththomas.com/  
