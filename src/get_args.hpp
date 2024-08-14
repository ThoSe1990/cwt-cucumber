#pragma once

#include <ostream>
#include <type_traits>
#include "value.hpp"

namespace cuke::internal
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
  const cuke::value& v;
  std::string_view file;
  std::size_t line;

  template <typename T>
  operator T() const
  {
    static_assert(has_conversion_v<T>, "conversion to T not supported");
    return conversion_impl<T>::get_arg(v, file, line);
  }
};

inline conversion get_arg(const cuke::value_array& values, std::size_t idx,
                          std::string_view file, std::size_t line)
{
  std::size_t zero_based_idx = idx - 1;
  if (zero_based_idx < values.max_size())
  {
    return conversion(values[zero_based_idx]);
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
  static T get_arg(const cuke::value& v, std::string_view file,
                   std::size_t line)
  {
    static_assert(
        !std::is_same_v<T, long long>,
        "Value access: long long is not supported. Underlying type is long");
    static_assert(
        !std::is_same_v<T, unsigned long long>,
        "Value access: long long is not supported. Underlying type is long");
    if (v.type() == cuke::value_type::integral)
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
  static std::size_t get_arg(const cuke::value& v, std::string_view file,
                             std::size_t line)
  {
    if (v.type() == cuke::value_type::integral)
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
struct conversion_impl<T, std::enable_if_t<std::is_floating_point_v<T> &&
                                           std::is_same_v<T, double>>>
{
  static double get_arg(const cuke::value& v, std::string_view file,
                        std::size_t line)
  {
    if (v.type() == cuke::value_type::_double)
    {
      return v.copy_as<double>();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not a double", file, line));
    }
  }
};
template <typename T>
struct conversion_impl<T, std::enable_if_t<std::is_floating_point_v<T> &&
                                           std::is_same_v<T, float>>>
{
  static float get_arg(const cuke::value& v, std::string_view file,
                       std::size_t line)
  {
    if (v.type() == cuke::value_type::floating)
    {
      return v.copy_as<float>();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not a float", file, line));
    }
  }
};

template <typename T>
struct conversion_impl<
    T, std::enable_if_t<std::is_convertible_v<T, std::string> ||
                        std::is_convertible_v<T, std::string_view>>>
{
  static const std::string get_arg(const cuke::value& v, std::string_view file,
                                   std::size_t line)
  {
    if (v.type() != cuke::value_type::nil)
    {
      return v.to_string();
    }
    else
    {
      throw std::runtime_error(std::format(
          "{}:{}: Nil value can not be converted to a string", file, line));
    }
  }
};
template <typename T>
struct conversion_impl<
    T, std::enable_if_t<std::is_same_v<T, std::vector<std::string>>>>
{
  static const std::vector<std::string>& get_arg(const cuke::value& v,
                                                 std::string_view file,
                                                 std::size_t line)
  {
    if (v.type() == cuke::value_type::string_array)
    {
      return v.as<std::vector<std::string>>();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not a table", file, line));
    }
  }
};
template <typename T>
struct conversion_impl<T, std::enable_if_t<std::is_same_v<T, cuke::table>>>
{
  static const cuke::table& get_arg(const cuke::value& v, std::string_view file,
                                    std::size_t line)
  {
    if (v.type() == cuke::value_type::table)
    {
      return *v.as<table_ptr>().get();
    }
    else
    {
      throw std::runtime_error(
          std::format("{}:{}: Value is not a table", file, line));
    }
  }
};

}  // namespace cuke::internal

/**
 * @def CUKE_ARG(index)
 * @brief Access variables from a step in the step body. ``CUKE_ARG`` starts at
 * index 1 on the first value from the left side.
 *
 * C++ auto type deduction does not work here. The underlying function is
 * overloaded by return type.
 *
 * @param index Variable index to access
 * @return The value from the index in the given step
 */

#define CUKE_ARG(index) \
  cuke::internal::get_arg(values, index, __FILE__, __LINE__)

/**
 * @def CUKE_DOC_STRING()
 * @brief Access a doc string in a step. Use std::string as type here, e.g.:
 *
 * ``std::string doc = CUKE_DOC_STRING();`` or
 * ``std::string_view doc = CUKE_DOC_STRING();``
 *
 * C++ auto type deduction does not work here. It's essentially the same as
 * CUKE_ARG(..), with index == last.
 *
 */
#define CUKE_DOC_STRING() \
  cuke::internal::get_arg(values, values.size(), __FILE__, __LINE__)
/**
 * @def CUKE_TABLE()
 * @brief Access a table in a step. Use cuke::table (or const cuke::table&) as
 * type, e.g. ``const cuke::table& my_table = CUKE_TABLE();``
 *
 *
 * C++ auto type deduction does not work here. It's essentially the same as
 * CUKE_ARG(..), with index == last.
 *
 */
#define CUKE_TABLE() \
  cuke::internal::get_arg(values, values.size(), __FILE__, __LINE__)
