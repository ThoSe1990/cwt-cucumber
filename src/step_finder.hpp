#pragma once

#include <string_view>
#include <algorithm>

#include "value.hpp"
#include "scanner.hpp"
#include "table.hpp"

namespace cuke::internal
{

class step_finder
{
 public:
  step_finder(std::string_view feature);
  step_finder(std::string_view feature, cuke::table::row hash_row);

  [[nodiscard]] bool step_matches(std::string_view defined_step);
  [[nodiscard]] cuke::value_array& values() noexcept;

  template <typename Iterator>
  Iterator find(Iterator first, Iterator last)
  {
    return std::find_if(first, last, [this](const cuke::internal::step& s)
                        { return step_matches(s.definition()); });
  }

 private:
  [[nodiscard]] bool parameter_matches_value(token_type parameter,
                                             token_type value);
  [[nodiscard]] bool is_parameter(token_type parameter);
  [[nodiscard]] bool is_at_end(const token& t);
  [[nodiscard]] bool is_not_equal(const token& lhs, const token& rhs);
  [[nodiscard]] std::pair<token, token> next();
  [[nodiscard]] cuke::value create_table();
  [[nodiscard]] cuke::value make_word_parameter(const token begin);
  void skip_linebreaks();

 private:
  scanner m_defined{""};
  scanner m_feature{""};
  cuke::table::row m_hash_row;
  cuke::value_array m_values;
};

}  // namespace cuke::internal
