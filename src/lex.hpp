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

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "token.hpp"

class Lex {
  private:
   std::string mLexema, mFilename;
   std::ifstream mInputFile;
   std::ofstream mOutputFile;
   const std::unordered_map<std::string, Token::TipoToken> mPalavrasReservadas{
       {"SE", Token::TipoToken::SE},
       {"FACA", Token::TipoToken::FACA},
       {"ACABOU", Token::TipoToken::ACABOU},
       {"SENAO", Token::TipoToken::SENAO},
       {"ENQUANTO", Token::TipoToken::ENQUANTO},
       {"INTEIRO", Token::TipoToken::TIPO},
       {"QUEBRADO", Token::TipoToken::TIPO},
       {"LOGICO", Token::TipoToken::TIPO}};
   unsigned mLinha{1}, mCol{};

  public:
   Lex(const std::string& inPath, const std::string& outPath = "lexemas.txt");
   ~Lex();
   inline Token getToken(void) {
      auto tk = proxToken();
      mOutputFile << tk << '\n';
      return tk;
   }
   inline const auto& getLinha(void) const { return mLinha; }
   inline const auto& getCol(void) const { return mCol; }
   inline const auto& getFilename(void) const { return mFilename; }
   inline auto& getFile(void) { return mInputFile; }
   inline void analiseAteEOF(void) {
      Token tk;
      while ((tk = getToken()) != Token::TipoToken::FIMARQ) {
         std::cout << tk << '\n';
      }
   }

  private:
   Token proxToken(void);
   inline Token::TipoToken reservadaOuID(const std::string& s) const {
      try {
         return mPalavrasReservadas.at(s);
      } catch (const std::out_of_range& e) {
         return Token::TipoToken::ID;
      }
   }
   inline char getChar(std::string& lexema) {
      mCol++;
      lexema.push_back(mInputFile.get());
      return lexema.back();
   }
   inline void ungetChar(std::string& lexema) {
      mCol--;
      mInputFile.unget();
      lexema.pop_back();
   }
};
