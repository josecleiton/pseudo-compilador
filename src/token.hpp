/*
 * =====================================================================================
 *
 *       Filename:  token.hpp
 *
 *    Description: Tokens
 *
 *        Version:  1.0
 *        Created:  02/13/2020 03:29:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#pragma once
#include <ostream>

struct Token {
   enum class TipoToken {
      INITIAl, // 0
      ID, // 1
      TIPO, // 2
      VALOR, // 3
      OPB, // 4
      OPA, // 5
      NEG, // 6
      SE, // 7
      SENAO, // 8
      ENQUANTO, // 9
      FACA, // 10
      ACABOU, // 11
      ATRIB, // 12
      PNTVIRG, // 13
      ABREPRNT, // 14
      FECHAPRNT, // 15
      FIMARQ // 16
   };
   TipoToken id;
   std::string lexema;
   Token(const TipoToken &tk = TipoToken::INITIAl, const std::string &s = "");
   friend std::ostream &operator<<(std::ostream &, const Token &);
};
