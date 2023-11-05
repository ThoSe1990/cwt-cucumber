# ./examples/features/scenario_outline.feature

Feature: Scenario Outline 

  Scenario Outline: A lot of boxes 
    Given A box with <width> x <height> x <depth>
    Then The volume is <volume>

    Examples:
      | width | height | depth | volume |
      | 1     | 1      | 1     | 1      |
      | 1     | 2      | 3     | 6      |
      | 2     | 2      | 4     | 16     |

    @mid_sized_boxes
    Examples:
      | width | height | depth | volume |
      | 10    | 5      | 10    | 500    |
      | 20    | 2      | 9     | 360    |
    
    @big_boxes
    Examples:
      | width | height | depth | volume  |
      | 200   | 99     | 150   | 2970000 |
      | 120   | 55     | 30    | 198000  |
