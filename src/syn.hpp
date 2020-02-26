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
   /*
    * Pilha auxiliar para o controle das transições entre produções
    */
   std::stack<TipoToken> mPilha;
   /*
    * LL(1) parser table
    */
   std::unordered_map<TipoToken, std::unordered_map<TipoToken, int>> mLL;
   std::size_t mTkCount{};
   /* referencia do objeto Lex (responsável pela análise léxica) */
   Lex& mLex;
   /*
    * Abstract Syntax Tree - Vai sendo construída ao decorrer do parsing
    * Ao final está num estado pronto para o Semântico
    */
   AST mAST;

  public:
   /*
    * Syn inicializado com o Lexico
    */
   Syn(Lex&);
   /*
    * Verifica as regras definidas em mLL com os tokens sendo lidos do Lexico
    * Se nenhum erro for encontrado, então o programa está sintaticamente e
    * lexicamente correto. A partir dai é retornada a AST resultante.
    */
   AST& parse(void);
   inline auto proximoToken(void) {
      ++mTkCount;
      return mLex.getToken();
   }
   inline auto getTkCounter(void) const { return mTkCount; }
   inline auto& getAST(void) const { return mAST; }
};
}  // namespace AnaliseSintatica
