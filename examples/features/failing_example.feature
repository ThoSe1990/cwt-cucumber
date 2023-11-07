# ./examples/features/first_examples.feature

Feature: My First Feature File
  Just for demonstration

  Scenario: An arbitrary box
    Given A box with 2 x 2 x 2
    Then The volume is 99 
    When I open the box
    Then The box is open 