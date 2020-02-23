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
   std::size_t analisaArvore(void);

  private:
   Dado avaliaExpressao(AST::NodeExp*) const;
   bool declaraVariavel(AST::Node*) const;
   void atribueVariavel(AST::Node*) const;
   double getValorVariavel(AST::NodeExp*) const;
   Dado* getVariavel(AST::Node*, const std::string&) const;
   Dado avaliaExpressao(
       std::list<std::unique_ptr<AST::Node>>::const_iterator,
       std::list<std::unique_ptr<AST::Node>>::const_iterator) const;
   void resolveOp(std::stack<AST::NodeExp*>& nums,
                  std::stack<AST::NodeExp*>& ops) const;
   void aplicaBinop(AST::NodeExp* no1, AST::NodeExp* op,
                    AST::NodeExp* no2) const;
   void aplicaUnop(AST::NodeExp* no1, AST::NodeExp* op) const;
};
}  // namespace AnaliseSemantica

