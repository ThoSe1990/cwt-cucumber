#pragma once

#include "identifier.hpp"

namespace cwt::details
{

class german : public identifier
{
 public:
  token_type get_token(std::string_view str) const override
  {
    if (str.starts_with("Szenario:"))
    {
      return token_type::scenario;
    }
    else if (str.starts_with("Beispiel:"))
    {
      return token_type::scenario;
    }
    else if (str.starts_with("Szenariogrundriss:"))
    {
      return token_type::scenario_outline;
    }
    else if (str.starts_with("Szenarien:"))
    {
      return token_type::scenario_outline;
    }
    else if (str.starts_with("Grundlage:"))
    {
      return token_type::background;
    }
    else if (str.starts_with("Hintergrund:"))
    {
      return token_type::background;
    }
    else if (str.starts_with("Voraussetzungen:"))
    {
      return token_type::background;
    }
    else if (str.starts_with("Vorbedingungen:"))
    {
      return token_type::background;
    }
    else if (str.starts_with("Beispiele:"))
    {
      return token_type::examples;
    }
    else if (str.starts_with("Angenommen"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Gegeben sei"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Gegeben seien"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Wenn"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Dann"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Und"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Aber"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Funktionalität:"))
    {
      return token_type::feature;
    }
    else if (str.starts_with("Funktion:"))
    {
      return token_type::feature;
    }
    else
    {
      return token_type::none;
    }
  }
};

}  // namespace cwt::details
