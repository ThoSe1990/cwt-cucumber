#include "cucumber.hpp"

int main(int argc, const char* argv[])
{
  cwt::details::vm cuke_vm;

const char* script_en = R"*(
  Feature: Hello World
  Scenario: First Scenario
  Given Hello World
  When Hello World
  Then Hello World
  And Hello World
  But Hello World
  * Hello World
  )*";
const char* script_ger = R"*(
  #language: de
  Funktion: Hello World
  Szenario: First Scenario
  Gegeben sei Hello World
  Wenn Hello World
  Dann Hello World
  Und Hello World
  Aber Hello World
  * Hello World
  )*";

  cwt::details::return_code result = cuke_vm.interpret(script_ger);

  return result == cwt::details::return_code::passed ? 0 : 1 ;
}