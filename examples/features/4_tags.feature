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
