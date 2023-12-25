#include <gtest/gtest.h>
#include <memory>

#include "cwt/cucumber.hpp"
#include "paths.hpp"

STEP(this_passes, "this passes")
{
  cuke::is_true(true);
}
STEP(this_fails, "this fails")
{
  cuke::is_true(false);
}

STEP(greater_comparision, "{int} is greater than {int}")
{
  const unsigned int lhs = CUKE_ARG(1);
  const unsigned int rhs = CUKE_ARG(2);
  cuke::greater(lhs, rhs);
}

class cpp_bindings : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_cuke = std::make_unique<cuke::details::runner>();
    m_cuke->init();
  }

  void TearDown() override 
  {
    m_cuke.reset();
  }

  int run_tests(int argc, const char* argv[])
  {
    return m_cuke->run(argc, argv);
  }

private:
    std::unique_ptr<cuke::details::runner> m_cuke; 
};


TEST_F(cpp_bindings, run_tests)
{
  std::vector<const char*> args = {"some program", paths::simple_scenarios.data()};
  EXPECT_EQ(CUKE_FAILED, run_tests(args.size(), args.data()));
}
TEST_F(cpp_bindings, run_a_million_tests)
{
  std::vector<const char*> args = {"some program", paths::a_million_steps.data()};
  EXPECT_EQ(CUKE_SUCCESS, run_tests(args.size(), args.data()));
}