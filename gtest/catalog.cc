#include <gtest/gtest.h>

#include "../src/registry.hpp"
#include "options.hpp"
#include "../src/catalog.hpp"

class catalog : public ::testing::Test
{
 protected:
  void TearDown() override {}
  void SetUp() override { cuke::registry().clear(); }
};

TEST_F(catalog, human_readable_string)
{
  const std::string expected = R"(Step Definitions (catalog):
---------------------------
Step a step with {int} and {string}
Step another step with {word} and {}
)";
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "a step with {int} and {string}"));
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "another step with {word} and {}"));

  EXPECT_EQ(expected, cuke::catalog::as_readable_text());
}
TEST_F(catalog, human_readable_string_with_keys)
{
  using namespace cuke::internal;

  const std::string expected = R"(Step Definitions (catalog):
---------------------------
Given a given step
When something happens
Then we expect a lot
)";
  cuke::registry().push_step(
      step([](const cuke::value_array& values, const auto&, const auto&) {},
           "a given step", step::type::given));
  cuke::registry().push_step(
      step([](const cuke::value_array& values, const auto&, const auto&) {},
           "something happens", step::type::when));
  cuke::registry().push_step(
      step([](const cuke::value_array& values, const auto&, const auto&) {},
           "we expect a lot", step::type::then));

  EXPECT_EQ(expected, cuke::catalog::as_readable_text());
}
TEST_F(catalog, human_readable_string_with_keys_and_sort)
{
  using namespace cuke::internal;

  const std::string expected = R"(Step Definitions (catalog):
---------------------------
Given c
Given d
When a
When f
When g
When i
Then b
Then e
Step h
)";

  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "a",
           step::type::when));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "b",
           step::type::then));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "c",
           step::type::given));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "d",
           step::type::given));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "e",
           step::type::then));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "f",
           step::type::when));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "g",
           step::type::when));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "h",
           step::type::step));
  cuke::registry().push_step(
      step([](const cuke::value_array&, const auto&, const auto&) {}, "i",
           step::type::when));

  EXPECT_EQ(expected, cuke::catalog::as_readable_text());
}

TEST_F(catalog, as_json_1)
{
  const std::string expected =
      R"({
  "steps_catalog": [
    {
      "definition": "a step with {int} and {string}",
      "type": "Step",
      "var_types": [
        "int",
        "string"
      ]
    },
    {
      "definition": "another step with {word} and {}",
      "type": "Step",
      "var_types": [
        "word",
        "anonymous"
      ]
    }
  ]
})";

  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "a step with {int} and {string}"));
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "another step with {word} and {}"));

  EXPECT_EQ(expected, cuke::catalog::as_json());
}

TEST_F(catalog, as_json_2)
{
  using namespace cuke::internal;

  const std::string expected =
      R"({"steps_catalog":[{"definition":"lets start with {}, {double} and {int}","type":"Given","var_types":["anonymous","double","int"]},{"definition":"lets start with {float} and {double}","type":"Given","var_types":["float","double"]},{"definition":"something with {string} and {long} happens","type":"When","var_types":["string","long"]},{"definition":"{float} added to {double}","type":"When","var_types":["float","double"]},{"definition":"step with {byte} and {word}","type":"Then","var_types":["byte","word"]},{"definition":"we have a lot {},{double},{int},{word},{string},{string},{int},{int},{string},{float},{byte}","type":"Then","var_types":["anonymous","double","int","word","string","string","int","int","string","float","byte"]}]})";

  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "step with {byte} and {word}", step::type::then));
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "something with {string} and {long} happens", step::type::when));
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "lets start with {}, {double} and {int}", step::type::given));
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "lets start with {float} and {double}", step::type::given));
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "{float} added to {double}", step::type::when));
  cuke::registry().push_step(cuke::internal::step(
      [](const cuke::value_array& values, const auto&, const auto&) {},
      "we have a lot "
      "{},{double},{int},{word},{string},{string},{int},{int},{string},{float},"
      "{byte}",
      step::type::then));

  EXPECT_EQ(expected, cuke::catalog::as_json(-1));
}
