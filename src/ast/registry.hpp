#pragma once

#include "../value.hpp"

namespace cuke
{

class registry
{
 public:
  static void push_step(const cwt::details::step& s);

 private:
  [[nodiscard]] static std::vector<cwt::details::step>& steps();

 private:
};

}  // namespace cuke
