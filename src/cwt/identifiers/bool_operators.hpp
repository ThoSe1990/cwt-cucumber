#pragma once

#include "identifier.hpp"

namespace cwt::details
{

class bool_operators : public identifier
{
  std::pair<token_type, std::size_t> get_token(
      std::string_view str) const override
  {
    for (const auto& element : m_identifiers)
    {
      if (str.starts_with(element.first))
      {
        return {element.second, element.first.length()};
      }
    }
    return {token_type::none, 0};
  }

 private:
   static constexpr std::array<std::pair<std::string_view, token_type>, 4>
      m_identifiers{{{"and", token_type::_and},
                        {"or", token_type::_or},
                        {"xor", token_type::_xor},
                        {"not", token_type::_not}}};

};

}  // namespace cwt::details
