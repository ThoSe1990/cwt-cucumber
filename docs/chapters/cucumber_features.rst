Cucumber Features
=================

Cucumber uses a *Given–When–Then* style to describe expected behavior in a human-readable way.  
Each test suite is written as a **Feature**, and each test case within it is a **Scenario**.

Scenarios
---------

A *Scenario* defines a single, concrete example of how a system should behave.

.. code-block:: gherkin

  Feature: Putting items into a box
    This feature demonstrates how to put items into a box.

    Scenario: Putting apples into a box
      Given An empty box
      When I place 2 x "apple" in it
      Then The box contains 2 items

Each *Given–When–Then* step describes:

- **Given** – the initial context or precondition  
- **When** – an action performed by the user or system  
- **Then** – the expected outcome or assertion

If there are additional steps, you can use **And** and **But** to make them more readable.  
Alternatively, you can use the universal **\*** keyword.

.. code-block:: gherkin

  Feature: Putting items into a box
    This feature demonstrates how to put items into a box.

    Scenario: Adding multiple items
      Given An empty box
      When I place 2 x "apple" in it
      And I place 2 x "orange" in it
      Then The box contains 4 items
      But I remove the last item
      * The box contains 3 items

Scenarios are independent from one another and are executed in isolation.

Scenario Outlines
-----------------

A *Scenario Outline* allows you to run the same scenario multiple times with different data sets.  
Placeholders inside the scenario are replaced by values defined in the ``Examples`` table.

.. code-block:: gherkin

  Scenario Outline: Putting items into a box
    Given An empty box
    When I place <count> x "<item>" in it
    Then The box contains <count> items

    Examples:
      | item   | count |
      | apple  | 2     |
      | orange | 3     |

Each row in the ``Examples`` table executes the scenario once,  
substituting the placeholders ``<item>`` and ``<count>`` accordingly.

Background
----------

A *Background* section defines steps that are executed before each scenario in a feature.  
It is useful for common setup tasks shared by multiple scenarios.

.. code-block:: gherkin

  Feature: Each box has at least one apple

    Background:
      Given An empty box
      When I place 1 x "apple" in it

    Scenario: Adding oranges
      When I place 2 x "orange" in it
      Then The box contains 3 items

    Scenario: Adding bananas
      When I place 3 x "banana" in it
      Then The box contains 4 items

Doc Strings
-----------

**Doc Strings** pass multi-line text blocks directly into your step definition as a single string.  
They are often used for messages, JSON snippets, or configuration data.  
See :ref:`subch-step-def-doc-strings` for details on how to access them in C++.

.. code-block:: gherkin

  Scenario: A fragile box
    Given An empty box with additional information
      """
      Fragile!
      Please handle with care!
      """

Data Tables
-----------

**Data Tables** provide structured input for a step.  
CWT-Cucumber supports several formats.  
See :ref:`subch-step-def-datatables` for examples of how to handle tables in step definitions.

**Option 1:** Raw table – no header or descriptive column.

.. code-block:: gherkin

  Given An empty box
  When I add all items using the *raw()* function:
    | apple  | 3 |
    | banana | 6 |
  Then The box contains 9 items

**Option 2:** Hashes – include a header row to describe each column.

.. code-block:: gherkin

  Given An empty box
  When I add all items using the *hashes()* function:
    | ITEM   | QUANTITY |
    | apple  | 2        |
    | banana | 5        |
  Then The box contains 7 items

**Option 3:** Key/Value pairs – the first column defines the key, the second the value.

.. code-block:: gherkin

  Given An empty box
  When I add the following item using the *rows_hash()* function:
    | ITEM     | really good apples |
    | QUANTITY | 3                  |
  Then The box contains 3 items

Tags
----

Tags allow you to organize and selectively execute features or scenarios.

.. code-block:: gherkin

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
      | 3     | "orange"     | 3        |
      | 1     | "some stuff" | 1        |

You can execute only specific tags or exclude them using command-line options like:

.. code-block:: sh

  ./build/bin/example ./examples/features/4_tags.feature -t "@apples or @bananas"

See :doc:`Program Arguments <program_arguments>` for more details on tag filtering.

Rules
-----

Rules provide an additional layer of structure within a feature file.  
They group related scenarios under a shared business rule or condition.

.. code-block:: gherkin

  Feature: The Rules keyword

    Rule: This is a rule that helps structure the feature file.
      You can also use 'Example:' as a synonym for 'Scenario'.

    Example: An example is equivalent to a scenario
      Given An empty box
      When I place 1 x "apple" in it
      Then The box contains 1 item

    Rule: Another rule for grouping related examples
      # ...
      # Additional scenarios or examples can follow here

Languages
---------

CWT-Cucumber follows the official Gherkin syntax and supports multiple languages for feature files.  
Currently supported languages are:

- English (default)  
- Spanish  
- German  

If you would like to add another language, please open an issue or contribute it yourself.  
See :ref:`subch-contribution-adding-a-language` for details.

You can specify the language at the top of a feature file using the ``# language:`` directive.  
Find all Gherkin keywords in each supported language in the  
`official Cucumber documentation <https://cucumber.io/docs/gherkin/languages/>`_.

.. code-block:: gherkin

  # language: de

  Funktionalität: Gegenstände in eine Kiste legen
    Szenario: Äpfel in eine Kiste legen
      Gegeben sei Eine leere Kiste
      Wenn Ich 2 x "Apfel" hineinlege
      Dann Enthält die Kiste 2 Gegenstände

.. note::
   The step definitions must be implemented in the same target language.
