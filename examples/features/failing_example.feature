# ./examples/features/first_examples.feature

Feature: My First Feature File
  Uncomment this step to let this fail
  otherwise my pipeline fails

  Scenario: An arbitrary box
    Given A box with 2 x 2 x 2
    # Then The volume is 99 
    When I open the box
    Then The box is open 