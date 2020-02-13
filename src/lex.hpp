/*
 * =====================================================================================
 *
 *       Filename:  lex.h
 *
 *    Description: Analisador léxico
 *
 *        Version:  1.0
 *        Created:  02/13/2020 02:34:26 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */
#pragma once

#include <exception>
#include <fstream>
#include <list>

#include "token.hpp"

class Lex {
  private:
   std::string mLexema;
   std::list<Token> mTokens;

   unsigned mLine{};

   enum class Estado {
      INICIAL,
      SE,
      SENAO,
      ENQUANTO,
      INTEIRO,
      QUEBRADO,
      LOGICO,
      VALOR,
      ID,
      ATRIB,
      OPA,
      OPB,
      PNTVIRG,
      NAO_ACEITACAO,
   };
  public:
   Lex(std::ifstream &file);

  private:
   inline bool ignore(const char c) {
      if (c == '\n' or c == '\t' or c == ' ') {
         if (c == '\n') {
            mLine++;
         }
         return true;
      }
      return false;
   }
};
