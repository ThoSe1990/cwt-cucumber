

Feature: My First Feature File
  Just for demonstration

  Scenario Outline: More boxes
    Given A box with <width> x <height> x <depth>
    When Put <water> liter water into the box
    Then The volume is <volume>
    And The weight is <weight> kg   

    Examples:
      | width | height | depth | volume | water   | weight |
      | 1     | 1      | 1     | 1      | 1       | 1      |
      | 1     | 2      | 3     | 6      | 2.5     | 2.5    |