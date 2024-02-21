Feature: We always need apples!

  Background: Add an apple 
    Given An empty box
    When I place 1 x "apple" in it

  Scenario: Apples Apples Apples
    When I place 1 x "apple" in it
    Then The box contains 2 item(s)

  Scenario: Apples and Bananas
    When I place 1 x "apple" in it
    And I place 1 x "banana" in it
    Then The box contains 3 item(s)