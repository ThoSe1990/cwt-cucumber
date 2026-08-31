Feature: Continue on failure 
  1. Remove/comment the skip-tag if you want to run this example (I don't want my pipeline to fail)
  2. Use the command line argument -c or --continue-on-failure
  3. Then the sceanrio will continue and all steps are triggered

   @skip
   Scenario: First Scenario
    Given An empty box
    When I place 1 x "apple" in it
    Then The box contains 2 item
    But I place 1 x "banana" in it
    Then The box contains 2 item
