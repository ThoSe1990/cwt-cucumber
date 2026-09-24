Feature: an escaped delimiter inside data table cells

  Scenario: a cell that holds an escaped pipe
    Given a step with a table
      | a\|b                      | escaped pipe |
      | \\A(?P<x>same\|root)\\z   | a regex      |
