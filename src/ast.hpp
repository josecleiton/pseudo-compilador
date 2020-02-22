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
#include <list>
#include <memory>
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
      Node(const Token&, const Tipo& = Tipo::REGULAR);
   };
   struct NodeBloco : public Node {
      AnaliseSemantica::SymbolTable st;
   };

  private:
   unsigned nodeCounter{1};
   std::unique_ptr<Node> root;

  public:
   AST(void);
   inline unsigned size(void) const { return nodeCounter; }
};
}  // namespace AnaliseSintatica
