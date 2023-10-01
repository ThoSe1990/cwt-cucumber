#pragma once 

#include <string_view>
#include <iostream>
#include <vector>

extern "C" {
  #include "cwt/cucumber.h"
}

namespace cwtc::details
{
  std::vector<std::pair<const char*, cwtc_step_t>> steps;
} // namespace cwtc::details

#define CONCAT_INTERNAL(a, b) a ## b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

#define INTERNAL_STEP(step_definition, n) \
    void CONCAT(__cwtc_step_impl_,n)(int arg_count, cwtc_value* args); \
    namespace { \
        struct CONCAT(cwt_step,n) { \
            CONCAT(cwt_step,n)() { \
             cwtc::details::steps.push_back( {step_definition ,CONCAT(__cwtc_step_impl_,n)} ); \
            } \
        } CONCAT(g_cwt_step,n); \
    } \
    void CONCAT(__cwtc_step_impl_,n)(int arg_count, cwtc_value* args)

#define CWTC_STEP(step) INTERNAL_STEP(step, __COUNTER__)


namespace cwtc::details
{
  template<typename T>
  struct cwtc_conversion_impl{};

  template<typename T>
  class has_cwtc_conversion_function
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
  static constexpr bool has_cwtc_conversion_v = has_cwtc_conversion_function<cwtc_conversion_impl<T>>::value;

  struct cwtc_conversion 
  {
    int n;
    cwtc_value* arg;
    template<typename T>
    operator T() const{
        static_assert(has_cwtc_conversion_v<T>, "conversion to T not supported");
        return cwtc_conversion_impl<T>::get_arg(n, arg);
    }
  };
  
  cwtc_conversion get_arg(int n, cwtc_value* args) 
  { 
      return cwtc_conversion{n, args}; 
  }

  template <>
  struct cwtc_conversion_impl<long long> 
  {
      static long long get_arg(int n, cwtc_value* arg) 
      {
          return cwtc_to_long(arg);
      }
  };

  template <>
  struct cwtc_conversion_impl<int> 
  {
      static int get_arg(int n, cwtc_value* arg) 
      {
          return static_cast<int>(cwtc_to_int(arg));
      }
  };

  template <>
  struct cwtc_conversion_impl<char> 
  {
      static char get_arg(int n, cwtc_value* arg) 
      {
        return cwtc_to_byte(arg);
      }
  };
  
  template <>
  struct cwtc_conversion_impl<short> 
  {
      static short get_arg(int n, cwtc_value* arg) 
      {
        return cwtc_to_short(arg);
      }
  };

  template <>
  struct cwtc_conversion_impl<std::string>
  {
      static std::string get_arg(int n, cwtc_value* arg)
      {
          return cwtc_to_string(arg);
      }
  };

  template <>
  struct cwtc_conversion_impl<float>
  {
      static float get_arg(int n, cwtc_value* arg)
      {
          return cwtc_to_float(arg);
      }
  };

  template <>
  struct cwtc_conversion_impl<double>
  {
      static double get_arg(int n, cwtc_value* arg)
      {
          return cwtc_to_double(arg);
      }
  };
} // namespace cwtc::details


#define CWTC_ARG(i) cwtc::details::get_arg(arg_count, &args[i-1])  


namespace cwtc
{

  class tests 
  {
    public:
      tests() 
      {
        open_cucumber();
        for (const auto& pair : details::steps) 
        {
          cwtc_step(pair.first, pair.second);
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
} // namespace cwtc
