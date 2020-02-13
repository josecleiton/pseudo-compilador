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
#include <string>

class Token {
public:
   enum class TipoToken {
      INITIAl,
      ID,
      TIPO,
      VALOR,
      OPB,
      OPA,
      SE,
      SENAO,
      ENQUANTO,
      FACA,
      ACABOU,
      ATRIB,
      PNTVIRG,
   };
   TipoToken id;
   const std::string lexema;
   Token(const std::string& s);
};
