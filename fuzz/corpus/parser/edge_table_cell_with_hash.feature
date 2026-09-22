Feature: a hash inside data table cells

  Scenario: a cell that holds a hash
    Given a step with a table
      | #1       | leading hash |
      | issue #7 | after space  |
      | a#b      | inside word  |
    # a comment directly after the table
