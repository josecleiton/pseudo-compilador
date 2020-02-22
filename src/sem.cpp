/*
 * =====================================================================================
 *
 *       Filename:  sem.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/22/2020 09:32:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "sem.hpp"

#include <iostream>

namespace AnaliseSemantica {
Sem::Sem(const AnaliseSintatica::AST& ast) : mAST(ast) {
   std::clog << "[SEMANTICO] - AST com " << mAST.size() << " nós.\n";
}
}  // namespace AnaliseSemantica

