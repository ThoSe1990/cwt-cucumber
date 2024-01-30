#pragma once

#include <array>
#include <string_view>

#include "../token.hpp"

namespace cwt::details
{

class identifier
{
 public:
  virtual ~identifier() = default;

  std::pair<token_type, std::size_t> get_token(std::string_view str)
  {
    for (const auto& element : m_bool_operators)
    {
      if (str.starts_with(element.first))
      {
        return {element.second, element.first.length()};
      }
    }
    return language_token(str);
  }

 private:
  virtual std::pair<token_type, std::size_t> language_token(
      std::string_view str) const = 0;

 private:
  static constexpr std::array<std::pair<std::string_view, token_type>, 4>
      m_bool_operators{{{"and", token_type::_and},
                        {"or", token_type::_or},
                        {"xor", token_type::_xor},
                        {"not", token_type::_not}}};
};

}  // namespace cwt::details