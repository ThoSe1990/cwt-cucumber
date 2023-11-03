

Feature: My First Feature File
  Just for demonstration

  Scenario: A simple Box
    Given A box with 2 x 2 x 2
    When Put 1.5 liter water into the box
    Then The volume is 8
    And The weight is 1.5 kg

  Scenario: Another box
    Given A box with 1 x 2 x 3
    Then The front area is 2
    And The top area is 3
    And The side area is 6
