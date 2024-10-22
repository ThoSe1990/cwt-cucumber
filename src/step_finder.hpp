#pragma once

#include <string_view>
#include <algorithm>

#include "token.hpp"
#include "value.hpp"
#include "table.hpp"

namespace cuke::internal
{

class step_finder
{
 public:
  step_finder(std::string_view feature);
  step_finder(std::string_view feature, cuke::table::row hash_row);

  [[nodiscard]] bool step_matches(const std::string& pattern,
                                  const std::vector<token_type>& types);
  [[nodiscard]] cuke::value_array& values() noexcept;

  template <typename Iterator>
  Iterator find(Iterator first, Iterator last)
  {
    return std::find_if(
        first, last, [this](const cuke::internal::step& s)
        { return step_matches(s.regex_string(), s.var_types()); });
  }

 private:
  std::string m_feature_string;
  cuke::table::row m_hash_row;
  cuke::value_array m_values;
};

}  // namespace cuke::internal
