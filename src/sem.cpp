/*
 * =====================================================================================
 *
 *       Filename:  sem.cpp
 *
 *    Description:
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

#include <limits>
#include <stdexcept>

namespace AnaliseSemantica {
using namespace AnaliseSintatica;
typedef Token::TipoToken TipoToken;
Sem::Sem(AST& ast) : mAST(ast) {}

void Sem::analisaArvore(void) {
   mAST.DFS([this](AST::Node* no) -> bool {
      if (!no) {
         return false;
      }
      Dado exp;
      if (no->tipo == AST::Tipo::BLOCO) {
         AST::NodeExp* aux;
         switch (no->tk) {
            case TipoToken::NT_SE:
            case TipoToken::NT_SENAO:
            case TipoToken::NT_ENQUANTO:
               aux = dynamic_cast<AST::NodeExp*>(no->childs.front().get());
               exp = avaliaExpressao(aux);
               if (exp.tipo != Tipo::LOGICO) {
                  throw std::domain_error("Expressão deveria ser lógica.");
               }
               break;
            default:
               break;
         }
         return true;
      } else if (no->tk == TipoToken::DECL) {
         declaraVariavel(no);
      } else if (no->tk == TipoToken::ATRIB) {
         atribueVariavel(no);
      }
      no->childs.clear();
      return false;
   });
}
Dado Sem::avaliaExpressao(AST::NodeExp* exp) const {
   return avaliaExpressao(exp->childs.cbegin(), exp->childs.cend());
}
constexpr int precedencia(const char op) {
   switch (op) {
      case '+':
      case '-':
         return 1;
      case '*':
      case '/':
         return 2;
      default:
         return 0;
   }
}

char getOp(AST::NodeExp* node) { return node->tk.lexema[0]; }

double Sem::getValorVariavel(AST::NodeExp* no) const {
   if (no->tk == TipoToken::VALOR) {
      return no->dado.tipo != Tipo::NULO ? no->dado.valor
                                         : std::stof(no->tk.lexema);
   }
   auto const var = getVariavel(no, no->tk.lexema);
   return var->valor;
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

Dado Sem::avaliaExpressao(
    std::list<std::unique_ptr<AST::Node>>::const_iterator begin,
    std::list<std::unique_ptr<AST::Node>>::const_iterator end) const {
   std::stack<AST::NodeExp*> nums, ops;
   while (begin != end) {
      auto no = dynamic_cast<AST::NodeExp*>(begin->get());
      switch (no->tk) {
         case TipoToken::ID:
         case TipoToken::VALOR:
            nums.push(no);
            break;
         case TipoToken::ABREPRNT:
            ops.push(no);
            break;
         case TipoToken::FECHAPRNT:
            while (!ops.empty() and ops.top()->tk != TipoToken::ABREPRNT) {
               resolveOp(nums, ops);
            }
            ops.pop();
            break;
         case TipoToken::SINAL:
         case TipoToken::BINOP:
            while (!ops.empty() and
                   precedencia(getOp(ops.top())) >= precedencia(getOp(no))) {
               resolveOp(nums, ops);
            }
            ops.push(no);
            break;
         default:
            break;
      }
      begin++;
   }
   while (!ops.empty()) {
      resolveOp(nums, ops);
   }
   auto& topo = nums.top();
   if (topo->dado != Tipo::NULO) {
      return Dado(topo->dado);
   }
   return Dado(getValorVariavel(topo));
}
Dado* Sem::getVariavel(AST::Node* no, const std::string& lexema) const {
   AST::NodeBloco *atual{}, *anterior{};
   Dado* result;
   while ((atual = mAST.getBlocoAcima(no))) {
      no = anterior = atual;
      if ((result = anterior->st.getDado(lexema))) {
         return result;
      }
   }
   if (anterior and (result = anterior->st.getDado(lexema))) {
      return result;
   }
   throw std::out_of_range(
       "Todas as variaveis devem ser declaradas antes do uo.");
}
bool Sem::declaraVariavel(AST::Node* no) const {
   auto const bloco = mAST.getBlocoAcima(no);
   auto itList = no->childs.cbegin();
   const auto tipo = lexemaTipo(itList->get()->tk.lexema);
   itList++;
   const auto& variavel = itList->get()->tk.lexema;
   bloco->st.insereDado(variavel, tipo);
   return true;
}
void Sem::atribueVariavel(AST::Node* no) const {
   auto itList = no->childs.cbegin();
   const auto& variavel = itList->get()->tk.lexema;
   itList++;
   auto const varDado = getVariavel(no, variavel);
   const auto dado = avaliaExpressao(itList, no->childs.cend());
   *varDado = dado;
}

}  // namespace AnaliseSemantica

