// Minimal, dependency-free benchmark for step matching.
//
// Measures how long it takes to find a matching step_definition for a
// batch of feature-step strings against a realistic set of registered
// steps, using the same step_finder::find() path the interpreter uses
// at runtime. Run it before/after a change to step_finder/step to see
// its effect on matching throughput.
//
// Usage: ./build/bin/step-matching-benchmark [iterations]

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "step.hpp"
#include "step_finder.hpp"
#include "util.hpp"

namespace
{

void noop_callback(const cuke::value_array&,
                   const std::vector<cuke::internal::param_info>&,
                   const cuke::internal::doc_string&, const cuke::table&)
{
}

std::vector<cuke::internal::step_definition> make_step_definitions()
{
  std::vector<cuke::internal::step_definition> steps;
  steps.reserve(50);

  steps.emplace_back(noop_callback, "I have {int} cukes in my belly");
  steps.emplace_back(noop_callback, "I eat {int} cukes");
  steps.emplace_back(noop_callback, "I place {int} x {string} in it");
  steps.emplace_back(noop_callback, "the stock count should be {int}");
  steps.emplace_back(noop_callback, "I see {} in the raw output");
  steps.emplace_back(noop_callback, "The value is {word}");
  steps.emplace_back(noop_callback, "It should equal {string}");
  steps.emplace_back(noop_callback, "the shipping label should equal {string}");
  steps.emplace_back(noop_callback,
                     "The box gets shipped at {int}-{int}-{int}");
  steps.emplace_back(noop_callback, "I have a doc string:");

  // Pad with additional, syntactically distinct steps so a realistic
  // step_finder::find() has to walk a non-trivial registry, similar to
  // a medium-sized real project.
  for (int i = 0; i < 40; ++i)
  {
    steps.emplace_back(
        noop_callback,
        std::format(
            "this is generated step number {} with {{int}} and {{string}}", i));
  }

  return steps;
}

std::vector<std::string> make_feature_steps()
{
  return {
      "I have 5 cukes in my belly",
      "I eat 3 cukes",
      "I place 2 x \"widget\" in it",
      "the stock count should be 42",
      "I see {status} in the raw output",
      "The value is foo/bar",
      "It should equal \"foo/bar\"",
      "the shipping label should equal \"Ship to: Tokyo, Japan\"",
      "The box gets shipped at 2026-08-27",
      "I have a doc string:",
      "this step definitely does not match anything",
  };
}

}  // namespace

int main(int argc, char** argv)
{
  std::size_t iterations = 100000;
  if (argc > 1)
  {
    try
    {
      const unsigned long parsed = std::stoul(argv[1]);
      if (parsed == 0)
      {
        throw std::invalid_argument("iterations must be positive");
      }
      iterations = parsed;
    }
    catch (const std::exception&)
    {
      std::cerr << "usage: " << argv[0]
                << " [iterations]  (must be a positive integer)\n";
      return 1;
    }
  }

  const std::vector<cuke::internal::step_definition> steps =
      make_step_definitions();
  const std::vector<std::string> feature_steps = make_feature_steps();

  std::size_t matched = 0;
  std::size_t unmatched = 0;

  const double seconds = cuke::internal::execute_and_count_time(
      [&]()
      {
        for (std::size_t i = 0; i < iterations; ++i)
        {
          const std::string& feature_step =
              feature_steps[i % feature_steps.size()];
          cuke::internal::step_finder finder(feature_step);
          auto it = finder.find(steps.begin(), steps.end());
          if (it != steps.end())
          {
            ++matched;
          }
          else
          {
            ++unmatched;
          }
        }
      });

  const double per_match_us =
      (seconds * 1'000'000.0) / static_cast<double>(iterations);

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "step definitions registered : " << steps.size() << '\n';
  std::cout << "feature step lookups        : " << iterations << '\n';
  std::cout << "matched                     : " << matched << '\n';
  std::cout << "unmatched                   : " << unmatched << '\n';
  std::cout << "total time                  : " << seconds << " s\n";
  std::cout << "avg time per lookup         : " << per_match_us << " us\n";

  return 0;
}
