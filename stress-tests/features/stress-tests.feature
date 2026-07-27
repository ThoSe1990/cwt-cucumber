Feature: Stress tests 

  Scenario Outline: a scenario outline  
    When A <word> and <anonymous>
    Then They will match <expected word> and <expected anonymous>
    
    Examples:
      | word      | anonymous | expected word | expected anonymous |
      | 123.123   | -999.9999 | "123.123"     | "-999.9999"        | 
      | -123,123  | -999,9999 | "-123,123"    | "-999,9999"        | 
      | 'abc'     | -999,9999 | "'abc'"       | "-999,9999"        | 
      | abc       | -00:00,00 | "abc"         | "-00:00,00"        | 


  Scenario Outline: lets put the quotes in the step ...  
    When A <word> and <anonymous>
    Then They will match "<expected word>" and "<expected anonymous>"
    
    Examples:
      | word      | anonymous     | expected word | expected anonymous |
      | 123.123   | -999.    9999 | 123.123       | -999.    9999      | 
      | 'abc'     | -00:00,00     | 'abc'         | -00:00,00          | 

  Scenario: Doc string with quotes
    When There is a doc string:
    """
    This is a docstring with quotes
    after a step
    """

  Scenario: Doc string with backticks
    When There is a doc string:
    ```
    This is a docstring with backticks
    after a step
    ```
  Scenario: Doc string as vector 
    When There is a doc string as vector:
    """
    This is a docstring 
    which we access 
    as std::vector<std::string>
    """

  Scenario Outline: Doc string with XML content in a scenario outline
    When There is a doc string with an xml payload:
    """
    <note><to>Someone</to><text><value></text></note>
    """

    Examples:
      | value  |
      | hello  |
      | world  |

  Scenario: Empty cells in data table
    When There is an empty table cell
      | | | |
      | | | |
  

  Scenario Outline: Empty cells in examples
    When Some values <val1>, <val2> and <val3> are empty

    Examples:
      | val1 | val2 | val3 |
      |      |      |      |
      |      |      |      |
    Examples:
      | val1 | val2 | val3 |
      | ""   | ""   | ""   |
      | ""   | ""   | ""   |
    Examples:
      | val1 | val2 | val3 |
      | ""   | ""   |      |
      | ""   | ""   |      |
    Examples:
      | val1 | val2 | val3 |
      |      | ""   |      |
      |      | ""   |      |

  Scenario: Literal parenthesis in step text
    When There is a point at (3,4)
    Then The point should be at coordinates 3 and 4

  Scenario: Literal parenthesis in step text
    When There is a point at (-3,-4)
    Then The point should be at coordinates -3 and -4

  Scenario: Singular optional text
    When I have 1 item in stock
    Then The stock count should be 1

  Scenario: Plural optional text
    When I have 5 items in stock
    Then The stock count should be 5

  Scenario: Literal curly braces in step text
    When I see {status} in the raw output
    Then The captured text should be "{status}"
