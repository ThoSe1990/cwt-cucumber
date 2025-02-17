#pragma once

#include "identifier.hpp"

namespace cuke::internal
{

class spanish : public identifier
{
  std::pair<token_type, std::size_t> get_token(
      std::string_view str) const override
  {
    for (const auto& element : m_identifiers)
    {
      if (str.starts_with(element.first))
      {
        return {element.second, element.first.length()};
      }
    }
    return {token_type::none, 0};
  }

 private:
  static constexpr std::array<std::pair<std::string_view, token_type>, 19>
      m_identifiers{{{"Característica:", token_type::feature},
                     {"Necesidad del negocio:", token_type::feature},
                     {"Requisito:", token_type::feature},
                     {"Antecedentes:", token_type::background},
                     {"Regla:", token_type::rule},
                     {"Regla de negocio:", token_type::rule},
                     {"Ejemplo:", token_type::scenario},
                     {"Escenario:", token_type::scenario},
                     {"Esquema del escenario:", token_type::scenario_outline},
                     {"Ejemplos:", token_type::examples},
                     {"Dado", token_type::step},
                     {"Dada", token_type::step},
                     {"Dados", token_type::step},
                     {"Dadas", token_type::step},
                     {"Cuando", token_type::step},
                     {"Entonces", token_type::step},
                     {"Y", token_type::step},
                     {"E", token_type::step},
                     {"Pero", token_type::step}}};
};

}  // namespace cuke::internal
