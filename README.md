[![CI](https://github.com/ThoSe1990/cwt-cucumber/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/ThoSe1990/cwt-cucumber/actions/workflows/main.yml) [![Build Status](https://dev.azure.com/thomassedlmair/cwt-cucumber/_apis/build/status%2FThoSe1990.cwt-cucumber?branchName=main)](https://dev.azure.com/thomassedlmair/cwt-cucumber/_build/latest?definitionId=14&branchName=main)


👋 Hello and Welcome to my C++ Cucumber interpreter 🥒🚀✨

This is a fun/educational project for me. After reading "Crafting Interpreters" by Robert Nystorm, I wanted a meaningful, not too complex example. Since I really like and use Cucumber, this seems like a perfect project to me.


## Getting Started

Find the full documentation [on GitHub Pages](https://those1990.github.io/cucumber-cpp-docs).
If you want to integrate this into your own projects, I have provided the same examples and a Conan recipe in [another GitHub repo](https://github.com/ThoSe1990/cucumber-cpp-conan).


In the `./examples` directory there are several examples for `cucumber-cpp`. So let's build the interpreter and run the first scenario:

```shell
cmake -S . -B ./build
cmake --build ./build
./build/bin/box ./examples/features/1_first_scenario.feature

Feature: My first feature  ./examples/features/1_first_scenario.feature:2

Scenario: First Scenario  ./examples/features/1_first_scenario.feature:5
[   PASSED    ] An empty box  ./examples/features/1_first_scenario.feature:6
[   PASSED    ] I place 2 x "apple" in it  ./examples/features/1_first_scenario.feature:7
[   PASSED    ] The box contains 2 item(s)  ./examples/features/1_first_scenario.feature:8

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

GIVEN(add_item, "I place {int} x {string} in it")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);

  for ([[maybe_unused]] int i = 0; i < count; i++)
  {
    cuke::context<box>().add_item(item);
  }
}

GIVEN(check_box_size, "The box contains {int} item(s)")
{
  const int items_count = CUKE_ARG(1);
  const box& my_box = cuke::context<box>();
  cuke::equal(my_box.items_count(), items_count);
}
```

And after the build, when we run the following feature file, we get the according output from above. 

```gherkin
Feature: My first feature
  This is my cucumber-cpp hello world

  Scenario: First Scenario
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 item(s)
```


### Steps / Value Access

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
GIVEN(add_item, "I place {int} x {string} in it")
{
  const std::size_t count = CUKE_ARG(1);
  const std::string item = CUKE_ARG(2);

  for ([[maybe_unused]] int i = 0; i < count; i++)
  {
    cuke::context<box>().add_item(item);
  }
}

GIVEN(check_box_size, "The box contains {int} item(s)")
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


### Single Scenarios / Directories

If you want to just run single scenarios, you can append the according line to the feature file:

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


## Found A Bug? 
Don't hesitate and open an Issue.

  
  

Cheers 🍻

2024 Coding with Thomas
https://www.codingwiththomas.com/