Program Arguments
=================

CWT-Cucumber supports several command-line options to control the execution of features and scenarios.

Executing Single Scenarios
--------------------------

You can execute only specific scenarios from a feature file by appending line numbers to the file path.  
Multiple line numbers can be specified, separated by colons:

.. code-block:: sh

  ./build/bin/example ./examples/features/1_basic.feature:4
  ./build/bin/example ./examples/features/1_basic.feature:4:10:15

This allows you to run a subset of scenarios in a feature without executing the entire file.

Name Filter: ``-n`` / ``--name``
--------------------------------

The program option ``-n <PATTERN>`` or ``--name <PATTERN>`` executes only Scenarios with the given pattern. It uses a pattern matching, like its used in other libraries (e.g. GTest): 

- ``*`` matches zero or more characters
- ``?`` matches exactly one character
- Multiple patterns can be separated by a colon (``:``)

.. _subch-program-arguments-tags:

Tags: ``-t`` / ``--tags``
-------------------------

Run only scenarios or features matching a tag expression:

.. code-block:: sh

  ./build/bin/example ./examples/features/4_tags.feature -t "@apples or @bananas"

Examples of tag expressions:

- "@tag1"
- "@tag1 or @tag2"
- "not @tag2"
- "(@tag1 and @tag2) or not @tag3"
- "((@tag1 and @tag2) or @tag3) xor @tag4"

Quiet Mode: ``-q`` / ``--quiet``
--------------------------------

Suppress detailed output and print only the final result.

Dry Run: ``-d`` / ``--dry-run``
-------------------------------

Check that all steps are defined without actually executing them.  
Useful to verify step definitions.

Verbose: ``-v`` / ``--verbose``
-------------------------------

Print detailed information, including skipped scenarios and tag evaluation.

Continue on Failure: ``-c`` / ``--continue-on-failure``
-------------------------------------------------------

Do not skip subsequent steps in a scenario after a failed step;  
all steps will run regardless of intermediate failures.

JSON Report: ``--report-json``
------------------------------

Print test results as JSON to stdout or a specified file.  
A small example of the JSON report output:

.. code-block:: json

  [
    {
      "description": "This is my cucumber-cpp hello world",
      "elements": [
        {
          "id": "My first feature;First Scenario",
          "keyword": "Scenario",
          "line": 4,
          "name": "First Scenario",
          "steps": [
            {
              "keyword": "Given",
              "line": 5,
              "name": "An empty box",
              "result": { "status": "passed" }
            },
            {
              "keyword": "When",
              "line": 6,
              "name": "I place 1 x \"apple\" in it",
              "result": { "status": "passed" }
            },
            {
              "keyword": "Then",
              "line": 7,
              "name": "The box contains 1 item",
              "result": { "status": "passed" }
            }
          ]
        }
      ]
    }
  ]

.. note::
   This is a simplified snippet; actual output contains all scenarios, steps, and additional metadata.

Steps Catalog: ``--steps-catalog [file]`` / ``--steps-catalog-json [file]``
---------------------------------------------------------------------------

CWT-Cucumber can output a catalog of all implemented steps.  
This can be printed as plain text or JSON.

**Plain Text Example:**

.. code-block:: text

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
  When The box gets shipped at {date}
  When I put {item with weight} in it
  Then They will match {string} and {string}
  Then The {int}. item is {string}
  Then The box contains {int} item(s)
  Then {word} is/are in the box
  Then {int} item(s) is/are {string}
  Then The box is labeled with: {string}
  Then The box weights {int} kilogram(s)
  Step doc string:

  Custom Parameter Types:
  -----------------------
  {date}
    Comment: shipping date
    Pattern: (\d{4})-(\d{2})-(\d{2})
  {item with weight}
    Comment:
    Pattern: (\d+) kilograms? of (\w+)

**JSON Example:**

.. code-block:: json

  {
    "steps_catalog": [
      {
        "definition": "An empty box",
        "function": "init_box",
        "type": "Given",
        "var_types": []
      },
      {
        "definition": "I place {int} x {string} in it",
        "function": "add_item",
        "type": "When",
        "var_types": ["int","string"]
      },
      {
        "definition": "The box gets shipped at {date}",
        "function": "ship_the_box",
        "type": "When",
        "var_types": ["shipping date"]
      },
      {
        "definition": "I put {item with weight} in it",
        "function": "add_item_with_weight",
        "type": "When",
        "var_types": [""]
      },
      {
        "definition": "The box contains {int} item(s)",
        "function": "check_box_size",
        "type": "Then",
        "var_types": ["int"]
      }
    ],
    "types": [
      {
        "type": "{date}",
        "comment": "shipping date",
        "pattern": "(\\d{4})-(\\d{2})-(\\d{2})"
      },
      {
        "type": "{item with weight}",
        "comment": "",
        "pattern": "(\\d+) kilograms? of (\\w+)"
      }
    ]
  }

.. note::
   - Plain text catalog is useful for a quick overview of all step definitions.  
   - JSON catalog can be consumed programmatically, e.g., by IDEs or documentation tools.

Exclude File: ``--exclude-file``
--------------------------------

Exclude a specific feature file from the test run:

.. code-block:: sh

  ./build/bin/example --exclude-file ./examples/features/ignore_this.feature

.. note::
   Useful for ignoring work-in-progress or deprecated feature files.
