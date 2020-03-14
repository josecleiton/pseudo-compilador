/*
 * =====================================================================================
 *
 *       Filename:  constexpr.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/14/2020 01:21:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#pragma once
#include "enum/tipo_dado.hpp"

namespace AnaliseSemantica {
/*
 * tipoSaoCompativeis - Compara se um tipo é atribuível a outro
 */
constexpr bool tipoSaoCompativeis(const TipoDado t1, const TipoDado t2) {
   switch (t1) {
      case TipoDado::INTEIRO:
         return t2 != TipoDado::QUEBRADO;
      case TipoDado::LOGICO:
         return t2 == TipoDado::LOGICO;
      default:
         return true;
   }
}
}  // namespace AnaliseSemantica
