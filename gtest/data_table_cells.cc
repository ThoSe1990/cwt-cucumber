#include <gtest/gtest.h>

#include "../src/test_runner.hpp"
#include "../src/parser.hpp"
#include "table.hpp"

// A data table cell is raw text delimited by '|'. A double quote, a '#' or an
// escaped '\|' inside it carries no syntax and must reach the step unchanged.
class data_table_cells : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    cells.clear();
    cuke::registry().clear();
    cuke::registry().push_step(cuke::internal::step_definition(
        [](const cuke::value_array&, const auto&, const auto&,
           const auto& table)
        {
          for (const auto& row : table.raw())
          {
            for (std::size_t col = 0; col < row.col_count(); ++col)
            {
              data_table_cells::cells.push_back(row[col].to_string());
            }
          }
        },
        "a step with a table"));
  }

  static std::vector<std::string> cells;
};
std::vector<std::string> data_table_cells::cells{};

static void run(const char* script)
{
  cuke::parser p;
  p.parse_script(script);
  ASSERT_FALSE(p.error());
  cuke::test_runner runner;
  p.for_each_scenario(runner);
}

TEST_F(data_table_cells, quotes_inside_a_cell)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | ["x = 1"] |
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 1);
  EXPECT_EQ(data_table_cells::cells.at(0), std::string("[\"x = 1\"]"));
}
TEST_F(data_table_cells, several_quoted_items_keep_their_order)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | ["m 7", "eval n"] |
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 1);
  EXPECT_EQ(data_table_cells::cells.at(0),
            std::string("[\"m 7\", \"eval n\"]"));
}
TEST_F(data_table_cells, an_odd_quote_is_text_not_a_string)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | "unterminated | second |
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 2);
  EXPECT_EQ(data_table_cells::cells.at(0), std::string("\"unterminated"));
  EXPECT_EQ(data_table_cells::cells.at(1), std::string("second"));
}
TEST_F(data_table_cells, hash_inside_a_cell)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | a#b | second |
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 2);
  EXPECT_EQ(data_table_cells::cells.at(0), std::string("a#b"));
  EXPECT_EQ(data_table_cells::cells.at(1), std::string("second"));
}
TEST_F(data_table_cells, hash_opening_a_cell)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | #1 | second |
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 2);
  EXPECT_EQ(data_table_cells::cells.at(0), std::string("#1"));
  EXPECT_EQ(data_table_cells::cells.at(1), std::string("second"));
}
TEST_F(data_table_cells, hash_after_a_space_inside_a_cell)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | issue #7 | second |
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 2);
  EXPECT_EQ(data_table_cells::cells.at(0), std::string("issue #7"));
  EXPECT_EQ(data_table_cells::cells.at(1), std::string("second"));
}
TEST_F(data_table_cells, escaped_delimiter_stays_in_the_cell)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | a\|b | second |
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 2);
  EXPECT_EQ(data_table_cells::cells.at(0), std::string("a\\|b"));
  EXPECT_EQ(data_table_cells::cells.at(1), std::string("second"));
}
TEST_F(data_table_cells, a_comment_line_after_a_table_is_still_a_comment)
{
  const char* script = R"*(
    Feature: a feature
    Scenario: a scenario
    Given a step with a table
      | first | second |
    # a comment right after the table
  )*";
  run(script);
  ASSERT_EQ(data_table_cells::cells.size(), 2);
  EXPECT_EQ(data_table_cells::cells.at(0), std::string("first"));
}
