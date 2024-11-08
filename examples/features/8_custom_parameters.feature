

Feature: Custom Parameters

  Scenario: An example 
    When this is var1=123, var2=99
    Then their values are 123 and 99


  Scenario: Date example
    When A cool event is from April 25, 2025 to Mai 13, 2025
    Then The beginning month is April and the ending month is Mai
