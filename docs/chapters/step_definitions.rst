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
-----

Hooks allow you to execute code **before or after scenarios or steps**.  
They are easy to implement and can be used for setup, teardown, logging, or conditional scenario handling.  
Multiple hooks of the same type are allowed; they are executed in the order they are defined.

Basic Hooks
^^^^^^^^^^^

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
^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^

You can manually fail a scenario or a step from within hooks using:

``cuke::fail_scenario()``
"""""""""""""""""""""""""

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
"""""""""""""""""""""

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

Doc Strings are multi-line text blocks attached to a step, often used for JSON, configuration, or long messages.  

You can access a doc string using the macro ``CUKE_DOC_STRING()``. There are two options:

- As a single string containing the entire content  
- As a ``std::vector<std::string>`` containing each line

Example:

.. code-block:: cpp

  WHEN(doc_string, "There is a doc string:")
  {
      // Get the entire doc string as a single string
      const std::string& str = CUKE_DOC_STRING();

      // Or get each line separately in a vector
      const std::vector<std::string>& lines = CUKE_DOC_STRING();
  }

.. _subch-step-def-datatables:

Datatables
----------

Datatables provide tabular input for a step.  
You can access a table attached to a step using the macro ``CUKE_TABLE()``:

- As a const reference: ``const cuke::table& t = CUKE_TABLE();``  
- As a copy: ``cuke::table t = CUKE_TABLE();``

The table contains ``cuke::value`` objects. You can access values with ``operator[]`` and convert them using ``as<>()`` or ``to_string()``:

.. code-block:: cpp

  const cuke::table& t = CUKE_TABLE();
  std::string s = t[0][0].to_string();
  int i = t[0][1].as<int>();

There are three main ways to access table data:

Raw Access
^^^^^^^^^^

Use raw tables when there is no header row or descriptive column names:

.. code-block:: gherkin

  When I add all items with the raw function:
    | apple      | 2 |
    | strawberry | 3 |
    | banana     | 5 |

You can iterate over the table using ``raw()``:

.. code-block:: cpp

  WHEN(add_table_raw, "I add all items with the raw function:")
  {
      const cuke::table& t = CUKE_TABLE();

      for (const auto& row : t.raw())
      {
          cuke::context<box>().add_items(row[0].to_string(), row[1].as<long>());
      }
  }

Hashes
^^^^^^

Use hashes tables when your table has a header row to make it more descriptive:

.. code-block:: gherkin

  When I add all items with the hashes function:
    | ITEM   | QUANTITY |
    | apple  | 3        |
    | banana | 6        |

You can iterate over the table using ``hashes()`` and access elements by header name:

.. code-block:: cpp

  WHEN(add_table_hashes, "I add all items with the hashes function:")
  {
      const cuke::table& t = CUKE_TABLE();
      for (const auto& row : t.hashes())
      {
          cuke::context<box>().add_items(row["ITEM"].to_string(), row["QUANTITY"].as<long>());
      }
  }

Key/Value Pairs (Rows Hash)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The rows hash provides a convenient way to access table data using key/value pairs.  
The first column acts as the key, and the second column holds the value:

.. code-block:: gherkin

   When I add the following item with the rows_hash function:
    | ITEM     | really good apples |
    | QUANTITY | 3                  |

You can convert the table into a hash map using ``rows_hash()``:

.. code-block:: cpp

  WHEN(add_table_rows_hash, "I add the following item with the rows_hash function:")
  {
      const cuke::table& t = CUKE_TABLE();

      // cuke::table::pair is an alias for std::unordered_map
      cuke::table::pair hash_rows = t.rows_hash();

      cuke::context<box>().add_items(hash_rows["ITEM"].to_string(), hash_rows["QUANTITY"].as<long>());
  }

.. note::
   Always cast ``cuke::value`` objects to the correct type using ``as<>()`` or ``to_string()``.  
   This ensures proper handling of integers, strings, or floating-point values.



   .. _subch-step-def-custom-parameters:




Custom Parameter Types
----------------------

CWT-Cucumber allows defining **custom parameter types** to make step definitions more readable and expressive.  
A custom parameter type converts a matched value from a step into a user-defined type.

Syntax
^^^^^^

Use the macro ``CUSTOM_PARAMETER``:

.. code-block:: cpp

  CUSTOM_PARAMETER(function_name,
                   "{custom_type}",
                   R"(regex_pattern)",
                   "Description")
  {
      // access regex capture groups via CUKE_PARAM_ARG(index)
      // return the desired type
  }

.. note::
   It is recommended to use **raw string literals** ``R"( ... )"`` for the regex pattern.  
   Raw strings allow you to write backslashes and quotes literally, avoiding excessive escaping.  
   You'll notice it in the examples below.

A few notes on ``CUKE_PARAMETER``:
- The callback **must return the specified type**.  
- Capture groups in the regex start counting from **1**.  
- The description is used in catalogs for documentation purposes and does not affect scenario execution.

Example: Items with Weight
^^^^^^^^^^^^^^^^^^^^^^^^^^

Suppose you want to match a step like:

.. code-block:: gherkin

  When I put 30 kilograms of apples in it

You can define a custom parameter type for the quantity and item:

.. code-block:: cpp

  CUSTOM_PARAMETER(item_with_weight,
                   "{item with weight}",
                   R"((\d+) kilograms? of (\w+))",
                   "Item and its weight in kilograms")
  {
      int weight = CUKE_PARAM_ARG(1);
      std::string item = CUKE_PARAM_ARG(2);
      return std::make_pair(weight, item); // returns a pair<int, std::string>
  }

Then, the step definition can consume the value:

.. code-block:: cpp

  WHEN(add_item_with_weight, "I put {item with weight} in it")
  {
      std::pair<int, std::string> item_with_weight = CUKE_ARG(1);
      box& b = cuke::context<box>();
      b.add_item(item_with_weight.second);
      b.add_weight(item_with_weight.first);
  }

**Example Scenario:**

.. code-block:: gherkin

  Scenario: Items with more weight
    Given An empty box
    When I put 30 kilograms of apples in it 
    Then The box weights 30 kilograms
    And apples are in the box

Example: Shipping Date
^^^^^^^^^^^^^^^^^^^^^^

Suppose you want to handle a shipping date in your scenarios. First, define a structured type:

.. code-block:: cpp

  struct date
  {
      int year{0};
      int month{0};
      int day{0};
      auto operator<=>(const date& other) const = default;
      std::string to_string() { return std::format("{}-{}-{}", year, month, day); }
  };

Define a **custom parameter type** that parses the date from a step:

.. code-block:: cpp

  CUSTOM_PARAMETER(custom_parameter_date,
                   "{date}",
                   R"((\d{4})-(\d{2})-(\d{2}))",
                   "shipping date")
  {
      date shipping_date;
      shipping_date.year  = CUKE_PARAM_ARG(1).as<int>();
      shipping_date.month = CUKE_PARAM_ARG(2).as<int>();
      shipping_date.day   = CUKE_PARAM_ARG(3).as<int>();
      return shipping_date;
  }

Consume the value in a step definition:

.. code-block:: cpp

  WHEN(ship_the_box, "The box gets shipped at {date}")
  {
      date shipping_date = CUKE_ARG(1);
      cuke::context<date>(shipping_date); // store in scenario context
  }

Use it later in another step:

.. code-block:: cpp

  THEN(box_label, "The box is labeled with: {string}")
  {
      std::string shipping_date = cuke::context<date>().to_string();
      std::string box_label = CUKE_ARG(1);
      cuke::is_true(box_label.find(shipping_date) != std::string::npos);
  }

**Example Scenario:**

.. code-block:: gherkin

  Scenario: Ship the box
    Given An empty box
    When The box gets shipped at 2025-11-12
    Then The box is labeled with: "box shipped at 2025-11-12"

