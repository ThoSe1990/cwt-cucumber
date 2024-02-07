#pragma once

#include "value.hpp"

namespace cwt::details
{

template <typename T>
struct conversion_impl
{
};

template <typename T>
class has_conversion
{
  using one = char;
  struct two
  {
    char x[2];
  };

  template <typename C>
  static one test(decltype(&C::get_arg));
  template <typename C>
  static two test(...);

 public:
  static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <typename T>
static constexpr bool has_conversion_v =
    has_conversion<conversion_impl<T>>::value;

struct conversion
{
  const value& v;
  std::string_view file;
  std::size_t line;

  template <typename T>
  operator T() const
  {
    static_assert(has_conversion_v<T>, "conversion to T not supported");
    return conversion_impl<T>::get_arg(v, file, line);
  }
};

inline conversion get_arg(argc n, argv values, std::size_t idx,
                          std::string_view file, std::size_t line)
{
  const std::size_t left_0_based_idx = n - idx;
  if (left_0_based_idx < n)
  {
    return conversion{*(values + left_0_based_idx), file, line};
  }
  else
  {
    throw std::runtime_error(
        std::format("{}:{}: Index out of range", file, line));
  }
}

template <>
struct conversion_impl<int>
{
  static int get_arg(const value& v, std::string_view file, std::size_t line)
  {
    if (v.type() == value_type::integral)
    {
      return v.copy_as<int>();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not an integral type", file, line));
    }
  }
};

}  // namespace cwt::details
