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

#include <cmath>

typedef Token::TipoToken TipoToken;

namespace AnaliseSemantica {
Tipo lexemaTipo(const std::string& lexema) {
   if (lexema == "INTEIRO") {
      return Tipo::INTEIRO;
   } else if (lexema == "QUEBRADO") {
      return Tipo::QUEBRADO;
   } else {
      return Tipo::LOGICO;
   }
}

Dado::Dado(const Tipo& t, const double v) : tipo(t), valor(v) {}
Dado::Dado(const Dado& d) : tipo(d.tipo), valor(d.valor) { preencheTipo(); }

void Dado::preencheTipo(void) {
   if (valor == 1 or valor == 0.0) {
      tipo = Tipo::LOGICO;
   } else if (valor == ceilf(valor)) {
      tipo = Tipo::INTEIRO;
   } else {
      tipo = Tipo::QUEBRADO;
   }
}

}  // namespace AnaliseSemantica
namespace AnaliseSintatica {
AST::AST(void)
    : root(std::make_unique<NodeBloco>(TipoToken::PROGRAMA, AST::Tipo::BLOCO)) {
   mPilha.push(root.get());
}
AST::Node* AST::inserirNode(const Token& tk, const Tipo& tipo) {
   auto& topo = mPilha.top();
   switch (tipo) {
      case Tipo::BLOCO:
         topo->childs.push_back(std::make_unique<NodeBloco>(tk, tipo, topo));
         break;
      case Tipo::EXP:
         topo->childs.push_back(std::make_unique<NodeExp>(tk, tipo, topo));
         break;
      default:
         topo->childs.push_back(std::make_unique<Node>(tk, tipo, topo));
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

std::size_t AST::DFS(const std::function<bool(AST::Node*)>& func) {
   return DFS(root.get(), func);
}
std::size_t AST::DFS(AST::Node* atual,
                     const std::function<bool(AST::Node*)>& func) {
   const auto descer = func(atual);
   std::size_t ac{1};
   if (descer) {
      for (auto& no : atual->childs) {
         ac += DFS(no.get(), func);
      }
   }
   return ac;
}

AST::Node::Node(const Token& _tk, const Tipo& _t, Node* _super)
    : tk(_tk), tipo(_t), super(_super) {}
AST::NodeBloco::NodeBloco(const Token& _tk, const Tipo& _t, Node* _super)
    : Node(_tk, _t, _super) {}
AST::NodeExp::NodeExp(const Token& _tk, const Tipo& _t, Node* _super)
    : Node(_tk, _t, _super) {}
}  // namespace AnaliseSintatica

