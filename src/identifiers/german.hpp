#pragma once

#include <array>

#include "identifier.hpp"

namespace cuke::internal
{

class german : public identifier
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
  static constexpr std::array<std::pair<std::string_view, token_type>, 20>
      m_identifiers{{{std::string_view("Funktionalität:"), token_type::feature},
                     {std::string_view("Funktion:"), token_type::feature},
                     {std::string_view("Szenario:"), token_type::scenario},
                     {std::string_view("Beispiel:"), token_type::scenario},
                     {std::string_view("Szenariogrundriss:"), token_type::scenario_outline},
                     {std::string_view("Szenarien:"), token_type::scenario_outline},
                     {std::string_view("Grundlage:"), token_type::background},
                     {std::string_view("Hintergrund:"), token_type::background},
                     {std::string_view("Rule:"), token_type::rule},
                     {std::string_view("Regel:"), token_type::rule},
                     {std::string_view("Voraussetzungen:"), token_type::background},
                     {std::string_view("Vorbedingungen:"), token_type::background},
                     {std::string_view("Beispiele:"), token_type::examples},
                     {std::string_view("Angenommen"), token_type::step},
                     {std::string_view("Gegeben sei"), token_type::step},
                     {std::string_view("Gegeben seien"), token_type::step},
                     {std::string_view("Wenn"), token_type::step},
                     {std::string_view("Dann"), token_type::step},
                     {std::string_view("Und"), token_type::step},
                     {std::string_view("Aber"), token_type::step}}};
};

}  // namespace cuke::internal
