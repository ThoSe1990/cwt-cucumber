#include <gtest/gtest.h>

#include "../src/registry.hpp"
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
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "a step with {int} and {string}"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
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
      step_definition([](const cuke::value_array& values, const auto&,
                         const auto&, const auto&) {},
                      "a given step", step_definition::type::given));
  cuke::registry().push_step(
      step_definition([](const cuke::value_array& values, const auto&,
                         const auto&, const auto&) {},
                      "something happens", step_definition::type::when));
  cuke::registry().push_step(
      step_definition([](const cuke::value_array& values, const auto&,
                         const auto&, const auto&) {},
                      "we expect a lot", step_definition::type::then));

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

  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "a", step_definition::type::when));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "b", step_definition::type::then));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "c", step_definition::type::given));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "d", step_definition::type::given));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "e", step_definition::type::then));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "f", step_definition::type::when));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "g", step_definition::type::when));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "h", step_definition::type::step));
  cuke::registry().push_step(step_definition(
      [](const cuke::value_array&, const auto&, const auto&, const auto&) {},
      "i", step_definition::type::when));

  EXPECT_EQ(expected, cuke::catalog::as_readable_text());
}

TEST_F(catalog, as_json_1)
{
  const std::string expected =
      R"({
  "steps_catalog": [
    {
      "definition": "a step with {int} and {string}",
      "function": "",
      "type": "Step",
      "var_types": [
        "int",
        "string"
      ]
    },
    {
      "definition": "another step with {word} and {}",
      "function": "",
      "type": "Step",
      "var_types": [
        "word",
        "anonymous"
      ]
    }
  ]
})";

  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "a step with {int} and {string}"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "another step with {word} and {}"));

  EXPECT_EQ(expected, cuke::catalog::as_json());
}

TEST_F(catalog, as_json_2)
{
  using namespace cuke::internal;

  const std::string expected =
      R"({"steps_catalog":[{"definition":"lets start with {}, {double} and {int}","function":"func3","type":"Given","var_types":["anonymous","double","int"]},{"definition":"lets start with {float} and {double}","function":"func4","type":"Given","var_types":["float","double"]},{"definition":"something with {string} and {long} happens","function":"func2","type":"When","var_types":["string","long"]},{"definition":"{float} added to {double}","function":"func5","type":"When","var_types":["float","double"]},{"definition":"step with {byte} and {word}","function":"func1","type":"Then","var_types":["byte","word"]},{"definition":"we have a lot {},{double},{int},{word},{string},{string},{int},{int},{string},{float},{byte}","function":"func6","type":"Then","var_types":["anonymous","double","int","word","string","string","int","int","string","float","byte"]}]})";

  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "step with {byte} and {word}", step_definition::type::then, "func1"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "something with {string} and {long} happens", step_definition::type::when,
      "func2"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "lets start with {}, {double} and {int}", step_definition::type::given,
      "func3"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "lets start with {float} and {double}", step_definition::type::given,
      "func4"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "{float} added to {double}", step_definition::type::when, "func5"));
  cuke::registry().push_step(cuke::internal::step_definition(
      [](const cuke::value_array& values, const auto&, const auto&,
         const auto&) {},
      "we have a lot "
      "{},{double},{int},{word},{string},{string},{int},{int},{string},{float},"
      "{byte}",
      step_definition::type::then, "func6"));

  EXPECT_EQ(expected, cuke::catalog::as_json(-1));
}
