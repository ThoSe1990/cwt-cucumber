CWT-Cucumber: A C++20 Cucumber Interpreter
==========================================

CWT-Cucumber is a lightweight, modern **C++20 BDD testing framework** for native C++ projects.  
It interprets **Gherkin feature files**, allowing you to write test cases in plain language and define behavior-driven development (BDD) scenarios in C++:

.. code-block:: gherkin

  Feature: Putting apples into a box 

    Scenario: Two apples 
      Given An empty box
      When I place 2 x "apple" in it
      Then The box contains 2 item(s) 

Originally started as a personal learning project, CWT-Cucumber has grown into a production-ready tool, now used by teams in real-world applications.  
Over this journey, it has become more robust and provides a full suite of BDD capabilities for modern C++ projects.

It is written in pure C++20 with no mandatory dependencies and supports ``Scenario``, ``Scenario Outline``, ``Background``, ``Rules``, ``Hooks``, ``Doc Strings``, ``Tables``, and more.  
Optionally, you can enable JSON reports by building it with `nlohmann/json <https://github.com/nlohmann/json>`_.

Get started quickly by following the :doc:`Installation guide <chapters/installation>` and implement your first steps.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   chapters/installation
   chapters/step_definitions
   chapters/cucumber_features
   chapters/program_arguments
   chapters/api_reference
