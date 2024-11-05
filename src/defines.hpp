#pragma once

#include "value.hpp"
#include "hooks.hpp"
#include "registry.hpp"

#define _CONCAT_(a, b) a##b
#define CONCAT(a, b) _CONCAT_(a, b)

#define _STEP(function_name, step_definition, type)                           \
  void function_name(                                                         \
      [[maybe_unused]] const ::cuke::value_array& __cuke__values__,           \
      [[maybe_unused]] const std::vector<std::string>& __cuke__doc__string__, \
      [[maybe_unused]] const ::cuke::table& __cuke__table__);                 \
  namespace                                                                   \
  {                                                                           \
  struct CONCAT(function_name, _t)                                            \
  {                                                                           \
    CONCAT(function_name, _t)()                                               \
    {                                                                         \
      ::cuke::registry().push_step(::cuke::internal::step(                    \
          function_name, step_definition, type, #function_name));             \
    }                                                                         \
  } CONCAT(g_, function_name);                                                \
  }                                                                           \
  void function_name(                                                         \
      [[maybe_unused]] const ::cuke::value_array& __cuke__values__,           \
      [[maybe_unused]] const std::vector<std::string>& __cuke__doc__string__, \
      [[maybe_unused]] const ::cuke::table& __cuke__table__)

/**
 * @def STEP(function_name, step_definition)
 * @brief Creates a Cucumber step, which is then available in your feature files
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 * @param step_definition The step definition string which is used by feature
 * files later
 */
#define STEP(function_name, step_definition) \
  _STEP(function_name, step_definition, cuke::internal::step::type::step)

/**
 * @def GIVEN(function_name, step_definition)
 * @brief An alias to STEP(name,step) to increase readability of your code
 */
#define GIVEN(function_name, step_definition) \
  _STEP(function_name, step_definition, cuke::internal::step::type::given)

/**
 * @def WHEN(function_name, step_definition)
 * @brief An alias to STEP(function_name, step_definition) to increase
 * readability of your code
 */
#define WHEN(function_name, step_definition) \
  _STEP(function_name, step_definition, cuke::internal::step::type::when)

/**
 * @def THEN(function_name, step_definition)
 * @brief An alias to STEP(function_name, step_definition) to increase
 * readability of your code
 */
#define THEN(function_name, step_definition) \
  _STEP(function_name, step_definition, cuke::internal::step::type::then)

#define _BEFORE(function_name, tag_expression)                    \
  void function_name();                                           \
  namespace                                                       \
  {                                                               \
  struct CONCAT(function_name, _t)                                \
  {                                                               \
    CONCAT(function_name, _t)()                                   \
    {                                                             \
      ::cuke::registry().push_hook_before(                        \
          ::cuke::internal::hook(function_name, tag_expression)); \
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

#define _AFTER(function_name, tag_expression)                     \
  void function_name();                                           \
  namespace                                                       \
  {                                                               \
  struct CONCAT(function_name, _t)                                \
  {                                                               \
    CONCAT(function_name, _t)()                                   \
    {                                                             \
      ::cuke::registry().push_hook_after(                         \
          ::cuke::internal::hook(function_name, tag_expression)); \
    }                                                             \
                                                                  \
  } CONCAT(g_, function_name);                                    \
  }                                                               \
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

#define _BEFORE_STEP(function_name)               \
  void function_name();                           \
  namespace                                       \
  {                                               \
  struct CONCAT(function_name, _t)                \
  {                                               \
    CONCAT(function_name, _t)()                   \
    {                                             \
      ::cuke::registry().push_hook_before_step(   \
          ::cuke::internal::hook(function_name)); \
    }                                             \
                                                  \
  } CONCAT(g_, function_name);                    \
  }                                               \
  void function_name()

/**
 * @def BEFORE_STEP(function_name)
 * @brief Creates a hook which is executed before every step
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define BEFORE_STEP(function_name) _BEFORE_STEP(function_name)

#define _AFTER_STEP(function_name)                \
  void function_name();                           \
  namespace                                       \
  {                                               \
  struct CONCAT(function_name, _t)                \
  {                                               \
    CONCAT(function_name, _t)()                   \
    {                                             \
      ::cuke::registry().push_hook_after_step(    \
          ::cuke::internal::hook(function_name)); \
    }                                             \
                                                  \
  } CONCAT(g_, function_name);                    \
  }                                               \
  void function_name()

/**
 * @def AFTER_STEP(function_name)
 * @brief Creates a hook which is executed after every step
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define AFTER_STEP(function_name) _AFTER_STEP(function_name)

#define _BEFORE_ALL(function_name)                \
  void function_name();                           \
  namespace                                       \
  {                                               \
  struct CONCAT(function_name, _t)                \
  {                                               \
    CONCAT(function_name, _t)()                   \
    {                                             \
      ::cuke::registry().push_hook_before_all(    \
          ::cuke::internal::hook(function_name)); \
    }                                             \
                                                  \
  } CONCAT(g_, function_name);                    \
  }                                               \
  void function_name()

/**
 * @def BEFORE_ALL(function_name)
 * @brief Creates a hook which is executed before any scenario/feature
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define BEFORE_ALL(function_name) _BEFORE_ALL(function_name)

#define _AFTER_ALL(function_name)                 \
  void function_name();                           \
  namespace                                       \
  {                                               \
  struct CONCAT(function_name, _t)                \
  {                                               \
    CONCAT(function_name, _t)()                   \
    {                                             \
      ::cuke::registry().push_hook_after_all(     \
          ::cuke::internal::hook(function_name)); \
    }                                             \
                                                  \
  } CONCAT(g_, function_name);                    \
  }                                               \
  void function_name()

/**
 * @def AFTER_ALL(function_name)
 * @brief Creates a hook which is executed after the test run
 *
 * @param function_name A unique function name, this function name has no
 * technical impact
 */
#define AFTER_ALL(function_name) _AFTER_ALL(function_name)
