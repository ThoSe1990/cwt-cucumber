#include <regex>
#include <string_view>

#include "step_finder.hpp"

#include "token.hpp"
#include "value.hpp"
#include "util.hpp"

namespace cuke::internal
{

step_finder::step_finder(std::string_view feature) : m_feature_string(feature)
{
}
step_finder::step_finder(std::string_view feature, cuke::table::row hash_row)
    : m_feature_string(feature), m_hash_row(std::move(hash_row))
{
}
cuke::value_array& step_finder::values() noexcept { return m_values; }
bool step_finder::step_matches(const std::string& pattern,
                               const std::vector<token_type>& types)
{
  bool found = false;
  {
    std::regex regex_pattern(pattern);
    std::smatch match;
    std::string text = m_feature_string;
    while (std::regex_search(text, match, regex_pattern))
    {
      found = true;
      for (std::size_t i = 1; i < match.size(); ++i)
      {
        if (!match[i].matched)
        {
          continue;
        }
        const std::string& value = match[i];
        if (match[i].str()[0] == '<')
        {
          m_values.push_back(
              m_hash_row[value.substr(1, value.size() - 2)].to_string());
        }
        else
        {
          m_values.push_back(value);
        }
      }
      text = match.suffix().str();
    }
  }
  return found;
}

}  // namespace cuke::internal
