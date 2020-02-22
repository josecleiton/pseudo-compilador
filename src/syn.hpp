/*
 * =====================================================================================
 *
 *       Filename:  syn.hpp
 *
 *    Description: Parser
 *
 *        Version:  1.0
 *        Created:  02/18/2020 04:06:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia
 *   Organization: UNEB
 *
 * =====================================================================================
 */

#pragma once

#include <stack>
#include <unordered_map>

#include "ast.hpp"
#include "lex.hpp"
namespace AnaliseSintatica {
typedef Token::TipoToken TipoToken;

class Syn {
   std::stack<TipoToken> mPilha;
   // LL(1) table
   std::unordered_map<TipoToken, std::unordered_map<TipoToken, int>> mLL;
   unsigned mTkCounter{};
   // referencia do objeto Lex (responsável pela análise léxica)
   Lex& mLex;
   AST mAST;

  public:
   Syn(Lex&);
   Token parse(void);
   inline unsigned getTkCounter(void) const { return mTkCounter; }
};
}  // namespace AnaliseSintatica
