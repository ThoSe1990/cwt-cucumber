#pragma once

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
      m_identifiers{{{"Funktionalität:", token_type::feature},
                     {"Funktion:", token_type::feature},
                     {"Szenario:", token_type::scenario},
                     {"Beispiel:", token_type::scenario},
                     {"Szenariogrundriss:", token_type::scenario_outline},
                     {"Szenarien:", token_type::scenario_outline},
                     {"Grundlage:", token_type::background},
                     {"Hintergrund:", token_type::background},
                     {"Rule:", token_type::rule},
                     {"Regel:", token_type::rule},
                     {"Voraussetzungen:", token_type::background},
                     {"Vorbedingungen:", token_type::background},
                     {"Beispiele:", token_type::examples},
                     {"Angenommen", token_type::step},
                     {"Gegeben sei", token_type::step},
                     {"Gegeben seien", token_type::step},
                     {"Wenn", token_type::step},
                     {"Dann", token_type::step},
                     {"Und", token_type::step},
                     {"Aber", token_type::step}}};
};

}  // namespace cuke::internal
