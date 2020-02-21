/*
 * =====================================================================================
 *
 *       Filename:  sem.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/20/2020 07:18:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia
 *   Organization:  Universidade do Estado da Bahia (UNEB)
 *
 * =====================================================================================
 */
#pragma once

#include <stack>
#include <unordered_map>

#include "syn.hpp"

namespace AnaliseSemantica {

class SymbolTable {
  public:
   enum class Tipo {
      INTEIRO,
      QUEBRADO,
      LOGICO,
   };
   const unsigned GLOBAL{1};
   unsigned mContextoCounter{1};

  private:
   struct ID {
      Tipo tipo;
   };
   struct Contexto {
      unsigned super;
      std::unordered_map<std::string, ID> mIDs;
   };
   std::unordered_map<unsigned, Contexto> mContextos{{GLOBAL, {}}};
   std::stack<unsigned> mPilha;

  public:
   SymbolTable(void);
   const ID* buscaID(const unsigned codContexto,
                     const std::string& lexema) const;
   inline unsigned entrarContexto() {
      mContextos[++mContextoCounter].super = getContexto();
      mPilha.push(mContextoCounter);
      return mContextoCounter;
   }
   inline unsigned getContexto(void) const { return mPilha.top(); }
   inline void sairContexto(void) { mPilha.pop(); }
   bool inserirID(const unsigned, const Token&, const Tipo&);
   bool inserirID(const Token&, const Tipo&);
   inline Tipo getTipoByLexema(const std::string& lexema) const {
      if (lexema == "INTEIRO") {
         return Tipo::INTEIRO;
      } else if (lexema == "QUEBRADO") {
         return Tipo::QUEBRADO;
      }
      return Tipo::LOGICO;
   }
};

class Sem {};
}  // namespace AnaliseSemantica
