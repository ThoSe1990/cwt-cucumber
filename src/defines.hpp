#pragma once

#include "any.hpp"         // NOLINT
#include "value.hpp"       // NOLINT
#include "hooks.hpp"       // NOLINT
#include "registry.hpp"    // NOLINT
#include "expression.hpp"  // NOLINT

#define _CONCAT_(a, b) a##b
#define CONCAT(a, b) _CONCAT_(a, b)

#define _STEP(function_name, definition, type)                                \
  static void function_name(                                                  \
      [[maybe_unused]] const ::cuke::value_array& __cuke__values__,           \
      [[maybe_unused]] const std::vector<::cuke::internal::param_info>&       \
          __cuke__parameter_info__,                                           \
      [[maybe_unused]] const std::vector<std::string>& __cuke__doc__string__, \
      [[maybe_unused]] const ::cuke::table& __cuke__table__);                 \
  namespace                                                                   \
  {                                                                           \
  struct CONCAT(function_name, _t)                                            \
  {                                                                           \
    CONCAT(function_name, _t)()                                               \
    {                                                                         \
      ::cuke::registry().push_step(::cuke::internal::step_definition(         \
          function_name, definition, type, #function_name, __FILE__,          \
          __LINE__));                                                         \
    }                                                                         \
  } CONCAT(g_, function_name);                                                \
  }                                                                           \
  static void function_name(                                                  \
      [[maybe_unused]] const ::cuke::value_array& __cuke__values__,           \
      [[maybe_unused]] const std::vector<::cuke::internal::param_info>&       \
          __cuke__parameter_info__,                                           \
      [[maybe_unused]] const std::vector<std::string>& __cuke__doc__string__, \
      [[maybe_unused]] const ::cuke::table& __cuke__table__)

#define _CUSTOM_PARAMETER_IMPL(function_name, key, pattern, description)   \
  static ::cuke::internal::any function_name(                              \
      [[maybe_unused]] cuke::value_array::const_iterator __cuke__values__, \
      [[maybe_unused]] std::size_t __cuke__values__count__);               \
  namespace                                                                \
  {                                                                        \
  struct CONCAT(function_name, _t)                                         \
  {                                                                        \
    CONCAT(function_name, _t)()                                            \
    {                                                                      \
      ::cuke::registry().push_expression(                                  \
          key, ::cuke::expression{pattern, description, function_name});   \
    }                                                                      \
  } CONCAT(g_, function_name);                                             \
  }                                                                        \
  static ::cuke::internal::any function_name(                              \
      [[maybe_unused]] cuke::value_array::const_iterator __cuke__values__, \
      [[maybe_unused]] std::size_t __cuke__values__count__)

/**
 * @def CUSTOM_PARAMETER(function_name, key, pattern, opt: description)
 * @brief Creates a custom expression type to use in steps.
 *
 * @param function_name A unique function name in order to find the callback.
 * The function name has no technical impact.
 * @param key The unique key for the expression. Provide this value with curly
 * braces, e.g. '{my custom parameter}'
 * @param pattern The regex pattern to match the step in the feature file with
 * the given expression.
 * @param description Optional description: This description is only printed as
 * is in the steps-catalog.
 */
#define CUSTOM_PARAMETER(function_name, key, pattern, description) \
  _CUSTOM_PARAMETER_IMPL(function_name, key, pattern, description)

/**
 * @def CUKE_PARAM_ARG(index)
 * @brief Access capture groups from a custom expression in its callback.
 * ``CUKE_PARAM_ARG`` starts at index 1 on the first capture group from the left
 * side.
 *
 * @param index Variable index to access
 * @return The value from the index in the given step
 */
#define CUKE_PARAM_ARG(index)                                                  \
  ::cuke::internal::get_param_value(__cuke__values__, __cuke__values__count__, \
                                    index)

/**                                                                           \
 * @def STEP(function_name, definition)                                  \
 * @brief Creates a Cucumber step, which is then available in your feature    \
 * files                                                                      \
 *                                                                            \
 * @param function_name A unique function name, this function name has no     \
 * technical impact                                                           \
 * @param definition The step definition string which is used by feature \
 * files later                                                                \
 */
#define STEP(function_name, definition) \
  _STEP(function_name, definition, cuke::internal::step_definition::type::step)

/**
 * @def GIVEN(function_name, definition)
 * @brief An alias to STEP(name,step) to increase readability of your code
 */
#define GIVEN(function_name, definition) \
  _STEP(function_name, definition, cuke::internal::step_definition::type::given)

/**
 * @def WHEN(function_name, definition)
 * @brief An alias to STEP(function_name, definition) to increase
 * readability of your code
 */
#define WHEN(function_name, definition) \
  _STEP(function_name, definition, cuke::internal::step_definition::type::when)

/**
 * @def THEN(function_name, definition)
 * @brief An alias to STEP(function_name, definition) to increase
 * readability of your code
 */
#define THEN(function_name, definition) \
  _STEP(function_name, definition, cuke::internal::step_definition::type::then)

#define _BEFORE(function_name, tag_expression)                    \
  void function_name();                                           \
  namespace                                                       \
  {                                                               \
  struct CONCAT(function_name, _t)                                \
  {                                                               \
    CONCAT(function_name, _t)()                                   \
    {                                                             \
      ::cuke::registry().push_hook_before(::cuke::internal::hook( \
          #function_name, function_name, tag_expression));        \
    }                                                             \
                                                                  \
  } CONCAT(g_, function_name);                                    \
  }                                                               \
  void function_name()

/**
 * @def BEFORE(function_name)
 * @brief Creates a hook which is executed before every Scenario
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define BEFORE(function_name) _BEFORE(function_name, "")
/**
 * @def BEFORE_T(function_name, tag_expression)
 * @brief Creates a hook with a tag expression, which can run before a tagged
 * scenario
 *
 * Before running a tagged scenario, the tag expression is evaluated. If it
 * evaluates to true, the hook is executed.
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 * @param tag_expression A tag expression (bool condition), like ``"@tag1 and
 * @tag2"``
 */
#define BEFORE_T(function_name, tag_expression) \
  _BEFORE(function_name, tag_expression)

#define _AFTER(function_name, tag_expression)                    \
  void function_name();                                          \
  namespace                                                      \
  {                                                              \
  struct CONCAT(function_name, _t)                               \
  {                                                              \
    CONCAT(function_name, _t)()                                  \
    {                                                            \
      ::cuke::registry().push_hook_after(::cuke::internal::hook( \
          #function_name, function_name, tag_expression));       \
    }                                                            \
                                                                 \
  } CONCAT(g_, function_name);                                   \
  }                                                              \
  void function_name()

/**
 * @def AFTER(function_name)
 * @brief Creates a hook which is executed after every scenario
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define AFTER(function_name) _AFTER(function_name, "")
/**
 * @def AFTER_T(function_name, tag_expression)
 * @brief Creates a hook with a tag expression, which can run after a tagged
 * scenario
 *
 * After running a tagged scenario, the tag expression is evaluated. If it
 * evaluates to true, the hook is executed.
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 * @param tag_expression A tag expression (bool condition), e.g. ``"@tag1 and
 * @tag2"``
 */
#define AFTER_T(function_name, tag_expression) \
  _AFTER(function_name, tag_expression)

#define _BEFORE_STEP(function_name)                               \
  void function_name();                                           \
  namespace                                                       \
  {                                                               \
  struct CONCAT(function_name, _t)                                \
  {                                                               \
    CONCAT(function_name, _t)()                                   \
    {                                                             \
      ::cuke::registry().push_hook_before_step(                   \
          ::cuke::internal::hook(#function_name, function_name)); \
    }                                                             \
                                                                  \
  } CONCAT(g_, function_name);                                    \
  }                                                               \
  void function_name()

/**
 * @def BEFORE_STEP(function_name)
 * @brief Creates a hook which is executed before every step
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define BEFORE_STEP(function_name) _BEFORE_STEP(function_name)

#define _AFTER_STEP(function_name)                                \
  void function_name();                                           \
  namespace                                                       \
  {                                                               \
  struct CONCAT(function_name, _t)                                \
  {                                                               \
    CONCAT(function_name, _t)()                                   \
    {                                                             \
      ::cuke::registry().push_hook_after_step(                    \
          ::cuke::internal::hook(#function_name, function_name)); \
    }                                                             \
                                                                  \
  } CONCAT(g_, function_name);                                    \
  }                                                               \
  void function_name()

/**
 * @def AFTER_STEP(function_name)
 * @brief Creates a hook which is executed after every step
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define AFTER_STEP(function_name) _AFTER_STEP(function_name)

#define _BEFORE_ALL(function_name)                                \
  void function_name();                                           \
  namespace                                                       \
  {                                                               \
  struct CONCAT(function_name, _t)                                \
  {                                                               \
    CONCAT(function_name, _t)()                                   \
    {                                                             \
      ::cuke::registry().push_hook_before_all(                    \
          ::cuke::internal::hook(#function_name, function_name)); \
    }                                                             \
                                                                  \
  } CONCAT(g_, function_name);                                    \
  }                                                               \
  void function_name()

/**
 * @def BEFORE_ALL(function_name)
 * @brief Creates a hook which is executed before any scenario/feature
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define BEFORE_ALL(function_name) _BEFORE_ALL(function_name)

#define _AFTER_ALL(function_name)                                 \
  void function_name();                                           \
  namespace                                                       \
  {                                                               \
  struct CONCAT(function_name, _t)                                \
  {                                                               \
    CONCAT(function_name, _t)()                                   \
    {                                                             \
      ::cuke::registry().push_hook_after_all(                     \
          ::cuke::internal::hook(#function_name, function_name)); \
    }                                                             \
                                                                  \
  } CONCAT(g_, function_name);                                    \
  }                                                               \
  void function_name()

/**
 * @def AFTER_ALL(function_name)
 * @brief Creates a hook which is executed after the test run
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define AFTER_ALL(function_name) _AFTER_ALL(function_name)
