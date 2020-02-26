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

#include <stdexcept>

#define TIPOS_INCOMPATIVEIS std::domain_error("Tipos incompatíveis.")

namespace AnaliseSintatica {
AST::AST(void)
    : root(std::make_unique<NodeBloco>(TipoToken::PROGRAMA, nullptr,
                                       TipoAST::BLOCO)) {
   mPilha.push(root.get());
}
AST::Node *AST::inserirNode(const Token &tk, const TipoAST tipo) {
   if (auto no = inserirFolha(tk, tipo); no) {
      mPilha.push(no);
   }
   return mPilha.top();
}
AST::Node *AST::inserirFolha(const Token &tk, const TipoAST tipo) {
   auto const topo = mPilha.top();
   switch (tipo) {
      case TipoAST::BLOCO:
         topo->childs.emplace_back(std::make_unique<NodeBloco>(tk, topo));
         break;
      case TipoAST::EXP:
         /*
          * Checando se o topo é uma EXP, se for então não precisa adicionar
          * novamente ao topo Cenários onde isso poderia acontecer:
          * exp -> sinal exp | ( exp )
          *
          * Nesses casos o sintatico manda um NodeExp mesmo já
          * estando dentro da exp, aqui tratamos isso, mantendo a expressão
          * linear para ser aplicável o algoritmo Shunting Yard
          */
         if (*topo != TipoAST::EXP) {
            topo->childs.emplace_back(std::make_unique<NodeExp>(tk, topo));
         } else {
            return nullptr;
         }
         break;
      case TipoAST::EXPOP: {
         auto expOp = std::make_unique<NodeExpOp>(tk, topo);
         if (*topo == TipoAST::EXP) {
            static_cast<NodeExp *>(topo)->insereOp(expOp.get());
         }
         topo->childs.emplace_back(std::move(expOp));
      } break;
      case TipoAST::ATRIB:
         topo->childs.emplace_back(std::make_unique<NodeAtrib>(tk, topo));
         break;
      case TipoAST::DECL:
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

AST::Node::Node(const Token &_tk, Node *_super, const TipoAST _t)
    : tk(_tk), tipo(_t), super(_super) {}
AST::NodeBloco::NodeBloco(const Token &_tk, Node *_super, const TipoAST _t)
    : Node(_tk, _super, _t) {}
AST::NodeExp::NodeExp(const Token &_tk, Node *_super, const TipoAST _t)
    : Node(_tk, _super, _t) {}
AST::NodeExpOp::NodeExpOp(const Token &_tk, Node *_super, const TipoAST _t)
    : Node(_tk, _super, _t) {}
AST::NodeAtrib::NodeAtrib(const Token &_tk, Node *_super, const TipoAST _t)
    : Node(_tk, _super, _t) {}
AST::NodeDecl::NodeDecl(const Token &_tk, Node *_super, const TipoAST _t)
    : Node(_tk, _super, _t) {}

AST::NodeBloco *AST::Node::getBlocoAcima(void) const {
   return getBlocoAcima(this);
}
AST::NodeBloco *AST::Node::getBlocoAcima(const AST::Node *const atual) const {
   AST::Node *result = atual->super;
   while (result and *result != TipoAST::BLOCO) {
      result = result->super;
   }
   return result and *result == TipoAST::BLOCO
              ? static_cast<AST::NodeBloco *>(result)
              : nullptr;
}

namespace Semantic = AnaliseSemantica;
typedef Semantic::Dado Dado;

Dado *AST::Node::getDadoVar(const std::string &lexema) const {
   const AST::Node *no = this;
   AST::NodeBloco *atual{}, *anterior{};
   Dado *result{};
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


void AST::NodeBloco::avaliar(void) {
   auto const aux = childs.front().get();
   const auto exp = static_cast<AST::NodeExp *>(aux);
   exp->avaliar();
   if (!Semantic::tipoSaoCompativeis(exp->resultadoExp, TipoDado::LOGICO)) {
      throw TIPOS_INCOMPATIVEIS;
   }
}

void AST::NodeDecl::avaliar(void) {
   auto const bloco = getBlocoAcima();
   auto itList = childs.cbegin();
   const auto tipo = Semantic::lexemaTipo((*itList)->getLexema());
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
   if (!Semantic::tipoSaoCompativeis(exp->resultadoExp, TipoDado::LOGICO)) {
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
         if (mPilha.size() > 1) {  // evitando retirar a raiz se a expressão
            mPilha.pop();          // começar com abre parenteses
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
