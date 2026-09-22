Feature: quoted text inside data table cells

  Scenario: a cell that holds a quoted list
    Given a step with a table
      | ["x = 1"]         | plain      |
      | ["m 7", "eval n"] | two quotes |
      | "unterminated     | odd quote  |
