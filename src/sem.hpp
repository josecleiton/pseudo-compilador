/*
 * =====================================================================================
 *
 *       Filename:  sem.hpp
 *
 *    Description:
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

#include "ast.hpp"

namespace AnaliseSemantica {
class Sem {
   typedef AnaliseSintatica::AST AST;
   AST& mAST;

  public:
   Sem(AnaliseSintatica::AST&);
   void analisaArvore(void);
   Dado avaliaExpressao(AST::NodeExp*) const;
   bool declaraVariavel(AST::Node*) const;
   void atribueVariavel(AST::Node*) const;
   double getValorVariavel(AST::NodeExp*) const;
   void aplicaBinop(AST::NodeExp* no1, AST::NodeExp* op,
                    AST::NodeExp* no2) const;

  private:
   Dado avaliaExpressao(
       std::list<std::unique_ptr<AST::Node>>::const_iterator,
       std::list<std::unique_ptr<AST::Node>>::const_iterator) const;
   bool tipoValido(const Tipo&, const Tipo&) const;
};
}  // namespace AnaliseSemantica

