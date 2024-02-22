#pragma once

#include <vector>

#include "scanner.hpp"
#include "file.hpp"

namespace cwt::details
{
class parser
{
 public:
  parser(const file& f);
  parser(std::string_view source);
  parser(std::string_view source, bool_operators);
  
  [[nodiscard]] const token& current() const noexcept;
  [[nodiscard]] const token& previous() const noexcept;
  [[nodiscard]] bool error() const noexcept;
  [[nodiscard]] std::vector<token> collect_tokens_to(token_type type);
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
  [[nodiscard]] bool check(token_type type, Args... args) const noexcept
  {
    return check(type) || check(args...);
  }
  template <typename... Args>
  [[nodiscard]] bool is_none_of(token_type type, Args... args) const noexcept
  {
    return !check(type) && !check(args...);
  }

  template <typename... Args>
  void advance_until_line_starts_with(Args... args)
  {
    while (!check(token_type::eof))
    {
      advance_to(token_type::linebreak, token_type::eof);
      if (match(token_type::linebreak))
      {
        if (check(std::forward<Args>(args)...))
        {
          return;
        }
      }
    }
  }

  template <typename... Args>
  [[nodiscard]] bool match(token_type type, Args... args) noexcept
  {
    return match(type) || match(args...);
  }
  [[nodiscard]] bool check(token_type type) const noexcept;
  [[nodiscard]] bool match(token_type type) noexcept;

  void skip_linebreaks();
  void error_at(const token& t, std::string_view msg) noexcept;

 private:
  scanner m_scanner;
  token m_current;
  token m_previous;
  bool m_error{false};
  std::string m_filepath{""};
};

}  // namespace cwt::details
