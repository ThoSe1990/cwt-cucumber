#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cwt::details;

class vm_run_scenarios : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step([](argc, argv) {}, "A Step"));
    test_vm.push_step(step([](argc, argv) {}, "A Step with {int}"));
  }

  void TearDown() override { test_vm.reset(); }

  vm test_vm;
};

TEST_F(vm_run_scenarios, run_no_scenario)
{
  const char* script = R"*(
  Feature: First Feature
        Following missbehavior during extension development
        with extensions in plan, the stage transition 'tx = {50,60}' was triggered 
        when we were in stage 4 (default stage in first transition)
        This means transitions with extensions were triggered during plan running and forking.
        Its a runtime problem, bc validation was good (as expected)

)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(0, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(0, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_run_scenarios, run_one_scenario)
{
  const char* script = R"*(
  Feature: First Feature
        Following missbehavior during extension development
        with extensions in plan, the stage transition 'tx = {50,60}' was triggered 
        when we were in stage 4 (default stage in first transition)
        This means transitions with extensions were triggered during plan running and forking.
        Its a runtime problem, bc validation was good (as expected)
  Scenario: First Scenario
  Given A Step

)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(1, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(1, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_run_scenarios, run_one_scenario_w_doc_stringg_1)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      ```
      Following missbehavior during extension development
      with extensions in plan, the stage transition 'tx = {50,60}' was triggered 
      when we were in stage 4 (default stage in first transition)
      This means transitions with extensions were triggered during plan running and forking.
      Its a runtime problem, bc validation was good (as expected)
      ```
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}

TEST_F(vm_run_scenarios, run_one_scenario_w_doc_string_2)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      ```
      graph = {
          {1,2,15},
          {2,1,15}
      }
      ```
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(2, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_run_scenarios, run_one_scenario_w_doc_string_3)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      """
      graph = {
          {1,2,15},
          {2,1,15}
      }
      """
    And A Step
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(3, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_run_scenarios, invalid_characters_after_doc_string)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      """
Lorem ipsum dolor sit amet, odio molestiae interesset has in, ad unum brute saperet quo, ius nibh paulo appellantur te. An tota dicam utroque nam, ea his constituam reprehendunt. Per tantas doming cu, facilis docendi evertitur ea sit. Vix ne ludus verterem, eu tota reprehendunt duo, consetetur comprehensam has ut. At pro doming voluptua posidonium.
Dolorum dissentiunt sea ei, te usu eloquentiam adversarium. Et ius aeque viris rationibus, at per minim ancillae. Per alia fierent omittam ex, usu illud soleat invenire et. Rebum partiendo dissentias mea eu. Nominavi laboramus te sea, pro melius consectetuer ne.
Quando vivendum antiopam usu eu, iudico gloriatur est ei. Sit ei melius mandamus pericula, libris aeterno referrentur nec at. Qui an sapientem eloquentiam, sumo debet dicunt quo eu. Melius appareat concludaturque an pro, no omnium viderer pro. Usu ei sumo choro doming, vero ludus contentiones qui ea. Ea conceptam incorrupte quo, cu decore oporteat recteque qui.
Tritani impedit tibique pri cu, no duo integre dignissim reprehendunt, vim cu error solet tacimates. At nam error percipit, mea sale animal officiis id. Admodum invidunt ut eum. Dicat aperiam explicari mel ut, at dolor lobortis eum. Eu cum diam zril.
An sed nullam moderatius percipitur, vel modus perfecto erroribus eu. His discere vocibus concludaturque ne, ex aperiam neglegentur disputationi est. Vivendum probatus duo ut, at delenit sententiae sed. Hinc nominavi inciderint ei qui, ut mediocrem hendrerit temporibus sea, an duo ludus conclusionemque. Harum clita eu pro, usu saperet torquatos instructior et, atqui accumsan eu qui.
      """some invalid stuff here 
    And A Step
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"", script, {}};
  ASSERT_EQ(return_code::compile_error, test_vm.run(f));
}
TEST_F(vm_run_scenarios, eof_after_doc_string)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      """
Lorem ipsum dolor sit amet, odio molestiae interesset has in, ad unum brute saperet quo, ius nibh paulo appellantur te. An tota dicam utroque nam, ea his constituam reprehendunt. Per tantas doming cu, facilis docendi evertitur ea sit. Vix ne ludus verterem, eu tota reprehendunt duo, consetetur comprehensam has ut. At pro doming voluptua posidonium.
Dolorum dissentiunt sea ei, te usu eloquentiam adversarium. Et ius aeque viris rationibus, at per minim ancillae. Per alia fierent omittam ex, usu illud soleat invenire et. Rebum partiendo dissentias mea eu. Nominavi laboramus te sea, pro melius consectetuer ne.
Quando vivendum antiopam usu eu, iudico gloriatur est ei. Sit ei melius mandamus pericula, libris aeterno referrentur nec at. Qui an sapientem eloquentiam, sumo debet dicunt quo eu. Melius appareat concludaturque an pro, no omnium viderer pro. Usu ei sumo choro doming, vero ludus contentiones qui ea. Ea conceptam incorrupte quo, cu decore oporteat recteque qui.
Tritani impedit tibique pri cu, no duo integre dignissim reprehendunt, vim cu error solet tacimates. At nam error percipit, mea sale animal officiis id. Admodum invidunt ut eum. Dicat aperiam explicari mel ut, at dolor lobortis eum. Eu cum diam zril.
An sed nullam moderatius percipitur, vel modus perfecto erroribus eu. His discere vocibus concludaturque ne, ex aperiam neglegentur disputationi est. Vivendum probatus duo ut, at delenit sententiae sed. Hinc nominavi inciderint ei qui, ut mediocrem hendrerit temporibus sea, an duo ludus conclusionemque. Harum clita eu pro, usu saperet torquatos instructior et, atqui accumsan eu qui.
      """)*";
  file f{"", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
}

TEST_F(vm_run_scenarios, whitespaces_after_doc_string)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
      """
Lorem ipsum dolor sit amet, odio molestiae interesset has in, ad unum brute saperet quo, ius nibh paulo appellantur te. An tota dicam utroque nam, ea his constituam reprehendunt. Per tantas doming cu, facilis docendi evertitur ea sit. Vix ne ludus verterem, eu tota reprehendunt duo, consetetur comprehensam has ut. At pro doming voluptua posidonium.
Dolorum dissentiunt sea ei, te usu eloquentiam adversarium. Et ius aeque viris rationibus, at per minim ancillae. Per alia fierent omittam ex, usu illud soleat invenire et. Rebum partiendo dissentias mea eu. Nominavi laboramus te sea, pro melius consectetuer ne.
Quando vivendum antiopam usu eu, iudico gloriatur est ei. Sit ei melius mandamus pericula, libris aeterno referrentur nec at. Qui an sapientem eloquentiam, sumo debet dicunt quo eu. Melius appareat concludaturque an pro, no omnium viderer pro. Usu ei sumo choro doming, vero ludus contentiones qui ea. Ea conceptam incorrupte quo, cu decore oporteat recteque qui.
Tritani impedit tibique pri cu, no duo integre dignissim reprehendunt, vim cu error solet tacimates. At nam error percipit, mea sale animal officiis id. Admodum invidunt ut eum. Dicat aperiam explicari mel ut, at dolor lobortis eum. Eu cum diam zril.
An sed nullam moderatius percipitur, vel modus perfecto erroribus eu. His discere vocibus concludaturque ne, ex aperiam neglegentur disputationi est. Vivendum probatus duo ut, at delenit sententiae sed. Hinc nominavi inciderint ei qui, ut mediocrem hendrerit temporibus sea, an duo ludus conclusionemque. Harum clita eu pro, usu saperet torquatos instructior et, atqui accumsan eu qui.
      """    )*";
  file f{"", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
}



TEST_F(vm_run_scenarios, run_one_scenario_w_table)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
  | 1 | 2 | 
  | 3 | 4 | 
    And A Step
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"no path", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
  EXPECT_EQ(2, test_vm.scenario_results().at(return_code::passed));
  EXPECT_EQ(3, test_vm.step_results().at(return_code::passed));
}
TEST_F(vm_run_scenarios, invalid_characters_after_table)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
    | 1 | 2 | 
    | 3 | 4 | some invalid stuff here 
    And A Step
  Scenario: Second Scenario
    Given A Step
)*";
  file f{"", script, {}};
  ASSERT_EQ(return_code::compile_error, test_vm.run(f));
}
TEST_F(vm_run_scenarios, eof_after_table)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
    | 1 | 2 | 
    | 3 | 4 |)*";
  file f{"", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
}
TEST_F(vm_run_scenarios, linebreak_and_eof_after_table)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
    | 1 | 2 | 
    | 3 | 4 |
    )*";
  file f{"", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
}

TEST_F(vm_run_scenarios, whitespaces_after_table)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given A Step
    | 1 | 2 | 
    | 3 | 4 |     )*";
  file f{"", script, {}};
  ASSERT_EQ(return_code::passed, test_vm.run(f));
}
