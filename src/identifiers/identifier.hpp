#pragma once

#include <string_view>

#include "../token.hpp"

namespace cuke::internal
{

class identifier
{
 public:
  virtual ~identifier() = default;

  virtual std::string_view language() const noexcept  = 0;
  virtual std::pair<token_type, std::size_t> get_token(
      std::string_view str) const = 0;

 
 private:
};

}  // namespace cuke::internal
