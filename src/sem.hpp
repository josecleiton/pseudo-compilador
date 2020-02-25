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

#include <list>
#include <stack>
#include <string>

#include "ast.hpp"

namespace AnaliseSemantica {
class Sem {
   typedef AnaliseSintatica::AST AST;
   AST& mAST;
   std::size_t mNodeExpCount{};

  public:
   /*
    * Espera a Abstract Syntax Tree gerada pelo Sintatico
    */
   Sem(AnaliseSintatica::AST&);
   inline auto& getNodeExpCount(void) const { return mNodeExpCount; }
   /*
    * Caminha na AST verificando a validade semantica dos nos
    */
   std::size_t analisaArvore(void);

  private:
   /*
    * Declara variável no escopo acima do Node*
    */
   bool declaraVariavel(AST::Node*) const;
   /*
    * Busca variável nos escopos acima de Node*
    * Se encontrar então
    *   atribue
    *   se os tipos forem diferentes então
    *     erro
    * Senao
    *   erro
    */
   void atribueVariavel(AST::NodeAtrib*);
   /*
    * Se for VALOR então
    *   retorna VALOR
    * Senão // é um identificador (variavel)
    *   Busca variável nos escopos acima de Node*
    *   Se encontrar então
    *    retorna valor da variável
    *   Senao
    *     erro
    */
   Dado getValorVariavel(AST::NodeExpOp*);
   Dado* getVariavel(const AST::Node*, const std::string&);
   /*
    * Resolve a expressão retornando Dado
    */
   Dado avaliaExpressao(AST::NodeExp*);
   /*
    * Avalia a expressão infixa guardada por uma lista encadeada
    * A AST mantém a expressão infixa até ser avaliada. Exemplo do comando k = x
    * * 4 : ATRIB
    *              /
    *             ID -> ID -> * -> 4
    *  O primeiro ID é onde a expressão vai ser atribuida (k no nosso exemplo)
    *  Por isso avaliaExpressao necessita de dois iterators, sendo eles:
    *  começo da expressão e final da expressão respectivamente.
    *  o algoritmo itera na lista (veja o cpp) e aplica o algoritmo feito por
    * Dijkstra: https://en.wikipedia.org/wiki/Shunting-yard_algorithm um pouco
    * modificado, visto que o Shunting Yard é para converter uma expressão de
    * infix para posfixa e é utilizado aqui para resolver toda a expressão,
    * retornando o Dado
    */
   Dado avaliaExpressao(AST::NodeExpOp*);
   /*
    * Aplica os operadores *, /, +, -, &&, || no no1 ou no2
    */
   Dado aplicaBinop(const Dado&, const char op, const Dado&);
   /*
    * Aplica os operadores +, !, - no no1
    */
   Dado aplicaUnop(const char op, const Dado&);
};
}  // namespace AnaliseSemantica

