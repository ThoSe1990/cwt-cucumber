#pragma once

#include "scanner.hpp"

namespace cwt::details
{
class parser
{
 public:
  parser(std::string_view source);
  [[nodiscard]] const token& current() const noexcept;
  [[nodiscard]] const token& previous() const noexcept;
  [[nodiscard]] bool error() const noexcept;
  void advance();
  void advance_to(token_type type);
  template <typename... Args>
  void advance_to(Args... args)
  {
    while (!check(std::forward<Args>(args)...))
    {
      advance();
    }
  }
  void consume(token_type type, std::string_view msg);
  template <typename... Args>
  [[nodiscard]] bool check(token_type type, Args... args)
  {
    return check(type) || check(args...);
  }
  template <typename... Args>
  [[nodiscard]] bool is_none_of(token_type type, Args... args)
  {
    return !check(type) && !check(args...);
  }
  [[nodiscard]] bool check(token_type type);
  [[nodiscard]] bool match(token_type type);

  void skip_linebreaks();
  void error_at(const token& t, std::string_view msg) noexcept;

 private:
  scanner m_scanner;
  token m_current;
  token m_previous;
  bool m_error{false};
};

}  // namespace cwt::details
