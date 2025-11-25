#pragma once

#include <array>

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
      m_identifiers{
          {{std::string_view("Característica:"), token_type::feature},
           {std::string_view("Necesidad del negocio:"), token_type::feature},
           {std::string_view("Requisito:"), token_type::feature},
           {std::string_view("Antecedentes:"), token_type::background},
           {std::string_view("Regla:"), token_type::rule},
           {std::string_view("Regla de negocio:"), token_type::rule},
           {std::string_view("Ejemplo:"), token_type::scenario},
           {std::string_view("Escenario:"), token_type::scenario},
           {std::string_view("Esquema del escenario:"),
            token_type::scenario_outline},
           {std::string_view("Ejemplos:"), token_type::examples},
           {std::string_view("Dado"), token_type::step},
           {std::string_view("Dada"), token_type::step},
           {std::string_view("Dados"), token_type::step},
           {std::string_view("Dadas"), token_type::step},
           {std::string_view("Cuando"), token_type::step},
           {std::string_view("Entonces"), token_type::step},
           {std::string_view("Y"), token_type::step},
           {std::string_view("E"), token_type::step},
           {std::string_view("Pero"), token_type::step}}};
};

}  // namespace cuke::internal
