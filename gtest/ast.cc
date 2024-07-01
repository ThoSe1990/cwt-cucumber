#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"

TEST(ast, init_obj) { cuke::parser p; }

TEST(ast, feature_w_parser)
{
  const char* script = "Feature: First Feature";
  cuke::parser p;
  p.parse_script(script);
  EXPECT_EQ(p.head().feature().keyword(), std::string("Feature:"));
  EXPECT_EQ(p.head().feature().name(), std::string("First Feature"));
}
TEST(ast, feature_error)
{
  const char* script = "this is no feature";
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  EXPECT_TRUE(lex.error());
  EXPECT_TRUE(feature.name().empty());
  EXPECT_TRUE(feature.keyword().empty());
}
TEST(ast, feature)
{
  const char* script = "Feature: A Feature";
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  EXPECT_EQ(feature.keyword(), std::string("Feature:"));
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  EXPECT_EQ(feature.tags().size(), 2);
  EXPECT_EQ(feature.tags().at(0), std::string("@tag1"));
  EXPECT_EQ(feature.tags().at(1), std::string("@tag2"));
  EXPECT_EQ(feature.line(), 3);
}

TEST(ast, feature_w_scenario)
{
  const char* script = R"*(
  Feature: A feature
  Scenario: A Scenario
  )*";
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  ASSERT_EQ(feature.scenarios().size(), 1);
  EXPECT_EQ(feature.scenarios().at(0)->keyword(), std::string("Scenario:"));
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  EXPECT_EQ(feature.description().size(), 2); 
  ASSERT_EQ(feature.scenarios().size(), 1);
  EXPECT_EQ(feature.scenarios().at(0)->keyword(), std::string("Scenario:"));
  EXPECT_EQ(feature.scenarios().at(0)->name(), std::string("A Scenario"));
}

TEST(ast, scenario_w_steps)
{
  const char* script = R"*(
  Scenario: A Scenario
  Given A step with value 5 
  )*";
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cwt::details::parse_scenarios(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cwt::details::parse_scenarios(lex);
  ASSERT_EQ(scenarios.size(), 2);
}
TEST(ast, scenarios_wo_name)
{
  const char* script = R"*(
  Scenario: 
  Given A step with value 5 
  )*";
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cwt::details::parse_scenarios(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cwt::details::parse_scenarios(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto scenarios = cwt::details::parse_scenarios(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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
    """  
    a multi line
    doc string
    is here 
    """  
    And One more
  )*";
  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
  ASSERT_TRUE(lex.error());
  ASSERT_EQ(steps.size(), 0);
}

TEST(ast, empty_datatable)
{
  const char* script = R"*(
    Given A step with value 5
  )*";

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
  EXPECT_EQ(steps.at(0).data_table().cells_count(), 0);
}

TEST(ast, datatable)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2 |
  )*";

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
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

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
  ASSERT_TRUE(lex.error());
}

TEST(ast, datatable_multi_row_wrong_row_size)
{
  const char* script = R"*(
    Given A step with value 5
    | 1 | 2 |
    | 3 | 4 | 5 | 
  )*";

  cwt::details::lexer lex(script);
  lex.advance();  // TODO delete me

  auto steps = cwt::details::parse_steps(lex);
  ASSERT_TRUE(lex.error());
}