Feature: My Fancy Cube Feature

  # Background: Background name
  #   Given hi

  @tag1 @tag2
  Scenario: My first cube
    Given A cube with 3 side length
    Then The side area is 9
    And The cube volume is 27

  Scenario Outline: My cubes  
    Given A cube with <side> side length
    Then The side area is <area>
    And The cube volume is <volume>

    Examples:
        | side | area | volume |
        | -1    | 1    | -1  |
        | 2    | 4    | 8  |
