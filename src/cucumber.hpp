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
/**
 * @brief Entry point to parse command-line arguments and execute all tests.
 *
 * @details This function acts as a convenient wrapper around the \c
 * cwt_cucumber class. It constructs the object, parses the arguments, executes
 * the tests, and returns the final test status.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return The final test status after executing all scenarios.
 */
[[nodiscard]] cuke::results::test_status entry_point(int argc,
                                                     const char* argv[]);

/**
 * @class cwt_cucumber
 * @brief Core class for running CWT-Cucumber tests.
 *
 * @details This class handles initialization with program arguments, parsing
 * feature files, executing all scenarios, evaluating results, and printing
 * output to the console.
 */
class cwt_cucumber
{
 public:
  /**
   * @brief Constructs the Cucumber runner with command-line arguments.
   * @param argc Number of command-line arguments.
   * @param argv Array of command-line argument strings.
   */
  cwt_cucumber(int argc, const char* argv[]);

  /**
   * @brief Executes all parsed tests/scenarios.
   *
   * @note Results are stored internally and can be retrieved using \c
   * final_result().
   */
  void run_tests() const noexcept;

  /**
   * @brief Prints a summary of test results to stdout.
   *
   * @details Includes passed/failed scenarios, skipped steps, and optionally
   * verbose output.
   */
  void print_results() const noexcept;

  /**
   * @brief Returns the final test status after executing all tests.
   * @return \c results::test_status indicating overall success or failure.
   */
  [[nodiscard]] results::test_status final_result() const noexcept;

  /**
   * @brief Returns the parsed command-line and environment options.
   * @return Reference to the internal \c options object.
   */
  const options& get_options() const noexcept;

  /**
   * @brief Prints the help message to stdout.
   * @return \c true if help was printed, otherwise \c false.
   */
  bool print_help() const noexcept;

  /**
   * @brief Exports a catalog of implemented steps.
   *
   * @param json_indents Number of spaces to use for JSON indentation (default
   * is 2).
   * @return \c true if export succeeded, otherwise \c false.
   */
  bool export_catalog(std::size_t json_indents = 2) const noexcept;
};
}  // namespace cuke
