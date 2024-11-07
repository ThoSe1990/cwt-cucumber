#pragma once

#include <string>

namespace cuke::internal
{
struct regex_conversion
{
  const std::string key;
  const std::string pattern;
  const std::string type_info;
};

}  // namespace cuke::internal
