Feature: an escaped quote in step text

  Scenario: a step whose string value holds an escaped quote
    Given the exact input is "separator means one of \\,, \\\", \\\\"
    And the list is ["amount means integer which defaults to 7"]
