#pragma once

#include <vector>
#include <memory>
#include <string>
#include <format>
#include <stdexcept>
#include <type_traits>

namespace cuke
{
enum class value_type
{
  integral,
  floating,
  _double,
  boolean,
  string,
  function,
  table,
  nil
};

class table;

class value;
using value_array = std::vector<value>;

}  // namespace cuke

namespace cwt::details
{
using table_ptr = std::unique_ptr<cuke::table>;

class chunk;
using function = std::unique_ptr<chunk>;

struct nil_value
{
};
using argv = const std::reverse_iterator<cuke::value_array::const_iterator>&;
using argc = std::size_t;
using step_callback = void (*)(argc, argv);
class step
{
 public:
  step(step_callback cb, const std::string& definition)
      : m_callback(cb), m_definition(definition)
  {
  }
  const std::string& definition() const noexcept { return m_definition; }
  void call(argc n, argv values) const { m_callback(n, values); }
  void operator()(argc n, argv values) const { m_callback(n, values); }

 private:
  step_callback m_callback;
  std::string m_definition;
};

using hook_callback = void (*)();
struct hook;

template <typename T, typename = void>
struct value_trait
{
  static constexpr cuke::value_type tag = cuke::value_type::nil;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_integral_v<T>>>
{
  static constexpr cuke::value_type tag = std::is_same_v<T, bool>
                                              ? cuke::value_type::boolean
                                              : cuke::value_type::integral;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_floating_point_v<T> &&
                                       std::is_same_v<T, float>>>
{
  static constexpr cuke::value_type tag = cuke::value_type::floating;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_floating_point_v<T> &&
                                       std::is_same_v<T, double>>>
{
  static constexpr cuke::value_type tag = cuke::value_type::_double;
};

template <typename T>
struct value_trait<T,
                   std::enable_if_t<std::is_convertible_v<T, std::string> ||
                                    std::is_convertible_v<T, std::string_view>>>
{
  static constexpr cuke::value_type tag = cuke::value_type::string;
};

template <typename T>
struct value_trait<T,
                   std::enable_if_t<std::is_same_v<T, cwt::details::function>>>
{
  static constexpr cuke::value_type tag = cuke::value_type::function;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_same_v<T, table_ptr>>>
{
  static constexpr cuke::value_type tag = cuke::value_type::table;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_same_v<T, nil_value>>>
{
  static constexpr cuke::value_type tag = cuke::value_type::nil;
};

}  // namespace cwt::details

namespace cuke
{

class value
{
 public:
  value() = default;
  value(cwt::details::function&& func)
      : m_type(value_type::function),
        m_value(std::make_unique<value_model<cwt::details::function>>(
            std::move(func)))
  {
  }
  value(cwt::details::table_ptr t)
      : m_type(value_type::table),
        m_value(std::make_unique<value_model<cwt::details::table_ptr>>(
            std::move(t)))
  {
  }

  value(value&& other) : m_type(other.m_type), m_value(std::move(other.m_value))
  {
    other.m_type = value_type::nil;
  }

  value(const value& other) : m_type(other.m_type) { clone(other); }

  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::remove_reference_t<T>, value>>>
  value(T&& value)
      : m_type(cwt::details::value_trait<std::remove_reference_t<T>>::tag),
        m_value(std::make_unique<value_model<std::remove_reference_t<T>>>(
            std::forward<T>(value)))
  {
  }

  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, value>>>
  value(const T& value)
      : m_type(cwt::details::value_trait<T>::tag),
        m_value(std::make_unique<value_model<T>>(value))
  {
  }

  value& operator=(const value& other)
  {
    clone(other);
    return *this;
  }

  ~value() = default;

  template <typename T>
  const T& as() const
  {
    if (m_type == cwt::details::value_trait<T>::tag) [[likely]]
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
    if (m_type == cwt::details::value_trait<T>::tag) [[likely]]
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
    m_type = cwt::details::value_trait<T>::tag;
  }

  value_type type() const noexcept { return m_type; }

  [[nodiscard]] std::string to_string()
  {
    switch (m_type)
    {
      case value_type::integral:
        return std::to_string(copy_as<long>());
      case value_type::_double:
        return std::to_string(copy_as<double>());
      case value_type::floating:
        return std::to_string(copy_as<float>());
      case value_type::boolean:
        return std::to_string(copy_as<bool>());
      case value_type::string:
        return copy_as<std::string>();
      default:
        throw std::runtime_error(
            std::format("cuke::value: Can not create string from value_type {}",
                        static_cast<std::size_t>(m_type)));
    }
  }

 private:
  void clone(const value& other)
  {
    if (this != &other)
    {
      m_type = other.type();
      switch (other.type())
      {
        case value_type::integral:
          m_value = std::make_unique<value_model<long>>(other.as<long>());
          break;
        case value_type::floating:
          m_value = std::make_unique<value_model<float>>(other.as<float>());
          break;
        case value_type::_double:
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
          const auto& func = other.as<cwt::details::function>();
          m_value = std::make_unique<value_model<cwt::details::function>>(
              cwt::details::function{
                  std::make_unique<cwt::details::chunk>(*func)});
        }
        break;
        case value_type::table:
        {
          const auto& t = other.as<cwt::details::table_ptr>();
          m_value = std::make_unique<value_model<cwt::details::table_ptr>>(
              cwt::details::table_ptr{std::make_unique<table>(*t)});
        }
        break;
        default:
          m_type = value_type::nil;
      }
    }
  }

  struct value_concept
  {
    virtual ~value_concept() {}
  };

  template <typename T>
  struct value_model : public value_concept
  {
    value_model() = default;

    template <typename Arg>
    value_model(const Arg& arg) : m_value{arg}
    {
    }

    template <typename Arg>
    value_model(Arg&& arg) : m_value{std::forward<Arg>(arg)}
    {
    }

    T m_value;
  };

 private:
  cuke::value_type m_type{cuke::value_type::nil};
  std::unique_ptr<value_concept> m_value;
};

using value_array = std::vector<value>;

}  // namespace cuke

#include "chunk.hpp"
#include "table.hpp"