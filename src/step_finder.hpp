#include <string_view>

#include "value.hpp"
#include "scanner.hpp"
// #include "chunk.hpp"

namespace cuke::internal
{

class step_finder
{
 public:
  step_finder(std::string_view defined, std::string_view feature);
  step_finder(std::string_view feature);
  
  void reset_with_next_step(std::string_view defined) noexcept;
  [[nodiscard]] bool step_matches();
  [[nodiscard]] const cuke::value_array& values() const noexcept; 

 private:
  [[nodiscard]] bool parameter_matches_value(token_type parameter,
                                             token_type value);
  [[nodiscard]] bool is_parameter(token_type parameter);
  [[nodiscard]] bool is_at_end(const token& t);
  [[nodiscard]] bool is_not_equal(const token& lhs, const token& rhs);
  [[nodiscard]] std::pair<token, token> next();
  [[nodiscard]] cuke::value create_table();
  void skip_linebreaks();
 private:
  scanner m_defined{""};
  scanner m_feature{""};
  cuke::value_array m_values;
};

}  // namespace cuke::internal
