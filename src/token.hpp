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
#include <iostream>

struct Token {
   enum class TipoToken {
      INITIAl,
      ID,
      TIPO,
      VALOR,
      OPB,
      OPA,
      NEG,
      SE,
      SENAO,
      ENQUANTO,
      FACA,
      ACABOU,
      ATRIB,
      PNTVIRG,
      FIMARQ
   };
   const TipoToken id;
   const std::string lexema;
   Token(const TipoToken& tk, const std::string& s);
   friend std::ostream& operator<<(std::ostream&, const Token&);
};

