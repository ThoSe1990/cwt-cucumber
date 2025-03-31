@all
Feature: Scenarios with tags

  Scenario: Tomatoes 
    Given An empty box
    When I place 5 x "Tomatoes" in it
    Then The box contains 5 items

  @skip  
  Scenario: This will be skipped 
    Given An empty box
    When I place 1 x "whatever" in it

  @skip  
  Scenario: This will be skipped too 
    Given An empty box
    When I place 1 x "whatever" in it
    Then This is an undefined step even when we use skip 

  @ignore
  Scenario: This is ignored completely
    No reporting with using cuke::ignore_scenario() in a hook
    Given It does not matter how many
    * Undefined steps are in my Scenario
    * Nothing will land in the report here ... 

  @apples
  Scenario: Apple
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 items

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
      | 3     | "oranges"    | 3        |
      | 1     | "some stuff" | 1        |
