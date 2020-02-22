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

namespace AnaliseSemantica {
Sem::Sem(const AnaliseSintatica::AST& ast) : mAST(ast) {}
}  // namespace AnaliseSemantica

