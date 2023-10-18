#include <gtest/gtest.h>

extern "C" {
    #include "cwt/cucumber.h"   
}

class test_hooks : public ::testing::Test 
{
protected:
  void SetUp() override
  {
    open_cucumber();
  }

  void TearDown() override 
  {
    close_cucumber();
  }
};

TEST_F(test_hooks, init)
{
    
}