[![CI](https://github.com/ThoSe1990/cucumber-cpp/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/ThoSe1990/cucumber-cpp/actions/workflows/main.yml)

## Coding With Thomas Cucumber: A C++ Cucumber Interpreter

👋🙋‍♂️🙋‍♀️ Hello and Welcome to my C++20 Cucumber interpreter 🥒🚀✨

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
1. [Custom Parameter Types](#custom-parameter-types)
    1. [Example: Pair of Integers](#example-pair-of-integers)
    1. [Example: Date-Range](#example-date-range)
1. [Catalog](#catalog)
1. [Disclaimer](#disclaimer)
1. [Found A Bug? Need A Feature?](#found-a-bug-need-a-feature)


Tested compilers: GCC 13, Clang17 and MSVC 19  
Conan (2.x) Recipe: https://github.com/ThoSe1990/cwt-cucumber-conan. I didn't push this project to conancenter yet, but you can just create the Conan package locally:  

```shell
git clone https://github.com/ThoSe1990/cwt-cucumber-conan.git
cd package
conan create . --version 2.3.1 --user cwt --channel stable
```

Thanks to [Jörg Kreuzberger](https://github.com/kreuzberger), who has contributed and tested a lot lately, which has improved this project a lot.

## Getting Started

Let us start with Cucumber. First of all build the project on your machine with CMake (you can use the Docker from this directory if you want): 

```shell
cmake -S . -B ./build
cmake --build ./build -j12
```

Provide `nlohmann-json 3.10.5`, in order to enable the [Catalog](#catalog) as json export. You can do so yourself or use `conan 2.x`, so that CMake's `find_package(nlohmann_json)` succeeds: 

```shell 
conan install . -of ./build --build missing
cmake -S . -B ./build -DCMAKE_TOOLCHAIN_FILE="./build/conan_toolchain.cmake"
cmake --build ./build -j12
```

Now, consider a simple box where we can put items. To run Cucumber tests, we create a scenario: 

```gherkin
Feature: My first feature
  This is my cucumber-cpp hello world

  Scenario: First Scenario
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 items
```
Now execute the first example from the example directory:

```shell
./build/bin/example ./examples/features/1_first_scenario.feature

Feature: My first feature  ./examples/features/1_first_scenario.feature:2

Scenario: First Scenario  ./examples/features/1_first_scenario.feature:5
[   PASSED    ] An empty box  ./examples/features/1_first_scenario.feature:6
[   PASSED    ] I place 2 x "apple" in it  ./examples/features/1_first_scenario.feature:7
[   PASSED    ] The box contains 2 items  ./examples/features/1_first_scenario.feature:8

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

THEN(check_box_size, "The box contains {int} items")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}
```

If custom error messages for steps are necessary, we can append a string which replaces the default error message:

```cpp 
THEN(check_box_size, "The box contains {int} items")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count, "Items count is wrong at this point!");
}
```

### Alternative Words in Steps 

We can use the backslash between two words and parenthesis around words or characters in order to use alternative words in step definitions. Consider Following step: 

```cpp 
THEN(alternative_words, "{int} item(s) is/are {string}")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);
  cuke::equal(count, cuke::context<box>().count(item));
}
```

And now I can write this Scenario: 

```gherkin 
  Scenario: Alternative Words 
    Given An empty box
    When I place 1 x "banana" in it
    Then 1 item is "banana" 
    And I place 1 x "banana" in it 
    Then 2 items are "banana" 
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

The values are implemted as [Cucumber expressions](https://github.com/cucumber/cucumber-expressions) and currently supported are: 
- Integer values: `{byte}`, `{short}`, `{int}`, `{long}`, 
- Floating point values: `{float}`, `{double}`
- Strings (in quotes `".."` `{string}`
- Word (w/o quotes) `{word}` -> read value as `std::string` in step definition
- Anonymous, any character sequence `{}` -> read value as `std::string` in step definition
- Custom Parameter Types: `{your type comes here}` -> check [Custom Parameter Types](#custom-parameter-types) for examples and how to use them 

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

THEN(check_box_size, "The box contains {int} items")
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
    Then The box contains <count> items

    Examples:
      | count | item      |
      | 1     | "apple"   |
      | 2     | "bananas" |
```

Which gives this output:

```shell
./build/bin/example ./examples/features/2_scenario_outline.feature

Feature: My first feature  ./examples/features/2_scenario_outline.feature:2

Scenario Outline: First Scenario Outline  ./examples/features/2_scenario_outline.feature:5
[   PASSED    ] An empty box  ./examples/features/2_scenario_outline.feature:6
[   PASSED    ] I place 1 x "apple" in it  ./examples/features/2_scenario_outline.feature:7
[   PASSED    ] The box contains 1 items  ./examples/features/2_scenario_outline.feature:8

Scenario Outline: First Scenario Outline  ./examples/features/2_scenario_outline.feature:5
[   PASSED    ] An empty box  ./examples/features/2_scenario_outline.feature:6
[   PASSED    ] I place 2 x "bananas" in it  ./examples/features/2_scenario_outline.feature:7
[   PASSED    ] The box contains 2 items  ./examples/features/2_scenario_outline.feature:8

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
    Then The box contains 2 items

  Scenario: Apples and Bananas
    When I place 1 x "apple" in it
    And I place 1 x "banana" in it
    Then The box contains 3 items
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
  const std::string& str = CUKE_DOC_STRING();
  // or get each line in a vector: 
  const std::vector<std::string>& v = CUKE_DOC_STRING();
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
  When I add all items with the raw function:
    | apple      | 2 |
    | strawberry | 3 |
    | banana     | 5 |
  Then The box contains 10 items
```

You can iterate over this table with `raw()`:

```cpp
WHEN(add_table_raw, "I add all items with the raw function:")
{
  // create a table 
  const cuke::table& t = CUKE_TABLE();

  // with the raw function you iterate over all rows 
  for (const auto& row : t.raw())
  {
    // and with the operator[] you get access to each cell in each row
    cuke::context<box>().add_items(row[0].to_string(), row[1].as<long>());
  }
}
```


#### Option 2: Hashes

With an additional header in the table we can make this table more descriptive:

```gherkin
Scenario: Adding items with hashes
  Given An empty box
  When I add all items with the hashes function:
    | ITEM   | QUANTITY |
    | apple  | 3        |
    | banana | 6        |
  Then The box contains 9 items
```

You can now iterate over the table using `hashes()` and  access the elements with string literals:

```cpp 
WHEN(add_table_hashes, "I add all items with the hashes function:")
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
  When I add the following item with the rows_hash function:
    | ITEM     | really good apples |
    | QUANTITY | 3                  |
  Then The box contains 3 items
```

And with `cuke::table::pair hash_rows = t.rows_hash();` you can create this hash map. The access to each element is again by the string literal. 

```cpp 
WHEN(add_table_rows_hash, "I add the following item with the rows_hash function:") 
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

  @apples @bananas
  Scenario: Apples and Bananas
    Given An empty box
    When I place 2 x "apple" in it
    And I place 2 x "banana" in it
    Then The box contains 4 items

  Scenario Outline: 
    Given An empty box
    When I place <count> x <fruit> in it
    Then The box contains <expected> items

    @oranges 
    Examples: 
      | count | fruit    | expected |
      | 1     | "orange" | 1        |

    @oranges @others
    Examples: 
      | count | fruit        | expected |
      | 3     | "oranges"    | 3        |
      | 1     | "some stuff" | 1        |
```

And when we run this with tags, we can control which scenarios are executed.

This executes both scenarios:
```shell
./build/bin/example ./examples/features/4_tags.feature -t "@apples or @bananas"
```
And this would just execute the second scenario due to the `and` condition:
```shell
./build/bin/example ./examples/features/4_tags.feature -t "@apples and @bananas"
```

Note: Tags can be applied to `Feature:`, `Scenario:`, `Scenario Outline:` and `Examples:`. The tags are inherited to the next child. 

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
BEFORE_ALL(before_all)
{
  // this runs at program start 
}
AFTER_ALL(after_all)
{
  // this runs at program end 
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
    Then The box contains 1 items

  @important
  Scenario: Important items must be shipped immediately
    Given An empty box
    When I place 2 x "important items" in it
    Then The box contains 2 items
```

And now we can see that our box was shipped:

```shell
./build/bin/example ./examples/features/5_tagged_hooks.feature

Feature: Scenarios with tags  ./examples/features/5_tagged_hooks.feature:1

Scenario: We want to ship cucumbers  ./examples/features/5_tagged_hooks.feature:4
[   PASSED    ] An empty box  ./examples/features/5_tagged_hooks.feature:5
[   PASSED    ] I place 1 x "cucumber" in it  ./examples/features/5_tagged_hooks.feature:6
[   PASSED    ] The box contains 1 items  ./examples/features/5_tagged_hooks.feature:7
The box is shipped!

Scenario: Important items must be shipped immediately  ./examples/features/5_tagged_hooks.feature:10
[   PASSED    ] An empty box  ./examples/features/5_tagged_hooks.feature:11
[   PASSED    ] I place 2 x "important items" in it  ./examples/features/5_tagged_hooks.feature:12
[   PASSED    ] The box contains 2 items  ./examples/features/5_tagged_hooks.feature:13
The box is shipped!


2 Scenarios (2 passed)
6 Steps (6 passed)
```

### Skip Steps 

We can use `cuke::skip_scenario()` inside a hook `BEFORE_T` in order to skip a `Scenario`. This can be useful when using a `@skip`, `@ignore` or `@wip` tag: 

```cpp
BEFORE_T(skip, "@skip") 
{ 
    cuke::skip_scenario();
}
```

Note: The hook `AFTER_ALL` still will be executed. The hook `AFTER` the skipped `Scenario` is not called.


### Run Single Scenarios / Directories

If you only want to run single scenarios, you can append the appropriate line to the feature file:

This runs a Scenario in Line 6:
```shell
./build/bin/example ./examples/features/box.feature:6
```
This runs each Scenario in line 6, 11, 14:
```shell
./build/bin/example ./examples/features/box.feature:6:11:14
```

If you want to execute all feature files in a directory (and subdirectory), just pass the directory as argument:
```shell
./build/bin/example ./examples/features
```

## Custom Parameter Types

CWT-Cucumber supports custom parameter types. This means that we can define custom expressions in our steps to introduce custom types (so to speak) and make the step definition more understandable.  
  
In general: A custom parameter type is an individually defined type that we can use in the step definition. So we give the parameter a function name (as in a step), give the custom type a meaningful name, a description and a regex pattern. Then we implement a callback to consume the capture groups from the regex pattern. Here we use `CUSTOM_PARAMETER(function-name, "{here goes your type}", "regex pattern", "description")`.  

- Function-name: A defined function name (same as in steps) 
- Custom-Type: Define the type you want, **with curly braces** as string 
- Regex-Pattern: The regex pattern to match the step, you can use raw string literals, which makes it easier to write regex pattern (see below)
- Description: A string value to give a meaning full description. This will be printed to the catalog and has no effect on the scenarios.
  
In order to access the capture groups, use `CUKE_PARAM_ARG(index)` where the index starts at 1 from left to right.

**Note: You must explicitly return the dedicated type in the callback. The implementation uses type erasure and does not know which type will be used later.**
   
Find all implementations in `examples/step_definition.cpp` and the examples in `examples/features/8_custom_parameters.feature`. 

### Example: Pair of Integers

Lets define a type `{pair of integers}`, which will create a `std::pair<int,int>`: 

```cpp 
CUSTOM_PARAMETER(custom, "{pair of integers}", R"(var1=(\d+), var2=(\d+))", "a pair of integers")
{
  int var1 = CUKE_PARAM_ARG(1);
  int var2 = CUKE_PARAM_ARG(2);
  return std::make_pair(var1, var2);
}
```

And now we can use this type in our step definition. And note this type no uses a single index with `CUKE_ARG(1)` and returns a `std::pair<int,int>`:

```cpp 
WHEN(custom_par_when, "this is {pair of integers}")
{
  std::pair<int, int> p = CUKE_ARG(1);
  cuke::context<const std::pair<int, int>>(p);
}
```

And a possible scenario looks like this: 

```gherkin
Scenario: An example 
    When this is var1=123, var2=99
    Then their values are 123 and 99
```

### Example: Date-Range

A more complex example is defined below. We want to parse an event (which is represented as string) and two dates. Therefore we add following regex patterns:
- Event in single quotes: `'(.*?)'` and
- The dates in the format: `from November 11, 2024 to December 12, 2024`: `from ([A-Za-z]+) (\d{1,2}), (\d{4}) to ([A-Za-z]+) (\d{1,2}), (\d{4})`.  
  
`{event}` is fairly easy here: 

```cpp
CUSTOM_PARAMETER(custom_event, "{event}", R"('(.*?)')", "a custom event")
{
  return CUKE_PARAM_ARG(1).to_string();
}
```

`{date}` requires two structs and in with it we want to `date_range`:

```cpp 
struct date
{
  int day;
  std::string month;
  int year;
};

struct date_range
{
  date start;
  date end;
};
```
  
In order to create the `date_range` we implement now the custom parameter: 

```cpp
CUSTOM_PARAMETER(
    custom_date, 
    "{date}",
    R"(from ([A-Za-z]+) (\d{1,2}), (\d{4}) to ([A-Za-z]+) (\d{1,2}), (\d{4}))",
    "a custom date pattern")
{
  date begin;
  begin.month = CUKE_PARAM_ARG(1).to_string();
  begin.day = int(CUKE_PARAM_ARG(2));
  begin.year = CUKE_PARAM_ARG(3).as<int>();

  date end;
  end.month = CUKE_PARAM_ARG(4).to_string();
  end.day = static_cast<int>(CUKE_PARAM_ARG(5));
  end.year = CUKE_PARAM_ARG(6).as<int>();

  return date_range{begin, end};
}
```

Note: When using structs, we have to be explicit about the types. You can either use static_casts or bracket initialization or you call `.as<..>()` on `CUKE_PARAM_ARG`.   
  
And this is now our step we want to use, where we put an arbitrary string in front to describe the date range:   
  
```cpp
WHEN(using_date, "{event} is {date}")
{
  std::string event = CUKE_ARG(1);
  date_range dr = CUKE_ARG(2);
  cuke::context<date_range>(dr);
}
```

Note: that the first argument here returns a string where we get the event and the second argument is `date_range`.
   
Now we can use this for a Scenario (check the full implementation in `examples/step_definition.cpp`):

```gherkin 
Scenario: Date example
    When 'The public festival in town' is from April 25, 2025 to Mai 13, 2025
    Then The beginning month is April and the ending month is Mai
```

### Strings in Custom Type Parameters 

There are two options in order to create a string value. Some compiler have problems with can not find the correct string type. Therefore we have two options to create a string value from a regex capture: 

Option 1: Dedicated `to_string()` function: 
```cpp 
CUKE_PARAM_ARG(..).to_string();
```
Option 2: Initialize a `std::string`
```cpp
std::string str = CUKE_PARAM_ARG(..)
```

## Catalog 

Run the compiled executable with the option `--steps-catalog` or `--steps-catalog-json` in order to print all implemented steps to stdout or a file. To print to a file just append a filepath to `--steps-catalog`. This does not write to `.feature` files and overwrites existing files. 

```shell 
./build/bin/example --steps-catalog

Step Definitions (catalog):
---------------------------
Given An empty box
When A {word} and {}
When I place {int} x {string} in it
When There is a doc string:
When There is a doc string as vector:
When I add all items with the raw function:
When I add all items with the hashes function:
When I add the following item with the rows_hash function:
Then They will match {string} and {string}
Then The {int}. item is {string}
Then The box contains {int} item(s)
Then {int} item(s) is/are {string}
Step doc string:
```

```shell 
./build/bin/example --steps-catalog-json

{
  "steps_catalog": [
    {
      "definition": "An empty box",
      "type": "Given",
      "var_types": []
    },
    {
      "definition": "A {word} and {}",
      "type": "When",
      "var_types": [
        "word",
        "anonymous"
      ]
    },
    {
      "definition": "I place {int} x {string} in it",
      "type": "When",
      "var_types": [
        "int",
        "string"
      ]
    },
    {
      "definition": "There is a doc string:",
      "type": "When",
      "var_types": []
    },
    {
      "definition": "There is a doc string as vector:",
      "type": "When",
      "var_types": []
    },
    {
      "definition": "I add all items with the raw function:",
      "type": "When",
      "var_types": []
    },
    {
      "definition": "I add all items with the hashes function:",
      "type": "When",
      "var_types": []
    },
    {
      "definition": "I add the following item with the rows_hash function:",
      "type": "When",
      "var_types": []
    },
    {
      "definition": "They will match {string} and {string}",
      "type": "Then",
      "var_types": [
        "string",
        "string"
      ]
    },
    {
      "definition": "The {int}. item is {string}",
      "type": "Then",
      "var_types": [
        "int",
        "string"
      ]
    },
    {
      "definition": "The box contains {int} item(s)",
      "type": "Then",
      "var_types": [
        "int"
      ]
    },
    {
      "definition": "{int} item(s) is/are {string}",
      "type": "Then",
      "var_types": [
        "int",
        "string"
      ]
    },
    {
      "definition": "doc string:",
      "type": "Step",
      "var_types": []
    }
  ]
}
```


## Disclaimer

This is a fun/educational project for me. After reading "Crafting Interpreters" by Robert Nystorm, I wanted a meaningful, not too complex example. Since I really like and use Cucumber, this seems like a perfect project to me. I'm not getting paid for this and I do all the implementation in my free time aside from work. 

## Found A Bug? Need A Feature?
Don't hesitate and open an Issue.  

  
    

Cheers 🍻  
  
2024 Coding with Thomas  
https://www.codingwiththomas.com/  
