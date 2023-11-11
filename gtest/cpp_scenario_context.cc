#include <gtest/gtest.h>
#include <cwt/cucumber_context.hpp>


#include <iostream>
struct foo
{
    foo(){ std::cout << "ctor\n"; }
    ~foo(){ std::cout << "dtor\n"; }
};

TEST(cpp_scenario_context, init)
{
    cuke::scenario_context sc;   
    sc.get<foo>();
}