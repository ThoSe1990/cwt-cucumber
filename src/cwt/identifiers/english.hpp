#pragma once

#include "identifier.hpp"

namespace cwt::details
{

class english : public identifier
{
  std::pair<token_type, std::size_t> language_token(
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
  static constexpr std::array<std::pair<std::string_view, token_type>, 10>
      m_identifiers{{{"Feature:", token_type::feature},
                     {"Scenario:", token_type::scenario},
                     {"Scenario Outline:", token_type::scenario_outline},
                     {"Background:", token_type::background},
                     {"Examples:", token_type::examples},
                     {"Given", token_type::step},
                     {"When", token_type::step},
                     {"Then", token_type::step},
                     {"And", token_type::step},
                     {"But", token_type::step}}};
};

}  // namespace cwt::details
