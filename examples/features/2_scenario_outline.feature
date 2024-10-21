
Feature: My first feature
  This is my cucumber-cpp hello world

  Scenario Outline: First Scenario Outline
    Given An empty box
    When I place <count> x <item> in it
    Then The box contains <count> item(s)

    Examples:
      | count | item      |
      | 1     | "apple"   |
      | 2     | "bananas" |

  Scenario Outline: a scenario outline  
    When A <word> and <anonymous>
    Then They will match <expected_word> and <expected_anonymous>
    
    Examples:
      | word  | anonymous | expected_word | expected_anonymous |
      | 123   | 999       | "123"         | "999"              | 
