# CWT Cucumber

CWT Cucumber is a Cucumber interpreter and engine to run cucumber tests in C/C++. 

## Disclaimer
This is an educational project for me. I have read Crafting Interpreters by Robert Nystrom and I want to apply these principles by scanning the document, creating bytecodes and running them in a VM. Since the original C++ Cucumber is not really supported and it's not that easy to use (build and runtime dependencies), it seemed like a useful learning project to do this. 

I have studied the Cucumber language and implemented everything to the best of my knowledge. If I missed any language features or there is something wrong, please let me know.


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
  [ ] Doc Strings  
  
Missing Syntax Tokens  
  [x] -  signed values  
  [ ] @  tags  
  [ ] "or" for tags   
  [ ] <>  variables  
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

  

