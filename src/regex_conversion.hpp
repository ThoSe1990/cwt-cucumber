#pragma once

#include <string>

namespace cuke::internal
{
struct regex_conversion
{
  std::string key;
  std::string pattern;
  std::string type_info;
};

}  // namespace cuke::internal
