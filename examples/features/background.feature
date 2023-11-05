# ./examples/features/background.feature

Feature: We want default open boxes!

  Background: create an opened the box!
    Given A box with 2 x 2 x 2
    When I open the box

  Scenario: An opened box
    Then The box is open 
