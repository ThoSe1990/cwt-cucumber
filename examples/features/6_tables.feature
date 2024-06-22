
Feature: This represents tables
  We have three options:
  - raw access
  - rows hash
  - key/value pairs

  Scenario: Adding items with raw
    Given An empty box
    When I add all items with raw():
      | apple      | 2 |
      | strawberry | 3 |
      | banana     | 5 |
    Then The box contains 10 item(s)

  Scenario: Adding items with hashes
    Given An empty box
    When I add all items with hashes():
      | ITEM   | QUANTITY |
      | apple  | 3        |
      | banana | 6        |
    Then The box contains 9 item(s)


  Scenario: Adding items with rows_hash
    Given An empty box
    When I add the following item with rows_hash():
      | ITEM     | really good apples |
      | QUANTITY | 3                  |
    Then The box contains 3 item(s)

