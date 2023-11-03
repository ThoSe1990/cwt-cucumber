

Feature: My First Feature File
  Just for demonstration

  Scenario: A simple Box
    Given A box with 2 x 2 x 2
    When Put 2 liter water into the box
    Then The volume is 8
    And The weight is 2 kg

  Scenario Outline: More boxes
    Given A box with <width> x <height> x <depth>
    When Put <water> liter water into the box
    Then The volume is <volume>
    And The weight is <weight> kg   

    Examples:
      | width | height | depth | volume | water | weight |
      | 1     | 1      | 1     | 1      | 1     | 1      |


  Scenario: Another box
    Given A box with 1 x 2 x 3
    Then The front area is 2
    And The top area is 3
    And The side area is 6
