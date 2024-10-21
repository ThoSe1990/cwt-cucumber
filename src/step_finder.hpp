#pragma once

#include <string_view>
#include <algorithm>

#include "value.hpp"
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
  std::string m_feature_string;
  cuke::table::row m_hash_row;
  cuke::value_array m_values;
};

}  // namespace cuke::internal
