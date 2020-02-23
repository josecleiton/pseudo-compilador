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

#include <functional>
#include <list>
#include <memory>
#include <stack>
#include <stdexcept>
#include <unordered_map>

#include "token.hpp"

namespace AnaliseSemantica {
enum class Tipo {
   NULO,
   INTEIRO,
   QUEBRADO,
   LOGICO,
};
Tipo lexemaTipo(const std::string&);
class Dado {
  public:
   Tipo tipo;
   double valor;
   Dado() = default;
   Dado(const Tipo&, const double = 0.0f);
   Dado(const Dado&);

  private:
   void preencheTipo(void);
};
class SymbolTable {
   std::unordered_map<std::string, Dado> mTable;

  public:
   inline Dado* getDado(const std::string& lexema) {
      try {
         return &mTable.at(lexema);
      } catch (const std::out_of_range& e) {
         return nullptr;
      }
   }
   inline void insereDado(const std::string& lexema, const Tipo& t,
                          const double v = {}) {
      if (getDado(lexema)) {
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
      EXP,
      REGULAR,
   };
   struct Node {
      Token tk;
      Tipo tipo;
      std::list<std::unique_ptr<Node>> childs;
      Node* super{};
      Node(const Token&, const Tipo& = Tipo::REGULAR, Node* = nullptr);
      virtual ~Node() {}
   };
   struct NodeBloco : public Node {
      AnaliseSemantica::SymbolTable st;
      NodeBloco(const Token&, const Tipo& = Tipo::REGULAR, Node* = nullptr);
      ~NodeBloco() {}
   };
   struct NodeExp : public Node {
      AnaliseSemantica::Dado dado;
      // AnaliseSemantica::Exp expTree;
      NodeExp(const Token&, const Tipo& = Tipo::REGULAR, Node* = nullptr);
      ~NodeExp() {}
   };

  private:
   std::unique_ptr<Node> root;
   std::stack<Node*> mPilha;
   unsigned mNodeCounter{1};

  public:
   AST(void);
   Node* inserirNode(const Token&, const Tipo& = Tipo::REGULAR);
   Node* inserirFolha(const Token&, const Tipo& = Tipo::REGULAR);
   std::size_t DFS(const std::function<bool(Node*)>&);
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
   inline NodeBloco* getBlocoAcima(const Node* const atual) {
      Node* result = atual->super;
      while (result and result->tipo != Tipo::BLOCO) {
         result = result->super;
      }
      return result ? dynamic_cast<NodeBloco*>(result) : nullptr;
   }
   inline Node* atual(void) { return mPilha.top(); }
   inline auto size(void) const { return mNodeCounter; }

  private:
   std::size_t DFS(Node* atual, const std::function<bool(Node*)>& func);
};
}  // namespace AnaliseSintatica
