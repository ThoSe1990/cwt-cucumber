#include <gtest/gtest.h>
#include <stdexcept>

#include "../src/value.hpp"
#include "../src/table.hpp"

cuke::value_array make_matrix(std::size_t dim)
{
  cuke::value_array values;
  std::size_t cells = dim * dim;
  for (std::size_t i = 0; i < cells; ++i)
  {
    values.push_back(cuke::value(std::to_string(i)));
  }
  return values;
}

TEST(table, init_table) { cuke::table t; }
TEST(table, inconsisten_table)
{
  cuke::value_array data{cuke::value("1"), cuke::value("2"), cuke::value("3"),
                         cuke::value("4"), cuke::value("5")};
  EXPECT_THROW({ cuke::table t(data, 2); }, std::runtime_error);
}
TEST(table, size)
{
  std::size_t dim = 3;
  cuke::table t(make_matrix(dim), dim);
  EXPECT_EQ(t.row_count(), dim);
  EXPECT_EQ(t.col_count(), dim);
  EXPECT_EQ(t.cells_count(), dim * dim);
}
TEST(table, raw_access)
{
  std::size_t dim = 3;
  cuke::table t(make_matrix(dim), dim);

  EXPECT_EQ(t[0][0].as<int>(), 0);
  EXPECT_EQ(t[0][1].as<int>(), 1);
  EXPECT_EQ(t[0][2].as<int>(), 2);
  EXPECT_EQ(t[1][0].as<int>(), 3);
  EXPECT_EQ(t[1][1].as<int>(), 4);
  EXPECT_EQ(t[1][2].as<int>(), 5);
  EXPECT_EQ(t[2][0].as<int>(), 6);
  EXPECT_EQ(t[2][1].as<int>(), 7);
  EXPECT_EQ(t[2][2].as<int>(), 8);
}
TEST(table, invalid_row_access)
{
  std::size_t dim = 3;
  cuke::table t(make_matrix(dim), dim);

  EXPECT_THROW({ [[maybe_unused]] auto row = t[3]; }, std::runtime_error);
  EXPECT_THROW({ [[maybe_unused]] auto row = t[4]; }, std::runtime_error);
}
TEST(table, invalid_column_access)
{
  std::size_t dim = 3;
  cuke::table t(make_matrix(dim), dim);

  EXPECT_THROW({ [[maybe_unused]] auto v = t[2][3]; }, std::runtime_error);
  EXPECT_THROW(
      { [[maybe_unused]] const auto& v = t[1][4]; }, std::runtime_error);
}
TEST(table, raw_iterator)
{
  std::size_t dim = 3;
  cuke::table t(make_matrix(dim), dim);

  std::size_t expected = 0;
  for (const cuke::table::row& data : t.raw())
  {
    EXPECT_EQ(data[0].as<int>(), expected++);
    EXPECT_EQ(data[1].as<int>(), expected++);
    EXPECT_EQ(data[2].as<int>(), expected++);
  }
}

TEST(table, hashes)
{
  cuke::value_array values;
  values.push_back(cuke::value(std::string("NAME")));
  values.push_back(cuke::value(std::string("CITY")));
  values.push_back(cuke::value(std::string("AGE")));

  values.push_back(cuke::value(std::string("Thomas")));
  values.push_back(cuke::value(std::string("Augsburg")));
  values.push_back(cuke::value("34"));

  values.push_back(cuke::value(std::string("Theodor")));
  values.push_back(cuke::value(std::string("Wonderland")));
  values.push_back(cuke::value("999"));

  cuke::table t(values, 3);

  std::size_t i = 1;
  for (const cuke::table::row& data : t.hashes())
  {
    EXPECT_EQ(data["NAME"].to_string(), values[i * 3 + 0].to_string());
    EXPECT_EQ(data["CITY"].to_string(), values[i * 3 + 1].to_string());
    EXPECT_EQ(data["AGE"].to_string(), values[i * 3 + 2].to_string());
    ++i;
  }
  EXPECT_EQ(i, 3);
}
TEST(table, hashes_access_row_failing)
{
  cuke::value_array values;
  values.push_back(cuke::value(std::string("NAME")));
  values.push_back(cuke::value(std::string("CITY")));
  values.push_back(cuke::value(std::string("AGE")));

  values.push_back(cuke::value(std::string("Thomas")));
  values.push_back(cuke::value(std::string("Augsburg")));
  values.push_back(cuke::value("34"));

  values.push_back(cuke::value(std::string("Theodor")));
  values.push_back(cuke::value(std::string("Wonderland")));
  values.push_back(cuke::value("999"));

  values.push_back(cuke::value(std::string("Alf")));
  values.push_back(cuke::value(std::string("Melmac")));
  values.push_back(cuke::value("12"));
  cuke::table t(values, 3);
  EXPECT_THROW(
      { [[maybe_unused]] const cuke::table::row& data = t.hash_row(9); },
      std::runtime_error);
}
TEST(table, hashes_access_row)
{
  cuke::value_array values;
  values.push_back(cuke::value(std::string("NAME")));
  values.push_back(cuke::value(std::string("CITY")));
  values.push_back(cuke::value(std::string("AGE")));

  values.push_back(cuke::value(std::string("Thomas")));
  values.push_back(cuke::value(std::string("Augsburg")));
  values.push_back(cuke::value("34"));

  values.push_back(cuke::value(std::string("Theodor")));
  values.push_back(cuke::value(std::string("Wonderland")));
  values.push_back(cuke::value("999"));

  values.push_back(cuke::value(std::string("Alf")));
  values.push_back(cuke::value(std::string("Melmac")));
  values.push_back(cuke::value("12"));
  cuke::table t(values, 3);

  {
    const cuke::table::row& data = t.hash_row(2);
    EXPECT_EQ(data["NAME"].to_string(), std::string("Theodor"));
    EXPECT_EQ(data["CITY"].to_string(), std::string("Wonderland"));
    EXPECT_EQ(data["AGE"].as<int>(), 999);
  }
  {
    const cuke::table::row& data = t.hash_row(3);
    EXPECT_EQ(data["NAME"].to_string(), std::string("Alf"));
    EXPECT_EQ(data["CITY"].to_string(), std::string("Melmac"));
    EXPECT_EQ(data["AGE"].as<int>(), 12);
  }
}

TEST(table, rows_hash)
{
  cuke::value_array values;
  values.push_back(cuke::value(std::string("NAME")));
  values.push_back(cuke::value(std::string("Thomas")));
  values.push_back(cuke::value(std::string("CITY")));
  values.push_back(cuke::value(std::string("Augsburg")));
  values.push_back(cuke::value(std::string("AGE")));
  values.push_back(cuke::value("34"));

  cuke::table t(values, 2);
  std::unordered_map<std::string, cuke::value> data = t.rows_hash();

  EXPECT_EQ(data["NAME"].to_string(), std::string("Thomas"));
  EXPECT_EQ(data["CITY"].to_string(), std::string("Augsburg"));
  EXPECT_EQ(data["AGE"].as<int>(), 34);
}
TEST(table, rows_hash_more_than_2_cols)
{
  cuke::value_array values;
  values.push_back(cuke::value(std::string("NAME")));
  values.push_back(cuke::value(std::string("CITY")));
  values.push_back(cuke::value(std::string("AGE")));

  values.push_back(cuke::value(std::string("Thomas")));
  values.push_back(cuke::value(std::string("Augsburg")));
  values.push_back(cuke::value("34"));

  values.push_back(cuke::value(std::string("Theodor")));
  values.push_back(cuke::value(std::string("Wonderland")));
  values.push_back(cuke::value("999"));

  cuke::table t(values, 3);
  EXPECT_THROW(
      { [[maybe_unused]] auto data = t.rows_hash(); }, std::runtime_error);
}
TEST(table, initialize_w_single_row)
{
  cuke::value_array row{cuke::value("1"), cuke::value("2"), cuke::value("3")};
  cuke::table t(std::move(row));

  ASSERT_TRUE(row.empty());
  ASSERT_EQ(t.cells_count(), 3);
  ASSERT_EQ(t.row_count(), 1);

  EXPECT_EQ(t[0][0].as<int>(), 1);
  EXPECT_EQ(t[0][1].as<int>(), 2);
  EXPECT_EQ(t[0][2].as<int>(), 3);
}
TEST(table, append_row)
{
  cuke::value_array row{cuke::value("1"), cuke::value("2"), cuke::value("3")};
  cuke::table t(std::move(row));

  cuke::value_array append_me{cuke::value("100"), cuke::value("101"),
                              cuke::value("102")};
  ASSERT_TRUE(t.append_row(std::move(append_me)));

  ASSERT_TRUE(append_me.empty());
  ASSERT_EQ(t.cells_count(), 6);
  ASSERT_EQ(t.row_count(), 2);

  EXPECT_EQ(t[1][0].as<int>(), 100);
  EXPECT_EQ(t[1][1].as<int>(), 101);
  EXPECT_EQ(t[1][2].as<int>(), 102);
}
TEST(table, append_row_failing_case)
{
  cuke::value_array row{cuke::value("1"), cuke::value("2")};
  cuke::table t(std::move(row));

  cuke::value_array append_me{cuke::value("100"), cuke::value("101"),
                              cuke::value("102")};
  ASSERT_FALSE(t.append_row(std::move(append_me)));

  ASSERT_FALSE(append_me.empty());
  ASSERT_EQ(t.cells_count(), 2);
  ASSERT_EQ(t.row_count(), 1);
}
TEST(table, to_string_1)
{
  cuke::table t(cuke::value_array{cuke::value(std::string{"hello world"}),
                                  cuke::value("2")});
  [[maybe_unused]] bool b = t.append_row(cuke::value_array{
      cuke::value(std::string{"hello world hello world"}), cuke::value("2")});

  auto lines = t.to_string_array();
  EXPECT_EQ(lines[0], std::string("| hello world             | 2 |"));
  EXPECT_EQ(lines[1], std::string("| hello world hello world | 2 |"));
}
TEST(table, to_string_2)
{
  cuke::table t(cuke::value_array{
      cuke::value(std::string{"hello world hello world"}), cuke::value("2")});
  [[maybe_unused]] bool b = t.append_row(cuke::value_array{
      cuke::value(std::string{"hello world"}), cuke::value("2")});

  auto lines = t.to_string_array();
  EXPECT_EQ(lines[0], std::string("| hello world hello world | 2 |"));
  EXPECT_EQ(lines[1], std::string("| hello world             | 2 |"));
}
TEST(table, to_string_3)
{
  cuke::table t(cuke::value_array{
      cuke::value(std::string{"hello world hello world"}), cuke::value("2")});
  [[maybe_unused]] bool b = t.append_row(cuke::value_array{
      cuke::value(std::string{"hello world"}), cuke::value("1234")});

  auto lines = t.to_string_array();
  EXPECT_EQ(lines[0], std::string("| hello world hello world | 2    |"));
  EXPECT_EQ(lines[1], std::string("| hello world             | 1234 |"));
}
