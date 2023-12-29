#pragma once

#include "../identifier.hpp"

namespace cwt::details
{

class english : public identifier
{
 public:
  token_type get_token(std::string_view str) const override
  {
    if (str.starts_with("Scenario:"))
    {
      return token_type::scenario;
    }
    else if (str.starts_with("Scenario Outline:"))
    {
      return token_type::scenario_outline;
    }
    else if (str.starts_with("Background:"))
    {
      return token_type::background;
    }
    else if (str.starts_with("Examples:"))
    {
      return token_type::examples;
    }
    else if (str.starts_with("Given"))
    {
      return token_type::step;
    }
    else if (str.starts_with("When"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Then"))
    {
      return token_type::step;
    }
    else if (str.starts_with("And"))
    {
      return token_type::step;
    }
    else if (str.starts_with("But"))
    {
      return token_type::step;
    }
    else if (str.starts_with("Feature:"))
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
