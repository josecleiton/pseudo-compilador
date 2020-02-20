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

#include <functional>
#include <stack>
#include <unordered_map>

#include "lex.hpp"
class Syn {
   typedef Token::TipoToken TipoToken;
   std::stack<TipoToken> mPilha;
   // abaixo está a inicialização da parse table (LL(1))
   const std::unordered_map<TipoToken, std::unordered_map<TipoToken, int>> mLL{
       {TipoToken::PROGRAMA,
        {{TipoToken::TIPO, 1},
         {TipoToken::ID, 1},
         {TipoToken::SE, 1},
         {TipoToken::ENQUANTO, 1}}},
       {TipoToken::PROGRAMA,
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
       {TipoToken::ATRIB, {{TipoToken::ID, 13}}},
   };
   Lex& mLex;

  public:
   Syn(Lex&);
   unsigned parse(void);
};
