# ./examples/features/tags.feature

Feature: Tags

  @small_boxes
  Scenario: An arbitrary box
    Given A box with 1 x 1 x 1
    Then The volume is 1

  @mid_sized_boxes
  Scenario: An arbitrary box
    Given A box with 10 x 15 x 12
    Then The volume is 1800

  @big_boxes
  Scenario: An arbitrary box
    Given A box with 90 x 80 x 70
    Then The volume is 504000

  @small_boxes @open
  Scenario: An opened box
    Then The box is open 

  