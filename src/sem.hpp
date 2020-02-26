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
   // std::size_t mNodeExpCount{};

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
   // inline auto& getNodeExpCount(void) const { return mNodeExpCount; }
   inline auto& getAST(void) const { return mAST; }

//   private:
   /*
    * Declara variável no escopo acima do Node*
    */
   // bool declaraVariavel(AST::Node*) const;
   /*
    * Avalia se o tipo da variavael e o tipo do resultado da expressão são
    * compatíveis
    */
   // void atribueVariavel(AST::NodeAtrib*);
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
   // Dado getValorVariavel(AST::NodeExpOp*);
   /*
    * Busca variável nos escopos acima de Node*
    * Se encontrar então
    *   atribue
    *   se os tipos forem diferentes então
    *     erro
    * Senao
    *   erro
    */
   // Dado* getVariavel(const AST::Node*, const std::string&);
   /*
    * Resolve a expressão retornando Dado
    */
   // Dado avaliaExpressao(AST::NodeExp*);
   /*
    * Caminho na árvore de expressão em Pos-Ordem
    *              *
    *             / \
    *            3   5
    *  Resultado -> { tipo: INTEIRO, valor: 15 }
    */
   // Dado avaliaExpressao(AST::NodeExpOp*);
   /*
    * Aplica os operadores *, /, +, -, &&, || nos parametros Dado
    */
   // Dado aplicaBinop(const Dado&, const char op, const Dado&);
   /*
    * Aplica os operadores +, !, - no parâmetro
    */
   // Dado aplicaUnop(const char op, const Dado&);
};
}  // namespace AnaliseSemantica

