#include "ast.hpp"
#include <gtest/gtest.h>

#include "../src/parser.hpp"
#include "table.hpp"

TEST(ast, init_obj) { cuke::parser p; }

TEST(ast, feature_w_parser)
{
  const char* script = "Feature: First Feature";
  cuke::parser p;
  p.parse_script(script);
  EXPECT_EQ(p.head().feature().keyword(), std::string("Feature"));
  EXPECT_EQ(p.head().feature().name(), std::string("First Feature"));
}
TEST(ast, feature_error)
{
  const char* script = "this is no feature";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  EXPECT_TRUE(lex.error());
  EXPECT_TRUE(feature.name().empty());
  EXPECT_TRUE(feature.keyword().empty());
}
TEST(ast, feature_error_parser)
{
  const char* script = "this is no feature";
  cuke::parser p;
  p.parse_script(script);
  EXPECT_TRUE(p.error());
}
TEST(ast, parser_scenario_and_scenario_outline)
{
  const char* script = R"*(
  Feature: A Feature
    
    Scenario: a scenario 
    Given a step 

    Scenario Outline: a scenario outline 
    Given a step with <var>
    
    Examples:  
    | var |
    | 1   |
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  EXPECT_EQ(p.head().feature().scenarios().size(), 2);
}
TEST(ast, parser_scenario_outline_two_examples)
{
  const char* script = R"*(
  Feature: A Feature
    
    Scenario: a scenario 
    Given a step 

    Scenario Outline: a scenario outline 
    Given a step with <var>
    
    Examples:  
    with some description


    | var |
    
    @tag1

    Examples:  
    with some description


    | var |
    | 1   |
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  EXPECT_EQ(p.head().feature().scenarios().size(), 2);
  ASSERT_EQ(p.head().feature().scenarios().at(1)->type(),
            cuke::ast::node_type::scenario_outline);

  auto scenario_outline = static_cast<cuke::ast::scenario_outline_node&>(
      *p.head().feature().scenarios().at(1));

  ASSERT_EQ(scenario_outline.examples().size(), 2);
  ASSERT_EQ(scenario_outline.examples().at(1).tags().size(), 1);
  ASSERT_EQ(scenario_outline.examples().at(1).description().size(), 3);
}
TEST(ast, feature)
{
  const char* script = "Feature: A Feature";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  EXPECT_EQ(feature.keyword(), std::string("Feature"));
  EXPECT_EQ(feature.name(), std::string("A Feature"));
  EXPECT_EQ(feature.line(), 1);
}
TEST(ast, feature_w_description)
{
  const char* script = R"*(
  Feature: A Feature
  with some 
  description below
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  EXPECT_EQ(feature.description().size(), 2);
  EXPECT_EQ(feature.description().at(0), std::string("with some"));
  EXPECT_EQ(feature.description().at(1), std::string("description below"));
  EXPECT_EQ(feature.line(), 2);
}

TEST(ast, feature_w_tags)
{
  const char* script = R"*(
  @tag1 @tag2
  Feature: A Feature
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  EXPECT_EQ(feature.tags().size(), 2);
  EXPECT_EQ(feature.tags().at(0), std::string("@tag1"));
  EXPECT_EQ(feature.tags().at(1), std::string("@tag2"));
  EXPECT_EQ(feature.line(), 3);
}

TEST(ast, no_background)
{
  const char* script = R"*(
  Feature: A feature
  Scenario: A Scenario
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  ASSERT_FALSE(feature.has_background());
}
TEST(ast, background_in_feature)
{
  const char* script = R"*(
  Feature: A feature
  Background: A background 
  Given With a step 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  ASSERT_TRUE(feature.has_background());
  EXPECT_EQ(feature.background().keyword(), std::string("Background"));
  EXPECT_EQ(feature.background().name(), std::string("A background"));
  EXPECT_EQ(feature.background().steps().size(), 1);
  EXPECT_EQ(feature.background().steps().at(0).keyword(), std::string("Given"));
  EXPECT_EQ(feature.background().steps().at(0).name(),
            std::string("With a step"));
}
TEST(ast, background_w_description)
{
  const char* script = R"*(
  Feature: A feature
  Background: A background 
  some description
  given here
  Given With a step 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  ASSERT_TRUE(feature.has_background());
  EXPECT_EQ(feature.background().description().size(), 2);
  EXPECT_EQ(feature.background().description().at(0),
            std::string("some description"));
  EXPECT_EQ(feature.background().description().at(1),
            std::string("given here"));
}

TEST(ast, background_w_following_scenario)
{
  const char* script = R"*(
  Feature: A feature
  Background: A background 
  some description
  given here
  Given With a step

  Scenario: Scenario follows here now 
  Given A Step
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  ASSERT_EQ(feature.scenarios().size(), 1);
  ASSERT_TRUE(feature.has_background());
  EXPECT_EQ(feature.background().steps().size(), 1);
}
TEST(ast, background_w_following_scenario_w_tags)
{
  const char* script = R"*(
  Feature: A feature
  Background: A background 
  some description
  given here
  Given With a step
  
  @tag
  Scenario: Scenario follows here now 
  Given A Step
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);

  ASSERT_TRUE(feature.has_background());
  EXPECT_EQ(feature.background().steps().size(), 1);

  ASSERT_EQ(feature.scenarios().size(), 1);
  cuke::ast::scenario_node& scenario =
      static_cast<cuke::ast::scenario_node&>(*feature.scenarios().at(0));
  EXPECT_EQ(scenario.tags().size(), 1);
  EXPECT_EQ(scenario.tags().at(0), std::string("@tag"));
}

TEST(ast, feature_w_scenario)
{
  const char* script = R"*(
  Feature: A feature
  Scenario: A Scenario
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  ASSERT_EQ(feature.scenarios().size(), 1);
  ASSERT_EQ(feature.scenarios().at(0)->type(), cuke::ast::node_type::scenario);
  EXPECT_EQ(feature.scenarios().at(0)->keyword(), std::string("Scenario"));
  EXPECT_EQ(feature.scenarios().at(0)->name(), std::string("A Scenario"));
}
TEST(ast, feature_w_descr_and_scenario)
{
  const char* script = R"*(
  Feature: A feature
  some description
  inserted here 
  Scenario: A Scenario
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  EXPECT_EQ(feature.description().size(), 2);
  ASSERT_EQ(feature.scenarios().size(), 1);
  EXPECT_EQ(feature.scenarios().at(0)->keyword(), std::string("Scenario"));
  EXPECT_EQ(feature.scenarios().at(0)->name(), std::string("A Scenario"));
}

TEST(ast, scenario_w_steps)
{
  const char* script = R"*(
  Scenario: A Scenario
  Given A step with value 5 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_EQ(scenarios.size(), 1);
}
TEST(ast, scenarios_w_steps)
{
  const char* script = R"*(
  Scenario: A Scenario
  Given A step with value 5 
  
  Scenario: A Scenario
  Given A step with value 5 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_EQ(scenarios.size(), 2);
}
TEST(ast, scenarios_wo_name)
{
  const char* script = R"*(
  Scenario: 
  Given A step with value 5 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_EQ(scenarios.size(), 1);
  EXPECT_TRUE(scenarios.at(0)->name().empty());
}
TEST(ast, scenario_w_description)
{
  const char* script = R"*(
  Scenario: A Scenario
  with some 
  multi line 
  description
  Given A step with value 5 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_EQ(scenarios.size(), 1);

  cuke::ast::scenario_node& scenario =
      static_cast<cuke::ast::scenario_node&>(*scenarios.at(0));
  ASSERT_EQ(scenario.description().size(), 3);
  EXPECT_EQ(scenario.description().at(0), std::string("with some"));
  EXPECT_EQ(scenario.description().at(1), std::string("multi line"));
  EXPECT_EQ(scenario.description().at(2), std::string("description"));

  EXPECT_EQ(scenario.steps().at(0).keyword(), std::string("Given"));
  EXPECT_EQ(scenario.steps().at(0).name(), std::string("A step with value 5"));
}

TEST(ast, scenario_w_tags)
{
  const char* script = R"*(
  @tag1 @tag2
  Scenario: A Scenario
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_EQ(scenarios.size(), 1);

  cuke::ast::scenario_node& scenario =
      static_cast<cuke::ast::scenario_node&>(*scenarios.at(0));
  ASSERT_EQ(scenario.tags().size(), 2);
  EXPECT_EQ(scenario.tags().at(0), std::string("@tag1"));
  EXPECT_EQ(scenario.tags().at(1), std::string("@tag2"));
}

TEST(ast, parse_step)
{
  const char* script = "Given A step with value 5";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_EQ(steps.size(), 1);
  EXPECT_EQ(steps.at(0).keyword(), std::string("Given"));
  EXPECT_EQ(steps.at(0).name(), std::string("A step with value 5"));
  EXPECT_TRUE(steps.at(0).doc_string().empty());
}
TEST(ast, parse_step_w_doc_string)
{
  const char* script = R"*(
    Given A step with value 5
    """
    this is 
    a multiline 
    doc string
    """ 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.at(0).doc_string().size(), 3);
  EXPECT_EQ(steps.at(0).doc_string().at(0), std::string("this is"));
  EXPECT_EQ(steps.at(0).doc_string().at(1), std::string("a multiline"));
  EXPECT_EQ(steps.at(0).doc_string().at(2), std::string("doc string"));
}
TEST(ast, parse_step_w_doc_string_w_backticks)
{
  const char* script = R"*(
    Given A step with value 5
    ``` 
    this is 
    a multiline 
    doc string
    ``` 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.at(0).doc_string().size(), 3);
  EXPECT_EQ(steps.at(0).doc_string().at(0), std::string("this is"));
  EXPECT_EQ(steps.at(0).doc_string().at(1), std::string("a multiline"));
  EXPECT_EQ(steps.at(0).doc_string().at(2), std::string("doc string"));
}

TEST(ast, multiple_steps)
{
  const char* script = R"*(
    Given A step with value 5
    Then Another step
    And One more
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.size(), 3);

  EXPECT_EQ(steps.at(0).keyword(), std::string("Given"));
  EXPECT_EQ(steps.at(1).keyword(), std::string("Then"));
  EXPECT_EQ(steps.at(2).keyword(), std::string("And"));

  EXPECT_EQ(steps.at(0).name(), std::string("A step with value 5"));
  EXPECT_EQ(steps.at(1).name(), std::string("Another step"));
  EXPECT_EQ(steps.at(2).name(), std::string("One more"));
}
TEST(ast, multiple_steps_w_docstring)
{
  const char* script = R"*(
    Given A step with value 5
    Then Another step
    """   
    a multi line
    doc string
    is here 
    """  
    And One more
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.size(), 3);

  ASSERT_EQ(steps.at(1).doc_string().size(), 3);
  EXPECT_EQ(steps.at(1).doc_string().at(0), std::string("a multi line"));
  EXPECT_EQ(steps.at(1).doc_string().at(1), std::string("doc string"));
  EXPECT_EQ(steps.at(1).doc_string().at(2), std::string("is here"));
}
TEST(ast, multiple_steps_error)
{
  const char* script = R"*(
    Given A step with value 5
    Then Another step
    this line is invalid!
    -> error detection does not happen in parse_steps()
    """  
    a multi line
    doc string
    is here 
    """  
    And One more
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_EQ(steps.size(), 2);
}

TEST(ast, empty_datatable)
{
  const char* script = R"*(
    Given A step with value 5
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  EXPECT_EQ(steps.at(0).data_table().cells_count(), 0);
}

TEST(ast, datatable)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2 |
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.size(), 1);
  ASSERT_EQ(steps.at(0).data_table().cells_count(), 2);
  ASSERT_EQ(steps.at(0).data_table().row_count(), 1);
  EXPECT_EQ(steps.at(0).data_table()[0][0].as<int>(), 1);
  EXPECT_EQ(steps.at(0).data_table()[0][1].as<int>(), 2);
}
TEST(ast, datatable_w_string_and_word)
{
  const char* script = R"*(
    Given A step with value 5
    | "some string" | a_word |
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.size(), 1);
  ASSERT_EQ(steps.at(0).data_table().cells_count(), 2);
  ASSERT_EQ(steps.at(0).data_table().row_count(), 1);
  EXPECT_EQ(steps.at(0).data_table()[0][0].as<std::string>(),
            std::string("some string"));
  EXPECT_EQ(steps.at(0).data_table()[0][1].as<std::string>(),
            std::string("a_word"));
}
TEST(ast, datatable_missing_vertical)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_TRUE(lex.error());
  ASSERT_EQ(steps.size(), 1);
}

TEST(ast, datatable_multi_row)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2 |
    | 3 | 4 | 
    | 5 | 6 | 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.size(), 1);
  ASSERT_EQ(steps.at(0).data_table().cells_count(), 6);
  ASSERT_EQ(steps.at(0).data_table().row_count(), 3);
  EXPECT_EQ(steps.at(0).data_table()[0][0].as<int>(), 1);
  EXPECT_EQ(steps.at(0).data_table()[0][1].as<int>(), 2);
  EXPECT_EQ(steps.at(0).data_table()[1][0].as<int>(), 3);
  EXPECT_EQ(steps.at(0).data_table()[1][1].as<int>(), 4);
  EXPECT_EQ(steps.at(0).data_table()[2][0].as<int>(), 5);
  EXPECT_EQ(steps.at(0).data_table()[2][1].as<int>(), 6);
}
TEST(ast, datatable_multi_row_w_following_step)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2 |
    | 3 | 4 | 
    | 5 | 6 | 
    And Another step 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(steps.size(), 2);
  ASSERT_EQ(steps.at(0).data_table().cells_count(), 6);
  ASSERT_EQ(steps.at(0).data_table().row_count(), 3);
  EXPECT_EQ(steps.at(0).data_table()[0][0].as<int>(), 1);
  EXPECT_EQ(steps.at(0).data_table()[0][1].as<int>(), 2);
  EXPECT_EQ(steps.at(0).data_table()[1][0].as<int>(), 3);
  EXPECT_EQ(steps.at(0).data_table()[1][1].as<int>(), 4);
  EXPECT_EQ(steps.at(0).data_table()[2][0].as<int>(), 5);
  EXPECT_EQ(steps.at(0).data_table()[2][1].as<int>(), 6);
}

TEST(ast, datatable_multi_row_error)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2 |
    | 3 | 4  
    | 5 | 6 | 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_TRUE(lex.error());
}

TEST(ast, datatable_multi_row_wrong_row_size)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2 |
    | 3 | 4 | 5 | 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cuke::internal::parse_steps(lex);
  ASSERT_TRUE(lex.error());
}

TEST(ast, feature_w_scenario_outline)
{
  const char* script = R"*(
  Feature: A feature
  Scenario Outline: A Scenario Outline
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  ASSERT_EQ(feature.scenarios().size(), 1);
  ASSERT_EQ(feature.scenarios().at(0)->type(),
            cuke::ast::node_type::scenario_outline);
  EXPECT_EQ(feature.scenarios().at(0)->keyword(),
            std::string("Scenario Outline"));
  EXPECT_EQ(feature.scenarios().at(0)->name(),
            std::string("A Scenario Outline"));
}

TEST(ast, feature_w_scenario_outline_steps)
{
  const char* script = R"*(
  Feature: A feature
  Scenario Outline: A Scenario Outline
  Given First Step
  Then Next Step
  And Another
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cuke::internal::parse_feature(lex);
  ASSERT_EQ(feature.scenarios().size(), 1);

  auto scenario_outline = static_cast<cuke::ast::scenario_outline_node&>(
      *feature.scenarios().at(0));

  ASSERT_EQ(scenario_outline.steps().size(), 3);
  EXPECT_EQ(scenario_outline.steps().at(0).name(), std::string("First Step"));
  EXPECT_EQ(scenario_outline.steps().at(1).name(), std::string("Next Step"));
  EXPECT_EQ(scenario_outline.steps().at(2).name(), std::string("Another"));
}
TEST(ast, parse_cell_integer)
{
  const char* script = "1234 |";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  cuke::value v = cuke::internal::parse_cell(lex, false);
  EXPECT_EQ(v.as<int>(), 1234);
}
TEST(ast, parse_cell_negative_integer)
{
  const char* script = "-1234 |";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  cuke::value v = cuke::internal::parse_cell(lex, false);
  EXPECT_EQ(v.as<int>(), -1234);
}
TEST(ast, parse_cell_double)
{
  const char* script = "1234.1234 |";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  cuke::value v = cuke::internal::parse_cell(lex, false);
  EXPECT_EQ(v.as<double>(), 1234.1234);
}
TEST(ast, parse_cell_string)
{
  const char* script = "\"hello world\" |";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  cuke::value v = cuke::internal::parse_cell(lex, true);
  EXPECT_EQ(v.as<std::string>(), std::string("hello world"));
}
TEST(ast, parse_cell_word)
{
  const char* script = "hello |";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  cuke::value v = cuke::internal::parse_cell(lex, false);
  EXPECT_EQ(v.as<std::string>(), std::string("hello"));
}
TEST(ast, parse_cell_unquoted_string)
{
  const char* script = "some arbitrary text and 1 2 3 numbers here |";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  cuke::value v = cuke::internal::parse_cell(lex, false);
  EXPECT_EQ(v.as<std::string>(),
            std::string("some arbitrary text and 1 2 3 numbers here"));
}
TEST(ast, parse_cell_empty_cell)
{
  const char* script = " |";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  cuke::value v = cuke::internal::parse_cell(lex, false);
  EXPECT_TRUE(v.is_nil());
  EXPECT_TRUE(lex.error());
}
TEST(ast, scenario_outline_w_example)
{
  const char* script = R"*(
  Scenario Outline: A Scenario Outline
  Given A step with <one> and <two> 

  Examples: 
  | one | two | 
  | 123 | 456 | 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(scenarios.size(), 1);

  auto& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(*scenarios.at(0));
  ASSERT_EQ(scenario_outline.examples().size(), 1);
}
TEST(ast, scenario_outline_w_2_example)
{
  const char* script = R"*(
  Scenario Outline: A Scenario Outline
  Given A step with <one> and <two> 

  Examples: 
  | one | two | 
  | 123 | 456 | 
  Examples: 
  | one | two | 
  | 123 | 456 | 
  | 123 | 456 | 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(scenarios.size(), 1);

  auto& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(*scenarios.at(0));
  ASSERT_EQ(scenario_outline.examples().size(), 2);
}

TEST(ast, example_w_tags)
{
  const char* script = R"*(
  Scenario Outline: A Scenario Outline
  Given A step with <var 1> and <var 2> 

  @tag1 @tag2 
  Examples: 
  | var 1 | var 2 | 
  | 123   | 456   | 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(scenarios.size(), 1);
  ASSERT_EQ(scenarios.back()->type(), cuke::ast::node_type::scenario_outline);

  auto& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(*scenarios.at(0));
  ASSERT_EQ(scenario_outline.examples().size(), 1);
  ASSERT_EQ(scenario_outline.examples().at(0).tags().size(), 2);
  ASSERT_EQ(scenario_outline.examples().at(0).tags().at(0),
            std::string("@tag1"));
  ASSERT_EQ(scenario_outline.examples().at(0).tags().at(1),
            std::string("@tag2"));
}

TEST(ast, example_w_name_description)
{
  const char* script = R"*(
  Scenario Outline: A Scenario Outline
  Given A step with <var 1> and <var 2> 

  @tag1 @tag2 
  Examples: some example
  with some 
  description 
  | var 1 | var 2 | 
  | 123   | 456   | 
  )*";
  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(scenarios.size(), 1);
  ASSERT_EQ(scenarios.back()->type(), cuke::ast::node_type::scenario_outline);

  auto& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(*scenarios.at(0));
  ASSERT_EQ(scenario_outline.examples().size(), 1);
  EXPECT_EQ(scenario_outline.examples().at(0).name(),
            std::string("some example"));
  EXPECT_EQ(scenario_outline.examples().at(0).description().size(), 2);
  EXPECT_EQ(scenario_outline.examples().at(0).description().at(0),
            std::string("with some"));
  EXPECT_EQ(scenario_outline.examples().at(0).description().at(1),
            std::string("description"));
  EXPECT_EQ(scenario_outline.examples().at(0).tags().size(), 2);
}
TEST(ast, two_example_w_name_description)
{
  const char* script = R"*(
  Scenario Outline: A Scenario Outline
  Given A step with <var 1> and <var 2> 

  @tag1 @tag2 
  Examples: some example
  with some 
  description 
  | var 1 | var 2 | 
  | 123   | 45    |

  Examples: another example
  with some 
  description 
  | var 1 | var 2 | 
  | 123   | 456   |
  
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me
  auto scenarios = cuke::internal::parse_scenarios(lex);
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(scenarios.size(), 1);
  ASSERT_EQ(scenarios.back()->type(), cuke::ast::node_type::scenario_outline);

  auto& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(*scenarios.at(0));
  ASSERT_EQ(scenario_outline.examples().size(), 2);
}
TEST(ast, example_name_description)
{
  const char* script = R"*(
  Examples: An example
  with some 
  multi line 
  description here 
  | one | two | 
  | 123 | 456 | 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto example = cuke::internal::parse_example(lex, {});
  EXPECT_EQ(example.name(), std::string("An example"));
  ASSERT_EQ(example.description().size(), 3);
  EXPECT_EQ(example.description().at(0), std::string("with some"));
  EXPECT_EQ(example.description().at(1), std::string("multi line"));
  EXPECT_EQ(example.description().at(2), std::string("description here"));
}
TEST(ast, examples_invalid_table)
{
  const char* script = R"*(
  Examples: An example
  with some 
  multi line 
  description here 
  | one | two  
  | 123 | 456 | 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto example = cuke::internal::parse_example(lex, {});
  ASSERT_TRUE(lex.error());
}
TEST(ast, examples_table_w_linebreak)
{
  const char* script = R"*(
  Examples: An example
  with some 
  multi line 
  description here 

  | one | two | 

  | 123 | 456 | 
 
  )*";

  cuke::internal::lexer lex(script);
  lex.advance();  // TODO delete me

  auto example = cuke::internal::parse_example(lex, {});
  ASSERT_FALSE(lex.error());
  ASSERT_EQ(example.table().cells_count(), 4);
  ASSERT_EQ(example.table().col_count(), 2);
  ASSERT_EQ(example.table().row_count(), 2);
}
TEST(ast, full_feature)
{
  const char* script = R"*(
  Feature: a full feature
    with some description 

    Scenario: First Scenario 
    Given a step
    Then another step 

    Scenario Outline: an scenario outline 
    with some description 
    Given a step with <var1> 
    Then another step with <var2> 

    Examples: 
      | var1 | var2 |
      | "hello world" | 1 |
      | "some text"   | 2 |

    Scenario: Another Scenario 
     Given with a step 
     And another step 
  )*";

  cuke::parser p;
  p.parse_script(script);
  const cuke::ast::feature_node& feature = p.head().feature();

  ASSERT_EQ(feature.scenarios().size(), 3);
}

#include "registry.hpp"

class ast_steps_w_values : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const auto&, const auto&, const auto&, const auto&) {},
        "a step with {int} and {string}"));
  }
};

TEST_F(ast_steps_w_values, scenario)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario: a scenario
    Given a step with 123 and "hello world"
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 1);

  cuke::ast::scenario_node& scenario = static_cast<cuke::ast::scenario_node&>(
      *p.head().feature().scenarios().at(0));
  ASSERT_EQ(scenario.steps().size(), 1);

  const cuke::ast::step_node& step = scenario.steps().at(0);

  ASSERT_EQ(step.values().size(), 2);
  EXPECT_EQ(step.values().at(0).as<int>(), 123);
  EXPECT_EQ(step.values().at(1).to_string(), "hello world");
}
TEST_F(ast_steps_w_values, scenario_w_undefined_step)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario: a scenario
    Given a step with 123 and "hello world"
    And an undefined step
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 1);

  cuke::ast::scenario_node& scenario = static_cast<cuke::ast::scenario_node&>(
      *p.head().feature().scenarios().at(0));
  ASSERT_EQ(scenario.steps().size(), 2);

  EXPECT_TRUE(scenario.steps().at(0).has_step_definition());
  EXPECT_FALSE(scenario.steps().at(1).has_step_definition());
}

TEST_F(ast_steps_w_values, scenario_outline)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario Outline: a scenario
    Given a step with <value-1> and <value-2> 

    Examples: 
    | value-1 | value-2 |
    | 101     | "hello" |
    | 999     | "world" | 
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 1);

  cuke::ast::scenario_outline_node& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(
          *p.head().feature().scenarios().at(0));
  ASSERT_EQ(scenario_outline.steps().size(), 1);
  ASSERT_EQ(scenario_outline.scenarios_count(), 2);

  const cuke::ast::scenario_node& first = scenario_outline.scenario(0);
  ASSERT_EQ(first.steps().size(), 1);
  ASSERT_EQ(first.steps().at(0).values().size(), 2);
  EXPECT_EQ(first.steps().at(0).values().at(0).as<int>(), 101);
  EXPECT_EQ(first.steps().at(0).values().at(1).to_string(), "hello");

  const cuke::ast::scenario_node& second = scenario_outline.scenario(1);
  ASSERT_EQ(second.steps().size(), 1);
  ASSERT_EQ(second.steps().at(0).values().size(), 2);
  EXPECT_EQ(second.steps().at(0).values().at(0).as<int>(), 999);
  EXPECT_EQ(second.steps().at(0).values().at(1).to_string(), "world");
}
TEST_F(ast_steps_w_values, scenario_outline_undefined_steps)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario Outline: a scenario
    Given a step with <value-1> and <value-2> 
    And an undefined step 

    Examples: 
    | value-1 | value-2 |
    | 101     | "hello" |
    | 999     | "world" | 
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 1);

  cuke::ast::scenario_outline_node& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(
          *p.head().feature().scenarios().at(0));
  ASSERT_EQ(scenario_outline.steps().size(), 2);
  ASSERT_EQ(scenario_outline.scenarios_count(), 2);

  const cuke::ast::scenario_node& first = scenario_outline.scenario(0);
  ASSERT_EQ(first.steps().size(), 2);
  EXPECT_TRUE(first.steps().at(0).has_step_definition());
  EXPECT_FALSE(first.steps().at(1).has_step_definition());

  const cuke::ast::scenario_node& second = scenario_outline.scenario(1);
  ASSERT_EQ(second.steps().size(), 2);
  EXPECT_TRUE(second.steps().at(0).has_step_definition());
  EXPECT_FALSE(second.steps().at(1).has_step_definition());
}
TEST_F(ast_steps_w_values,
       scenario_outline_var_replacement_in_name_and_description)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario Outline: a scenario with <value-1> and <value-2>
      we can also use a description 
      where we see <value-1> 
      and <value-2> 
      or both <value-1> and <value-2> 
    Given a step with <value-1> and <value-2> 

    Examples: 
    | value-1 | value-2 |
    | 101     | "hello" |
    | 999     | "world" | 
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 1);

  cuke::ast::scenario_outline_node& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(
          *p.head().feature().scenarios().at(0));
  ASSERT_EQ(scenario_outline.steps().size(), 1);
  ASSERT_EQ(scenario_outline.scenarios_count(), 2);

  const cuke::ast::scenario_node& first = scenario_outline.scenario(0);
  EXPECT_EQ(first.name(), std::string("a scenario with 101 and \"hello\""));
  ASSERT_EQ(first.description().size(), 4);
  EXPECT_EQ(first.description().at(0),
            std::string("we can also use a description"));
  EXPECT_EQ(first.description().at(1), std::string("where we see 101"));
  EXPECT_EQ(first.description().at(2), std::string("and \"hello\""));
  EXPECT_EQ(first.description().at(3),
            std::string("or both 101 and \"hello\""));

  const cuke::ast::scenario_node& second = scenario_outline.scenario(1);
  EXPECT_EQ(second.name(), std::string("a scenario with 999 and \"world\""));
  ASSERT_EQ(second.description().size(), 4);
  EXPECT_EQ(second.description().at(0),
            std::string("we can also use a description"));
  EXPECT_EQ(second.description().at(1), std::string("where we see 999"));
  EXPECT_EQ(second.description().at(2), std::string("and \"world\""));
  EXPECT_EQ(second.description().at(3),
            std::string("or both 999 and \"world\""));
}
TEST_F(ast_steps_w_values, scenario_outline_var_replacement_table)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario Outline: a scenario 
    Given a step with <value-1> and <value-2> 
    | 12345678          | "an arbitrary table ..." | here            |
    | "with: <value-1>" | <value-1>                | "and <value-2>" | 

    Examples: 
    | value-1 | value-2 |
    | 101     | "hello" |
    | 999     | "world" | 
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 1);

  cuke::ast::scenario_outline_node& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(
          *p.head().feature().scenarios().at(0));
  ASSERT_EQ(scenario_outline.steps().size(), 1);
  ASSERT_EQ(scenario_outline.scenarios_count(), 2);

  const cuke::ast::scenario_node& first = scenario_outline.scenario(0);
  ASSERT_EQ(first.steps().size(), 1);

  const cuke::table& t1 = first.steps().at(0).data_table();
  ASSERT_EQ(t1.col_count(), 3);
  ASSERT_EQ(t1.row_count(), 2);
  EXPECT_EQ(t1[0][0].as<long>(), 12345678);
  EXPECT_EQ(t1[0][1].to_string(), "an arbitrary table ...");
  EXPECT_EQ(t1[0][2].to_string(), "here");
  EXPECT_EQ(t1[1][0].to_string(), "with: 101");
  EXPECT_EQ(t1[1][1].as<int>(), 101);
  EXPECT_EQ(t1[1][2].to_string(), "and \"hello\"");

  const cuke::ast::scenario_node& second = scenario_outline.scenario(1);
  ASSERT_EQ(second.steps().size(), 1);

  const cuke::table& t2 = second.steps().at(0).data_table();
  ASSERT_EQ(t2.col_count(), 3);
  ASSERT_EQ(t2.row_count(), 2);
  EXPECT_EQ(t2[0][0].as<long>(), 12345678);
  EXPECT_EQ(t2[0][1].to_string(), "an arbitrary table ...");
  EXPECT_EQ(t2[0][2].to_string(), "here");
  EXPECT_EQ(t2[1][0].to_string(), "with: 999");
  EXPECT_EQ(t2[1][1].as<int>(), 999);
  EXPECT_EQ(t2[1][2].to_string(), "and \"world\"");
}
TEST_F(ast_steps_w_values, scenario_outline_var_replacement_doc_string)
{
  const char* script = R"*(
  Feature: A Feature

    Scenario Outline: a scenario 
    Given a step with <value-1> and <value-2> 
    """ 
    and a doc string 
    with <value-1> and <value-2> 
    """

    Examples: 
    | value-1 | value-2 |
    | 101     | "hello" |
    | 999     | "world" | 
  )*";
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  ASSERT_EQ(p.head().feature().scenarios().size(), 1);

  cuke::ast::scenario_outline_node& scenario_outline =
      static_cast<cuke::ast::scenario_outline_node&>(
          *p.head().feature().scenarios().at(0));
  ASSERT_EQ(scenario_outline.steps().size(), 1);
  ASSERT_EQ(scenario_outline.scenarios_count(), 2);

  const cuke::ast::scenario_node& first = scenario_outline.scenario(0);
  ASSERT_EQ(first.steps().size(), 1);

  const std::vector<std::string>& doc_string_1 =
      first.steps().at(0).doc_string();
  ASSERT_EQ(doc_string_1.size(), 2);
  EXPECT_EQ(doc_string_1.at(0), std::string("and a doc string"));
  EXPECT_EQ(doc_string_1.at(1), std::string("with 101 and \"hello\""));

  const cuke::ast::scenario_node& second = scenario_outline.scenario(1);
  ASSERT_EQ(second.steps().size(), 1);

  const std::vector<std::string>& doc_string_2 =
      second.steps().at(0).doc_string();
  ASSERT_EQ(doc_string_2.size(), 2);
  EXPECT_EQ(doc_string_2.at(0), std::string("and a doc string"));
  EXPECT_EQ(doc_string_2.at(1), std::string("with 999 and \"world\""));
}
