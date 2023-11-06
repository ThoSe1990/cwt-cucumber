.. _include_00_intro:

============
Introduction
============

This introduction is a summary of my reasons for starting this project and a description of how this interpreter works. 
  
I started reading `Crafting Interpreters by Robert Nystrom <https://craftinginterpreters.com/>`_ because I wanted to learn how interpreters work. As a side note, I can highly recommend this book, I think it is one of the best books I have ever had.
  
After reading and implementing the interpreter example from the books, I wanted to do a meaningful project on my own. But implementing another general purpose scripting language didn't feel right. In my projects from work I often use Cucumber and I like the idea. And this brought me to the idea to implement this interpreter. 

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

If we run this feature file with a valid implementation of these steps, cucumber will execute the three steps and evaluate them. If we were to translate this feature file into JavaScript code, it might look something like this:

.. code-block:: js 

  function feature() 
  {
      function scenario() 
      {
        first_step();
        second_step();
        third_step();
      }
    scenario();
  }

  feature();

This means that all steps are native functions (like print functions, clock or time functions, etc.). A scenario is nested within a feature function, and right after the function definition of each, we call it. And this is basically what happens. Of course there is more, like hooks, tags, failing steps, etc., which makes it more complex, but this was a start.

Feature-File to Bytecode
========================

