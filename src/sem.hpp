/*
 * =====================================================================================
 *
 *       Filename:  sem.hpp
 *
 *    Description: Análise Semantica - Utiliza a AST gerada pelo Sintatico para
 *                 checar escopo e tipo das variáveis (e expressões)
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

#include <cstddef>

namespace AnaliseSintatica {
class AST;
}

namespace AnaliseSemantica {
class Sem {
   AnaliseSintatica::AST& mAST;

  public:
   /*
    * Espera a Abstract Syntax Tree gerada pelo Sintatico
    */
   Sem(AnaliseSintatica::AST&);
   /*
    * Caminha na AST verificando a validade semantica dos nos
    * Faz uso do polimorfismo dos nós, então essa função controlar a qual
    * profundidade deve-se ir
    */
   std::size_t analisaArvore(void);
   auto& getAST(void) const { return mAST; }
};
}  // namespace AnaliseSemantica

