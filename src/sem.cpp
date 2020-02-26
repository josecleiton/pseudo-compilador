/*
 * =====================================================================================
 *
 *       Filename:  sem.cpp
 *
 *    Description: Analise semantica (documentação no sem.hpp)
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
using namespace AnaliseSintatica;
Sem::Sem(AST &ast) : mAST(ast) {}

std::size_t Sem::analisaArvore(void) {
   return mAST.DFS([](AST::Node *no) -> bool {
      if (!no) {
         return false;
      }
      bool descer{};
      if (*no == TipoAST::BLOCO) {
         if (no->tk != TipoToken::PROGRAMA) {
            no->avaliar();
         }
         descer = true;
      } else {
         no->avaliar();
         /* Após a avaliação do DECL, toda as informações vão para a
          * tabela de simbolos do escopo mais próximo
          */
         if (no->tk == TipoToken::DECL) {
            no->childs.clear();
         }
      }
      return descer;
   });
}
}  // namespace AnaliseSemantica
