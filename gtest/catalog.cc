#include <gtest/gtest.h>

#include "../src/registry.hpp"

// class catalog : public ::testing::Test
// {
//  protected:
//   void TearDown() override {}
//   void SetUp() override
//   {
//     testing::internal::CaptureStdout();
//
//     cuke::registry().clear();
//
//     cuke::registry().push_step(cuke::internal::step(
//         [](const cuke::value_array&, const auto&, const auto&) {}, "a
//         step"));
//
//     cuke::registry().push_step(cuke::internal::step(
//         [](const cuke::value_array& values, const auto&, const auto&) {},
//         "a step with {int} and {string}"));
//   }
// };
//
// TEST_F(catalog, scenario_pass) {}
