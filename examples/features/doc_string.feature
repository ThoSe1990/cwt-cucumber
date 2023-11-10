

Feature: Lets see a doc string

  Scenario: Add some name and description
    Given A box with 2 x 2 x 2
    When The box's name is "A cool box"
    And The box's description is
    """
    This box contains 
    wonderful items!!
    """ 
    Then I print the box's name and description