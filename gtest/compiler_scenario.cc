#include <gtest/gtest.h>

#include "../src/cwt/compiler/feature.hpp"

using namespace cwt::details;


TEST(compiler_scenario, init)
{
  cuke_compiler cuke("Feature: Hello World");
  feature f(&cuke);
  f.compile();
  std::cout << f.get_chunk().size() << std::endl;
}