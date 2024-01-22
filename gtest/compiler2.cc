#include <gtest/gtest.h>

#include "../src/cwt/compiler/cuke_compiler.hpp"

using namespace cwt::details;

TEST(compiler2, init_object) 
{ 
  cuke_compiler c("Feature: asdf", "some crazy file");
  c.compile();
}

