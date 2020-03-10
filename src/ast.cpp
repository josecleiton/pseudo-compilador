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

#include "enum/prec.hpp"
#include "erro.hpp"

#define TIPOS_INCOMPATIVEIS \
   AnaliseSemantica::ErroSemantico("Tipos incompatíveis.")

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
          * novamente ao topo
          * Cenários onde isso poderia acontecer:
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
         std::unique_ptr<NodeExpOp> op;
         if (tk == TipoToken::ID) {
            op = std::make_unique<NodeExpID>(tk, topo);
         } else if (tk == TipoToken::VALOR) {
            op = std::make_unique<NodeExpValor>(tk, topo);
         } else {
            op = std::make_unique<NodeExpOp>(tk, topo);
         }
         if (*topo == TipoAST::EXP) {
            /*
             * Coloca op (num ou operador) na árvore de expressão pos-fixa
             */
            static_cast<NodeExp *>(topo)->insereOp(op.get());
         }
         topo->childs.emplace_back(std::move(op));
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

std::size_t AST::DFS(const std::function<bool(Node *)> &func) {
   return DFS(root.get(), func);
}
std::size_t AST::DFS(AST::Node *atual,
                     const std::function<bool(Node *)> &func) {
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
    : Node(_tk, _super, _t), mAbreParentese(getOp() == '(') {}
AST::NodeAtrib::NodeAtrib(const Token &_tk, Node *_super, const TipoAST _t)
    : Node(_tk, _super, _t) {}
AST::NodeDecl::NodeDecl(const Token &_tk, Node *_super, const TipoAST _t)
    : Node(_tk, _super, _t) {}

namespace Semantic = AnaliseSemantica;
typedef Semantic::Dado Dado;

void AST::NodeBloco::avaliar(void) {
   auto const aux = childs.front().get();
   const auto exp = static_cast<NodeExp *>(aux);
   exp->avaliar();
   if (!Semantic::tipoSaoCompativeis(exp->resultadoExp, TipoDado::LOGICO)) {
      throw TIPOS_INCOMPATIVEIS;
   }
}

void AST::NodeDecl::avaliar(void) {
   auto itList = childs.cbegin();
   const auto tipo = Semantic::lexemaTipo((*itList)->getLexema());
   ++itList;
   auto const id = static_cast<NodeExpID *>(itList->get());
   id->avaliarExp();
   id->setTipo(tipo);
}

void AST::NodeAtrib::avaliar(void) {
   auto itList = childs.cbegin();
   const auto var = static_cast<NodeExpID *>(itList->get())->avaliarExp();
   ++itList;
   auto const exp = static_cast<NodeExp *>(itList->get());
   exp->avaliar();
   if (!Semantic::tipoSaoCompativeis(var, exp->resultadoExp)) {
      throw TIPOS_INCOMPATIVEIS;
   }
}

void AST::NodeExp::avaliar(void) { resultadoExp = raizExp->avaliarExp(); }
void AST::NodeExpOp::avaliar(void) { avaliarExp(); }

Dado AST::NodeExpOp::avaliarExp(void) {
   if (auto noEsq = getEsquerda(); noEsq) {
      const auto esq = noEsq->avaliarExp();
      if (auto noDir = getDireita(); noDir) {
         return aplicaBinop(esq, noDir->avaliarExp());
      }
      return aplicaUnop(esq);
   }
   return avaliarExp();
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
         return Dado(num1.valor and num2.valor);
      case '|':
         return Dado(num1.valor or num2.valor);
      default:
         throw std::invalid_argument("Operador binário inválido.");
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
         throw std::invalid_argument("Operador unário inválido.");
   }
}

typedef AST::NodeExpOp::Direcao Direcao;

Direcao AST::NodeExpOp::adicionaChild(NodeExpOp *no) {
   childs[mDirecao] = no;
   return mDirecao = (mDirecao == ESQUERDA) ? DIREITA : mDirecao;
}

constexpr Prec precedencia(const char op) {
   switch (op) {
      case '+':
      case '-':
      case '!':
      case '|':
         return Prec::TERMO_OP;
      case '*':
      case '/':
      case '&':
         return Prec::FATOR_OP;
      default:
         return Prec::NULO;
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
         while (!mPilha.top()->abreParentese()) {
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
         if (!topo->abreParentese()) {
            if (topo->size() == 2) {  // tem os dois operadores
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
            mPilha.push(no);
         } else {
            topo->tk = no->tk;
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
AST::NodeExpID::NodeExpID(const Token &_tk, Node *_super, const TipoAST _t)
    : NodeExpOp(_tk, _super, _t) {}
AnaliseSemantica::Dado AST::NodeExpID::avaliarExp(void) {
   if(mIteratorInicializado) {
      return mSTDado->second;
   }
   if (super->tk == TipoToken::DECL) {
      declaraVar();
   } else {
      getDadoVar();
   }
   mIteratorInicializado = true;
   return avaliarExp();
}
void AST::NodeExpID::declaraVar(void) {
   auto const bloco = getBlocoAcima(this);
   mSTDado = bloco->st.inserirVariavel(getLexema());
}
AST::NodeBloco *AST::NodeExpID::getBlocoAcima(
    const AST::Node *const atual) const {
   AST::Node *result = atual->super;
   while (result and *result != TipoAST::BLOCO) {
      result = result->super;
   }
   return static_cast<AST::NodeBloco *>(result);
}
void AST::NodeExpID::getDadoVar(void) {
   const AST::Node *no = this;
   AST::NodeBloco *atual{}, *anterior{};
   std::pair<std::unordered_map<std::string, Dado>::iterator, bool> result;
   while ((atual = getBlocoAcima(no))) {
      no = anterior = atual;
      if ((result = anterior->st.getDado(getLexema())).second) {
         mSTDado = result.first;
         return;
      }
   }
   if (anterior and (result = anterior->st.getDado(getLexema())).second) {
      mSTDado = result.first;
      return;
   }
   throw Erro(tk, "semantico",
              "Todas as variaveis devem ser declaradas antes do uso.");
}
AST::NodeExpValor::NodeExpValor(const Token &_tk, Node *_super,
                                const TipoAST _t)
    : NodeExpOp(_tk, _super, _t), val(std::stof(getLexema())) {}
}  // namespace AnaliseSintatica
