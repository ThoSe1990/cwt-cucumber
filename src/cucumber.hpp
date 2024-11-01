#pragma once

#include "defines.hpp"   // NOLINT
#include "asserts.hpp"   // NOLINT
#include "context.hpp"   // NOLINT
#include "get_args.hpp"  // NOLINT

#include "test_results.hpp"

namespace cuke
{

[[nodiscard]] cuke::results::test_status entry_point(int argc,
                                                     const char* argv[]);

class cwt_cucumber
{
 public:
  cwt_cucumber(int argc, const char* argv[]);
  void run_tests() const noexcept;
  void print_results() const noexcept;
  bool print_help() const noexcept;
  [[nodiscard]] results::test_status final_result() const noexcept;

 private:
  cuke_args m_args;
};

}  // namespace cuke
