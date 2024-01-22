#include "feature.hpp"

namespace cwt::details
{

feature::feature(cuke_compiler* enclosing) : m_enclosing(enclosing), compiler(*enclosing) {}

void feature::compile()
{
  std::cout << "compiling feature" << std::endl;
  std::cout << m_enclosing->location() << std::endl;
}

}  // namespace cwt::details