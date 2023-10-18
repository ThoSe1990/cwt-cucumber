#pragma once 

#include <string_view>
#include <iostream>
#include <vector>

extern "C" {
  #include "cwt/cucumber.h"
}

#include "cucumber_context.hpp"
#include "cucumber_asserts.hpp"

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

#define STEP(name, step) INTERNAL_STEP(step, CONCAT(__cuke_step_,name))

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


#define BEFORE(name) INTERNAL_HOOK("before", CONCAT(__cuke_before,name),"")
#define BEFORE_T(name,tag_expression) INTERNAL_HOOK("before", CONCAT(__cuke_before,name),tag_expression)
#define AFTER(name) INTERNAL_HOOK("after", CONCAT(__cuke_after,name),"")
#define AFTER_T(name, tag_expression) INTERNAL_HOOK("after", CONCAT(__cuke_after,name),tag_expression)
#define BEFORE_STEP(name) INTERNAL_HOOK("before_step", CONCAT(__cuke_before_step,name),"")
#define AFTER_STEP(name) INTERNAL_HOOK("after_step", CONCAT(__cuke_after_step,name),"")


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

  static void print_error() {}

  template <typename T, typename... Args>
  static void print_error(const T& t, const Args&... args) 
  {
      std::cerr << "\x1B[31m" << t << "\x1B[0m";
      print_error(args...);
  }

  struct cuke_conversion 
  {
    cuke_value* arg;
    const std::string& file;
    int line;

    template<typename T>
    operator T() const{
        static_assert(has_cuke_conversion_v<T>, "conversion to T not supported");
        return cuke_conversion_impl<T>::get_arg(arg, file, line);
    }
  };
  
  inline cuke_conversion get_arg(cuke_value* args, int i, int arg_count, const std::string& file, int line) 
  {  
    if (i <= arg_count)
    {
      return cuke_conversion{&args[i-1], file, line}; 
    }
    print_error(file, ':', line, ": Can not acces index ", i, ". Argument count is: ", arg_count);
    throw std::out_of_range("");
  }

  template <>
  struct cuke_conversion_impl<long long> 
  {
      static long long get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return cuke_to_long(arg);
        }
        print_error(file, ':', line, ": Value is not an integer type.");
        throw std::bad_cast();
      }
  };

  template <>
  struct cuke_conversion_impl<std::size_t> 
  {
      static std::size_t get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return static_cast<std::size_t>(cuke_to_long(arg));
        }
        print_error(file, ':', line, ": Value is not an integer type.");
        throw std::bad_cast();
      }
  };

  template <>
  struct cuke_conversion_impl<int> 
  {
      static int get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return static_cast<int>(cuke_to_int(arg));
        }
        print_error(file, ':', line, ": Value is not an integer type.");
        throw std::bad_cast();
      }
  };

  template <>
  struct cuke_conversion_impl<char> 
  {
      static char get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return cuke_to_byte(arg);
        }
        print_error(file, ':', line, ": Value is not an integer type.");
        throw std::bad_cast();
      }
  };
  
  template <>
  struct cuke_conversion_impl<short> 
  {
      static short get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return cuke_to_short(arg);
        }
        print_error(file, ':', line, ": Value is not an integer type.");
        throw std::bad_cast();
      }
  };

  template <>
  struct cuke_conversion_impl<std::string>
  {
      static std::string get_arg(cuke_value* arg, const std::string& file, int line)
      {
        if (arg->type == VAL_OBJ)
        {
          return cuke_to_string(arg);
        }
        print_error(file, ':', line, ": Value is not an integer type.");
        throw std::bad_cast();
      }
  };

  template <>
  struct cuke_conversion_impl<float>
  {
      static float get_arg(cuke_value* arg, const std::string& file, int line)
      {
        if (arg->type == VAL_DOUBLE)
        {
          return cuke_to_float(arg);
        }
        print_error(file, ':', line, ": Value is not a floating point type.");
        throw std::bad_cast();
      }
  };

  template <>
  struct cuke_conversion_impl<double>
  {
      static double get_arg(cuke_value* arg, const std::string& file, int line)
      {
        if (arg->type == VAL_DOUBLE)
        {
          return cuke_to_double(arg);
        }
        print_error(file, ':', line, ": Value is not a floating point type.");
        throw std::bad_cast();
      }
  };
} // namespace cuke::details


#define CUKE_ARG(i) cuke::details::get_arg(args, i, arg_count, __FILE__, __LINE__)  


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
