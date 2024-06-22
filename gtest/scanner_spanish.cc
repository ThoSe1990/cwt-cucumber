#include <gtest/gtest.h>

#include "../src/scanner.hpp"

using namespace cwt::details;

TEST(spanish_keywords, feature_1)
{
  const char* script = R"*(
  # language:  es
  Característica:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(spanish_keywords, feature_2)
{
  const char* script = R"*(
  # language:  es
  Necesidad del negocio:
)*";
}
TEST(spanish_keywords, feature_3)
{
  const char* script = R"*(
  # language:  es
  Requisito:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(spanish_keywords, background)
{
  const char* script = R"*(
  # language:  es
  Antecedentes:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::background);
}
TEST(spanish_keywords, scenario_1)
{
  const char* script = R"*(
  # language:  es
  Ejemplo:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::scenario);
}
TEST(spanish_keywords, scenario_2)
{
  const char* script = R"*(
  # language:  es
  Escenario:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::scenario);
}
TEST(spanish_keywords, scenario_outline)
{
  const char* script = R"*(
  # language:  es
  Esquema del escenario:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::scenario_outline);
}
TEST(spanish_keywords, examples)
{
  const char* script = R"*(
  # language:  es
  Ejemplos:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::examples);
}
TEST(spanish_keywords, given_1)
{
  const char* script = R"*(
  # language:  es
  Dado
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, given_2)
{
  const char* script = R"*(
  # language:  es
  Dada
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, given_3)
{
  const char* script = R"*(
  # language:  es
  Dados
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, given_4)
{
  const char* script = R"*(
  # language:  es
  Dadas
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, when)
{
  const char* script = R"*(
  # language:  es
  Cuando
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, then)
{
  const char* script = R"*(
  # language:  es
  Entonces
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, and_1)
{
  const char* script = R"*(
  # language:  es
  Y
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, and_2)
{
  const char* script = R"*(
  # language:  es
  E
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(spanish_keywords, but)
{
  const char* script = R"*(
  # language:  es
  Pero
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}