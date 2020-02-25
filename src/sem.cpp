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
         no->childs.clear();
      } else if (no->tk == TipoToken::ATRIB) {
         /*
          * Analisa nó ATRIB que tem pelo menos 2 filhos
          * 1) Identificador
          * 2 ou mais) componentes de uma expressão
          */
         atribueVariavel(dynamic_cast<AST::NodeAtrib*>(no));
      }
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
void Sem::atribueVariavel(AST::NodeAtrib* no) {
   auto itList = no->childs.cbegin();
   const auto& variavel = itList->get()->tk.lexema;
   ++itList;
   /*
    * Se a variavel não existir em nenhum dos contextos acima:
    * throw out_of_range exception
    */
   auto dadoVar = *getVariavel(no, variavel);
   auto resultadoExp =
       avaliaExpressao(dynamic_cast<AST::NodeExp*>(itList->get()));
   /*
    * checa validade da atribuição entre o tipo retornado pela exp e a variável
    */
   dadoVar = resultadoExp;
}

Dado Sem::avaliaExpressao(AST::NodeExp* exp) {
   return avaliaExpressao(exp->expRoot);
}

Dado Sem::avaliaExpressao(AST::NodeExpOp* no) {
   ++mNodeExpCount;
   if (!no->getEsquerda()) {
      return getValorVariavel(no);
   }
   const Dado esq = avaliaExpressao(no->getEsquerda());
   if (auto noDir = no->getDireita(); noDir) {
      const Dado dir = avaliaExpressao(no->getDireita());
      return aplicaBinop(esq, no->getOp(), dir);
   }
   return aplicaUnop(no->getOp(), esq);
}
Dado Sem::aplicaUnop(const char op, const Dado& num) {
   switch (op) {
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

Dado Sem::aplicaBinop(const Dado& val1, const char op, const Dado& val2) {
   switch (op) {
      case '*':
         return Dado(val1.valor * val2.valor);
      case '/':
         return Dado(val1.valor / val2.valor);
      case '-':
         return Dado(val1.valor - val2.valor);
      case '+':
         return Dado(val1.valor + val2.valor);
      case '&':
         return Dado(val1.valor && val2.valor);
      case '|':
         return Dado(val1.valor || val2.valor);
      default:
         return {};
   }
}

Dado Sem::getValorVariavel(AST::NodeExpOp* no) {
   if (no->tk == TipoToken::VALOR) {
      return std::stof(no->tk.lexema);
   }
   return *getVariavel(no, no->tk.lexema);
}

Dado* Sem::getVariavel(const AST::Node* no, const std::string& lexema) {
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

}  // namespace AnaliseSemantica

