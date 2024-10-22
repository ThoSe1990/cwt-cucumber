#pragma once

#include <cinttypes>
#include <vector>
#include <string>
#include <format>
#include <type_traits>

#include "token.hpp"
#include "util.hpp"
#include "util_regex.hpp"

namespace cuke
{

class table;

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
  value(const std::string& value) : m_value(value) {}

  value& operator=(const value& other)
  {
    m_value = other.m_value;
    return *this;
  }

  ~value() = default;

  /**
   * @brief Checks if the cuke::value is nil
   * @return True if it is a nil type, else its false
   */
  [[nodiscard]] bool is_nil() const noexcept { return m_value.empty(); }

  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, T>::type as() const
  {
    if (m_value == "true" || m_value == "True")
    {
      return true;
    }
    if (m_value == "false" || m_value == "False")
    {
      return true;
    }
    try
    {
      return static_cast<T>(std::stoll(m_value));
    }
    catch (...)
    {
      internal::println(
          internal::color::red,
          std::format("cuke::value::as: Cannot convert {} to integral type",
                      m_value));
      return T();
    }
  }

  template <typename T>
  typename std::enable_if<std::is_floating_point<T>::value, T>::type as() const
  {
    try
    {
      return static_cast<T>(std::stod(m_value));
    }
    catch (...)
    {
      internal::println(
          internal::color::red,
          std::format(
              "cuke::value::as: Cannot convert {} to floating point type",
              m_value));
      return T();  // Return default initialized value
    }
  }
  template <typename T>
  typename std::enable_if<std::is_same<T, std::string>::value,
                          std::string>::type
  as() const
  {
    return m_value;
  }

  template <typename T>
  typename std::enable_if<std::is_same<T, std::string_view>::value,
                          std::string_view>::type
  as() const
  {
    return m_value;
  }
  /**
   * @brief Emplace or replace a value
   * @param value The value to store
   *
   * @details Constructs or replaces the value in a given cuke::value.
   */
  void emplace_or_replace(const std::string& value) { m_value = value; }

  /**
   * @brief Converts the underlying value to a string. If not possible, this
   * function throws a std::runtime_error
   */
  [[nodiscard]] std::string to_string() const { return m_value; }

 private:
  std::string m_value;
};

using value_array = std::vector<value>;

}  // namespace cuke

namespace cuke::internal
{

using step_callback = void (*)(const cuke::value_array& args,
                               const std::vector<std::string>& doc_string,
                               const cuke::table& t);
class step
{
 public:
  step(step_callback cb, const std::string& definition)
      : m_callback(cb), m_definition(definition)
  {
    std::tie(m_regex_definition, m_value_types) =
        create_regex_definition(add_escape_chars(m_definition));
  }
  const std::string& definition() const noexcept { return m_definition; }
  const std::vector<token_type>& value_types() const noexcept
  {
    return m_value_types;
  }
  const std::string& regex_string() const noexcept
  {
    return m_regex_definition;
  }
  void call(const value_array& values,
            const std::vector<std::string>& doc_string, const table& t) const
  {
    m_callback(values, doc_string, t);
  }

 private:
  step_callback m_callback;
  std::string m_definition;
  std::string m_regex_definition;
  std::vector<token_type> m_value_types;
};

}  // namespace cuke::internal

#include "table.hpp"
