#pragma once 

#include <string>
#include <memory>
#include <vector> 

namespace cwt::details
{

class chunk; 
class value;
using value_array = std::vector<value>;

struct function
{
  const std::size_t arity;
  std::shared_ptr<chunk> chunk_data;
  const std::string name;
};

using cuke_step = void (*)(const value_array&);

struct native
{
  cuke_step func;
};

}  // namespace cwt::details
