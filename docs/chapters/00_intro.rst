.. _include_00_intro:


.. note:: 
  This chapter is about all my thoughts, how I came to this project and how this interpreter works in general. It's just an informative chapter and nice to know. You can skip this chapter if you are looking for the technical documentation of cwt-cucumber and its API.


============
Introduction
============

This introduction is a summary of my reasons for starting this project and a description of how this interpreter works. 
  
I started reading `Crafting Interpreters by Robert Nystrom <https://craftinginterpreters.com/>`_ because I wanted to learn how interpreters work. As a side note, I can highly recommend this book, I think it is one of the best books I have ever had. In his book you go step by step through the implementation of his scripting language *lox*. First there is *jlox*, the Java version and then there is *clox*, the C version. Find both implementations on `GitHub <https://github.com/munificent/craftinginterpreters>`_.
  
After reading and implementing the interpreter example from the books, I wanted to do a meaningful project on my own. But implementing another general-purpose scripting language didn't feel right, because it would be more or less a copy of *lox*. Now consider that Cucumber is different. In Cucumber, you don't have the freedom to write any script you want. The rules for it are pretty much set with given-when-then. In my projects from work I often use Cucumber and I like the idea. And this brought me to the idea to implement this interpreter. 

So this is not really a competing project to the official `Cucumber-cpp <https://github.com/cucumber/cucumber-cpp>`_. It is an educational project for me to fully understand the C implementation of *Crafting Interpreters*. And maybe it will turn out to be a good project.

How I started 
=============

At first it seemed really simple compared to a scripting language, because the structure is pretty much defined by the Cucumber language. There were some tricky parts, but I guess there was always a solution. Anyway, to get my initial idea, take a look at this feature file:

.. code-block:: gherkin
  
  Feature: feature

    Scenario: scenario 
      Given first_step
      When second_step 
      Then third_step

If we run this feature file with a valid implementation of these steps, cucumber will execute the three steps and evaluate them. If we were to translate this feature file into *lox* from Crafting Interpreters, it might look something like this:

.. code-block::

  fun feature() 
  {
      fun scenario() 
      {
        first_step();
        second_step();
        third_step();
      }
    scenario();
  }

  feature();

This means that all steps are native functions (like print functions, clock or time functions, etc.). A scenario is nested within a feature function, and right after the function definition of each, we call it. And this is basically what happens. Of course there is more, like hooks, tags, failing steps, etc., which makes it more complex, but this was a start.

Crafting Interpreters *clox*
============================

Lets have a look at the *clox* interpreter, which compiles the script into a chunk of byte code. This chunk is then executed by a virtual machine. The cool thing here is, that you can print the byte code and the stack during execution. So lets have another look and replace some native functions with prints. They represent a step now:

.. code-block::

  fun feature() 
  {
      fun scenario() 
      {
        print "Given any initial step";
        print "When something happens";
        print "Then we evaluate something";
      }
    scenario();
  }

  feature();

And this is translated into bytecode:

.. code-block::

  # 3.:
  == scenario ==
  0000    5 OP_CONSTANT         0 'Given any initial step'
  0002    | OP_PRINT
  0003    6 OP_CONSTANT         1 'When something happens'
  0005    | OP_PRINT
  0006    7 OP_CONSTANT         2 'Then we evaluate something'
  0008    | OP_PRINT
  0009    8 OP_NIL
  0010    | OP_RETURN
  
  # 2.:
  == feature ==
  0000    8 OP_CONSTANT         0 '<fn scenario>'
  0002    9 OP_GET_LOCAL        1
  0004    | OP_CALL             0
  0006    | OP_POP
  0007   10 OP_NIL
  0008    | OP_RETURN

  # 1.:
  == <script> ==
  0000   10 OP_CONSTANT         1 '<fn feature>'
  0002    | OP_DEFINE_GLOBAL    0 'feature'
  0004   12 OP_GET_GLOBAL       2 'feature'
  0006    | OP_CALL             0
  0008    | OP_POP
  0009    | OP_NIL
  0010    | OP_RETURN

Now we can see three parts. 

1. The script: A global function is defined and the call is emitted. 
2. This represents the function or its function body in which the nested scenario is defined. After its declaration we emit the call.
3. The execution of each step works the same way, we pull a constant (which is later the step) and then we call the underlying function, which in this case is a single print. 

And this was my starting point for cwt-cucumber.

Feature-File to Bytecode
========================

I left these debugging prints in the source. During CMake configuration you can set `STACK_TRACE=1` and all the bytecode and its stack will be printed to the terminal. And it really helped me during the implementation. To be honest, it has grown quite a bit and can be a bit hard to read the stack, because it overlaps with the general prints. But let's have a look at the chunk in which a feature file is translated. I used some `box` examples:

.. code-block:: gherkin

  # ./examples/features/first_examples.feature

  Feature: My First Feature File
    Just for demonstration

    Scenario: An arbitrary box
      Given A box with 2 x 2 x 2
      When I open the box
      Then The box is open 
      And The volume is 8 

As far as I noticed, it is valid to have same scenario names in Cucumber. And if I want to create a function call from the the scenarios and features, I can not rely on its name. Therefore I used the filepaths and the linenumber as function name.


If we now go through the chunk as above from 1. to 3. step by step, we will see. Let us start with 1:

.. code-block::

  == <script> ==
  0000   10 OP_CONSTANT         0 '<fn ./examples/features/first_example.feature:3>'
  0002    | OP_DEFINE_VARIABLE    1 './examples/features/first_example.feature:3'
  0004    | OP_CALL             0
  0006    | OP_RETURN

And this is pretty straightforward: the feature is represented by a global function, with its path and linenumber as the function name. This is pushed to the stack and then, we immediately call it.
  
Now we go into the function body of the feature, which means we are now in the chunk `./examples/features/first_example.feature:3`. In order to make it more understandable, I will add my comments to the chunk:

.. code-block::

  == ./examples/features/first_example.feature:3 ==
  # I added print operations, to later enable/disable them during the compilation:
  0000    3 OP_PRINT_LINE       0 'Feature: My First Feature File'
  0002    | OP_PRINT_BLACK      1 './examples/features/first_example.feature:3'
  0004    | OP_PRINT_LINEBREAK
  0005    | OP_PRINT_LINEBREAK
  
  # an internal hook to reset the scenario context
  # you find a detailed explanation for the context and hooks in this documentation too
  # and after we get the hook, we call it 
  0006   10 OP_CONSTANT         2 'reset_context'
  0008    | OP_HOOK             0
  
  # same happens with the hook before 
  0010    | OP_CONSTANT         3 'before'
  0012    | OP_HOOK             0

  # now there is the actual scenario at line 6, it get pushed to the stack
  # and we'll call it. 
  0014    | OP_CONSTANT         4 '<fn ./examples/features/first_example.feature:6>'
  0016    | OP_CALL             0

  # after a scenario call there is another hook which might get called:
  0018    | OP_CONSTANT         5 'after'
  0020    | OP_HOOK             0

  # we evaluate the scenario result in the vm and we're done 
  0022    | OP_SCENARIO_RESULT
  0023    | OP_PRINT_LINEBREAK
  0024    | OP_RETURN


And now the chunk for the scenario, which is getting bigger again:

.. code-block::

  # we're in line 6 now, inside the body of the scenario 
  == ./examples/features/first_example.feature:6 ==
  # we begin with prints
  0000    6 OP_PRINT_LINE       0 'Scenario: An arbitrary box'
  0002    | OP_PRINT_BLACK      1 './examples/features/first_example.feature:6'
  0004    | OP_PRINT_LINEBREAK

  # the function name (= its location) and the given name to the stack 
  # and initialize it
  # this is necessary because if it fails we need this later
  0005    | OP_CONSTANT         2 './examples/features/first_example.feature:6'
  0007    | OP_CONSTANT         3 'Scenario: An arbitrary box'
  0009    | OP_INIT_SCENARIO

  # now we have a jump operation before every step 
  # because if a step fails, we want to skip the following steps 
  # (for the first step this does not make to much sense, I know)
  # and then we have another hook which can be there, the call to the step 
  # and another hook after the step 
  0010    7 OP_JUMP_IF_FAILED   10 -> 23
  0013    | OP_CONSTANT         4 'before_step'
  0015    | OP_HOOK             0
  0017    8 OP_CALL_STEP        5 'A box with 2 x 2 x 2'
  0019    | OP_CONSTANT         6 'after_step'
  0021    | OP_HOOK             0

  # now we executed the step and set the step result 
  0023    | OP_PRINT_STEP_RESULT    7 'A box with 2 x 2 x 2'
  0025    | OP_SET_STEP_RESULT
  0026    | OP_PRINT_BLACK      8 './examples/features/first_example.feature:7'
  0028    | OP_PRINT_LINEBREAK

  # and this continues now for all steps: 
  # if the previous step failed we skip to 42, else we stay and execute the hooks and the step:
  0029    | OP_JUMP_IF_FAILED   29 -> 42
  0032    | OP_CONSTANT         9 'before_step'
  0034    | OP_HOOK             0
  0036    9 OP_CALL_STEP       10 'I open the box'
  0038    | OP_CONSTANT        11 'after_step'
  0040    | OP_HOOK             0

  # if we skip, we end up here: setting its result as skipped
  0042    | OP_PRINT_STEP_RESULT   12 'I open the box'
  0044    | OP_SET_STEP_RESULT
  0045    | OP_PRINT_BLACK     13 './examples/features/first_example.feature:8'
  0047    | OP_PRINT_LINEBREAK
  0048    | OP_JUMP_IF_FAILED   48 -> 61
  0051    | OP_CONSTANT        14 'before_step'
  0053    | OP_HOOK             0
  0055   10 OP_CALL_STEP       15 'The box is open '
  0057    | OP_CONSTANT        16 'after_step'
  0059    | OP_HOOK             0

  # if we skip, we end up here: setting its result as skipped
  0061    | OP_PRINT_STEP_RESULT   17 'The box is open '
  0063    | OP_SET_STEP_RESULT
  0064    | OP_PRINT_BLACK     18 './examples/features/first_example.feature:9'
  0066    | OP_PRINT_LINEBREAK
  0067    | OP_JUMP_IF_FAILED   67 -> 80
  0070    | OP_CONSTANT        19 'before_step'
  0072    | OP_HOOK             0
  0074    | OP_CALL_STEP       20 'The volume is 8 '
  0076    | OP_CONSTANT        21 'after_step'
  0078    | OP_HOOK             0

  # if we skip, we end up here: setting its result as skipped
  0080    | OP_PRINT_STEP_RESULT   22 'The volume is 8 '
  0082    | OP_SET_STEP_RESULT
  0083    | OP_PRINT_BLACK     23 './examples/features/first_example.feature:10'
  0085    | OP_PRINT_LINEBREAK
  
  # and there we are, scenario done, up to 4 steps executed (or skipped)
  # and we return from the scenario
  0086    | OP_RETURN

And this is essentially what happens under the hood when you run this cucumber interpreter. The feature file is compiled into this chunk and executed in its vm. The final output from the user perspective (without the stack trace) looks like this:

.. code-block::

  $ ./build/bin/box ./examples/features/first_example.feature 

    Feature: My First Feature File  ./examples/features/first_example.feature:3

    Scenario: An arbitrary box  ./examples/features/first_example.feature:6
  [   PASSED    ] A box with 2 x 2 x 2  ./examples/features/first_example.feature:7
  [   PASSED    ] I open the box  ./examples/features/first_example.feature:8
  [   PASSED    ] The box is open   ./examples/features/first_example.feature:9
  [   PASSED    ] The volume is 8   ./examples/features/first_example.feature:10


  1 Scenarios (1 passed)
  4 Steps (4 passed)

