Feature: My Fancy Cube Feature

  Background: Background name
    Given hi
    When hi
    Then hi 

  Scenario: My first cube
    Given A cube with 3 side length
    Then The side area is 9
    And The cube volume is 27

  Scenario: My second cube
    Given A cube with 4 side length
    Then The side area is 16
    And The cube volume is 65

  Scenario Outline: My cubes  
    Given A cube with <side> side length
    Then The side area is <area>
    And The cube volume is <volume>

    Examples:
        | side | area | volume |
        | 1    | 2    | 1  |
        | 2    | 4    | 8  |
        
  Scenario Outline: My cubes  
    Given A cube with <side> side length
    Then The side area is <area>
    And The cube volume is <volume>

    Examples:
        | side | area | volume |
        | 3  | 9  | 27  |
        | 4  | 16  | 64  |
        
