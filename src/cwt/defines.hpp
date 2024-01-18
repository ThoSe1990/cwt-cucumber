#pragma once

#include "vm.hpp"
#include "value.hpp"

#define _CONCAT_(a, b) a##b
#define CONCAT(a, b) _CONCAT_(a, b)

// TODO check if args change to iterators
#define _STEP(function_name, step_definition)    \
  void function_name(const value_array& args);   \
  namespace                                      \
  {                                              \
  struct CONCAT(function_name, _t)               \
  {                                              \
    CONCAT(function_name, _t)()                  \
    {                                            \
      ::cwt::details::vm::push_step(     \
          step(function_name, step_definition)); \
    }                                            \
  } CONCAT(g_, function_name);                   \
  }                                              \
  void function_name(const value_array& args)

/**
 * @def STEP(name, step)
 * @brief Creates a Cucumber step, which is then available in your feature files
 *
 * @param name A unique function name, this function name has no technical
 * impact
 * @param step The step definition string which is used by feature files later
 */
#define STEP(function_name, step_definition) \
  _STEP(function_name, step_definition)
