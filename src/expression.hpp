#pragma once

#include <string>
#include "any.hpp"
#include "value.hpp"

namespace cuke::internal
{
using expression_callback = any (*)(cuke::value_array::const_iterator begin,
                                    std::size_t count);

struct expression
{
  std::string pattern;
  std::string type_info;
  expression_callback callback;
};

}  // namespace cuke::internal
