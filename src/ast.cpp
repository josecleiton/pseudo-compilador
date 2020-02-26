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
#include <stdexcept>

#define TIPOS_INCOMPATIVEIS std::domain_error("Tipos incompatíveis.")

typedef Token::TipoToken TipoToken;

namespace AnaliseSemantica {
Tipo lexemaTipo(const std::string &lexema) {
   if (lexema == "INTEIRO") {
      return Tipo::INTEIRO;
   } else if (lexema == "QUEBRADO") {
      return Tipo::QUEBRADO;
   } else {
      return Tipo::LOGICO;
   }
}

Dado::Dado(const Tipo t, const double v) : tipo(t), valor(v) {}
Dado::Dado(const Dado &d) : tipo(d.tipo), valor(d.valor) {
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

constexpr bool Dado::ehCompativelCom(const Tipo t1, const Tipo t2) {
   switch (t1) {
      case Tipo::INTEIRO:
         return t2 != Tipo::QUEBRADO;
      case Tipo::LOGICO:
         return t2 == Tipo::LOGICO;
         break;
      default:
         break;
   }
   return true;
}

Dado &Dado::operator=(const Dado &other) {
   if (this != &other) {
      if (bool valido = Dado::ehCompativelCom(tipo, other); !valido) {
         throw std::domain_error("Tipos incompatíveis.");
      }
      tipo = other.tipo;
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
AST::Node *AST::inserirNode(const Token &tk, const Tipo tipo) {
   if (auto no = inserirFolha(tk, tipo); no) {
      mPilha.push(no);
   }
   return mPilha.top();
}
AST::Node *AST::inserirFolha(const Token &tk, const Tipo tipo) {
   auto const topo = mPilha.top();
   switch (tipo) {
      case Tipo::BLOCO:
         topo->childs.emplace_back(std::make_unique<NodeBloco>(tk, topo));
         break;
      case Tipo::EXP:
         /*
          * Checando se o topo é uma EXP, se for então não precisa adicionar
          * novamente ao topo Cenários onde isso poderia acontecer:
          * exp -> sinal exp | ( exp )
          *
          * Nesses casos o sintatico manda um NodeExp mesmo já
          * estando dentro da exp, aqui tratamos isso, mantendo a expressão
          * linear para ser aplicável o algoritmo Shunting Yard
          */
         if (*topo != Tipo::EXP) {
            topo->childs.emplace_back(std::make_unique<NodeExp>(tk, topo));
         } else {
            return nullptr;
         }
         break;
      case Tipo::EXPOP: {
         auto expOp = std::make_unique<NodeExpOp>(tk, topo);
         if (*topo == Tipo::EXP) {
            static_cast<NodeExp *>(topo)->insereOp(expOp.get());
         }
         topo->childs.emplace_back(std::move(expOp));
      } break;
      case Tipo::ATRIB:
         topo->childs.emplace_back(std::make_unique<NodeAtrib>(tk, topo));
         break;
      case Tipo::DECL:
         topo->childs.emplace_back(std::make_unique<NodeDecl>(tk, topo));
         break;
      default:
         topo->childs.emplace_back(std::make_unique<Node>(tk, topo, tipo));
   }
   ++mNodeCount;
   return topo->childs.back().get();
}

std::size_t AST::DFS(const std::function<bool(AST::Node *)> &func) {
   return DFS(root.get(), func);
}
std::size_t AST::DFS(AST::Node *atual,
                     const std::function<bool(AST::Node *)> &func) {
   const bool descer = func(atual);
   std::size_t count{1};
   if (descer) {
      for (auto &no : atual->childs) {
         count += DFS(no.get(), func);
      }
   }
   return count;
}

AST::Node::Node(const Token &_tk, Node *_super, const Tipo _t)
    : tk(_tk), tipo(_t), super(_super) {}
AST::NodeBloco::NodeBloco(const Token &_tk, Node *_super, const Tipo _t)
    : Node(_tk, _super, _t) {}
AST::NodeExp::NodeExp(const Token &_tk, Node *_super, const Tipo _t)
    : Node(_tk, _super, _t) {}
AST::NodeExpOp::NodeExpOp(const Token &_tk, Node *_super, const Tipo _t)
    : Node(_tk, _super, _t) {}
AST::NodeAtrib::NodeAtrib(const Token &_tk, Node *_super, const Tipo _t)
    : Node(_tk, _super, _t) {}
AST::NodeDecl::NodeDecl(const Token &_tk, Node *_super, const Tipo _t)
    : Node(_tk, _super, _t) {}

AST::NodeBloco *AST::Node::getBlocoAcima(void) const {
   return getBlocoAcima(this);
}
AST::NodeBloco *AST::Node::getBlocoAcima(const AST::Node *const atual) const {
   AST::Node *result = atual->super;
   while (result and *result != AST::Tipo::BLOCO) {
      result = result->super;
   }
   return result and *result == AST::Tipo::BLOCO
              ? static_cast<AST::NodeBloco *>(result)
              : nullptr;
}

AnaliseSemantica::Dado *AST::Node::getDadoVar(const std::string &lexema) const {
   const AST::Node *no = this;
   AST::NodeBloco *atual{}, *anterior{};
   AnaliseSemantica::Dado *result{};
   while ((atual = getBlocoAcima(no))) {
      no = anterior = atual;
      if ((result = anterior->st.getDado(lexema))) {
         return result;
      }
   }
   if (anterior and (result = anterior->st.getDado(lexema))) {
      return result;
   }
   throw std::out_of_range(
       "Todas as variaveis devem ser declaradas antes do uso.");
}

typedef AnaliseSemantica::Dado Dado;

void AST::NodeBloco::avaliar(void) {
   auto const aux = childs.front().get();
   const auto exp = static_cast<AST::NodeExp *>(aux);
   exp->avaliar();
   if (!Dado::ehCompativelCom(exp->resultadoExp,
                              AnaliseSemantica::Tipo::LOGICO)) {
      throw TIPOS_INCOMPATIVEIS;
   }
}

void AST::NodeDecl::avaliar(void) {
   auto const bloco = getBlocoAcima();
   auto itList = childs.cbegin();
   const auto tipo = AnaliseSemantica::lexemaTipo((*itList)->getLexema());
   ++itList;
   const auto &varNome = (*itList)->getLexema();
   bloco->st.inserirVariavel(varNome, tipo);
}

void AST::NodeAtrib::avaliar(void) {
   auto itList = childs.cbegin();
   const auto &nomeVar = (*itList)->getLexema();
   ++itList;
   var = getDadoVar(nomeVar);
   auto const exp = static_cast<AST::NodeExp *>(itList->get());
   exp->avaliar();
   if (!Dado::ehCompativelCom(*var, exp->resultadoExp)) {
      throw TIPOS_INCOMPATIVEIS;
   }
}

void AST::NodeExp::avaliar(void) {
   resultadoExp = raizExp->avaliarExp(mNodeCount);
}
void AST::NodeExpOp::avaliar(void) {
   std::size_t count{};
   avaliarExp(this, count);
}

Dado AST::NodeExpOp::avaliarExp(std::size_t &count) const {
#ifdef DEBUG
   auto result = avaliarExp(this, count);
   std::clog << "[DEBUG - AST] Foram avaliados " << count
             << " nós na árvore de expressão." << std::endl;
   return result;
#else
   return avaliarExp(this, count);
#endif
}

Dado AST::NodeExpOp::avaliarExp(const AST::NodeExpOp *const atual,
                                std::size_t &count) const {
   ++count;
   if (auto noEsq = atual->getEsquerda(); noEsq) {
      const auto esq = avaliarExp(noEsq, count);
      if (auto noDir = atual->getDireita(); noDir) {
         return atual->aplicaBinop(esq, avaliarExp(noDir, count));
      }
      return atual->aplicaUnop(esq);
   }
   return atual->getValorVariavel();
}

Dado AST::NodeExpOp::getValorVariavel() const {
   if (tk == TipoToken::VALOR) {
      return std::stof(getLexema());
   }
   return *getDadoVar(getLexema());
}

Dado AST::NodeExpOp::aplicaBinop(const Dado &num1, const Dado &num2) const {
   switch (getOp()) {
      case '*':
         return Dado(num1.valor * num2.valor);
      case '/':
         return Dado(num1.valor / num2.valor);
      case '-':
         return Dado(num1.valor - num2.valor);
      case '+':
         return Dado(num1.valor + num2.valor);
      case '&':
         return Dado(num1.valor && num2.valor);
      case '|':
         return Dado(num1.valor || num2.valor);
      default:
         return {};
   }
}

Dado AST::NodeExpOp::aplicaUnop(const Dado &num) const {
   switch (getOp()) {
      case '-':
         return Dado(-num.valor);
      case '+':
         return Dado(num.valor);
      case '!':
         return Dado(!num.valor);
      default:
         return {};
   }
}

typedef AST::NodeExpOp::Direcao Direcao;

Direcao AST::NodeExpOp::adicionaChild(NodeExpOp *no) {
   childs[mDirecao] = no;
   return mDirecao = (mDirecao == ESQUERDA) ? DIREITA : mDirecao;
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
void AST::NodeExp::insereOp(NodeExpOp *const no) {
   if (mPilha.empty()) {
      mPilha.push(no);
      return;
   }
   auto topo = mPilha.top();
   switch (no->tk) {
      case TipoToken::FECHAPRNT:
         while (mPilha.top()->tk != TipoToken::ABREPRNT) {
            mPilha.pop();
         }
         if (mPilha.size() > 1) { // evitando retirar a raiz se a expressão 
            mPilha.pop();        // começar com abre parenteses
         }
         break;
      case TipoToken::ABREPRNT:
         mPilha.push(no);
         [[fallthrough]];
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

AST::NodeExpOp *AST::NodeExp::fimExp(void) {
   while (mPilha.size() > 1) {
      mPilha.pop();
   }
   return raizExp = mPilha.top();
}
}  // namespace AnaliseSintatica
