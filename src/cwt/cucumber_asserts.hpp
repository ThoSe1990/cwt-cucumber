#include <string>
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


  /**
   * @brief Compares the given values to be equal. If not, the current step and scenario are set to Failed.
   * 
   * The given types must be comparable. If they are not comparable (like string and integer), the current step and the scenario are set to failed.
   * 
   * @param lhs Left hand side parameter
   * @param rhs Right hand side parameter
   * 
  */
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

  /**
   * @brief Compares whether the first value (left) is greater than the second value (right). If not, the current step and scenario are set to Failed.
   * 
   * The given types must be comparable. If they are not comparable (like string and integer), the current step and the scenario are set to failed.
   * 
   * @param lhs Left hand side parameter
   * @param rhs Right hand side parameter
   * 
  */
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
  
  /**
   * @brief Compares whether the first value (left) is greater or equal than the second value (right). If not, the current step and scenario are set to Failed.
   * 
   * The given types must be comparable. If they are not comparable (like string and integer), the current step and the scenario are set to failed.
   * 
   * @param lhs Left hand side parameter
   * @param rhs Right hand side parameter
   * 
  */
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
  
  /**
   * @brief Compares whether the first value (left) is less the second value (right). If not, the current step and scenario are set to Failed.
   * 
   * The given types must be comparable. If they are not comparable (like string and integer), the current step and the scenario are set to failed.
   * 
   * @param lhs Left hand side parameter
   * @param rhs Right hand side parameter
   * 
  */
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

  /**
   * @brief Compares whether the first value (left) is less or equal than the second value (right). If not, the current step and scenario are set to Failed.
   * 
   * The given types must be comparable. If they are not comparable (like string and integer), the current step and the scenario are set to failed.
   * 
   * @param lhs Left hand side parameter
   * @param rhs Right hand side parameter
   * 
  */
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

  /**
   * @brief Asserts the given condition to true. If it is false, the current step and scenario are set to Failed.
   * 
   * @param condition Bool expression to be evaluated
   * 
  */
  inline void is_true(bool condition)
  {
    cuke_assert(condition, "Expected given condition true, but its false:");
  }
  /**
   * @brief Asserts the given condition to false. If it is true, the current step and scenario are set to Failed.
   * 
   * @param condition Bool expression to be evaluated
   * 
  */
  inline void is_false(bool condition)
  {
    cuke_assert(!condition, "Expected given condition true, but its false:");
  }
} // namespace cuke 
