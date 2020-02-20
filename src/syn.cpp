/*
 * =====================================================================================
 *
 *       Filename:  syn.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/18/2020 05:29:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia
 *   Organization: UNEB
 *
 * =====================================================================================
 */

#include "syn.hpp"

#ifdef DEBUG
#include <iostream>
#endif

#include <stdexcept>

namespace AnaliseSintatica {

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

bool SymbolTable::inserirID(const unsigned codContexto,
                            const std::string& lexema, const Tipo& tipo) {
   const auto itContexto = mContextos.find(codContexto);
   if (itContexto != mContextos.end()) {
      auto& [_, mIDs] = itContexto->second;
      mIDs[lexema] = {tipo};
      return true;
   }
   return false;
}

Syn::Syn(Lex& l) : mLex(l) {
   mPilha.push(TipoToken::FIMARQ);
   mPilha.push(TipoToken::PROGRAMA);
}

unsigned Syn::parse(void) {
   Token tk = mLex.getToken();
   unsigned count{};
   while (mPilha.size()) {
      auto& topo = mPilha.top();
      try {
         if (tk.id != topo) {
            const auto& producao = mLL.at(topo).at(tk.id);
            mPilha.pop();
            switch (producao) {
               case 1:  // programa -> comando
                  mPilha.push(TipoToken::COMANDO);
                  // inicia contexto global
                  break;
               case 2:  // bloco -> comando
                  mPilha.push(TipoToken::COMANDO);
                  // insere contexto na arvore de contexto
                  break;
               case 3:  // comando -> sebloco
                  mPilha.push(TipoToken::SEBLOCO);
                  break;
               case 4:  // comando -> enquanto
                  mPilha.push(TipoToken::NT_ENQUANTO);
               case 5:  // comando -> stat;
                  mPilha.push(TipoToken::PNTVIRG);
                  mPilha.push(TipoToken::STAT);
                  break;
               case 6:  // stat -> decl
                  mPilha.push(TipoToken::DECL);
                  break;
               case 7:  // stat -> atrib
                  mPilha.push(TipoToken::ATRIB);
                  break;
               case 8:  // sebloco -> se senao
                  mPilha.push(TipoToken::NT_SENAO);
                  mPilha.push(TipoToken::NT_SE);
                  break;
               case 9:  // se -> SE expl FACA bloco
                  mPilha.push(TipoToken::BLOCO);
                  mPilha.push(TipoToken::FACA);
                  mPilha.push(TipoToken::EXPL);
                  mPilha.push(TipoToken::SE);
                  break;
               case 10:  // senao -> ACABOU
                  mPilha.push(TipoToken::ACABOU);
                  break;
               case 11:  // senao -> SENAO bloco ACABOU
                  mPilha.push(TipoToken::ACABOU);
                  mPilha.push(TipoToken::BLOCO);
                  mPilha.push(TipoToken::SENAO);
                  break;
               case 12:  // enquanto -> ENQUANTO expl FACA bloco ACABOU
                  mPilha.push(TipoToken::ACABOU);
                  mPilha.push(TipoToken::BLOCO);
                  mPilha.push(TipoToken::FACA);
                  mPilha.push(TipoToken::EXPL);
                  mPilha.push(TipoToken::ENQUANTO);
                  break;
               case 13:  // decl -> tipo id
                  mPilha.push(TipoToken::ID);
                  mPilha.push(TipoToken::TIPO);
                  break;
               case 14:  // atrib -> id = exp
                  mPilha.push(TipoToken::EXP);
                  mPilha.push(TipoToken::ATRIB);
                  mPilha.push(TipoToken::ID);
                  break;
               default:
                  throw std::runtime_error("Erro sintático");
            }
         } else {
            mPilha.pop();
#ifdef DEBUG
            std::clog << "[DEBUG - parser] " << tk << '\n';
#endif
            tk = mLex.getToken();
            count++;
         }
      } catch (const std::out_of_range& e) {
#ifdef DEBUG
         std::clog
             << "[DEBUG - parser] Transição não encontrada na parse table.\n";
#endif
         throw e;
      } catch (const std::runtime_error& e) {
#ifdef DEBUG
         std::clog
             << "[DEBUG - parser] Produção não registrada na parse table.\n";
#endif
         throw e;
      }
   }
   return count;
}
}  // namespace AnaliseSintatica

