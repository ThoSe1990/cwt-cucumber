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
