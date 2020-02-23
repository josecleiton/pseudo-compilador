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

#ifndef DEBUG
#define DEBUG 1
#endif

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
   AST::Node* mBloco;

  public:
   Syn(Lex&);
   AST& parse(void);
   inline auto getTkCounter(void) const { return mTkCounter; }
   inline const auto& getAST(void) const { return mAST; }
};
}  // namespace AnaliseSintatica
