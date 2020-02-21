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
   mLL[TipoToken::S][TipoToken::TIPO] = mLL[TipoToken::S][TipoToken::ID] =
       mLL[TipoToken::S][TipoToken::SE] =
           mLL[TipoToken::S][TipoToken::ENQUANTO] = 1;
   mLL[TipoToken::PROGRAMA][TipoToken::TIPO] =
       mLL[TipoToken::PROGRAMA][TipoToken::ID] =
           mLL[TipoToken::PROGRAMA][TipoToken::SE] =
               mLL[TipoToken::PROGRAMA][TipoToken::ENQUANTO] = 1;
   mLL[TipoToken::PROGRAMA][TipoToken::FIMARQ] = 21;
   mLL[TipoToken::BLOCO][TipoToken::TIPO] =
       mLL[TipoToken::BLOCO][TipoToken::ID] =
           mLL[TipoToken::BLOCO][TipoToken::SE] =
               mLL[TipoToken::BLOCO][TipoToken::ENQUANTO] = 2;
   mLL[TipoToken::COMANDO][TipoToken::SE] = 3;
   mLL[TipoToken::COMANDO][TipoToken::ENQUANTO] = 4;
   mLL[TipoToken::COMANDO][TipoToken::TIPO] =
       mLL[TipoToken::COMANDO][TipoToken::ID] = 5;
   mLL[TipoToken::STAT][TipoToken::TIPO] = 6;
   mLL[TipoToken::STAT][TipoToken::ID] = 7;
   mLL[TipoToken::SEBLOCO][TipoToken::SE] = 8;
   mLL[TipoToken::NT_SE][TipoToken::SE] = 9;
   mLL[TipoToken::NT_SENAO][TipoToken::ACABOU] = 10;
   mLL[TipoToken::NT_SENAO][TipoToken::SENAO] = 11;
   mLL[TipoToken::NT_ENQUANTO][TipoToken::ENQUANTO] = 12;
   mLL[TipoToken::DECL][TipoToken::TIPO] = 13;
   mLL[TipoToken::ATRIB][TipoToken::ID] = 14;
   mLL[TipoToken::EXP][TipoToken::ID] =
       mLL[TipoToken::EXP][TipoToken::ABREPRNT] =
           mLL[TipoToken::EXP][TipoToken::VALOR] = 15;
   mLL[TipoToken::EXP][TipoToken::SINAL] = 16;
   mLL[TipoToken::EXPL][TipoToken::ID] =
       mLL[TipoToken::EXPL][TipoToken::ABREPRNT] = 17;
   mLL[TipoToken::EXPL][TipoToken::NEG] = 19;
   mLL[TipoToken::TERMOEXP][TipoToken::ID] =
       mLL[TipoToken::TERMOEXP][TipoToken::ABREPRNT] =
           mLL[TipoToken::TERMOEXP][TipoToken::VALOR] = 20;
   mLL[TipoToken::TERMOEXP2][TipoToken::PNTVIRG] =
       mLL[TipoToken::TERMOEXP2][TipoToken::FECHAPRNT] = 21;
   mLL[TipoToken::TERMOEXP2][TipoToken::SINAL] = 22;
   mLL[TipoToken::TERMOEXP2][TipoToken::FATOR_OP] = 27;
   mLL[TipoToken::FATOREXP][TipoToken::ID] = 23;
   mLL[TipoToken::FATOREXP][TipoToken::ABREPRNT] = 24;
   mLL[TipoToken::FATOREXP][TipoToken::VALOR] = 25;
   mLL[TipoToken::FATOREXP2][TipoToken::PNTVIRG] =
       mLL[TipoToken::FATOREXP2][TipoToken::SINAL] =
           mLL[TipoToken::FATOREXP2][TipoToken::FECHAPRNT] = 26;
   mLL[TipoToken::FATOREXP2][TipoToken::FATOR_OP] = 27;
   mLL[TipoToken::TERMOEXPL][TipoToken::ID] =
       mLL[TipoToken::TERMOEXPL][TipoToken::ABREPRNT] = 28;
   mLL[TipoToken::TERMOEXPL2][TipoToken::FACA] =
       mLL[TipoToken::TERMOEXPL2][TipoToken::FECHAPRNT] = 29;
   mLL[TipoToken::TERMOEXPL2][TipoToken::OR] = 30;
   mLL[TipoToken::TERMOEXPL2][TipoToken::AND] = 34;
   mLL[TipoToken::FATOREXPL][TipoToken::ID] = 31;
   mLL[TipoToken::FATOREXPL][TipoToken::ABREPRNT] = 32;
   mLL[TipoToken::FATOREXPL2][TipoToken::FACA] =
       mLL[TipoToken::FATOREXPL2][TipoToken::FECHAPRNT] =
           mLL[TipoToken::FATOREXPL2][TipoToken::OR] = 33;
   mLL[TipoToken::FATOREXPL2][TipoToken::AND] = 34;
   mLL[TipoToken::NEG][TipoToken::NEG] = 35;
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

               case 21:
               case 26:
               case 29:
               case 33:
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

