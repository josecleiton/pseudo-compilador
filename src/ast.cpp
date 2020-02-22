/*
 * =====================================================================================
 *
 *       Filename:  ast.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/21/2020 11:26:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "ast.hpp"

typedef Token::TipoToken TipoToken;

namespace AnaliseSemantica {}
namespace AnaliseSintatica {
AST::AST(void)
    : root(std::make_unique<Node>(TipoToken::PROGRAMA, AST::Tipo::BLOCO)) {
   mPilha.push(root.get());
}
AST::Node* AST::inserirNode(const Token& tk, const Tipo& tipo) {
   auto& topo = mPilha.top();
   switch (tipo) {
      case Tipo::BLOCO:
         topo->childs.emplace_back(std::make_unique<NodeBloco>(tk, tipo, topo));
         break;
      case Tipo::EXP:
         topo->childs.emplace_back(std::make_unique<NodeExp>(tk, tipo, topo));
         break;
      default:
         topo->childs.emplace_back(std::make_unique<Node>(tk, tipo, topo));
   }
   mPilha.push(topo->childs.back().get());
   mNodeCounter++;
   return mPilha.top();
}
AST::Node* AST::inserirFolha(const Token& tk, const Tipo& tipo) {
   auto node = inserirNode(tk, tipo);
   mPilha.pop();
   return node;
}
AST::Node::Node(const Token& _tk, const Tipo& _t, Node* _super)
    : tk(_tk), tipo(_t), super(_super) {}
AST::NodeBloco::NodeBloco(const Token& _tk, const Tipo& _t, Node* _super)
    : Node(_tk, _t, _super) {}
AST::NodeExp::NodeExp(const Token& _tk, const Tipo& _t, Node* _super)
    : Node(_tk, _t, _super) {}
}  // namespace AnaliseSintatica

