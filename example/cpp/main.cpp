
#include "cwt/cucumber.hpp"

class cube 
{
public:
  cube()
  {
    std::cout << "creating a new cube - cool\n";
  }
  ~cube()
  {
    std::cout << "destroying my cube - sad\n";
  }
  void set_side(std::size_t side)
  {
    m_side = side;
  }

  std::size_t square_area()
  {
    return m_side * m_side;
  }

  std::size_t volume()
  {
    return m_side * m_side * m_side;
  }

private:
  std::size_t m_side;
};

AFTER("@tag1 or @tag2")
{
  std::cout << "hook after step! " << std::endl;
}

STEP("A cube with {int} side length")
{
  const std::size_t side = CUKE_ARG(1);
  cuke::context<cube>().set_side(side);
}

STEP("The side area is {int}")
{
  const std::size_t area = CUKE_ARG(1);
  cuke::assert_equal(area, cuke::context<cube>().square_area());  
}
STEP("The cube volume is {int}")
{
  const std::size_t volume = CUKE_ARG(1);
  cuke::assert_equal(volume, cuke::context<cube>().volume());  
}

int main(int argc, const char* argv[])
{
  cuke::tests c; 
  c.run(argc, argv);
  return 0;
}