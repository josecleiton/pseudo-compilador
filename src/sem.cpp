/*
 * =====================================================================================
 *
 *       Filename:  sem.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/20/2020 07:17:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia
 *   Organization:  Universidade do Estado da Bahia (UNEB)
 *
 * =====================================================================================
 */

#include "sem.hpp"

namespace AnaliseSemantica {

SymbolTable::SymbolTable(void) { mPilha.push(GLOBAL); }

const SymbolTable::ID* SymbolTable::buscaID(const unsigned codContexto,
                                            const std::string& lexema) const {
   const auto itContexto = mContextos.find(codContexto);
   if (itContexto != mContextos.end()) {
      // c++17 feature, binda os campos de um objeto para variáveis
      const auto& [super, mIDs] = itContexto->second;
      const auto itID = mIDs.find(lexema);
      if (itID != mIDs.end()) {
         return &itID->second;
      }
      return buscaID(super, lexema);
   }
   return nullptr;
}

bool SymbolTable::inserirID(const unsigned codContexto, const Token& tk,
                            const Tipo& tipo) {
   const auto itContexto = mContextos.find(codContexto);
   if (itContexto != mContextos.end()) {
      auto& [_, mIDs] = itContexto->second;
      mIDs[tk.lexema] = {tipo};
      return true;
   }
   return false;
}

bool SymbolTable::inserirID(const Token& tk, const Tipo& tipo) {
   auto& [_, mIDs] = mContextos.at(getContexto());
   mIDs[tk.lexema] = {tipo};
   return true;
}
}  // namespace AnaliseSemantica
