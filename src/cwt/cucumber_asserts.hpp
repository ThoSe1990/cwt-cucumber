

extern "C" {
  #include "cwt/cucumber.h"
}


namespace cuke 
{
  template<typename T>
  inline void assert_equal(T t1, T t2)
  {
    cuke_assert(t1 == t2);
  }
} // namespace cuke 
