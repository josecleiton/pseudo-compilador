/*
 * =====================================================================================
 *
 *       Filename:  ast.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/21/2020 09:54:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */
#pragma once

#include <list>
#include <memory>
#include <stack>
#include <stdexcept>
#include <unordered_map>

#include "token.hpp"

namespace AnaliseSemantica {
enum class Tipo {
   INTEIRO,
   QUEBRADO,
   LOGICO,
};
struct ID {
   Tipo tipo;
   double valor{};
};
class SymbolTable {
   std::unordered_map<std::string, ID> mTable;
   inline const ID* getID(const std::string& lexema) const {
      try {
         return &mTable.at(lexema);
      } catch (const std::out_of_range& e) {
         return nullptr;
      }
   }
   inline void insereID(const std::string& lexema, const Tipo& t,
                        const double v) {
      if (getID(lexema)) {
         throw std::domain_error(
             "Sobescrever uma entrada na tabela de simbolos não é permitido");
      }
      mTable[lexema] = {t, v};
   }
};
}  // namespace AnaliseSemantica

namespace AnaliseSintatica {
class AST {
  public:
   enum class Tipo {
      BLOCO,
      EXP_NUM,
      EXP_OP,
      EXPL_PROP,
      EXPL_OP,
      REGULAR,
   };
   struct Node {
      Token tk;
      Tipo tipo;
      std::list<std::unique_ptr<Node>> childs;
      Node* super{};
      Node(const Token&, const Tipo& = Tipo::REGULAR, Node* = nullptr);
   };
   struct NodeBloco : public Node {
      AnaliseSemantica::SymbolTable st;
      NodeBloco(const Token&, const Tipo& = Tipo::REGULAR, Node* = nullptr);
   };

  private:
   std::unique_ptr<Node> root;
   std::stack<Node*> mPilha;
   unsigned nodeCounter{1};

  public:
   AST(void);
   Node* inserirNode(const Token&, const Tipo& = Tipo::REGULAR);
   Node* inserirFolha(const Token&, const Tipo& = Tipo::REGULAR);
   inline std::size_t subirNivel(Node* blocoAtual) {
      std::size_t count{};
      while (mPilha.size() and blocoAtual != mPilha.top()) {
         mPilha.pop();
         count++;
      }
      return count;
   }
   inline std::size_t subirNivel(void) {
      std::size_t count{};
      bool retirado{};
      while (mPilha.size() and mPilha.top()->tipo != Tipo::BLOCO) {
         mPilha.pop();
         retirado = true;
         count++;
      }
      if (retirado and mPilha.size() > 1) {
         mPilha.pop();
         count++;
      }
      return count;
   }
   inline std::size_t subirNivel(const std::size_t n) {
      std::size_t i = 0;
      for (; mPilha.size() and i < n; i++) {
         mPilha.pop();
      }
      return i;
   }
   inline Node* atual(void) { return mPilha.top(); }
   inline unsigned size(void) const { return nodeCounter; }
};
}  // namespace AnaliseSintatica
