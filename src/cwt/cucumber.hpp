#pragma once 

#include <string_view>
#include <iostream>
#include <vector>

extern "C" {
  #include "cwt/cucumber.h"
}

#include "cucumber_context.hpp"

namespace cuke::details
{
  static std::vector<std::pair<const char*, cuke_step_t>> steps;
  struct hook
  {
    const char* name;
    const char* tag_expression;
    cuke_step_t function;
  };
  static std::vector<hook> hooks;
} // namespace cuke::details

#define CONCAT_INTERNAL(a, b) a ## b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

#define INTERNAL_STEP(step_definition, name) \
    void name(int arg_count, cuke_value* args); \
    namespace { \
        struct CONCAT(name, _t) { \
            CONCAT(name, _t)() { \
             cuke::details::steps.push_back({step_definition,name}); \
            } \
        } CONCAT(g_,name); \
    } \
    void name(int arg_count, cuke_value* args)

#define STEP(step) INTERNAL_STEP(step, CONCAT(__cuke_step_,__LINE__))
#define STEP_NAME(name, step) INTERNAL_STEP(step, name)


#define INTERNAL_HOOK(hook, name, tag_expression) \
    void name(int arg_count, cuke_value* args); \
    namespace { \
        struct CONCAT(name, _t) { \
            CONCAT(name, _t)() { \
             cuke::details::hooks.push_back({hook,tag_expression,name}); \
            } \
        } CONCAT(g_,name); \
    } \
    void name(int arg_count, cuke_value* args)

// #define BEFORE() INTERNAL_HOOK("before", __cuke_hook_before)
// TODO is __LINE__ so good here? 
#define BEFORE() INTERNAL_HOOK("before", CONCAT(__cuke_hook_before,__LINE__),"")
#define BEFORE_T(tag_expression) INTERNAL_HOOK("before", CONCAT(__cuke_hook_before,__LINE__),tag_expression)
#define AFTER() INTERNAL_HOOK("after", CONCAT(__cuke_hook_after,__LINE__),"")
#define AFTER_T(tag_expression) INTERNAL_HOOK("after", CONCAT(__cuke_hook_after,__LINE__),tag_expression)
#define BEFORE_STEP() INTERNAL_HOOK("before_step", CONCAT(__cuke_hook_before_step,__LINE__),"")
#define AFTER_STEP() INTERNAL_HOOK("after_step", CONCAT(__cuke_hook_after_step,__LINE__),"")


namespace cuke::details
{
  template<typename T>
  struct cuke_conversion_impl{};

  template<typename T>
  class has_cuke_conversion_function
  {
      using one = char;
      struct two
      {
          char x[2];
      };

      template<typename C>
      static one test(decltype(&C::get_arg));
      template<typename C>
      static two test(...);

  public:
      static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
  };

  template<typename T>
  static constexpr bool has_cuke_conversion_v = has_cuke_conversion_function<cuke_conversion_impl<T>>::value;

  struct cuke_conversion 
  {
    int n;
    cuke_value* arg;
    template<typename T>
    operator T() const{
        static_assert(has_cuke_conversion_v<T>, "conversion to T not supported");
        return cuke_conversion_impl<T>::get_arg(n, arg);
    }
  };
  
  inline cuke_conversion get_arg(int n, cuke_value* args) 
  { 
      return cuke_conversion{n, args}; 
  }

  template <>
  struct cuke_conversion_impl<long long> 
  {
      static long long get_arg(int n, cuke_value* arg) 
      {
        if (arg->type == VAL_LONG)
        {
          return cuke_to_long(arg);
        }
        // TODO error 
        return 0;
      }
  };

  template <>
  struct cuke_conversion_impl<std::size_t> 
  {
      static std::size_t get_arg(int n, cuke_value* arg) 
      {
          return static_cast<std::size_t>(cuke_to_long(arg));
      }
  };

  template <>
  struct cuke_conversion_impl<int> 
  {
      static int get_arg(int n, cuke_value* arg) 
      {
          return static_cast<int>(cuke_to_int(arg));
      }
  };

  template <>
  struct cuke_conversion_impl<char> 
  {
      static char get_arg(int n, cuke_value* arg) 
      {
        return cuke_to_byte(arg);
      }
  };
  
  template <>
  struct cuke_conversion_impl<short> 
  {
      static short get_arg(int n, cuke_value* arg) 
      {
        return cuke_to_short(arg);
      }
  };

  template <>
  struct cuke_conversion_impl<std::string>
  {
      static std::string get_arg(int n, cuke_value* arg)
      {
          return cuke_to_string(arg);
      }
  };

  template <>
  struct cuke_conversion_impl<float>
  {
      static float get_arg(int n, cuke_value* arg)
      {
        return cuke_to_float(arg);
      }
  };

  template <>
  struct cuke_conversion_impl<double>
  {
      static double get_arg(int n, cuke_value* arg)
      {
        return cuke_to_double(arg);
      }
  };
} // namespace cuke::details


#define CUKE_ARG(i) cuke::details::get_arg(arg_count, &args[i-1])  


namespace cuke
{
  class tests 
  {
    public:
      tests() 
      {
        open_cucumber();
        for (const auto& pair : details::steps) 
        {
          cuke_step(pair.first, pair.second);
        }
        for (const details::hook& h : details::hooks)
        {
          cuke_hook(h.name, h.function, h.tag_expression);
        }
        cuke_hook("reset_context", details::reset_scenario_context, "");
      }
      ~tests() 
      {
        close_cucumber();
      }

      int run(int argc, const char* argv[])
      {
        return run_cuke_from_argv(argc, argv);
      }
  };
} // namespace cuke
