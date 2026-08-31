

Feature: The Rules keyword

  Scenario: A scenario as we used before
    Given An empty box
    When I place 1 x "apple" in it
    Then The box contains 1 item

  Rule: This is a rule, which helps me to structure my feature file 
    and some description here ... 

  Example: An example is the same as a scenario
    Given An empty box
    When I place 1 x "apple" in it
    Then The box contains 1 item
