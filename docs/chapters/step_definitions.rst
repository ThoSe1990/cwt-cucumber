Step Definitions
================

Include ``cucumber.hpp`` (or ``cwt-cucumber/cucumber.hpp``, depending on your installation) to define and implement your steps.

Implementing Steps & Value Access
---------------------------------

Use the provided macros to define your steps:

- ``GIVEN(function_name, "your step goes here")``  
- ``WHEN(function_name, "your step goes here")``  
- ``THEN(function_name, "your step goes here")``  
- Alternatively, use ``STEP(function_name, "your step goes here")`` if you don’t want to specify the step type explicitly.

Example:

.. code-block:: cpp

  #include "cucumber.hpp"
  #include "box.hpp"

  GIVEN(init_box, "An empty box")
  {
    const box& my_box = cuke::context<box>();
    cuke::equal(my_box.items_count(), 0);
  }

  WHEN(add_item, "I place {int} x {string} in it")
  {
    const std::size_t count = CUKE_ARG(1); // {int}
    const std::string item = CUKE_ARG(2);  // {string}

    cuke::context<box>().add_items(item, count);
  }

  THEN(check_box_size, "The box contains {int} items")
  {
    const int items_count = CUKE_ARG(1); // {int}
    const box& my_box = cuke::context<box>();
    cuke::equal(my_box.items_count(), items_count);
  }

Accessing Values
^^^^^^^^^^^^^^^^^

Use ``CUKE_ARG(index)`` to access a value from a step by its index (starting at 1, left to right).  
The values are automatically converted according to their type.

Example:

.. code-block:: cpp

  STEP(your_function, "A {string}, an {int} and a {float}")
  {
    std::string str = CUKE_ARG(1); // {string}
    int i = CUKE_ARG(2);           // {int}
    float f = CUKE_ARG(3);         // {float}
    // ...
  }

.. note::
   ``CUKE_ARG`` relies on implicit conversion operators; declaring variables with ``auto`` will not work.

Supported Cucumber expressions include:

- Integer values: ``{byte}``, ``{short}``, ``{int}``, ``{long}``
- Floating point values: ``{float}``, ``{double}``
- Strings (quoted): ``{string}``
- Words (unquoted): ``{word}`` → read as ``std::string``
- Anonymous placeholders: ``{}`` → read as ``std::string``
- Custom Parameter Types: ``{your_type}`` → see :ref:`subch-step-def-custom-parameters` for examples

Asserts
^^^^^^^

Use the following functions to verify expected values:

- ``cuke::equal(val1, val2)``
- ``cuke::not_equal(val1, val2)``
- ``cuke::greater(val1, val2)``
- ``cuke::greater_or_equal(val1, val2)``
- ``cuke::less(val1, val2)``
- ``cuke::less_or_equal(val1, val2)``
- ``cuke::is_true(val)``
- ``cuke::is_false(val)``

Each assert function optionally accepts a custom error message:

.. code-block:: cpp

  THEN(check_box_size, "The box contains {int} items")
  {
    const int items_count = CUKE_ARG(1);
    const box& my_box = cuke::context<box>();
    cuke::equal(my_box.items_count(), items_count, "Items count is incorrect!");
  }

For more details, see :ref:`subch-api-ref-asserts`.

Scenario Context (``cuke::context``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The scenario context stores objects for the duration of a scenario.  
Objects are automatically destroyed at the end of the scenario. One instance per type can be stored per scenario.

Example:

.. code-block:: cpp

  // Forward constructor arguments
  cuke::context<some_object>(1, 2, 3);

  // Default initialization
  cuke::context<some_object>();

Accessing stored objects:

.. code-block:: cpp

  WHEN(add_item, "I place {int} x {string} in it")
  {
    const std::size_t count = CUKE_ARG(1); // {int}
    const std::string item = CUKE_ARG(2);  // {string}

    cuke::context<box>().add_items(item, count);
  }

  THEN(check_box_size, "The box contains {int} items")
  {
    const int items_count = CUKE_ARG(1); // {int}
    const box& my_box = cuke::context<box>();
    cuke::equal(my_box.items_count(), items_count);
  }

Word Alternations
-----------------

CWT-Cucumber supports **word alternations** in step definitions using parentheses `()` and slashes `/`.  
This allows a single step definition to match multiple word variations in your feature files.

Example:

.. code-block:: cpp

   THEN(alternative_words, "{int} item(s) is/are {string}")
   {
       // Implementation here
   }

With this step definition, you can write scenarios using either word form:

.. code-block:: gherkin

  Scenario: Alternative Words
    Given An empty box
    When I place 1 x "banana" in it
    Then 1 item is "banana"
    And I place 1 x "banana" in it
    Then 2 items are "banana"

Hooks
=====

Hooks allow you to execute code **before or after scenarios or steps**.  
They are easy to implement and can be used for setup, teardown, logging, or conditional scenario handling.  
Multiple hooks of the same type are allowed; they are executed in the order they are defined.

Basic Hooks
-----------

CWT-Cucumber provides several types of hooks:

.. code-block:: cpp

  BEFORE(before)
  {
      // Runs before every scenario
  }

  AFTER(after)
  {
      // Runs after every scenario
  }

  BEFORE_STEP(before_step)
  {
      // Runs before every step
  }

  AFTER_STEP(after_step)
  {
      // Runs after every step
  }

  BEFORE_ALL(before_all)
  {
      // Runs once at program start
  }

  AFTER_ALL(after_all)
  {
      // Runs once at program end
  }

Tagged Hooks
------------

Hooks can be limited to specific scenarios using **tag expressions**.  

- ``BEFORE_T(name, "tag expression")`` – runs before a scenario that matches the tag expression  
- ``AFTER_T(name, "tag expression")`` – runs after a scenario that matches the tag expression  

A tagged hook is executed **only** when a scenario matches the specified tag expression. You can use any logical combination of tags.

Example:

.. code-block:: cpp

  AFTER_T(dispatch_box, "@ship or @important")
  {
      std::cout << "The box is shipped!" << std::endl;
  }

.. code-block:: gherkin

  @ship
  Scenario: We want to ship cucumbers
    Given An empty box
    When I place 1 x "cucumber" in it
    Then The box contains 1 item

  @important
  Scenario: Important items must be shipped immediately
    Given An empty box
    When I place 2 x "important items" in it
    Then The box contains 2 items

Skipping & Ignoring Scenarios
-----------------------------

You can skip or ignore scenarios from within hooks:

- ``cuke::skip_scenario()`` – marks a scenario as skipped (reported as skipped)  
- ``cuke::ignore_scenario()`` – marks a scenario as ignored (not reported)  

Example:

.. code-block:: cpp

  BEFORE_T(skip, "@skip")
  {
      // Skip this scenario
      cuke::skip_scenario();
  }

  BEFORE_T(ignore, "@ignore")
  {
      // Ignore this scenario
      cuke::ignore_scenario();
  }

.. note::
   The ``AFTER_ALL`` hook is always executed.  
   Hooks ``AFTER`` and ``AFTER_T`` are **not** executed for skipped or ignored scenarios.

Manual Failures
---------------

You can manually fail a scenario or a step from within hooks using:

``cuke::fail_scenario()``
^^^^^^^^^^^^^^^^^^^^^^^^^^

Fails the entire scenario, optionally with a custom message:

.. code-block:: cpp

  BEFORE(before)
  {
      // Fail scenario due to a setup error
      cuke::fail_scenario();
      cuke::fail_scenario("Custom error message");
  }

- When called in a *before* hook:
  - All steps are skipped
  - The message is displayed with each skipped step
- When called in an *after* hook:
  - The scenario is marked as failed even if all steps passed

.. note::
   CWT-Cucumber reports the message under the steps; the scenario itself does not display a separate error.

``cuke::fail_step()``
^^^^^^^^^^^^^^^^^^^^^

Fails the current step, optionally with a custom message:

.. code-block:: cpp

  BEFORE_STEP(before_step)
  {
      // Fail the step due to an error condition
      cuke::fail_step();
      cuke::fail_step("Step failed due to invalid setup");
  }

- Intended for use in *before step* hooks
- Has no effect in *after step* hooks
- Marks the step as failed and shows the error message in output


.. _subch-step-def-doc-strings:
   
Doc Strings
-----------

.. _subch-step-def-datatables:

Datatables
----------

Raw Access 
^^^^^^^^^^

Hashes
^^^^^^

Key/Value Pairs
^^^^^^^^^^^^^^^

.. _subch-step-def-custom-parameters:

Custom Parameter Types
----------------------

Example: Pair of Integers
^^^^^^^^^^^^^^^^^^^^^^^^^

Example: Date-Range
^^^^^^^^^^^^^^^^^^^
