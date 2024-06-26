#include <gtest/gtest.h>

#include "../src/ast/parser.hpp"

TEST(ast, init_obj)
{
  cuke::parser p;
}

TEST(ast, feature_w_parser)
{
  const char* script = "Feature: First Feature";
  cuke::parser p;
  p.parse_script(script);
  EXPECT_EQ(p.head().type(), cuke::ast::node_type::gherkin_document);
  EXPECT_EQ(p.head().feature().keyword(), std::string("Feature:"));
  EXPECT_EQ(p.head().feature().name(), std::string("First Feature"));
}
TEST(ast, feature)
{
  const char* script = "Feature: A Feature";
  cwt::details::lexer lex(script);
  lex.advance();
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  EXPECT_EQ(feature.keyword(), std::string("Feature:"));
  EXPECT_EQ(feature.name(), std::string("A Feature"));
}
TEST(ast, feature_w_description)
{
  const char* script = R"*(
  Feature: A Feature
  with some 
  description below
  )*";
  cwt::details::lexer lex(script);
  lex.advance();
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  EXPECT_EQ(feature.description().size(), 2);
  EXPECT_EQ(feature.description().at(0), std::string("with some"));
  EXPECT_EQ(feature.description().at(1), std::string("description below"));
}

TEST(ast, feature_w_tags)
{
  const char* script = R"*(
  @tag1 @tag2
  Feature: A Feature
  )*";
  cwt::details::lexer lex(script);
  lex.advance();
  cuke::ast::feature_node feature = cwt::details::parse_feature(lex);
  EXPECT_EQ(feature.tags().size(), 2);
  EXPECT_EQ(feature.tags().at(0), std::string("@tag1"));
  EXPECT_EQ(feature.tags().at(1), std::string("@tag2"));
}


// TEST(ast, make_ast)
// {
//   const char* script = R"*(
//   Feature:
// )*";
//   cuke::ast::node head = cwt::details::make_ast(script); 
//   EXPECT_EQ(head.type, cuke::ast::node_type::gherkin_document);
//   EXPECT_EQ(head.children[0].type, cuke::ast::node_type::feature);
// }


// TEST(ast, feature_wo_parser) 
// {
//   const char* script = "Feature:";
//   cwt::details::lexer lex(script);
//   cuke::ast::node node;
//   cwt::details::parse_feature(lex, node);
//   EXPECT_EQ(node.type, cuke::ast::node_type::feature);
// }

// TEST(ast, scenario)
// {
//   const char* script = R"*(
//   Feature:
//   Scenario: 
// )*";
//   cwt::details::parser p(script);
//   p.parse();
//   EXPECT_EQ(p.head().children.size(), 1);
//   EXPECT_EQ(p.head().children[0].type, cuke::ast::node_type::feature); 
// }
// TEST(ast, scenario_wo_parser)
// {
//   const char* script = "Scenario:";
//   cwt::details::lexer lex(script);
//   lex.advance();
//   cuke::ast::node node;
//   cwt::details::parse_scenario(lex, node);
//   EXPECT_EQ(node.type, cuke::ast::node_type::scenario);
// }