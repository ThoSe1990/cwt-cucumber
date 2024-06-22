#include <gtest/gtest.h>

#include "../src/scanner.hpp"

using namespace cwt::details;

TEST(german_keywords, feature_1)
{
  const char* script = R"*(
  # language:  de
  Funktionalität:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(german_keywords, feature_2)
{
  const char* script = R"*(
  # language:  de
  Funktion:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::feature);
}
TEST(german_keywords, background_1)
{
  const char* script = R"*(
  # language:  de
  Grundlage:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::background);
}
TEST(german_keywords, background_2)
{
  const char* script = R"*(
  # language:  de
  Hintergrund:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::background);
}
TEST(german_keywords, background_3)
{
  const char* script = R"*(
  # language:  de
  Voraussetzungen:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::background);
}
TEST(german_keywords, background_4)
{
  const char* script = R"*(
  # language:  de
  Vorbedingungen:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::background);
}
TEST(german_keywords, scenario_1)
{
  const char* script = R"*(
  # language:  de
  Beispiel:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::scenario);
}
TEST(german_keywords, scenario_2)
{
  const char* script = R"*(
  # language:  de
  Szenario:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::scenario);
}
TEST(german_keywords, scenario_outline_1)
{
  const char* script = R"*(
  # language:  de
  Szenariogrundriss:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::scenario_outline);
}
TEST(german_keywords, scenario_outline_2)
{
  const char* script = R"*(
  # language:  de
  Szenarien:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::scenario_outline);
}
TEST(german_keywords, examples)
{
  const char* script = R"*(
  # language:  de
  Beispiele:
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::examples);
}
TEST(german_keywords, given_1)
{
  const char* script = R"*(
  # language:  de
  Angenommen
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(german_keywords, given_2)
{
  const char* script = R"*(
  # language:  de
  Gegeben sei
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(german_keywords, given_3)
{
  const char* script = R"*(
  # language:  de
  Gegeben seien
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(german_keywords, when)
{
  const char* script = R"*(
  # language:  de
  Wenn
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(german_keywords, then)
{
  const char* script = R"*(
  # language:  de
  Dann
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(german_keywords, and)
{
  const char* script = R"*(
  # language:  de
  Und
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}
TEST(german_keywords, but)
{
  const char* script = R"*(
  # language:  de
  Aber
)*";
  EXPECT_EQ(scanner(script).scan_token().type, token_type::step);
}