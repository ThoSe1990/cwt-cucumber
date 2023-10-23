#include <gtest/gtest.h>

#include "cwt/cucumber.hpp"

class cpp_scenario_context : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    m_cuke = std::make_unique<cuke::runner>();
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
    std::unique_ptr<cuke::runner> m_cuke; 
};


TEST_F(cpp_scenario_context, init)
{

}