#pragma once

#include <array>
#include <string_view>

#include "identifier.hpp"

namespace cuke::internal
{

class english : public identifier
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
  static constexpr std::array<std::pair<std::string_view, token_type>, 12>
      m_identifiers{{{std::string_view("Feature:"), token_type::feature},
                     {std::string_view("Scenario:"), token_type::scenario},
                     {std::string_view("Example:"), token_type::scenario},
                     {std::string_view("Scenario Outline:"), token_type::scenario_outline},
                     {std::string_view("Rule:"), token_type::rule},
                     {std::string_view("Background:"), token_type::background},
                     {std::string_view("Examples:"), token_type::examples},
                     {std::string_view("Given"), token_type::step},
                     {std::string_view("When"), token_type::step},
                     {std::string_view("Then"), token_type::step},
                     {std::string_view("And"), token_type::step},
                     {std::string_view("But"), token_type::step}}};
};

}  // namespace cuke::internal
