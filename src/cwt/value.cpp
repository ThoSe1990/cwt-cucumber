#pragma once 

#include "value.hpp"
#include "object.hpp"
#include "chunk.hpp"

namespace cwt::details
{
  
  template <>
  struct value::value_model<function> : public value::value_concept
  {
      value_model() = default;
      value_model(const function& func) : m_value{
        function{
          func.arity,
          std::make_unique<chunk>(func.chunk),
          func.name
        }
      } {}

      function m_value;
  };


} // namespace cwt::details