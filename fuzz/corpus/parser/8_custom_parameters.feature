Feature: Custom Parameters

  Scenario: Items with weight
    Given An empty box
    When I put 1 kilogram of bananas in it 
    Then The box weights 1 kilograms
    And bananas are in the box

  Scenario: Items with more weight
    Given An empty box
    When I put 30 kilograms of apples in it 
    Then The box weights 30 kilograms
    And apples are in the box

  Scenario: Ship the box 
    Given An empty box
    When The box gets shipped at 2025-11-12
    Then The box is labeled with: "box shipped at 2025-11-12"

