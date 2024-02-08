#pragma once

#include "value.hpp"

namespace cwt::details
{

template <typename T, typename = void>
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

template <typename T>
struct conversion_impl<T, std::enable_if_t<std::is_integral_v<T>>>
{
  static T get_arg(const value& v, std::string_view file, std::size_t line)
  {
    static_assert(
        !std::is_same_v<T, long long>,
        "Value access: long long is not supported. Underlying type is long");
    static_assert(
        !std::is_same_v<T, unsigned long long>,
        "Value access: long long is not supported. Underlying type is long");
    if (v.type() == value_type::integral)
    {
      return v.copy_as<T>();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not an integral type", file, line));
    }
  }
};

template <>
struct conversion_impl<std::size_t>
{
  static std::size_t get_arg(const value& v, std::string_view file,
                             std::size_t line)
  {
    if (v.type() == value_type::integral)
    {
      return v.copy_as<unsigned long>();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not an integral type", file, line));
    }
  }
};

template <typename T>
struct conversion_impl<T, std::enable_if_t<std::is_floating_point_v<T>>>
{
  static double get_arg(const value& v, std::string_view file, std::size_t line)
  {
    static_assert(
        !std::is_same_v<T, long long>,
        "Value access: long long is not supported. Underlying type is long");
    static_assert(
        !std::is_same_v<T, unsigned long long>,
        "Value access: long long is not supported. Underlying type is long");
    if (v.type() == value_type::floating)
    {
      return v.copy_as<double>();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not an integral type", file, line));
    }
  }
};

}  // namespace cwt::details
