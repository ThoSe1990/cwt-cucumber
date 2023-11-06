[![Build Status](https://dev.azure.com/thomassedlmair/cwt-cucumber/_apis/build/status%2FThoSe1990.cwt-cucumber?branchName=main)](https://dev.azure.com/thomassedlmair/cwt-cucumber/_build/latest?definitionId=14&branchName=main)

# Coding With Thomas: Cucumber

This is a Cucumber interpreter written in C with C++ (17 or higher) bindings for ease of use. So my main goal was to have the basic implementation in C and an easy to use API for C++.
  
Find the full documentation for cwt-cucumber on <a href="https://those1990.github.io/cwt-cucumber/" target=_blank>GitHub Pages</a>.

## Disclaimer
This is a fun/educational project for me. After reading "Crafting Interpreters" by Robert Nystorm, I wanted a meaningful, not too complex example. Since I really like and use Cucumber, this seemed like a good project.
  

## CWT Cucumber for C++
The C++ API compiles its own `main`, which means you only need to implement the step definitions. I use a simple `box` class for demsontration (the `box` related methods should be self-explanatory). 

### Implementing Steps - First Example
Include the `cwt/cucumber.hpp` header and give each step a function name and a step name. The details (assertions, scenario context, defines) <a href="https://those1990.github.io/cwt-cucumber/" target="_blank">GitHub Pages</a>.

```cpp 
#include "cwt/cucumber.hpp"
#include "box.hpp"

GIVEN(box_init, "A box with {int} x {int} x {int}")
{
  const unsigned int w = CUKE_ARG(1);
  const unsigned int l = CUKE_ARG(2);
  const unsigned int h = CUKE_ARG(3);

  cuke::context<box>(w,l,h);
}

WHEN(box_open, "I open the box")
{
  cuke::context<box>().open(); 
}

THEN(box_is_open, "The box is open")
{
  cuke::is_true(cuke::context<box>().is_open());
}

THEN(box_volume, "The volume is {long}")
{
  const std::size_t volume = CUKE_ARG(1);
  cuke::equal(volume, cuke::context<box>().volume());  
}
```

And with this first feature file:

```gherkin
# ./examples/features/first_examples.feature

Feature: My First Feature File
  Just for demonstration

  Scenario: An arbitrary box
    Given A box with 2 x 2 x 2
    When I open the box
    Then The box is open 
    And The volume is 8 
```

Build the examples with CMake: 
```
cmake -S . -B ./build 
cmake --build ./build
```

We are ready to run this example:

```
$ ./build/bin/box ./examples/features/first_example.feature 

  Feature: My First Feature File  ./examples/features/first_example.feature:3

  Scenario: An arbitrary box  ./examples/features/first_example.feature:6
[   PASSED    ] A box with 2 x 2 x 2  ./examples/features/first_example.feature:7
[   PASSED    ] I open the box  ./examples/features/first_example.feature:8
[   PASSED    ] The box is open   ./examples/features/first_example.feature:9


1 Scenarios (1 passed)
3 Steps (3 passed)
```

### Steps / Value Access

To implement a step, use a define with a function name and a step name. There is no technical difference between these definitions:
- `GIVEN(function_name, "your step goes here")` 
- `WHEN(function_name, "your step goes here")` 
- `THEN(function_name, "your step goes here")` 
- Alternatively just use `STEP(function_name, "your step goes here")` 

Use `CUKE_ARG(..)` to access a value in a step. Start with index `1` for the first value. Use the corresponding type when you access the value, for instance:

```cpp
STEP(some_step, "A {string}, an {int} and a {float}")
{
  std::string str = CUKE_ARG(1);
  int i = CUKE_ARG(2);
  float f = CUKE_ARG(3);
  // ...
}
```

The values are implemted as [Cucumber expressions](https://github.com/cucumber/cucumber-expressions) and currently supported are: `{byte}`, `{short}`, `{int}`, `{long}`, `{float}`, `{double}`, `{string}`. 

Note: Use unsigned or const values in your implementation, if you need them:
```cpp
const unsigned int i = CUKE_ARG(2);
```

### Scenario Context

The scenario context or `cuke::context` stores an object for the duration of a scenario. After the scenario is finished, the object is destroyed. 

Recall these steps from the first example:

```cpp
GIVEN(box_init, "A box with {int} x {int} x {int}")
{
  const unsigned int w = CUKE_ARG(1);
  const unsigned int l = CUKE_ARG(2);
  const unsigned int h = CUKE_ARG(3);

  cuke::context<box>(w,l,h);
}
WHEN(box_open, "I open the box")
{
  cuke::context<box>().open(); 
}
```

We create in the given-step the `box` and forward all arguments to the constructor and in the when-step we modify the object to call the `open()` method from box. After the scenario the box is destroyed. 
  
This means: 

```cpp 
// this returns a reference to box
// if no box is in the context, the default constructor is called
cuke::context<box>();

// when you pass in arguments, and no box is in the context
// all values are forwarded to its constructor. 
// if a box already exists, the arguments are ignored.
cuke::context<box>(w,l,h);
```

In `cuke::context` you can put from each type one instance.

### Tags 

You can add tags to your Scenarios (and Scenario Outlines) as following:

```gherkin
# ./examples/features/tags.feature

Feature: Tags

  @small_boxes
  Scenario: An arbitrary box
    Given A box with 1 x 1 x 1
    Then The volume is 1

  @mid_sized_boxes
  Scenario: An arbitrary box
    Given A box with 10 x 15 x 12
    Then The volume is 1800

  @big_boxes
  Scenario: An arbitrary box
    Given A box with 90 x 80 x 70
    Then The volume is 504000
```

Now you can control the execution with `-t` or `--tags` with a tag expression (bool condition) inside the quotes and the corresponding tags:

```
$ ./build/bin/box ./examples/features/tags.feature -t "@small_boxes or @big_boxes"
```

The rules / syntax keywords are: 
- Write the tags with `@` symbol
- Logical operators: `and`, `or`, `xor`, `not`
- Parentheses `(`, `)`

So for instance this would be a valid statement: `"(@small_boxes and @big_boxes) or @mid_sized_boxes"`

If you don't pass `-t` or `--tags` to the program options, all Scenarios are executed.

### Scenario Outline
In a scenario outline you can define variables and run a scenario with different values:

```gherkin 
# ./examples/features/scenario_outline.feature

Feature: Scenario Outline 

  Scenario Outline: A lot of boxes 
    Given A box with <width> x <height> x <depth>
    Then The volume is <volume>

    Examples:
      | width | height | depth | volume |
      | 1     | 1      | 1     | 1      |
      | 1     | 2      | 3     | 6      |
      | 2     | 2      | 4     | 16     |
```

This Scenario is now executed three times, with each row of values. 
  
You can also add tags to examples. Begin a new table with the tag, followed by `Examples:` in the next line:

```gherkin
# ./examples/features/scenario_outline.feature
# ... 
  
  @mid_sized_boxes
  Examples:
    | width | height | depth | volume |
    | 10    | 5      | 10    | 500    |
    | 20    | 2      | 9     | 360    |
  
  @big_boxes
  Examples:
    | width | height | depth | volume  |
    | 200   | 99     | 150   | 2970000 |
    | 120   | 55     | 30    | 198000  |
```

The program option `-t` / `--tags` works exactly as before. Pass tags to execute the tags, without tags all examples/scenarios are executed. 

### Hooks 

Hooks are executed before and after each scenario or step. The implementation is pretty straightforward. Just use the dedicated hook defines and give the hook a unique function name (remove the comments to see the prints):

```cpp
// ./examples/cpp/step_definition.cpp: 

BEFORE(before)
{
  std::puts("this runs before every scenario");
}
AFTER(after)
{
  std::puts("this runs after every scenario");
}
BEFORE_STEP(before_step)
{
  std::puts("this runs before every step");
}
AFTER_STEP(after_step)
{
  std::puts("this runs after every step");
}
```


### Tagged Hooks

You can add a tag expression to your hooks (similar to `-t`/`--tags`). Use  
- `BEFORE_T(name, "tags come here")` for a hook before a scenrio
- `AFTER_T(name, "tags come here")` for a hook after a scenario
  
For example if we want to execute a hook only when it has the tags `@small_boxes` and `@open` we'd do this:

```cpp
// ./examples/cpp/step_definition.cpp: 

// a function name and tag expression (same for AFTER_T):
BEFORE_T(open_small_boxes, "@small_boxes and @open")
{
  // we create a box with some default values
  // and then we call immediately open()
  cuke::context<box>(1u,1u,1u).open();
}
```

Which means all scenarios with theses tags contain an opened box by default and this passes:

```gherkin
# ./examples/features/tags.feature
# ... 

  @small_boxes @open
  Scenario: An opened box
    Then The box is open 
```

### Background

A background is a set of steps (or a single step) which are the first steps of every `Scenario` in a `Feature`. After the feature definition add `Background`:

```gherkin
# ./examples/features/background.feature

Feature: We want default open boxes!

  Background: create an opened the box!
    Given A box with 2 x 2 x 2
    When I open the box

  Scenario: An opened box
    Then The box is open 
```

### Single Scenarios 

If you want to execute only single Scenarios, use the `-l` or `--lines` program option after the feature filepath. Run multiple feature files and append one or more lines: 

```
$ ./build/bin/box ./examples/features/box.feature -l 6
$ ./build/bin/box ./examples/features/box.feature -l 6 -l 18
$ ./build/bin/box ./examples/features/box.feature -l 6 -l 18 ./examples/features/scenario_outline.feature -l 12
```

### Executing All Files From A Directory

In the C++ implementation you can execute all feature files from a directory. Pass the directory as program option and all feature files in there are executed:

```
$ ./build/bin/box ./examples/features
```

### Use Your Own Main
There is also a `cucumber-no-main` target if you need your own main and implement more to you program. To execute the cucumber test you have to call the `init()` and `run()` method. The standard main looks like this:

```cpp
// ./src/cwt/implementation/main.cpp
#include "cwt/cucumber.hpp"

int main(int argc, const char* argv[])
{
  cuke::details::init(); 
  return cuke::details::run(argc, argv);
}
```

## Whats Missing

So what is missing? By now I can think of is:
- Comprehensive documentation (I'm working on that)
- Conan recipe (after first version tag)
- `-h` / `--help` option 
- Languages (currently only english keywords are implemented)
- Rules 
- Reports (json, ...) 
- ...


## And Finally ... 
... I'm currently on a sabbatical timeout from work and travel through Central America. If you open issues, find bugs or have anything for this project, please be patient. I have limited resources and time during traveling and in March/April 2024 I'm back home in my usual work environment.