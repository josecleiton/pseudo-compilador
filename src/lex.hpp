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

#define DEBUG 1

#include <exception>
#include <fstream>
#include <stdexcept>
#include <unordered_map>

#include "token.hpp"

class Lex {
  private:
   std::string mLexema, mFilename;
   std::ifstream mFile;
   const std::unordered_map<std::string, Token::TipoToken> mPalavrasReservadas{
       {"SE", Token::TipoToken::SE},
       {"FACA", Token::TipoToken::FACA},
       {"ACABOU", Token::TipoToken::ACABOU},
       {"SENAO", Token::TipoToken::SENAO},
       {"ENQUANTO", Token::TipoToken::ENQUANTO}};
   unsigned mLinha{1}, mCol{};

  public:
   Lex(const std::string& file);
   ~Lex();
   Token getToken(void);
   inline const auto& getLinha(void) const { return mLinha; }
   inline const auto& getCol(void) const { return mCol; }
   inline const auto& getFilename(void) const { return mFilename; }
   inline auto& getFile(void) { return mFile; }

  private:
   inline Token::TipoToken reservada(const std::string& s) const {
      try {
         return mPalavrasReservadas.at(s);
      } catch (std::out_of_range& e) {
         return {};
      }
   }
   inline char getChar(std::string& lexema) {
      mCol++;
      lexema.push_back(mFile.get());
      return lexema.back();
   }
   inline void ungetChar(std::string& lexema) {
      mCol--;
      mFile.unget();
      lexema.pop_back();
   }
};
