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

Syn::Syn(Lex& l) : mLex(l) {
   mPilha.push(TipoToken::FIMARQ);
   mPilha.push(TipoToken::S);
}

Token Syn::parse(void) {
   Token tk = mLex.getToken();
   mTkCounter++;
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
               case 15:  // exp -> termoexp termoexp2
                  mPilha.push(TipoToken::TERMOEXP2);
                  mPilha.push(TipoToken::TERMOEXP);
                  break;
               case 16:  // exp -> sinal termoexp
                  mPilha.push(TipoToken::TERMOEXP);
                  mPilha.push(TipoToken::SINAL);
                  break;
               case 17:  // exp -> termoexpl termoexpl2
                  mPilha.push(TipoToken::TERMOEXPL2);
                  mPilha.push(TipoToken::TERMOEXPL);
                  break;
               case 19:
                  mPilha.push(TipoToken::EXPL);
                  mPilha.push(TipoToken::NEG);
                  break;
               case 20:
                  mPilha.push(TipoToken::FATOREXP2);
                  mPilha.push(TipoToken::FATOREXP);
                  break;

               case 33:
               case 29:
               case 26:
               case 21:
                  // transicao nula
                  // mPilha.pop();
                  break;
               case 22:
                  mPilha.push(TipoToken::TERMOEXP);
                  mPilha.push(TipoToken::SINAL);
                  break;
               case 23:
               case 31:
                  mPilha.push(TipoToken::ID);
                  break;
               case 24:
                  mPilha.push(TipoToken::FECHAPRNT);
                  mPilha.push(TipoToken::EXP);
                  mPilha.push(TipoToken::ABREPRNT);
                  break;
               case 25:
                  mPilha.push(TipoToken::VALOR);
                  break;
               case 27:
                  mPilha.push(TipoToken::FATOREXP);
                  mPilha.push(TipoToken::FATOR_OP);
                  break;
               case 28:
                  mPilha.push(TipoToken::FATOREXPL2);
                  mPilha.push(TipoToken::FATOREXPL);
                  break;
               case 30:
                  mPilha.push(TipoToken::TERMOEXPL);
                  mPilha.push(TipoToken::OR);
                  break;
               case 32:
                  mPilha.push(TipoToken::FECHAPRNT);
                  mPilha.push(TipoToken::EXPL);
                  mPilha.push(TipoToken::ABREPRNT);
                  break;
               case 34:
                  mPilha.push(TipoToken::FATOREXP);
                  mPilha.push(TipoToken::AND);
                  break;
               case 35:
                  mPilha.push(TipoToken::EXPL);
                  mPilha.push(TipoToken::NEG);
                  break;

               default:
                  throw std::runtime_error(
                      "Erro sintatico - Produção não registrada");
            }
         } else {
            mPilha.pop();
#ifdef DEBUG
            std::clog << "[DEBUG - parser] " << tk << '\n';
#endif
            return tk;
         }
      } catch (const std::exception& e) {
#ifdef DEBUG
         std::clog
             << "[DEBUG - parser] Transição não encontrada na parse table.\n";
#endif
         throw e;
      }
   }
   return Token(TipoToken::FIMARQ);
}
}  // namespace AnaliseSintatica

