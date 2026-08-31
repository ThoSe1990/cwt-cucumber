
Feature: Examples - Scenario Outline

  Scenario Outline: Scenario Outline with <item> 
    Given An empty box
    When I place <count> x <item> in it
    Then The box contains <count> items

    Examples:
      | count | item       |
      | 1     | "apple"    |
      | 2     | "bananas"  |

  Scenario Template: Alternative Keywords
    Given An empty box
    When I place <count> x <item> in it
    Then The box contains <count> items

    Scenarios:
      | count | item            |
      | 1     | "pen"           |
      | 3     | "collegeblocks" |
