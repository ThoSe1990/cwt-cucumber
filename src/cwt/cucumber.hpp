#pragma once 

#include <vector>
#include <iostream>
#include <filesystem>
#include <string_view>

extern "C" {
  #include "cucumber.h"
}

#include "cucumber_context.hpp"
#include "cucumber_asserts.hpp"
#include "cucumber_error.hpp"

namespace cuke::details
{
  namespace fs = std::filesystem;

  struct feature 
  {
    fs::path file;
    std::vector<int> lines;
  };
  
  class runner 
  {
    public:
      runner() = default;
      ~runner();
      void init();
      int run(int argc, const char* argv[]);

    private:
      void find_feature_in_dir(const fs::path& dir);
      void get_feature_files(int argc, const char* argv[]);
      int internal_run();

    private:
      std::vector<feature> m_features;      
  };

  struct hook
  {
    const char* name;
    const char* tag_expression;
    cuke_step_t function;
  };

  std::vector<std::pair<const char*, cuke_step_t>>& steps();
  std::vector<hook>& hooks();
  runner& get_runner();
  void init();
  int run(int argc, const char* argv[]);
} // namespace cuke::details


#define CONCAT_INTERNAL(a, b) a ## b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

#define INTERNAL_STEP(step_definition, name) \
    void name(int arg_count, cuke_value* args); \
    namespace { \
        struct CONCAT(name, _t) { \
            CONCAT(name, _t)() { \
             cuke::details::steps().push_back({step_definition,name}); \
            } \
        } CONCAT(g_,name); \
    } \
    void name(int arg_count, cuke_value* args)



#define INTERNAL_HOOK(hook, name, tag_expression) \
    void name(int arg_count, cuke_value* args); \
    namespace { \
        struct CONCAT(name, _t) { \
            CONCAT(name, _t)() { \
             cuke::details::hooks().push_back({hook,tag_expression,name}); \
            } \
        } CONCAT(g_,name); \
    } \
    void name(int arg_count, cuke_value* args)


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
  struct cuke_conversion_impl<long> 
  {
      static long get_arg(cuke_value* arg, const std::string& file, int line) 
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
  struct cuke_conversion_impl<unsigned long long> 
  {
      static unsigned long long get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return static_cast<unsigned long long>(cuke_to_long(arg));
        }
        print_error(file, ':', line, ": Value is not an integer type.");
        throw std::bad_cast();
      }
  };

  template <>
  struct cuke_conversion_impl<unsigned long> 
  {
      static unsigned long get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return static_cast<unsigned long>(cuke_to_long(arg));
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
  struct cuke_conversion_impl<unsigned int> 
  {
      static unsigned int get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return static_cast<unsigned int>(cuke_to_int(arg));
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
  struct cuke_conversion_impl<unsigned char> 
  {
      static unsigned char get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return static_cast<unsigned char>(cuke_to_byte(arg));
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
  struct cuke_conversion_impl<unsigned short> 
  {
      static unsigned short get_arg(cuke_value* arg, const std::string& file, int line) 
      {
        if (arg->type == VAL_LONG)
        {
          return static_cast<unsigned short>(cuke_to_short(arg));
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
        if (arg->type == VAL_DOUBLE || arg->type == VAL_LONG)
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
        if (arg->type == VAL_DOUBLE || arg->type == VAL_LONG)
        {
          return cuke_to_double(arg);
        }
        print_error(file, ':', line, ": Value is not a floating point type.");
        throw std::bad_cast();
      }
  };
} // namespace cuke::details


/**
 * @def STEP(name, step)
 * @brief Creates a Cucumber step, which is then available in your feature files
 * 
 * @param name A unique function name, this function name has no technical impact
 * @param step The step definition string which is used by feature files later
 */
#define STEP(name, step) INTERNAL_STEP(step, CONCAT(__cuke_step_,name))

/**
 * @def GIVEN(name, step)
 * @brief An alias to STEP(name,step) to increase readability of your code
 */
#define GIVEN(name, step) STEP(name, step)

/**
 * @def WHEN(name, step)
 * @brief An alias to STEP(name,step) to increase readability of your code
 */
#define WHEN(name, step) STEP(name, step)

/**
 * @def THEN(name, step)
 * @brief An alias to STEP(name,step) to increase readability of your code
 */
#define THEN(name, step) STEP(name, step)



/**
 * @def BEFORE(name)
 * @brief Creates a hook which is executed before every Scenario
 * 
 * @param name Optional name: If you want to separate code and implement this hook multiple times, give each one a unique function name
 */
#define BEFORE(name) INTERNAL_HOOK("before", CONCAT(__cuke_before,name),"")
/**
 * @def BEFORE_T(name, tag_expression)
 * @brief Creates a hook with a tag expression, which can run before a tagged scenario
 * 
 * Before running a tagged scenario, the tag expression is evaluated. If it evaluates to true, the hook is executed.
 * 
 * @param name A unique function name, this function name has no technical impact
 * @param tag_expression A tag expression (bool condition), like ``"@tag1 and @tag2"``
 */
#define BEFORE_T(name,tag_expression) INTERNAL_HOOK("before", CONCAT(__cuke_before,name),tag_expression)
/**
 * @def AFTER(name)
 * @brief Creates a hook which is executed after every scenario
 * 
 * @param name Optional name: If you want to separate code and implement this hook multiple times, give each one a unique function name
 */
#define AFTER(name) INTERNAL_HOOK("after", CONCAT(__cuke_after,name),"")
/**
 * @def AFTER_T(name, tag_expression)
 * @brief Creates a hook with a tag expression, which can run after a tagged scenario
 * 
 * After running a tagged scenario, the tag expression is evaluated. If it evaluates to true, the hook is executed.
 * 
 * @param name A unique function name, this function name has no technical impact
 * @param tag_expression A tag expression (bool condition), like ``"@tag1 and @tag2"``
 */
#define AFTER_T(name, tag_expression) INTERNAL_HOOK("after", CONCAT(__cuke_after,name),tag_expression)
/**
 * @def BEFORE_STEP(name)
 * @brief Creates a hook which is executed before every step
 * 
 * @param name Optional name: If you want to separate code and implement this hook multiple times, give each one a unique function name
 */
#define BEFORE_STEP(name) INTERNAL_HOOK("before_step", CONCAT(__cuke_before_step,name),"")
/**
 * @def AFTER_STEP(name)
 * @brief Creates a hook which is executed after every step
 * 
 * @param name Optional name: If you want to separate code and implement this hook multiple times, give each one a unique function name
 */
#define AFTER_STEP(name) INTERNAL_HOOK("after_step", CONCAT(__cuke_after_step,name),"")

/**
 * @def CUKE_ARG(index)
 * @brief Access variables from a step in the step body. ``CUKE_ARG`` starts at index 1 on the first value from the left side.
 * 
 * C++ auto type deduction does not work here. The underlying function is overloaded by return type.
 * 
 * @param index Variable index to access 
 * @return The value from the index in the given step
 */
#define CUKE_ARG(index) cuke::details::get_arg(args, index, arg_count, __FILE__, __LINE__)  

