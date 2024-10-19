@all
Feature: Scenarios with tags

  Scenario: Tomatoes 
    Given An empty box
    When I place 5 x "Tomatoes" in it
    Then The box contains 5 item(s)

  @apples
  Scenario: Apple
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 item(s)

  @apples @bananas
  Scenario: Apples and Bananas
    Given An empty box
    When I place 2 x "apple" in it
    And I place 2 x "banana" in it
    Then The box contains 4 item(s)

  Scenario Outline: 
    Given An empty box
    When I place <count> x <fruit> in it
    Then The box contains <expected> item(s)

    @oranges 
    Examples: 
      | count | fruit    | expected |
      | 1     | "orange" | 1        |

    @oranges @others
    Examples: 
      | count | fruit        | expected |
      | 3     | "oranges"    | 3        |
      | 1     | "some stuff" | 1        |
