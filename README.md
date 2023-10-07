# Coding With Thomas: Cucumber

This is a Cucumber interpreter and engine to run cucumber tests in C/C++. 

## Disclaimer
This is my personal fun and educational project. I have read Crafting Interpreters by Robert Nystrom and I wanted a meaningful real world example to apply the principles. I really love the idea of Cucumber and use it a lot in my projects. Therefore I decided to create a Cucumber interpreter just for the fun of it. 


## Implementation Status 

Current Status of what comes to my mind: ([x] = done)
  
[ ] Cucumber Language Feauters   
  [ ] langauge tag  
  [ ] Other languages  
  [x] Comments  
  [x] Feature  
  [x] Scenario  
  [ ] Scenario Outline  
  [ ] Examples  
  [x] Steps  
  [ ] Rules  
  [ ] Tags  
  [x] Doc Strings  
  [ ] before, after, before scenario, after scenario 
  [ ] background
  [ ] undefined scenarios (proper error handling)


[ ] syntax error after examples table -> endless loop!

[x] print results (colored + overall results)
[ ] error handling invalid indices on get arg
[x] optional arg, eg. doc strings? 
[x] currently all variables are captured, skip constants in steps? 
[x] Scenario Outline after Scenario Outline does not work

Missing Syntax Tokens  
  [x] -  signed values  
  [ ] @  tags  
  [ ] "or" for tags   
  [x] <>  variables  
  [ ] #language:   
  
  
[x] Mising English Keywords: And, But, *  
  
[x] Concept for C++ bindings  
[ ] Scenario Scope / Context  
[ ] json report  
[ ] Program Options (argc, argv from main)  
[ ] Running all files from dir  
[ ] Running single step  
[ ] Tests, tests, tests, ...   
  
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

  

