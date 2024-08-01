
#include "test_results.hpp"


namespace cuke::results 
{

[[nodiscard]] test_result& test_results()
{
  static test_result instance;
  return instance;
}

} // namespace cuke::internal::results 
