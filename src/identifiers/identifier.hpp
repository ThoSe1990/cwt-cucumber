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

  virtual std::pair<token_type, std::size_t> get_token(std::string_view str) const = 0;

 private:

};

}  // namespace cwt::details