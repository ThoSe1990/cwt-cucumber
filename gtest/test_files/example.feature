
Feature: example feature 

  Scenario: first scenario 
    Given a step 
    And a step 

  Scenario: second scenario 
    * a step 
    * a step 

  Scenario: third scenario 
    * a step 
    * a step 
    * a step 

  Scenario Outline: first scenario outline 
    Given a step with <var 1> and <var 2> 

    Examples: first examples 
      | var 1 | var 2       |
      | 1     | hello world | 
      | 1     | foo bar baz | 

    Examples: second examples 
      | var 1 | var 2           |
      | 1     | sh!t happens    | 
      | 1     | some %^&() text | 
