Feature: my feature

  Scenario Outline: test:
    Given one double: <d>
    Given fail

  Examples:
    | d   | 
    | 123 | 
    | 789 | 

  Scenario: meainginless scenario
    Given two numbers: 11111 and 999
    Given fail
    Given two numbers: 11111 and 999

  Scenario: meainginless scenario
    Given two numbers: 11111 and 999
    Given fail
    Given two numbers: 11111 and 999

  Scenario Outline: foo
    Given two numbers: <n1> and <n2>
    Given one number: <one>
    Given one double: <double>
    Given two numbers: 17 and <n2>
    Given string: <str>

    Examples:
        | n1  | n2 | one  | double  | str |
        | 11  | 22 | 100  | 1.1122  | "first string here" |
        # | 11  | 22 | 100  | 1.1122  | "first string here" |
        | 99  | 98 | 900  | 900  | "and another here ...!!" |