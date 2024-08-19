Feature: this will feature 

  Scenario: first scenario 
    Given a step 
    And this fails  

  Scenario Outline: first scenario outline 
    Given a step with <var 1> and <var 2> 
    And this fails  

    Examples: first examples 
      | var 1 | var 2       |
      | 1     | hello world | 
      | 1     | foo bar baz | 

    Examples: second examples 
      | var 1 | var 2           |
      | 1     | sh!t happens    | 
      | 1     | some %^&() text | 
