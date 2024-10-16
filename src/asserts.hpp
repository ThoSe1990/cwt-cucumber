#pragma once

#include <optional>
#include <string_view>

#include "util.hpp"
#include "options.hpp"

namespace cuke::internal
{
inline void internal_assert(bool condition, std::string_view error_msg)
{
  if (condition == false)
  {
    cuke::results::set_step_to(cuke::results::test_status::failed);
    if (!program_arguments().get_options().quiet)
    {
      println(color::red, error_msg);
    }
  }
}
}  // namespace cuke::internal

namespace cuke
{

template <typename T, typename U, typename = std::void_t<>>
struct is_comparable : std::false_type
{
};

template <typename T, typename U>
struct is_comparable<
    T, U, std::void_t<decltype(std::declval<T>() == std::declval<U>())>>
    : std::true_type
{
};

/**
 * @brief Compares the given values to be equal. If not, the current step and
 * scenario are set to Failed.
 *
 * The given types must be comparable. If they are not comparable (like string
 * and integer), the current step and the scenario are set to failed.
 *
 * @param lhs Left hand side parameter
 * @param rhs Right hand side parameter
 *
 */
template <typename T, typename U>
inline void equal(const T& lhs, const U& rhs,
                  const std::optional<std::string>& custom_msg = std::nullopt)
{
  if constexpr (is_comparable<T, U>::value)
  {
    std::string msg = custom_msg.value_or(std::format(
        "Value {} is not equal to {} in following step:", lhs, rhs));
    cuke::internal::internal_assert(lhs == rhs, msg);
  }
  else
  {
    cuke::internal::internal_assert(false, "Expect values to be compareable.");
  }
}

/**
 * @brief Compares the given values not to be equal. If not, the current step
 * and scenario are set to Failed.
 *
 * The given types must be comparable. If they are not comparable (like string
 * and integer), the current step and the scenario are set to failed.
 *
 * @param lhs Left hand side parameter
 * @param rhs Right hand side parameter
 *
 */
template <typename T, typename U>
inline void not_equal(
    const T& lhs, const U& rhs,
    const std::optional<std::string>& custom_msg = std::nullopt)
{
  if constexpr (is_comparable<T, U>::value)
  {
    std::string msg = custom_msg.value_or(std::format(
        "Value {} is not equal to {} in following step:", lhs, rhs));
    cuke::internal::internal_assert(lhs != rhs, msg);
  }
  else
  {
    cuke::internal::internal_assert(false, "Expect values to be compareable.");
  }
}

/**
 * @brief Compares whether the first value (left) is greater than the second
 * value (right). If not, the current step and scenario are set to Failed.
 *
 * The given types must be comparable. If they are not comparable (like string
 * and integer), the current step and the scenario are set to failed.
 *
 * @param lhs Left hand side parameter
 * @param rhs Right hand side parameter
 *
 */
template <typename T, typename U>
inline void greater(const T& lhs, const U& rhs,
                    const std::optional<std::string>& custom_msg = std::nullopt)
{
  if constexpr (is_comparable<T, U>::value)
  {
    std::string msg = custom_msg.value_or(std::format(
        "Value {} is not greater than {} in following step:", lhs, rhs));
    cuke::internal::internal_assert(lhs > rhs, msg);
  }
  else
  {
    cuke::internal::internal_assert(false, "Expect values to be compareable.");
  }
}

/**
 * @brief Compares whether the first value (left) is greater or equal than the
 * second value (right). If not, the current step and scenario are set to
 * Failed.
 *
 * The given types must be comparable. If they are not comparable (like string
 * and integer), the current step and the scenario are set to failed.
 *
 * @param lhs Left hand side parameter
 * @param rhs Right hand side parameter
 *
 */
template <typename T, typename U>
inline void greater_or_equal(
    const T& lhs, const U& rhs,
    const std::optional<std::string>& custom_msg = std::nullopt)
{
  if constexpr (is_comparable<T, U>::value)
  {
    std::string msg = custom_msg.value_or(std::format(
        "Value {} is not greater or equal than {} in following step:", lhs,
        rhs));
    cuke::internal::internal_assert(lhs >= rhs, msg);
  }
  else
  {
    cuke::internal::internal_assert(false, "Expect values to be compareable.");
  }
}

/**
 * @brief Compares whether the first value (left) is less the second value
 * (right). If not, the current step and scenario are set to Failed.
 *
 * The given types must be comparable. If they are not comparable (like string
 * and integer), the current step and the scenario are set to failed.
 *
 * @param lhs Left hand side parameter
 * @param rhs Right hand side parameter
 *
 */
template <typename T, typename U>
inline void less(const T& lhs, const U& rhs,
                 const std::optional<std::string>& custom_msg = std::nullopt)
{
  if constexpr (is_comparable<T, U>::value)
  {
    std::string msg = custom_msg.value_or(std::format(
        "Value {} is not less than {} in following step:", lhs, rhs));
    cuke::internal::internal_assert(lhs < rhs, msg);
  }
  else
  {
    cuke::internal::internal_assert(false, "Expect values to be compareable.");
  }
}

/**
 * @brief Compares whether the first value (left) is less or equal than the
 * second value (right). If not, the current step and scenario are set to
 * Failed.
 *
 * The given types must be comparable. If they are not comparable (like string
 * and integer), the current step and the scenario are set to failed.
 *
 * @param lhs Left hand side parameter
 * @param rhs Right hand side parameter
 *
 */
template <typename T, typename U>
inline void less_or_equal(
    const T& lhs, const U& rhs,
    const std::optional<std::string>& custom_msg = std::nullopt)
{
  if constexpr (is_comparable<T, U>::value)
  {
    std::string msg = custom_msg.value_or(std::format(
        "Value {} is not less or equal than {} in following step:", lhs, rhs));
    cuke::internal::internal_assert(lhs <= rhs, msg);
  }
  else
  {
    cuke::internal::internal_assert(false, "Expect values to be compareable.");
  }
}

/**
 * @brief Asserts the given condition to true. If it is false, the current step
 * and scenario are set to Failed.
 *
 * @param condition Bool expression to be evaluated
 *
 */
inline void is_true(bool condition,
                    const std::optional<std::string>& custom_msg = std::nullopt)
{
  std::string msg =
      custom_msg.value_or("Expected given condition true, but its false:");
  cuke::internal::internal_assert(condition, msg);
}
/**
 * @brief Asserts the given condition to false. If it is true, the current step
 * and scenario are set to Failed.
 *
 * @param condition Bool expression to be evaluated
 *
 */
inline void is_false(
    bool condition, const std::optional<std::string>& custom_msg = std::nullopt)
{
  std::string msg =
      custom_msg.value_or("Expected given condition true, but its false:");
  cuke::internal::internal_assert(!condition, msg);
}
};  // namespace cuke
