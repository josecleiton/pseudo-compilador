/*
 * =====================================================================================
 *
 *       Filename:  sem.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/22/2020 09:27:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#pragma once

#include "ast.hpp"

namespace AnaliseSemantica {
class Sem {
   const AnaliseSintatica::AST& mAST;

  public:
   Sem(const AnaliseSintatica::AST&);
};
}  // namespace AnaliseSemantica

