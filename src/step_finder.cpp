#include <regex>
#include <string_view>

#include "step_finder.hpp"

#include "value.hpp"
#include "table.hpp"
#include "util_regex.hpp"

namespace cuke::internal
{

step_finder::step_finder(std::string_view feature,
                         std::optional<cuke::table::row> hash_row)
    : m_feature_string(hash_row.has_value()
                           ? replace_variables(feature.data(), hash_row.value())
                           : feature),
      m_hash_row(std::move(hash_row.value_or(cuke::table::row{})))
{
}
cuke::value_array& step_finder::values() noexcept { return m_values; }
bool step_finder::step_matches(const std::string& pattern)
{
  std::regex regex_pattern(pattern);
  std::smatch match;
  if (std::regex_match(m_feature_string, match, regex_pattern))
  {
    for (std::size_t i = 1; i < match.size(); ++i)
    {
      if (!match[i].matched)
      {
        continue;
      }
      m_values.push_back(cuke::value(std::string{match[i]}));
    }
    return true;
  }
  return false;
}

}  // namespace cuke::internal
