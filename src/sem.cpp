/*
 * =====================================================================================
 *
 *       Filename:  sem.cpp
 *
 *    Description: Analise semantica (documentação no sem.hpp)
 *
 *        Version:  1.0
 *        Created:  02/22/2020 09:32:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "sem.hpp"

#include <stdexcept>

namespace AnaliseSemantica {
using namespace AnaliseSintatica;
typedef Token::TipoToken TipoToken;
Sem::Sem(AST& ast) : mAST(ast) {}

std::size_t Sem::analisaArvore(void) {
   return mAST.DFS([this](AST::Node* no) -> bool {
      if (!no) {
         return false;
      }
      if (no->tipo == AST::Tipo::BLOCO) {
         /*
          * Analisa nó do tipo BLOCO
          * NT_SE/NT_SENAO/NT_ENQUANTO tem EXP como primeiro nó
          * tal EXP é a condição então deve retornar tipo LOGICO
          */
         if (no->tk != TipoToken::PROGRAMA) {
            auto const aux =
                dynamic_cast<AST::NodeExp*>(no->childs.front().get());
            aux->dado = avaliaExpressao(aux);
            if (aux->dado != Tipo::LOGICO) {
               throw std::domain_error("Condição deve retornar tipo LOGICO.");
            }
         }
         return true;
      } else if (no->tk == TipoToken::DECL) {
         /*
          * Analisa nó DECL que tem sempre 2 nós filhos
          * 1) Tipo
          * 2) Identificador
          */
         declaraVariavel(no);
      } else if (no->tk == TipoToken::ATRIB) {
         /*
          * Analisa nó ATRIB que tem pelo menos 2 filhos
          * 1) Identificador
          * 2 ou mais) componentes de uma expressão
          */
         atribueVariavel(dynamic_cast<AST::NodeAtrib*>(no));
      }
      /*
       * Após análise do DECL/EXP/ATRIB seus filhos não são mais necessários
       */
      no->childs.clear();
      return false;
   });
}
bool Sem::declaraVariavel(AST::Node* no) const {
   auto const bloco = mAST.getBlocoAcima(no);
   auto itList = no->childs.cbegin();
   const auto tipo = lexemaTipo(itList->get()->tk.lexema);
   ++itList;
   const auto& variavel = itList->get()->tk.lexema;
   bloco->st.insereDado(variavel, tipo);
   return true;
}
void Sem::atribueVariavel(AST::NodeAtrib* no) const {
   auto itList = no->childs.cbegin();
   const auto& variavel = itList->get()->tk.lexema;
   ++itList;
   /*
    * Se a variavel não existir em nenhum dos contextos acima:
    * throw out_of_range exception
    */
   no->var = getVariavel(no, variavel);
   no->resultadoExp =
       std::make_unique<Dado>(avaliaExpressao(itList, no->childs.cend()));
}

Dado Sem::avaliaExpressao(AST::NodeExp* exp) const {
   return avaliaExpressao(exp->childs.cbegin(), exp->childs.cend());
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
char getOp(AST::NodeExp* node) { return node->tk.lexema[0]; }
/*
 * Weird type i know
 * avaliaExpressao usa o algoritmo feito por Dijkstra, estendido para resolver
 * expressões infixa.
 * A função espera iterators da lista de childs. Apesar de eu tentar simplificar
 * os tipos, iterators são inimigos da simplicidade.
 */
Dado Sem::avaliaExpressao(
    std::list<std::unique_ptr<AST::Node>>::const_iterator atual,
    std::list<std::unique_ptr<AST::Node>>::const_iterator fim) const {
   std::stack<AST::NodeExp*> nums, ops;
   while (atual != fim) {
      auto const no = dynamic_cast<AST::NodeExp*>(atual->get());
      switch (no->tk) {
         case TipoToken::ID:
         case TipoToken::VALOR:
            nums.push(no);
            break;
         case TipoToken::ABREPRNT:
            ops.push(no);
            break;
         case TipoToken::FECHAPRNT:
            while (ops.size() and ops.top()->tk != TipoToken::ABREPRNT) {
               resolveOp(nums, ops);
            }
            ops.pop();
            break;
         case TipoToken::SINAL:
         case TipoToken::BINOP:
         case TipoToken::NEG:
            while (ops.size() and
                   precedencia(getOp(ops.top())) >= precedencia(getOp(no))) {
               resolveOp(nums, ops);
            }
            ops.push(no);
            break;
         default:
            break;
      }
      ++atual;
   }
   while (!ops.empty()) {
      resolveOp(nums, ops);
   }
   auto const topo = nums.top();
   if (topo->dado != Tipo::NULO) {
      return Dado(topo->dado);
   }
   return Dado(getValorVariavel(topo));
}

void Sem::resolveOp(std::stack<AST::NodeExp*>& nums,
                    std::stack<AST::NodeExp*>& ops) const {
   const auto val1 = nums.top();
   nums.pop();
   const auto op = ops.top();
   ops.pop();
   if (nums.size()) {
      aplicaBinop(nums.top(), op, val1);
   } else {
      aplicaUnop(val1, op);
      nums.push(val1);
   }
}

double Sem::getValorVariavel(AST::NodeExp* no) const {
   if (no->tk == TipoToken::VALOR) {
      return no->dado.tipo != Tipo::NULO ? no->dado.valor
                                         : std::stof(no->tk.lexema);
   }
   return getVariavel(no, no->tk.lexema)->valor;
}

Dado* Sem::getVariavel(const AST::Node* no, const std::string& lexema) const {
   AST::NodeBloco *atual{}, *anterior{};
   Dado* result;
   while ((atual = mAST.getBlocoAcima(no))) {
      no = anterior = atual;
      if ((result = anterior->st.getDado(lexema))) {
         return result;
      }
   }
   /*
    * Se chegar aqui procuraremos o lexema no escopo global
    */
   if (anterior and (result = anterior->st.getDado(lexema))) {
      return result;
   }
   throw std::out_of_range(
       "Todas as variaveis devem ser declaradas antes do uso.");
}

void Sem::aplicaUnop(AST::NodeExp* no1, AST::NodeExp* op) const {
   double val1 = getValorVariavel(no1);
   switch (getOp(op)) {
      case '-':
         no1->dado.setValor(-val1);
         break;
      case '+':
         no1->dado.setValor(val1);
         break;
      case '!':
         no1->dado.setValor(!val1);
         break;
      default:
         break;
   }
}

void Sem::aplicaBinop(AST::NodeExp* no1, AST::NodeExp* op,
                      AST::NodeExp* no2) const {
   double val1 = getValorVariavel(no1), val2 = getValorVariavel(no2);
   switch (getOp(op)) {
      case '*':
         no1->dado.setValor(val1 * val2);
         break;
      case '/':
         no1->dado.setValor(val1 / val2);
         break;
      case '-':
         no1->dado.setValor(val1 - val2);
         break;
      case '+':
         no1->dado.setValor(val1 + val2);
         break;
      case '&':
         no1->dado.setValor(val1 && val2);
         break;
      case '|':
         no1->dado.setValor(val1 || val2);
         break;
      default:
         break;
   }
}

}  // namespace AnaliseSemantica

