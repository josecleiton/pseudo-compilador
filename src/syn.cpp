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

#ifndef DEBUG
#define DEBUG 1
#include <iostream>
#endif

#include <stdexcept>

namespace AnaliseSintatica {

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

Syn::Syn(Lex& l) : mLex(l) {
   mPilha.push(TipoToken::FIMARQ);
   mPilha.push(TipoToken::S);
}

unsigned Syn::parse(void) {
   Token tk = mLex.getToken();
   unsigned count{};
   Token* tipo{};
   while (mPilha.size()) {
      auto& topo = mPilha.top();
      try {
         if (tk != topo) {
            const auto& producao = mLL.at(topo).at(tk);
            mPilha.pop();
            switch (producao) {
               case 0:  // S -> programa
                  mPilha.push(TipoToken::PROGRAMA);
                  break;
               case 1:  // programa -> comando
                  mPilha.push(TipoToken::PROGRAMA);
                  mPilha.push(TipoToken::COMANDO);
                  break;
               case 2:  // bloco -> comando
                  mPilha.push(TipoToken::COMANDO);
                  break;
               case 3:  // comando -> sebloco
                  mPilha.push(TipoToken::SEBLOCO);
                  break;
               case 4:  // comando -> enquanto
                  mPilha.push(TipoToken::NT_ENQUANTO);
                  break;
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
                  mST.entrarContexto();
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
                  throw std::runtime_error(
                      "Erro sintatico - Produção não registrada");
            }
         } else {
            handleContexto(topo);
            mTokens.push_back(tk);
            if (tk == TipoToken::TIPO) {
               tipo = &mTokens.back();
            } else if (tipo and tk == TipoToken::ID) {
               mST.inserirID(tk, mST.getTipoByLexema(tipo->lexema));
               tipo = nullptr;
            }
            mPilha.pop();
#ifdef DEBUG
            std::clog << "[DEBUG - parser] " << tk << '\n';
#endif
            tk = mLex.getToken();
            count++;
         }
      } catch (const std::exception& e) {
#ifdef DEBUG
         std::clog
             << "[DEBUG - parser] Transição não encontrada na parse table.\n";
#endif
         throw e;
      }
   }
   return count;
}
void Syn::handleContexto(const Token& tk) {
   switch (tk) {
      case TipoToken::SE:
      case TipoToken::SENAO:
      case TipoToken::ENQUANTO:
         mST.entrarContexto();
         break;
      case TipoToken::ACABOU:
         mST.sairContexto();
         break;
      default:
         break;
   }
}
}  // namespace AnaliseSintatica

