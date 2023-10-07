#pragma once 

#include <string_view>
#include <iostream>
#include <vector>

extern "C" {
  #include "cwt/cucumber.h"
}

namespace cuke::details
{
  std::vector<std::pair<const char*, cuke_step_t>> steps;
} // namespace cuke::details

#define CONCAT_INTERNAL(a, b) a ## b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

#define INTERNAL_STEP(step_definition, name) \
    void name(int arg_count, cuke_value* args); \
    namespace { \
        struct CONCAT(cwt_step_,name) { \
            CONCAT(cwt_step_,name)() { \
             cuke::details::steps.push_back( {step_definition ,name } ); \
            } \
        } CONCAT(g_cwt_step_,name); \
    } \
    void name(int arg_count, cuke_value* args)

#define STEP(step) INTERNAL_STEP(step, CONCAT(__cuke_step_impl_,__LINE__))
#define STEP_(name, step) INTERNAL_STEP(step, name)


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
  
  cuke_conversion get_arg(int n, cuke_value* args) 
  { 
      return cuke_conversion{n, args}; 
  }

  template <>
  struct cuke_conversion_impl<long long> 
  {
      static long long get_arg(int n, cuke_value* arg) 
      {
          return cuke_to_long(arg);
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
      }
      ~tests() 
      {
        close_cucumber();
      }

      void run(int argc, const char* argv[])
      {
        run_cucumber(argc, argv);
      }
  };
} // namespace cuke
