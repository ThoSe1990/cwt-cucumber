.. _include_01_cwt_cucumber:

============
CWT-Cucumber
============

.. note:: 
  I recommend using Conan for dependency management. I have a conan recipe in  `another GitHub repository <https://github.com/ThoSe1990/cwt-cucumber-conan>`_ to create the conan package. This will do the job for now (I will add instructions, once I'm done with the recipe). Later I can push it to conancenter, when this project is really in use.

The `./examples` directory contains various examples that have been created during the life of this project. All examples are compatible with C and C++ (17 or higher):

- `./examples/cpp`: Sources for the C++ implementation 
- `./examples/c`: Sources for the C implementation 
- `./examples/features`: Feature files to run the examples with either the C or C++ implementation 

There are no third party libraries, which means that all you need to do is a regular CMake build within this project directory: 

.. code-block:: sh 

  cmake -S . -B ./build 
  cmake --build ./build

In this documentation I'll focus on the C++ version. If there is a need for more detailed documentation on the C version, please let me know.

The context of all examples is a `box` class, without complex logic. Lets have a quick look at it, but it should be self-explanatory:

.. code-block:: cpp 

  // examples/cpp/box.hpp
  class box 
  {
    public:
      box () = default;
      box (unsigned int width, unsigned int height, unsigned int depth) : 
        m_width(width), m_height(height), m_depth(depth), m_weight(0), m_is_open(false) {}
      
      unsigned int volume() { return m_width * m_height * m_depth; }

      unsigned int front() { return m_width * m_height; }
      unsigned int side() { return m_depth * m_height; }
      unsigned int top() { return m_depth * m_width; }
      
      void set_weight(double weight) { m_weight = weight; }
      double weight() { return m_weight; }
      
      void open() { m_is_open = true; }
      void close() { m_is_open = false; }
      bool is_open() { return m_is_open; }

    private:
      unsigned int m_width;
      unsigned int m_height;
      unsigned int m_depth;
      double m_weight;
      bool m_is_open;
  };



Implementing Steps 
==================

In this chapter you will find all the information about implementing the step. First there are the macros to use, second there is the step itself and third there is the first example.

Step-Macros
-----------

There are four defines to implement steps. Each define requires a unique function name and the step definition. All four are exactly the same and it makes no difference which you use:

- ``STEP(function_name, "step definition goes here")``
- ``GIVEN(function_name, "step definition goes here")``
- ``WHEN(function_name, "step definition goes here")``
- ``THEN(function_name, "step definition goes here")``

Step Definition and Values 
--------------------------

The step definition itself is a string. All values are defined inside curly brackets ``{`` ``}`` as `Cucumber expression <https://github.com/cucumber/cucumber-expressions>`_. 

Example:

.. code-block:: cpp 
  
  GIVEN(func, "This is a step with a {string} and a {int}")

Wich later can be used as: 

.. code-block:: gherkin 
  
  Given This is a step with a "string value" and a 5


Currently supported values are: ``{byte}`` , ``{short}``,  ``{int}`` , ``{long}``, ``{float}`` , ``{double}`` and ``{string}``.

.. warning:: 
  A ``{string}`` has to be in double quotes in the feature file. Anonymus strings are not supported.


Example 
-------

CWT-Cucumber compiles already its own ``main``. This means that you only have to include your sources and ``cwt/cucumber.hpp``. Then use the provided defines to implement the step body: 

.. code-block:: cpp

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


Executing Tests
---------------

After compiling the examples there is `./build/bin/box` (this path can vary, depending on your operating system and defined build directory).

Execute `./build/bin/box` from the project root directory, with the feature file as first argument and you'll see the test result: 

.. code-block:: sh 

  $ ./build/bin/box ./examples/features/first_example.feature 

    Feature: My First Feature File  ./examples/features/first_example.feature:3

    Scenario: An arbitrary box  ./examples/features/first_example.feature:6
  [   PASSED    ] A box with 2 x 2 x 2  ./examples/features/first_example.feature:7
  [   PASSED    ] I open the box  ./examples/features/first_example.feature:8
  [   PASSED    ] The box is open   ./examples/features/first_example.feature:9


  1 Scenarios (1 passed)
  3 Steps (3 passed)


Evaluating a Step 
================= 

Use the assert functions in the step body. If this brings a negative result, the step is evaluated to ``FAILED`` and all following steps in a scenario are ``SKIPPED``. 
  
There are different assert functions. Use the most apropriate one, depending on your values. The values have to be compareable (see :ref:`api asserts`):

- ``cuke::equal(lhs, rhs)``
- ``cuke::not_equal(lhs, rhs)``
- ``cuke::greater(lhs, rhs)``
- ``cuke::greater_or_equal(lhs, rhs)``
- ``cuke::less(lhs, rhs)``
- ``cuke::less_or_equal(lhs, rhs)``
- ``cuke::is_true(condition)``
- ``cuke::is_false(condition)``


After the failing step the rest is skipped, as you can see here:  

.. code-block::

  $ ./build/bin/box ./examples/features/failing_example.feature 

    Feature: My First Feature File  ./examples/features/first_example.feature:3

    Scenario: An arbitrary box  ./examples/features/first_example.feature:6
  [   PASSED    ] A box with 2 x 2 x 2  ./examples/features/first_example.feature:7
  Value 99 is not equal to 8 in following step:
  [   FAILED    ] The volume is 99   ./examples/features/first_example.feature:8
  [   SKIPPED   ] I open the box  ./examples/features/first_example.feature:9
  [   SKIPPED   ] The box is open   ./examples/features/first_example.feature:10

  Failed Scenarios:
    Scenario: An arbitrary box    ./examples/features/first_example.feature:6

  1 Scenarios (1 skipped)
  4 Steps (1 failed, 2 skipped, 1 passed)


Accessing Values ``CUKE_ARG``
=============================

Use :ref:`api cuke_arg` in the step body to access a value. Start with index 1 for the first value. Use the corresponding type when you access the value, for instance:

.. code-block:: cpp 

  STEP(some_step, "A {string}, an {int} and a {float}")
  {
    std::string str = CUKE_ARG(1);
    int i = CUKE_ARG(2);
    float f = CUKE_ARG(3);
    // ...
  }

.. warning:: 
  ``auto`` type deduction does not work here. The underlying function is overloaded by returntype.

.. note:: 
  Declare your variables as ``const`` or ``unsigned`` if you need them ``const`` or ``unsigned``. 


Scenario Context ``cuke::context``
==================================

The scenario context or ``cuke::context`` stores an object for the duration of a scenario. After the scenario is finished, the object is destroyed.

Recall these steps from the first example:

.. code-block:: cpp

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

In one step we forward all arguments to the dedicated constructor and in the other modify the object. ``cuke::context`` returns a reference to the object, see :ref:`api cuke_context`. 

These are the key points for ``cuke::context``:

1. Destroys all objects at the end of a scenario 
2. From each type one instance can be hold 
3. A constructor is invoked once. If the object exists, it returns a reference to the object 
4. Given arguments are ignored when the object already exists


Tags ``-t`` / ``--tags``
========================

You can add tags to your Scenarios (and Scenario Outlines) as following:

.. code-block:: gherkin

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


Now you can control the execution with ``-t`` or ``--tags`` with a tag expression (bool condition) inside the quotes and the corresponding tags:

.. code-block:: sh

  $ ./build/bin/box ./examples/features/tags.feature -t "@small_boxes or @big_boxes"

The rules / syntax keywords are:

- Write the tags with ``@`` symbol
- Logical operators: ``and``, ``or``, ``xor``, ``not``
- Parentheses ``(``, ``)``

So for instance this would be a valid statement: ``"(@small_boxes and @big_boxes) or @mid_sized_boxes"``

If you don't pass ``-t`` or ``--tags`` to the program options, all Scenarios are executed.


Scenario Outline
================

In a scenario outline you can define variables and run a scenario with different values:


.. code-block:: gherkin 

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

This Scenario is now executed three times, with each row of values.

You can also add tags to examples. Begin a new table with the tag, followed by ``Examples:`` in the next line:

.. code-block:: gherkin 

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


The program option ``-t`` / ``--tags`` works exactly as before. Pass tags to execute the tags, without tags all examples/scenarios are executed.


Hooks
=====

:ref:`api hooks` are executed before and after each scenario or step. The implementation is pretty straightforward. Just use the dedicated hook defines and give the hook a unique function name (in the current examples I commented the prints, uncomment them if you want to see):

.. code-block:: gherkin 

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

When executing the ``first_example.feature`` you see (the scenario print comes after the execution of the hook): 

.. code-block:: sh 

  $ ./build/bin/box ./examples/features/first_example.feature 

    Feature: My First Feature File  ./examples/features/first_example.feature:3

  this runs before every scenario
    Scenario: An arbitrary box  ./examples/features/first_example.feature:6
  this runs before every step
  this runs after every step
  [   PASSED    ] A box with 2 x 2 x 2  ./examples/features/first_example.feature:7
  this runs before every step
  this runs after every step
  [   PASSED    ] I open the box  ./examples/features/first_example.feature:8
  this runs before every step
  this runs after every step
  [   PASSED    ] The box is open   ./examples/features/first_example.feature:9
  this runs before every step
  this runs after every step
  [   PASSED    ] The volume is 8   ./examples/features/first_example.feature:10
  this runs after every scenario


  1 Scenarios (1 passed)
  4 Steps (4 passed)


Tagged Hooks
============
You can add a tag expression to :ref:`api tagged_hooks` (similar to -t/--tags). Use

- ``BEFORE_T(name, "tags come here")`` for a hook before a scenrio 
- ``AFTER_T(name, "tags come here")`` for a hook after a scenario 

For example if we want to execute a hook only when it has the tags ``@small_boxes`` and ``@open`` we'd do this:

.. code-block:: cpp

  // ./examples/cpp/step_definition.cpp: 

  // a function name and tag expression (same for AFTER_T):
  BEFORE_T(open_small_boxes, "@small_boxes and @open")
  {
    // we create a box with some default values
    // and then we call immediately open()
    cuke::context<box>(1u,1u,1u).open();
  }

Which means all scenarios with theses tags contain an opened box by default and this passes:

.. code-block:: gherkin 

  # ./examples/features/tags.feature
  # ... 

    @small_boxes @open
    Scenario: An opened box
      Then The box is open 

.. code-block:: sh 

    Feature: Tags  ./examples/features/tags.feature:3

    Scenario: An opened box  ./examples/features/tags.feature:21
  [   PASSED    ] The box is open   ./examples/features/tags.feature:22


  1 Scenarios (1 passed)
  1 Steps (1 passed)


Background
==========

A background is a set of steps (or a single step) which are the first steps of every ``Scenario`` in a ``Feature``. After the feature definition add ``Background``:

.. code-block:: gherkin 

  # ./examples/features/background.feature

  Feature: We want default open boxes!

    Background: create an opened the box!
      Given A box with 2 x 2 x 2
      When I open the box

    Scenario: An opened box
      Then The box is open 

The ``Background`` is printed separately, before the scenario in the terminal: 

.. code-block:: sh 

  $ ./build/bin/box ./examples/features/background.feature 

    Feature: We want default open boxes!  ./examples/features/background.feature:3

    Background: create an opened the box!  ./examples/features/background.feature:5
  [   PASSED    ] A box with 2 x 2 x 2  ./examples/features/background.feature:6
  [   PASSED    ] I open the box  ./examples/features/background.feature:7
    Scenario: An opened box  ./examples/features/background.feature:9
  [   PASSED    ] The box is open   ./examples/features/background.feature:10


  1 Scenarios (1 passed)
  3 Steps (3 passed)


Single Scenarios ``-l`` / ``--line``
====================================


If you want to execute only single Scenarios, use the ``-l`` or ``--line`` program option after the feature filepath. Run multiple feature files and append one or more lines:

.. code-block:: sh

  $ ./build/bin/box ./examples/features/box.feature -l 6
  $ ./build/bin/box ./examples/features/box.feature -l 6 -l 18
  $ ./build/bin/box ./examples/features/box.feature -l 6 -l 18 ./examples/features/scenario_outline.feature -l 12


Executing All Files From A Directory
====================================

In the C++ implementation you can execute all feature files from a directory. Pass the directory as program option and all feature files in there are executed:

.. code-block:: sh

  $ ./build/bin/box ./examples/features

Using your own ``main`` 
=======================

There is also a ``cucumber-no-main`` target if you need your own main and implement more to you program. To execute the cucumber test you have to call the ``init()`` and ``run()`` method. The standard main looks like this:

.. code-block:: cpp 

  // ./src/cwt/implementation/main.cpp
  #include "cwt/cucumber.hpp"

  int main(int argc, const char* argv[])
  {
    cuke::details::init(); 
    return cuke::details::run(argc, argv);
  } 



Whats Missing
=============

So what is missing? By now I can think of is:
- Comprehensive documentation (I'm working on that)
- Conan recipe (after first version tag)
- ``-h`` / ``--help`` option 
- Languages (currently only english keywords are implemented)
- Rules 
- Reports (json, ...) 
- ...

If you have anything or in case I missed something, just reach out to me in any form. 