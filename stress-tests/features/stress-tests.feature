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
    Then The doc string should equal "<expected>"

    Examples:
      | value  | expected                                          |
      | hello  | <note><to>Someone</to><text>hello</text></note>  |
      | world  | <note><to>Someone</to><text>world</text></note>  |

  Scenario: Doc string with a content type tag
    When There is a tagged doc string:
    """json
    { "key": "value" }
    """
    Then The doc string type should be "json"

  Scenario: Doc string without a content type tag
    When There is a tagged doc string:
    """
    plain text, no tag
    """
    Then The doc string type should be ""

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

  Scenario: Anonymous placeholder followed by an unescaped digit-only literal brace group
    When I have foo{56} things
    Then The captured value should be "foo"

  Scenario: Digit-only literal brace group with a comma stays literal
    When The range is {2,4} units
    Then The range match should succeed

  Scenario: Digit-only literal parenthesis group was never special
    When The reference number is (56)
    Then The reference number match should succeed

  Scenario: Registered custom expression with a digit-only key still matches
    When I have 56 apples
    Then The apple count should be 56
