#include <string_view>

#include "value.hpp"
#include "scanner.hpp"
#include "chunk.hpp"

namespace cwt::details
{

class step_finder
{
 public:
  step_finder(std::string_view implemented, std::string_view feature);

  [[nodiscard]] bool step_matches();

  template <typename Callback>
  void for_each_value(Callback&& cb)
  {
    for (const value& v : m_values)
    {
      cb(v);
    }
  }

 private:
  void make_value(const token& t, bool negative);
  [[nodiscard]] bool parameter_matches_value(token_type parameter,
                                             token_type value);
  [[nodiscard]] bool is_parameter(token_type parameter);
  [[nodiscard]] bool is_at_end(const token& t);
  [[nodiscard]] bool is_not_equal(const token& lhs, const token& rhs);
  [[nodiscard]] std::pair<token, token> next();

 private:
  scanner m_implemented;
  scanner m_feature;
  value_array m_values;
};

}  // namespace cwt::details
