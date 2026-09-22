Feature: tokens that sit next to each other

  Scenario: text with no separating whitespace
    Given key="a b" and a#b and issue #7
      | ["x = 1"] | a\|b | "unterminated |
