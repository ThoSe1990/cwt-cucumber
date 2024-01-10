#pragma once

#include <string_view>

#include "token.hpp"

namespace cwt::details
{

class identifier
{
 public:
  virtual ~identifier() = default;
  virtual token_type get_token(std::string_view str) const = 0;
};

}  // namespace cwt::details