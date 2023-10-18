# Coding With Thomas: Cucumber

This is a Cucumber interpreter and engine to run cucumber tests in C/C++. 

## Disclaimer
This is my personal fun and educational project. I have read Crafting Interpreters by Robert Nystrom and I wanted a meaningful real world example to apply the principles. I really love the idea of Cucumber and use it a lot in my projects. Therefore I decided to create a Cucumber interpreter just for the fun of it. 


## Implementation Status 

Current Status of what comes to my mind: ([x] = done)
  
scenario / scenarou outline step with beginning variable does not work! alwaays undefined..

[ ] Cucumber Language Feauters   
  [ ] langauge tag  
  [ ] Other languages  
  [x] Comments  
  [x] Feature  
  [x] Scenario  
  [x] Scenario Outline  
  [x] Examples  
  [x] Steps  
  [ ] Rules  
  [x] Tags   
  [x] Doc Strings  
  [x] hooks: before, after, before scenario, after scenario 
  [x] background
  [ ] background/examples description
  [ ] undefined scenarios (proper error handling)

[ ] compiler copmpile(): quick n dirty tags which "simulate" argv
[x] after tag with hook works for now but:
  [x] call all hooks (all after, all begin accordingly)
  [x] implement hooks for begin
  [x] implment step hooks too (no tags needed)
  [x] implement tags for scenario outline before example.


[x] adjust capacity (tables) for steps crashes with >8 steps

[ ] print file location to results
[ ] scenario outline -> variables are printed into the steps -> steps look similar in different examples
[ ] return value from run_cuke_from_argv -> return 0 if all tests pass from main
[ ] syntax error after examples table -> endless loop!
[ ] uint8_t => uint16_t (or 32?)
[x] print results (colored + overall results)
[x] error handling invalid indices on get arg
[x] optional arg, eg. doc strings? 
[x] currently all variables are captured, skip constants in steps? 
[x] Scenario Outline after Scenario Outline does not work

Missing Syntax Tokens  
  [x] -  signed values  
  [x] @  tags    
  [x] <>  variables  
  [ ] #language:   
  
  
[x] Mising English Keywords: And, But, *  
  
[x] Concept for C++ bindings  
[x] Scenario Scope / Context  
[ ] Program Options (argc, argv from main)  
[ ] Running all files from dir  
[ ] Running single step  
[ ] Tests, tests, tests, ...   
[ ] cleanup cleanup cleanup
  
[x] Steps: trim  
[ ] Steps: Missing Cucumber Expressions:   
  [x] int   
  [x] string   
  [x] float   
  [ ] word  
  [ ] anonymous  
  [x] double  
  [x] byte  
  [x] short  
  [x] long  
