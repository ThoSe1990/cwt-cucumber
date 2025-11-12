Build Options & Environment Variables
=====================================

CWT-Cucumber supports a few environment variables that can influence its behavior during execution.

Undefined Steps Are a Failure
-----------------------------

By default, undefined steps are reported as skipped or undefined, depending on the scenario.  
To treat undefined steps as a scenario failure, set the following **CMake option** when building:

.. code-block:: cmake

  -DCUCUMBER_UNDEFINED_STEPS_ARE_A_FAILURE=ON

This ensures that any undefined step will cause the scenario to fail.

Step Delay: ``CWT_CUCUMBER_STEP_DELAY``
---------------------------------------

You can introduce a delay between step executions using the environment variable:

.. code-block:: sh

  export CWT_CUCUMBER_STEP_DELAY=100   # delay in milliseconds

This is useful for debugging, observing step execution, or interacting with external systems.

Suppress Colored Output: ``NO_COLOR``
-------------------------------------

Some terminals or tools do not handle ANSI color codes well.  
To disable colored output, set the standard ``NO_COLOR`` environment variable:

.. code-block:: sh

  export NO_COLOR=1

This is compatible with many CLI applications and ensures clean plain-text output.
