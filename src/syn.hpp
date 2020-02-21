/*
 * =====================================================================================
 *
 *       Filename:  syn.hpp
 *
 *    Description: Parser
 *
 *        Version:  1.0
 *        Created:  02/18/2020 04:06:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia
 *   Organization: UNEB
 *
 * =====================================================================================
 */

#pragma once

#include <stack>
#include <unordered_map>

#include "lex.hpp"
namespace AnaliseSintatica {
typedef Token::TipoToken TipoToken;


class Syn {
   std::stack<TipoToken> mPilha;
   // abaixo está a inicialização da parse table (LL(1))
   const std::unordered_map<TipoToken, std::unordered_map<TipoToken, int>> mLL{
       {TipoToken::S,
        {{TipoToken::TIPO, 1},
         {TipoToken::ID, 1},
         {TipoToken::SE, 1},
         {TipoToken::ENQUANTO, 1}}},
       {TipoToken::PROGRAMA,
        {{TipoToken::TIPO, 1},
         {TipoToken::ID, 1},
         {TipoToken::SE, 1},
         {TipoToken::ENQUANTO, 1},
         {TipoToken::$, 2}}},
       {TipoToken::BLOCO,
        {{TipoToken::TIPO, 2},
         {TipoToken::ID, 2},
         {TipoToken::SE, 2},
         {TipoToken::ENQUANTO, 2}}},
       {TipoToken::COMANDO,
        {{TipoToken::SE, 3},
         {TipoToken::ENQUANTO, 4},
         {TipoToken::TIPO, 5},
         {TipoToken::ID, 5}}},
       {TipoToken::STAT, {{TipoToken::TIPO, 6}, {TipoToken::ID, 7}}},
       {TipoToken::SEBLOCO, {{TipoToken::SE, 8}}},
       {TipoToken::NT_SE, {{TipoToken::SE, 9}}},
       {TipoToken::NT_SENAO, {{TipoToken::ACABOU, 10}, {TipoToken::SENAO, 11}}},
       {TipoToken::NT_ENQUANTO, {{TipoToken::ENQUANTO, 12}}},
       {TipoToken::DECL, {{TipoToken::TIPO, 13}}},
       {TipoToken::ATRIB, {{TipoToken::ID, 14}}},
       {TipoToken::EXP, {{TipoToken::ID, 15}, {TipoToken::ABREPRNT, 15}, {TipoToken::VALOR, 15}, {TipoToken::SINAL, 16}}},
       {TipoToken::EXPL, {{TipoToken::ID, 17}, {TipoToken::ABREPRNT, 17}, {TipoToken::NEG, 19}}},
       {TipoToken::TERMOEXP, {{TipoToken::ID, 20}, {TipoToken::ABREPRNT, 20}, {TipoToken::VALOR, 20}}},
       {TipoToken::TERMOEXP2, {{TipoToken::FACA, 21}, {TipoToken::FECHAPRNT, 21}, {TipoToken::SINAL, 22}, {TipoToken::FATOR_OP, 27}}},
       {TipoToken::FATOREXP, {{TipoToken::ID,23}, {TipoToken::ABREPRNT,24}, {TipoToken::VALOR,25}}},
       {TipoToken::FATOREXP2, {{TipoToken::PNTVIRG, 26}, {TipoToken::SINAL,26}, {TipoToken::FATOR_OP,27}, {TipoToken::FECHAPRNT,26}}},
       {TipoToken::TERMOEXPL, {{TipoToken::ID, 28}, {TipoToken::ABREPRNT, 28}}},
       {TipoToken::TERMOEXP2, {{TipoToken::FACA,29 }, {TipoToken::OR, 30}, {TipoToken::FECHAPRNT, 29}, {TipoToken::AND, 34}}},
       {TipoToken::FATOREXPL, {{TipoToken::ID, 31}, {TipoToken::ABREPRNT, 32}}},
       {TipoToken::FATOREXPL2, {{TipoToken::FACA, 33}, {TipoToken::AND, 34}, {TipoToken::OR, 33}, {TipoToken::FECHAPRNT, 33}}},
       {TipoToken::NEG, {{TipoToken::NEG, 35}}},
   };
   unsigned mTkCounter{};
   // referencia do objeto Lex (responsável pela análise léxica)
   Lex& mLex;

  public:
   Syn(Lex&);
   Token parse(void);
   /* inline const auto& getTokens(void) const { return mTokens; } */
   inline unsigned getTkCounter(void) const { return mTkCounter; }
};
}  // namespace AnaliseSintatica
