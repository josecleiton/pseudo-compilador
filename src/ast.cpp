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
   auto no = inserirFolha(tk, tipo);
   if (auto exp = dynamic_cast<AST::NodeExp*>(mPilha.top()); !exp) {
      mPilha.push(no);
   }
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
      case Tipo::EXPOP: {
         auto expOp = std::make_unique<NodeExpOp>(tk, topo);
         if (auto exp = dynamic_cast<NodeExp*>(topo); exp) {
            exp->insereOp(expOp.get());
         }
         topo->childs.emplace_back(std::move(expOp));
      } break;
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
AST::NodeExpOp::NodeExpOp(const Token& _tk, Node* _super, const Tipo _t)
    : Node(_tk, _super, _t) {}
AST::NodeAtrib::NodeAtrib(const Token& _tk, Node* _super, const Tipo _t)
    : Node(_tk, _super, _t) {}

typedef AST::NodeExpOp::Direcao Direcao;

Direcao AST::NodeExpOp::adicionaChild(NodeExpOp* no) {
   childs[mDirecao] = no;
   if (mDirecao == ESQUERDA) {
      mDirecao = DIREITA;
   }
   return mDirecao;
}

constexpr unsigned precedencia(const char op) {
   switch (op) {
      case '+':
      case '-':
      case '!':
      case '|':
         return 1;
      case '*':
      case '/':
      case '&':
         return 2;
      default:
         return 0;
   }
}
void AST::NodeExp::insereOp(NodeExpOp* const no) {
   if (mPilha.empty()) {
      mPilha.push(no);
      return;
   }
   auto topo = mPilha.top();
   switch (no->tk) {
      case TipoToken::FECHAPRNT:
         while (mPilha.size() and mPilha.top()->tk != TipoToken::ABREPRNT) {
            mPilha.pop();
         }
         if (mPilha.size()) {
            mPilha.pop();
         }
         break;
      case TipoToken::ABREPRNT:
         mPilha.push(no);
      case TipoToken::ID:
      case TipoToken::VALOR:
         topo->adicionaChild(no);
         break;
      case TipoToken::SINAL:
      case TipoToken::BINOP:
      case TipoToken::NEG:
         if (topo->getOp() != '(') {
            if (topo->size() == 2) {  // binario
               if (precedencia(no->getOp()) > precedencia(topo->getOp())) {
                  no->adicionaChild(topo->getDireita());
                  /* opPtr->childs.push_back(std::move(atual->childs.back()));
                   */
                  /* atual->childs.back() = std::move(op); */
                  topo->getDireita() = no;
                  mPilha.push(no);
                  return;
               }
            }
            mPilha.pop();
            no->adicionaChild(topo);
            if (mPilha.size()) {
               mPilha.top()->getDireita() = no;
            }
            /* opPtr->childs.push_back(std::move(expRoot)); */
            mPilha.push(no);
         } else {
            topo->tk.lexema = std::move(no->tk.lexema);
         }
         break;
      default:
         break;
   }
}
void AST::NodeExp::fimExp(void) {
   while (mPilha.size() > 1) {
      mPilha.pop();
   }
   expRoot = mPilha.top();
}
}  // namespace AnaliseSintatica

