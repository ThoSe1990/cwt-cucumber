#pragma once

#include <vector>
#include <memory>
#include <string>
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

class chunk;
class value;
using value_array = std::vector<value>;

struct function
{
  const std::string name;
  std::unique_ptr<chunk> chunk_ptr;
};

using cuke_step = void (*)(const value_array&);

struct native
{
  cuke_step func;
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

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_same_v<T, function>>>
{
  static constexpr value_type tag = value_type::function;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_same_v<T, native>>>
{
  static constexpr value_type tag = value_type::native;
};

class value
{
 public:
  value() = default;
  value(function&& func)
      : m_type(value_type::function),
        m_value(std::make_unique<value_model<function>>(std::move(func)))
  {
  }
  value(native&& func)
      : m_type(value_type::native),
        m_value(std::make_unique<value_model<native>>(std::move(func)))
  {
  }

  value(value&& other) : m_type(other.m_type), m_value(std::move(other.m_value))
  {
    other.m_type = value_type::nil;
  }

  value(const value& other) : m_type(other.m_type)
  {
    switch (other.m_type)
    {
      case value_type::integral:
        m_value = std::make_unique<value_model<long>>(other.as<long>());
        break;
      case value_type::floating:
        m_value = std::make_unique<value_model<double>>(other.as<double>());
        break;
      case value_type::boolean:
        m_value = std::make_unique<value_model<bool>>(other.as<bool>());
        break;
      case value_type::string:
        m_value =
            std::make_unique<value_model<std::string>>(other.as<std::string>());
        break;
      case value_type::function:
      {
        const auto& func = other.as<function>();
        m_value = std::make_unique<value_model<function>>(
            function{func.name, std::make_unique<chunk>(*func.chunk_ptr)});
      }
      break;
      case value_type::native:
        m_value = std::make_unique<value_model<native>>(other.as<native>());
        break;
      default:
        m_type = value_type::nil;
    }
  }

  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, value>>>
  value(T&& value)
      : m_type(value_trait<T>::tag),
        m_value(std::make_unique<value_model<T>>(std::forward<T>(value)))
  {
  }

  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, value>>>
  value(const T& value)
      : m_type(value_trait<T>::tag),
        m_value(std::make_unique<value_model<T>>(value))
  {
  }

  value& operator=(const value& other)
  {
    if (this != &other)
    {
      m_type = other.m_type;

      switch (other.m_type)
      {
        case value_type::integral:
          m_value = std::make_unique<value_model<long>>(other.as<long>());
          break;
        case value_type::floating:
          m_value = std::make_unique<value_model<double>>(other.as<double>());
          break;
        case value_type::boolean:
          m_value = std::make_unique<value_model<bool>>(other.as<bool>());
          break;
        case value_type::string:
          m_value = std::make_unique<value_model<std::string>>(
              other.as<std::string>());
          break;
        case value_type::function:
        {
          const auto& func = other.as<function>();
          m_value = std::make_unique<value_model<function>>(
              function{func.name, std::make_unique<chunk>(*func.chunk_ptr)});
        }
        break;
        case value_type::native:
          m_value = std::make_unique<value_model<native>>(other.as<native>());
          break;
        default:
          m_type = value_type::nil;
      }
    }

    return *this;
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
  T copy_as() const
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
