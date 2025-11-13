
Feature: This is a doc string example

  Scenario: Doc string with quotes
    Given An empty box with a label
    """
    This is a docstring with quotes
    after a step
    """
    Then The box is labeled

  Scenario: Doc string with backticks
    Given An empty box with a label
    ```
    This is a docstring with backticks
    after a step
    ```
    Then The box is labeled

  Scenario: Doc string as vector 
    Given An empty box with a label
    """
    This is a docstring 
    which we access 
    as std::vector<std::string>
    """
    Then The box is labeled
