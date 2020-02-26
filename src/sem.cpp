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

#include <stdexcept>

namespace AnaliseSemantica {
using namespace AnaliseSintatica;
typedef Token::TipoToken TipoToken;
Sem::Sem(AST &ast) : mAST(ast) {}

std::size_t Sem::analisaArvore(void) {
   return mAST.DFS([](AST::Node *no) -> bool {
      if (!no) {
         return false;
      }
      bool descer{};
      if (*no == AST::Tipo::BLOCO) {
         /*
          * Analisa nó do tipo BLOCO
          * NT_SE/NT_SENAO/NT_ENQUANTO tem EXP como primeiro nó
          * tal EXP é a condição então deve retornar tipo LOGICO
          */
         if (no->tk != TipoToken::PROGRAMA) {
            /* auto const aux = */
            /*     dynamic_cast<AST::NodeExp*>(no->childs.front().get()); */
            /* aux->dado = avaliaExpressao(aux); */
            /* if (aux->dado != Tipo::LOGICO) { */
            /*    throw std::domain_error("Condição deve retornar tipo
             * LOGICO."); */
            /* } */
            no->avaliar();
         }
         descer = true;
      } else {
         no->avaliar();
         if (no->tk == TipoToken::DECL) {
            no->childs.clear();
         }
      }
      return descer;
      /*
       * Analisa nó ATRIB que tem pelo menos 2 filhos
       * 1) Identificador
       * 2) EXP
       * A analise parte da "resolução" da expressão
       * A função abaixo verificará se o tipo resultante da expressao e o
       * tipo do identificador são compatíveis
       *
       * o valor dessa expressão não pode ser usado para otimizações se:
       *    alguma dos nós folha da expressão for um Identificador
       */

      /*
       * Analisa nó DECL que tem sempre 2 nós filhos
       * declaraVariavel selecionará o bloco imediatamente acima do 'no'
       * e inserirá o Identificador na sua Tabela de Símbolos.
       * 1) Tipo
       * 2) Identificador
       */
      /* declaraVariavel(no); */
   });
}
}  // namespace AnaliseSemantica
