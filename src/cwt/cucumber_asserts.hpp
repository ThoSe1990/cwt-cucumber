#include <type_traits>

extern "C" {
  #include "cucumber.h"
}
#include "cucumber_error.hpp"

namespace cuke 
{
  template <typename T, typename U, typename = std::void_t<>>
  struct is_comparable : std::false_type {};

  template <typename T, typename U>
  struct is_comparable<T, U, std::void_t<decltype(std::declval<T>() == std::declval<U>())>> : std::true_type {};


  template <typename T, typename U>
  inline void equal(const T& lhs, const U& rhs) 
  {
    if constexpr (is_comparable<T, U>::value)
    {
      cuke_assert(lhs == rhs, "Value %s is not equal to %s in following step:", std::to_string(lhs).c_str(), std::to_string(rhs).c_str());
    }
    else 
    {
      cuke_assert(false, "Expect values to be compareable.");
    }
  }

  template <typename T, typename U>
  inline void greater(const T& lhs, const U& rhs) 
  {
    if constexpr (is_comparable<T, U>::value)
    {
      cuke_assert(lhs > rhs, "Value %s is not greater than %s in following step:", std::to_string(lhs).c_str(), std::to_string(rhs).c_str());
    }
    else 
    {
      cuke_assert(false, "Expect values to be compareable.");
    }
  }
  
  template <typename T, typename U>
  inline void greater_or_equal(const T& lhs, const U& rhs) 
  {
    if constexpr (is_comparable<T, U>::value)
    {
      cuke_assert(lhs >= rhs, "Value %s is not greater or equal than %s in following step:", std::to_string(lhs).c_str(), std::to_string(rhs).c_str());
    }
    else 
    {
      cuke_assert(false, "Expect values to be compareable.");
    }
  }
  

  template <typename T, typename U>
  inline void less(const T& lhs, const U& rhs) 
  {
    if constexpr (is_comparable<T, U>::value)
    {
      cuke_assert(lhs < rhs, "Value %s is not less than %s in following step:", std::to_string(lhs).c_str(), std::to_string(rhs).c_str());
    }
    else 
    {
      cuke_assert(false, "Expect values to be compareable.");
    }
  }
  
  template <typename T, typename U>
  inline void less_or_equal(const T& lhs, const U& rhs) 
  {
    if constexpr (is_comparable<T, U>::value)
    {
      cuke_assert(lhs <= rhs, "Value %s is not less or equal than %s in following step:", std::to_string(lhs).c_str(), std::to_string(rhs).c_str());
    }
    else 
    {
      cuke_assert(false, "Expect values to be compareable.");
    }
  }

  inline void is_true(bool condition)
  {
    cuke_assert(condition, "Expected given condition true, but its false:");
  }
  inline void is_false(bool condition)
  {
    cuke_assert(!condition, "Expected given condition true, but its false:");
  }
} // namespace cuke 
