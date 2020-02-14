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
#include <map>
#include <stdexcept>

#include "token.hpp"

class Lex {
  private:
   std::string mLexema;
   std::list<Token> mTokens;
   std::fstream mFile;
   const std::map<std::string, Token::TipoToken> mPalavrasReservadas;

   unsigned mLine{};

   /* enum class Estado { */
   /*    INICIAL, */
   /*    SE, */
   /*    SENAO, */
   /*    ENQUANTO, */
   /*    INTEIRO, */
   /*    QUEBRADO, */
   /*    LOGICO, */
   /*    VALOR, */
   /*    ID, */
   /*    ATRIB, */
   /*    OPA, */
   /*    OPB, */
   /*    PNTVIRG, */
   /*    NAO_ACEITACAO, */
   /* }; */
  public:
   Lex(const std::string &file);
   ~Lex();
   Token getToken(void);

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
   inline Token::TipoToken reservada(const std::string& s) {
      try {
         return mPalavrasReservadas.at(s);
      } catch(std::out_of_range &e) {
         return {};
      }
   }
   inline char getChar(std::string& s) {
      s.push_back(mFile.get());
      return s.back();
   }
   inline void ungetChar(std::string& s) {
      mFile.unget();
      s.pop_back();
   }

};
