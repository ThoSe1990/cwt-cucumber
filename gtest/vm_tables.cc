#include <gtest/gtest.h>

#include "../src/cucumber.hpp"

using namespace cwt::details;

class vm_tables : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    test_vm = vm();
    test_vm.push_step(step(
        [](argc n, argv values)
        {
          vm_tables::table = CUKE_TABLE();
          ++vm_tables::call_count;
        },
        "This is a datatable:"));
  }

  void TearDown() override
  {
    test_vm.reset();
    call_count = 0;
  }

  vm test_vm;
  static cuke::table table;
  static std::size_t call_count;
};
std::size_t vm_tables::call_count{0};
cuke::table vm_tables::table{};

TEST_F(vm_tables, raw_table)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  | 1 | 2 |
  | 3 | 4 |
)*";

  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 1);

  std::size_t expected = 1;
  for (const auto& row : table.raw())
  {
    EXPECT_EQ(row[0].as<int>(), expected++);
    EXPECT_EQ(row[1].as<int>(), expected++);
  }
}

TEST_F(vm_tables, raw_table_2_scenarios)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  | 1 | 2 |
  | 3 | 4 |


  Scenario: Second Scenario
  Given This is a datatable:
  | 1 | 2 |
  | 3 | 4 |
)*";

  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 2);

  std::size_t expected = 1;
  for (const auto& row : table.raw())
  {
    EXPECT_EQ(row[0].as<int>(), expected++);
    EXPECT_EQ(row[1].as<int>(), expected++);
  }
}

TEST_F(vm_tables, hash_table)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  | NAME    | EMAIL               | AGE |
  | Thomas  | thomas@example.com  | 33  |
  | Theodor | theodor@example.com | 99  |
)*";

  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 1);

  std::vector<std::tuple<std::string, std::string, int>> expected
  {
    std::make_tuple("Thomas", "thomas@example.com", 33),
    std::make_tuple("Theodor", "theodor@example.com", 99)
  };
  std::size_t i = 0;
  for (const auto& row : table.hashes())
  {
    EXPECT_EQ(row["NAME"].to_string(), std::get<0>(expected[i]));
    EXPECT_EQ(row["EMAIL"].to_string(), std::get<1>(expected[i]));
    EXPECT_EQ(row["AGE"].as<int>(), std::get<2>(expected[i++]));
  }
}

TEST_F(vm_tables, rows_hash)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  | NAME  | Thomas              |
  | EMAIL | thomas@example.com  |
  | CITY  | Augsburg            | 
  | AGE   | 33                  |
)*";


  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 1);

  auto hashtable = table.rows_hash();

  EXPECT_EQ(hashtable["NAME"].to_string(), std::string("Thomas"));
  EXPECT_EQ(hashtable["EMAIL"].to_string(), std::string("thomas@example.com"));
  EXPECT_EQ(hashtable["CITY"].to_string(), std::string("Augsburg"));
  EXPECT_EQ(hashtable["AGE"].as<int>(), 33);
}
TEST_F(vm_tables, rows_hash_with_linebreaks)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  
  | NAME  | Thomas              |

  | EMAIL | thomas@example.com  |


  | CITY  | Augsburg            | 

  | AGE   | 33                  |


)*";


  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 1);

  auto hashtable = table.rows_hash();

  EXPECT_EQ(hashtable["NAME"].to_string(), std::string("Thomas"));
  EXPECT_EQ(hashtable["EMAIL"].to_string(), std::string("thomas@example.com"));
  EXPECT_EQ(hashtable["CITY"].to_string(), std::string("Augsburg"));
  EXPECT_EQ(hashtable["AGE"].as<int>(), 33);
}


TEST_F(vm_tables, more_steps_1)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  | 1 | 2 |
  Given This is a datatable:
  | 3 | 4 |
)*";

  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 2);

  std::size_t expected = 3;
  for (const auto& row : table.raw())
  {
    EXPECT_EQ(row[0].as<int>(), expected++);
    EXPECT_EQ(row[1].as<int>(), expected++);
  }
  EXPECT_EQ(call_count, 2);
}
TEST_F(vm_tables, more_steps_2)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  | 1 | 2 |
  # some comment here ... 
  Given This is a datatable:
  | 3 | 4 |
)*";

  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 2);

  std::size_t expected = 3;
  for (const auto& row : table.raw())
  {
    EXPECT_EQ(row[0].as<int>(), expected++);
    EXPECT_EQ(row[1].as<int>(), expected++);
  }
  EXPECT_EQ(call_count, 2);
}
TEST_F(vm_tables, more_steps_3)
{
  const char* script = R"*(
  Feature: First Feature
  Scenario: First Scenario
  Given This is a datatable:
  | 1 | 2 |


  Given This is a datatable:
  | 3 | 4 |
)*";

  ASSERT_EQ(return_code::passed, test_vm.run(script));
  ASSERT_EQ(call_count, 2);

  std::size_t expected = 3;
  for (const auto& row : table.raw())
  {
    EXPECT_EQ(row[0].as<int>(), expected++);
    EXPECT_EQ(row[1].as<int>(), expected++);
  }
  EXPECT_EQ(call_count, 2);
}