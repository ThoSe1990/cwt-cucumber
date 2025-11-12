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

Hooks
-----

Tagged Hooks 
^^^^^^^^^^^^

Skip & Ignore Scenarios
^^^^^^^^^^^^^^^^^^^^^^^

Manual Fail Steps & Scenarios
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
