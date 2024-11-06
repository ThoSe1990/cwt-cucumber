#pragma once

#include <string>
#include <vector>

namespace cuke::internal
{
struct regex_conversion
{
  std::string key;
  std::string pattern;
  std::string type_info;
};

static std::vector<regex_conversion> custom_conversions;

}  // namespace cuke::internal
