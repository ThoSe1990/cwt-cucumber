
Feature: My first feature
  This is my cucumber-cpp hello world

  Scenario: First Scenario
    Given An empty box
    When I place 2 x "apple" in it
    Then The box contains 2 item(s)

  Scenario Outline: First Scenario Outline
    Given An empty box
    When I place <count> x <item> in it
    Then The box contains <count> item(s)

    Examples:
      | count | item      |
      | 1     | "apple"   |
      | 2     | "bananas" |
      | 3     | "pies"    |

