
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

  Scenario: Doc string with a content type tag for a customs declaration
    Given An empty box with a label
    """
    Fragile electronics
    """
    When The box gets a customs declaration
    """json
    { "value_usd": 250, "contents": "electronics" }
    """
    Then The customs declaration content type should be "json"

  Scenario Outline: Doc string with a shipping label template in a scenario outline
    Given An empty box with a label
    """
    Fragile electronics
    """
    When The box gets a shipping label
    """
    Ship to: <destination>. Barcode template: <barcode>
    """
    Then The shipping label should equal "<expected_label>"

    Examples:
      | destination      | expected_label                                          |
      | Berlin, Germany  | Ship to: Berlin, Germany. Barcode template: <barcode>   |
      | Tokyo, Japan     | Ship to: Tokyo, Japan. Barcode template: <barcode>      |
