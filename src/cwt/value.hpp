#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <stdexcept>
#include <type_traits>


namespace cwt::details
{

enum class value_type
{
  integral,
  floating,
  boolean,
  string,
  function,
  native,
  nil
};

template <typename T, typename = void>
struct value_trait
{
  static constexpr value_type tag = value_type::nil;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_integral_v<T>>>
{
  static constexpr value_type tag =
      std::is_same_v<T, bool> ? value_type::boolean : value_type::integral;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_floating_point_v<T>>>
{
  static constexpr value_type tag = value_type::floating;
};

template <typename T>
struct value_trait<T,
                   std::enable_if_t<std::is_convertible_v<T, std::string> ||
                                    std::is_convertible_v<T, std::string_view>>>
{
  static constexpr value_type tag = value_type::string;
};

class function;
template <typename T>
struct value_trait<T, std::enable_if_t<std::is_same_v<T, function>>>
{
  static constexpr value_type tag = value_type::function;
};

class native;
template <typename T>
struct value_trait<T, std::enable_if_t<std::is_same_v<T, native>>>
{
  static constexpr value_type tag = value_type::native;
};

class value
{
 public:
  value() = default;

  template <typename T>
  value(T&& value)
      : m_type(value_trait<T>::tag),
        m_value(std::make_unique<value_model<T>>(std::forward<T>(value)))
  {
    std::cout << "type: " << static_cast<int>(m_type) << std::endl;
    std::cout << "type: " << typeid(T).name() << std::endl;
  }

  ~value() = default;

  template <typename T>
  const T& as() const
  {
    if (m_type == value_trait<T>::tag) [[likely]]
    {
      return static_cast<value_model<T>*>(m_value.get())->m_value;
    }
    else [[unlikely]]
    {
      throw std::runtime_error("cwt::value: Invalid value type");
    }
  }

  template <typename T>
  void emplace_or_replace(T&& value)
  {
    std::unique_ptr<value_concept> new_value =
        std::make_unique<value_model<T>>(std::forward<T>(value));
    m_value.swap(new_value);
    m_type = value_trait<T>::tag;
  }

  value_type type() const noexcept { return m_type; }

 private:
  struct value_concept
  {
    virtual ~value_concept() {}
  };

  template <typename T>
  struct value_model : public value_concept
  {
    value_model() = default;
    template <typename Arg>
    value_model(Arg&& arg) : m_value{std::forward<Arg>(arg)}
    {
    }

    T m_value;
  };

 private:
  value_type m_type{value_type::nil};
  std::unique_ptr<value_concept> m_value;
};

using value_array = std::vector<value>;

}  // namespace cwt::details
