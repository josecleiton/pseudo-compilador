/*
 * =====================================================================================
 *
 *       Filename:  st.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/26/2020 11:32:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "stable.hpp"

#include <cmath>

#include "constexpr.hpp"
#include "enum/tipo_dado.hpp"

namespace AnaliseSemantica {
TipoDado lexemaTipo(const std::string_view lexema) {
   if (lexema == "INTEIRO") {
      return TipoDado::INTEIRO;
   } else if (lexema == "QUEBRADO") {
      return TipoDado::QUEBRADO;
   } else {
      return TipoDado::LOGICO;
   }
}

std::string tipoLexema(const TipoDado t) {
   switch (t) {
      case TipoDado::INTEIRO:
         return "INTEIRO";
      case TipoDado::LOGICO:
         return "LOGICO";
      case TipoDado::QUEBRADO:
         return "QUEBRADO";
      default:
         return "NULO";
   }
}

Dado::Dado(const TipoDado t, const double v) : tipo(t), valor(v) {}
Dado::Dado(const Dado &d) : tipo(d.tipo), valor(d.valor) {
   if (tipo == TipoDado::NULO) {
      preencheTipo();
   }
}
Dado::Dado(const double v) : valor(v) { preencheTipo(); }
Dado::Dado(const Dado &d1, const Dado &d2) {
   if (d1 == TipoDado::QUEBRADO) {
      tipo = d1;
   } else if (d1 == TipoDado::INTEIRO) {
      if (d2 == TipoDado::QUEBRADO) {
         tipo = d2;
      } else {
         tipo = d1;
      }
   } else {
      tipo = d2;
   }
}

void Dado::preencheTipo(void) {
   if (valor == 1 or valor == 0.0f) {
      tipo = TipoDado::LOGICO;
   } else if (valor == ceilf(valor)) {
      tipo = TipoDado::INTEIRO;
   } else {
      tipo = TipoDado::QUEBRADO;
   }
}

Dado &Dado::operator=(const Dado &other) {
   if (this != &other) {
      if (bool valido = tipoSaoCompativeis(tipo, other); !valido) {
         throw std::domain_error("Tipos incompatíveis");
      }
      tipo = other.tipo;
      valor = other.valor;
   }
   return *this;
}

}  // namespace AnaliseSemantica
