# Coding With Thomas: Cucumber

This is a Cucumber interpreter written in C with C++ (17 or higher) bindings for ease of use. A more detailed documentation for the C and C++ API will follow. 

## Disclaimer
This is my personal fun and educational project. I read Crafting Interpreters by Robert Nystrom and wanted a meaningful real-world example to apply the principles. I really like Cucumber and use it a lot in my projects. So I decided to start this project for my own Cucumber interpreter and of course just for fun.
  
And, I'm currently on a sabbatical timeout from work and travel through Central America. If you open issues, please note that I'm haven really limited time to work on this project. In April 2024 I'm back home and have more time for this. 

## Using CWT-Cucumber

I'm using a simple `box` to demonstrate the behavior. Currently all implemented Cucumber featuers are compatible to C and C++ without third party libraries. My main goal was to implement the interpreter itself in C and then create some C++ bindings for easy access to it (similar to lua).

## C++ API 

The C++ API compiles its own `main`, which means we only need to implement the step definitions. There is also a `cucumber-no-main` target if you need your own main.

So lets have some easy steps (the box class should be self-explanatory):

```cpp 
GIVEN(box_init, "A box with {int} x {int} x {int}")
{
  const unsigned int w = CUKE_ARG(1);
  const unsigned int l = CUKE_ARG(2);
  const unsigned int h = CUKE_ARG(3);

  cuke::context<box>(w,l,h);
}

WHEN(box_open, "I open the box")
{
  cuke::context<box>().open(); 
}

THEN(box_is_open, "The box is open")
{
  cuke::is_true(cuke::context<box>().is_open());
}
```

And now we can run the first feature file: 
```gherkin
Feature: My First Feature File
  Just for demonstration

  Scenario: An arbitrary box
    Given A box with 2 x 2 x 2
    When I open the box
    Then The box is open 
```

## Whats Missing

So what is missing? By now I can think of is:
- Comprehensive documentation (I'm working on that)
- Languages (currently only english keywords are implemented)
- Rules
- Reports (json, ...) 
- Refactore, cleanups ... 
- ...
