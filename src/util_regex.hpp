
#include <array>
#include <regex>
#include <algorithm>
#include <unordered_set>

namespace cuke::internal
{

struct regex_conversion
{
  std::string key;
  std::string pattern;
  std::string type_info;
};

static /* constexpr */ const std::array<regex_conversion, 10> conversions = {
    {{"{byte}", "(-?\\d+)", "byte"},
     {"{int}", "(-?\\d+)", "int"},
     {"{short}", "(-?\\d+)", "short"},
     {"{long}", "(-?\\d+)", "long"},
     {"{float}", "(-?\\d*\\.?\\d+)", "float"},
     {"{double}", "(-?\\d*\\.?\\d+)", "double"},
     {"{word}", "([^\\s<]+)", "word"},
     {"{string}", "\"(.*?)\"", "string"},
     {"{}", "(.+)", "anonymous"},
     {"{pair of integers}", "var1 = (\\d+), var2 = (\\d+)", "two integers"}}};

static /* constexpr */ const std::array<regex_conversion, 1> custom_conversion =
    {{{"{pair of integers}", "var1 = (\\d+), var2 = (\\d+)", "two integers"}}};

// TODO: tests ...
static std::size_t count_capture_groups(const std::string& pattern)
{
  std::regex re(pattern);
  return re.mark_count();
}

static /* constexpr */ const regex_conversion& get_regex_conversion(
    std::string_view key)
{
  auto it = std::find_if(conversions.begin(), conversions.end(),
                         [&key](const regex_conversion& conversion)
                         { return conversion.key == key; });

  if (it != conversions.end()) [[likely]]
  {
    return (*it);
  }

  throw std::runtime_error("Conversion not found");
}

[[nodiscard]] static std::string create_word_alternation(
    const std::string& step)
{
  std::string result = step;

  result = std::regex_replace(result, std::regex("\\)"), ")?");
  result = std::regex_replace(result, std::regex("\\("), "(?:");

  std::regex pattern("(\\w+)/(\\w+)");
  std::smatch match;

  while (std::regex_search(result, match, pattern))
  {
    result = std::regex_replace(
        result, pattern, "(?:" + match[1].str() + "|" + match[2].str() + ")",
        std::regex_constants::format_first_only);
  }

  return result;
}

[[nodiscard]] static /* constexpr */ const std::pair<std::string,
                                                     std::vector<std::string>>
create_regex_definition(const std::string& step)
{
  std::string test = step;
  std::string result = '^' + create_word_alternation(step);
  std::regex pattern("\\{(.*?)\\}");
  std::smatch match;

  std::vector<std::string> type_info;

  while (std::regex_search(result, match, pattern))
  {
    const auto& conversion = get_regex_conversion(match[0].str());
    result = std::regex_replace(result, pattern, conversion.pattern,
                                std::regex_constants::format_first_only);
    type_info.push_back(conversion.type_info);
  }

  result += '$';
  return {result, type_info};
}

static const std::unordered_set<char> special_chars = {
    '.', '^', '$', '*', '+', '?', '[', ']', /* '(', ')', */ '\\',
    /* '|' */};

static std::string add_escape_chars(const std::string& input)
{
  std::string result;
  for (char c : input)
  {
    if (special_chars.find(c) != special_chars.end())
    {
      result += '\\';
    }
    result += c;
  }
  return result;
}

}  // namespace cuke::internal
