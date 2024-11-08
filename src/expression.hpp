#pragma once

#include <string>
#include "any.hpp"
#include "value.hpp"

namespace cuke
{
using expression_callback = internal::any (*)(
    cuke::value_array::const_iterator begin, std::size_t count);

struct expression
{
  // TODO: remove key here, since it's the same key as in registry's map
  std::string key;
  std::string pattern;
  std::string type_info;
  expression_callback callback;
};

}  // namespace cuke
