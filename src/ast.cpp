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

Dado::Dado(const Tipo t, const double v) : tipo(t), valor(v) {}
Dado::Dado(const Dado& d) : tipo(d.tipo), valor(d.valor) {
   if (tipo == Tipo::NULO) {
      preencheTipo();
   }
}
Dado::Dado(const double v) : valor(v) { preencheTipo(); }

void Dado::preencheTipo(void) {
   if (valor == 1 or valor == 0.0f) {
      tipo = Tipo::LOGICO;
   } else if (valor == ceilf(valor)) {
      tipo = Tipo::INTEIRO;
   } else {
      tipo = Tipo::QUEBRADO;
   }
}

Dado& Dado::operator=(const Dado& other) {
   if (this != &other) {
      bool valido = true;
      switch (tipo) {
         case Tipo::INTEIRO:
            valido = other.tipo != Tipo::QUEBRADO;
            break;
         case Tipo::LOGICO:
            valido = other.tipo == Tipo::LOGICO;
            break;
         default:
            break;
      }
      if (valido) {
         tipo = other.tipo;
      } else {
         throw std::domain_error("Tipos incompatíveis.");
      }
      valor = other.valor;
   }
   return *this;
}

}  // namespace AnaliseSemantica
namespace AnaliseSintatica {
AST::AST(void)
    : root(std::make_unique<NodeBloco>(TipoToken::PROGRAMA, nullptr,
                                       AST::Tipo::BLOCO)) {
   mPilha.push(root.get());
}
AST::Node* AST::inserirNode(const Token& tk, const Tipo tipo) {
   mPilha.push(inserirFolha(tk, tipo));
   return mPilha.top();
}
AST::Node* AST::inserirFolha(const Token& tk, const Tipo tipo) {
   auto const topo = mPilha.top();
   switch (tipo) {
      case Tipo::BLOCO:
         topo->childs.emplace_back(std::make_unique<NodeBloco>(tk, topo));
         break;
      case Tipo::EXP:
         topo->childs.emplace_back(std::make_unique<NodeExp>(tk, topo));
         break;
      case Tipo::ATRIB:
         topo->childs.emplace_back(std::make_unique<NodeAtrib>(tk, topo));
         break;
      default:
         topo->childs.emplace_back(std::make_unique<Node>(tk, topo, tipo));
   }
   ++mNodeCount;
   return topo->childs.back().get();
}

std::size_t AST::DFS(const std::function<bool(AST::Node*)>& func) {
   return DFS(root.get(), func);
}
std::size_t AST::DFS(AST::Node* atual,
                     const std::function<bool(AST::Node*)>& func) {
   const bool descer = func(atual);
   std::size_t count{1};
   if (descer) {
      for (auto& no : atual->childs) {
         count += DFS(no.get(), func);
      }
   }
   return count;
}

AST::Node::Node(const Token& _tk, Node* _super, const Tipo _t)
    : tk(_tk), tipo(_t), super(_super) {}
AST::NodeBloco::NodeBloco(const Token& _tk, Node* _super, const Tipo _t)
    : Node(_tk, _super, _t) {}
AST::NodeExp::NodeExp(const Token& _tk, Node* _super, const Tipo _t)
    : Node(_tk, _super, _t) {}
AST::NodeAtrib::NodeAtrib(const Token& _tk, Node* _super, const Tipo _t)
    : Node(_tk, _super, _t) {}
}  // namespace AnaliseSintatica

