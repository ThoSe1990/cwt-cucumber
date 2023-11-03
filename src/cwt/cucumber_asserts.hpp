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
  void assert_equal(const T& rhs, const U& lhs) 
  {
    if constexpr (is_comparable<T, U>::value)
    {
      cuke_assert(rhs == lhs, "Expect value in following step to be %s but it is %s:", std::to_string(rhs).c_str(), std::to_string(lhs).c_str());
    }
    else 
    {
      cuke_assert(false, "Expect values to be compareable.");
    }
  }
} // namespace cuke 
