#pragma once

#include <vector>
#include <memory>
#include <string>
#include <format>
#include <stdexcept>
#include <type_traits>

#include "token.hpp"
#include "util_regex.hpp"

namespace cuke
{
/**
 * @brief This enum class represents the cuke::value_type for cuke::value
 */
enum class value_type
{
  integral,      ///< Represents an integer value
  floating,      ///< Represents a float value
  _double,       ///< Represents a double value
  boolean,       ///< Represents a boolean value
  string,        ///< Represents a string value
  string_array,  ///< Represents an array/vector of strings
  table,         ///< Represents a table
  nil            ///< Represents a nil value
};

class table;

}  // namespace cuke

namespace cuke::internal
{

using table_ptr = std::unique_ptr<cuke::table>;

struct nil_value
{
};

template <typename T, typename = void>
struct value_trait
{
  static constexpr cuke::value_type tag = cuke::value_type::nil;
};

template <typename T>
struct value_trait<T, std::enable_if_t<std::is_integral_v<T> ||
                                       std::is_same_v<T, std::size_t>>>
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
struct value_trait<
    T, std::enable_if_t<std::is_convertible_v<T, std::vector<std::string>>>>
{
  static constexpr cuke::value_type tag = cuke::value_type::string_array;
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

}  // namespace cuke::internal

namespace cuke
{

/**
 * @class value
 * @brief The value type for all values in CWT Cucumber
 *
 * @details cuke::value uses type erasure to store all possible types.
 *
 */
class value
{
 public:
  value() = default;
  value(cuke::internal::table_ptr t)
      : m_type(value_type::table),
        m_value(std::make_unique<value_model<cuke::internal::table_ptr>>(
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
      : m_type(cuke::internal::value_trait<std::remove_reference_t<T>>::tag),
        m_value(std::make_unique<value_model<std::remove_reference_t<T>>>(
            std::forward<T>(value)))
  {
  }

  template <typename T, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<T>, value>>>
  value(const T& value)
      : m_type(cuke::internal::value_trait<T>::tag),
        m_value(std::make_unique<value_model<T>>(value))
  {
  }

  value& operator=(const value& other)
  {
    clone(other);
    return *this;
  }

  ~value() = default;

  /**
   * @brief Checks if the cuke::value is nil
   * @return True if it is a nil type, else its false
   */
  [[nodiscard]] bool is_nil() const noexcept
  {
    return m_type == value_type::nil;
  }

  /**
   * @brief Retrieve a const reference to the underlying value.
   * @tparam T Return type / target type
   * @return Constant reference to the underlying value
   *
   * @details This function returns a const reference to the underlying value.
   * It casts to the given template parameter T. If this results in a bad cast,
   * this function throws a std::runtime_error. Note: It creates a copy for
   * std::size_t
   */
  template <typename T,
            std::enable_if_t<std::is_same_v<T, std::size_t>, bool> = true>
  T as() const
  {
    if (m_type == cuke::internal::value_trait<long>::tag) [[likely]]
    {
      return static_cast<value_model<long>*>(m_value.get())->m_value;
    }
    else [[unlikely]]
    {
      throw std::runtime_error("cwt::value: Invalid value type");
    }
  }

  /**
   * @brief Retrieve a const reference to the underlying value.
   * @tparam T Return type / target type
   * @return Constant reference to the underlying value
   *
   * @details This function returns a const reference to the underlying value.
   * It casts to the given template parameter T. If this results in a bad cast,
   * this function throws a std::runtime_error. Note: It creates a copy for
   * std::size_t
   */
  template <typename T,
            std::enable_if_t<!std::is_same_v<T, std::size_t>, bool> = true>
  const T& as() const
  {
    if (m_type == cuke::internal::value_trait<T>::tag) [[likely]]
    {
      return static_cast<value_model<T>*>(m_value.get())->m_value;
    }
    else [[unlikely]]
    {
      throw std::runtime_error("cwt::value: Invalid value type");
    }
  }
  /**
   * @brief Retrieve a copy of the underlying value.
   * @tparam T Return type / target type
   * @return Copy of the underlying value
   *
   * @details This function returns a copye of the underlying value. It casts to
   * the given template parameter T. If this results in a bad cast, this
   * function throws a std::runtime_error.
   */
  template <typename T>
  T copy_as() const
  {
    if (m_type == cuke::internal::value_trait<T>::tag) [[likely]]
    {
      return static_cast<value_model<T>*>(m_value.get())->m_value;
    }
    else [[unlikely]]
    {
      throw std::runtime_error("cwt::value: Invalid value type");
    }
  }

  /**
   * @brief Emplace or replace a value
   * @param value The value to store
   *
   * @details Constructs or replaces the value in a given cuke::value.
   */
  template <typename T>
  void emplace_or_replace(T&& value)
  {
    std::unique_ptr<value_concept> new_value =
        std::make_unique<value_model<T>>(std::forward<T>(value));
    m_value.swap(new_value);
    m_type = cuke::internal::value_trait<T>::tag;
  }

  /**
   * @brief Type info for the underlying value
   * @return Returns cuke::value_type of the underlying value
   *
   */
  value_type type() const noexcept { return m_type; }

  /**
   * @brief Converts the underlying value to a string. If not possible, this
   * function throws a std::runtime_error
   */
  [[nodiscard]] std::string to_string() const
  {
    switch (m_type)
    {
      case value_type::integral:
        return std::format("{}", copy_as<long>());
      case value_type::_double:
        return std::format("{}", copy_as<double>());
      case value_type::floating:
        return std::format("{}", copy_as<float>());
      case value_type::boolean:
        return std::format("{}", copy_as<bool>());
      case value_type::string:
        return copy_as<std::string>();
      case value_type::string_array:
      {
        std::string str = "";
        for (const std::string& line : as<std::vector<std::string>>())
        {
          str += line;
        }
        return str;
      }
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
        case value_type::table:
        {
          const auto& t = other.as<cuke::internal::table_ptr>();
          m_value = std::make_unique<value_model<cuke::internal::table_ptr>>(
              cuke::internal::table_ptr{std::make_unique<table>(*t)});
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

namespace cuke::internal
{

using step_callback = void (*)(const cuke::value_array& args);
class step
{
 public:
  step(step_callback cb, const std::string& definition)
      : m_callback(cb), m_definition(definition)
  {
    std::tie(m_regex_definition, m_types) =
        create_regex_definition(add_escape_chars(m_definition));
  }
  const std::string& definition() const noexcept { return m_definition; }
  const std::string& regex_string() const noexcept
  {
    return m_regex_definition;
  }
  const std::vector<token_type>& var_types() const noexcept { return m_types; }
  void call(const value_array& values) const { m_callback(values); }

 private:
  step_callback m_callback;
  std::string m_definition;
  std::string m_regex_definition;
  std::vector<token_type> m_types;
};

}  // namespace cuke::internal

#include "table.hpp"
