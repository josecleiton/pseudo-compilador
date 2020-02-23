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
               aux->childs.clear();
               break;
            default:
               break;
         }
         return true;
      }
      if (no->tk == TipoToken::DECL) {
         declaraVariavel(no);
      }
      if (no->tk == TipoToken::ATRIB) {
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
      return std::stof(no->tk.lexema);
   }
   auto const bloco = mAST.getBlocoAcima(no);
   if (!bloco) {
      return std::numeric_limits<int>::max();
   }
   return bloco->st.getDado(no->tk.lexema)->valor;
}

void Sem::aplicaBinop(AST::NodeExp* no1, AST::NodeExp* op,
                      AST::NodeExp* no2) const {
   double val1 = getValorVariavel(no1), val2 = getValorVariavel(no2);
   switch (getOp(op)) {
      case '*':
         no1->dado.valor = val1 * val2;
         break;
      case '/':
         no1->dado.valor = val1 / val2;
         break;
      case '-':
         no1->dado.valor = val1 - val2;
         break;
      case '+':
         no1->dado.valor = val1 - val2;
         break;
      default:
         no1->dado.valor = std::numeric_limits<int>::max();
         break;
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
            while (!ops.empty() and ops.top()->tk != TipoToken::FECHAPRNT) {
               const auto val1 = nums.top();
               nums.pop();
               const auto op = ops.top();
               ops.pop();
               aplicaBinop(nums.top(), op, val1);
            }
            break;
         case TipoToken::SINAL:
         case TipoToken::BINOP:
            while (!ops.empty() and
                   precedencia(getOp(ops.top())) >= precedencia(getOp(no))) {
               const auto val1 = nums.top();
               nums.pop();
               const auto op = ops.top();
               ops.pop();
               aplicaBinop(nums.top(), op, val1);
            }
            ops.push(no);
            break;
         default:
            break;
      }
      begin++;
   }
   while (!ops.empty()) {
      const auto val1 = nums.top();
      nums.pop();
      const auto op = ops.top();
      ops.pop();
      aplicaBinop(nums.top(), op, val1);
   }
   return Dado(nums.top()->dado);
}
bool Sem::declaraVariavel(AST::Node* no) const {
   auto const bloco = mAST.getBlocoAcima(no);
   if (!bloco) {
      return false;
   }
   auto itList = no->childs.cbegin();
   const auto tipo = lexemaTipo(itList->get()->tk.lexema);
   itList++;
   const auto& variavel = itList->get()->tk.lexema;
   bloco->st.insereDado(variavel, tipo);
   return true;
}
void Sem::atribueVariavel(AST::Node* no) const {
   auto const bloco = mAST.getBlocoAcima(no);
   if (!bloco) {
      return;
   }
   auto itList = no->childs.cbegin();
   const auto& variavel = itList->get()->tk.lexema;
   itList++;
   auto varDado = bloco->st.getDado(variavel);
   const auto dado = avaliaExpressao(itList, no->childs.cend());
   if (!tipoValido(varDado->tipo, dado.tipo)) {
      throw std::domain_error("Variáveis de tipos incompatíveis.");
   }
   varDado->valor = dado.valor;
}

bool Sem::tipoValido(const Tipo& t1, const Tipo& t2) const {
   switch (t1) {
      case Tipo::QUEBRADO:
         return true;
      case Tipo::INTEIRO:
         return t2 == Tipo::INTEIRO || t2 == Tipo::LOGICO;
      case Tipo::LOGICO:
         return t2 == Tipo::LOGICO;
      default:
         return false;
   }
}

}  // namespace AnaliseSemantica

