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

  public:
   /*
    * Espera a Abstract Syntax Tree gerada pelo Sintatico
    */
   Sem(AnaliseSintatica::AST&);
   /*
    * Caminha na AST verificando a validade semantica dos nos
    */
   std::size_t analisaArvore(void);

  private:
   /*
    * Resolve a expressão retornando Dado
    */
   Dado avaliaExpressao(AST::NodeExp*) const;
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
   void atribueVariavel(AST::Node*) const;
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
   double getValorVariavel(AST::NodeExp*) const;
   Dado* getVariavel(AST::Node*, const std::string&) const;
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
   Dado avaliaExpressao(
       std::list<std::unique_ptr<AST::Node>>::const_iterator,
       std::list<std::unique_ptr<AST::Node>>::const_iterator) const;
   /*
    * Função auxiliar de avaliaExpressao para escolher o(s) número(s) da pilha
    * de números e o operador da pilha de operador após isso escolhe aplicar a
    * função aplicaBinop ou aplicaUnop dependendo da pilha de números
    */
   void resolveOp(std::stack<AST::NodeExp*>& nums,
                  std::stack<AST::NodeExp*>& ops) const;
   /*
    * Aplica os operadores *, /, +, -, &&, || no no1 ou no2
    */
   void aplicaBinop(AST::NodeExp* no1, AST::NodeExp* op,
                    AST::NodeExp* no2) const;
   /*
    * Aplica os operadores +, !, - no no1
    */
   void aplicaUnop(AST::NodeExp* no1, AST::NodeExp* op) const;
};
}  // namespace AnaliseSemantica

