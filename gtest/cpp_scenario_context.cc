#include <gtest/gtest.h>
#include <cwt/cucumber_context.hpp>


#include <iostream>
struct foo
{
    foo(){ std::cout << "foo ctor\n"; }
    ~foo(){ std::cout << "foo dtor\n"; }
};

struct bar
{
    bar() = default;
    bar(int value) : m_value(value) { std::cout << "bar ctor\n"; }
    ~bar(){ std::cout << "bar dtor\n"; }
    int m_value;
};

TEST(cpp_scenario_context, init)
{
    cuke::scenario_context sc;   
    sc.get<foo>();
    sc.get<bar>(6);
    sc.get<std::string>("some value");

    std::cout << sc.get<bar>().m_value << std::endl;
    std::cout << sc.get<std::string>() << std::endl;

    std::cout << sc.size() << std::endl;
    sc.clear();

    std::cout << sc.size() << std::endl;
}