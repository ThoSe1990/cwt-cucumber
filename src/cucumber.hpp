#pragma once

#include "table.hpp"     // NOLINT
#include "defines.hpp"   // NOLINT
#include "asserts.hpp"   // NOLINT
#include "context.hpp"   // NOLINT
#include "get_args.hpp"  // NOLINT

#include "options.hpp"
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
  [[nodiscard]] results::test_status final_result() const noexcept;

  const options& get_options() const noexcept;
  bool print_help() const noexcept;
  bool export_catalog(std::size_t json_indents = 2) const noexcept;
};

}  // namespace cuke
