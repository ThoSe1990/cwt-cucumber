Feature: My Fancy Cube Feature

  # Background: Background name
  #   Given hi

  Scenario Outline: My cubes  
    Given A cube with <side> side length
    Then The side area is <area>
    And The cube volume is <volume>

    Examples:
        | side | area | volume |
        | -1    | 1    | -1  |
        | 2    | 4    | 8  |
