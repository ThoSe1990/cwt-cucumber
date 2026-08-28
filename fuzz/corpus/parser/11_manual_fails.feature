Feature: Set Scenario manual to fail

   @will_fail_before
   Scenario: Steps are skipped 
    Given An empty box
    When I place 1 x "apple" in it
    Then The box contains 1 item

   @will_fail_after
   Scenario: Steps aren't skipped 
    Given An empty box
    When I place 1 x "apple" in it
    Then The box contains 1 item
