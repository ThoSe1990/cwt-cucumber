
Feature: This represents tables
  We have three options:
  - raw access
  - rows hash
  - key/value pairs

  Scenario: Adding items with raw
    Given An empty box
    When I add all items with the raw function:
      | apple      | 2 |
      | strawberry | 3 |
      | banana     | 5 |
    Then The box contains 10 items

  Scenario Outline: Adding items with raw
    Given An empty box
    When I add all items with the raw function:
      | apple | <quantity> |
    Then The box contains <quantity> items

  Scenario Outline: Adding items with raw
    Given An empty box
    When I add all items with the raw function:
      | a very fresh <fruit> | <quantity> |
    Then The box contains <quantity> items

	Examples:
      | fruit  | quantity |
		  | orange | 2        |
		  | apple  | 3        |

  Scenario Outline: Adding items with raw
    Given An empty box
    When I add all items with the raw function:
      | <item1> and <item2> | <quantity> |
    Then The box contains <quantity> items

	Examples:
      | item1 | item2 | quantity |
		  | cars  | bikes | 2        |

  Scenario Outline: This works in scenario outlines too 
    When I add all items with the raw function:
      | <item 1> | <count 1> |
      | <item 2> | <count 2> |
    Then The box contains <total count> items

    Examples:
      | item 1 | item 2 | count 1 | count 2 | total count |
      | pen    | book   | 5       | 3       | 8           |
      | chair  | table  | 4       | 1       | 5           |

  Scenario: Adding items with hashes
    Given An empty box
    When I add all items with the hashes function:
      | ITEM   | QUANTITY |
      | apple  | 3        |
      | banana | 6        |
    Then The box contains 9 items

  Scenario Outline: Adding items with hashes in a scenario outline
    Given An empty box
    When I add all items with the hashes function:
      | ITEM   | QUANTITY   |
      | <item> | <quantity> |
    Then The box contains <quantity> items

    Examples:
      | item    | quantity |
      | "shirt" | 3        |
      | "phone" | 100      |

  Scenario: Adding items with rows_hash
    Given An empty box
    When I add the following item with the rows_hash function:
      | ITEM     | really good apples |
      | QUANTITY | 3                  |
    Then The box contains 3 items

