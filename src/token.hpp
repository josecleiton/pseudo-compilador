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
      INITIAl,  // 0
      ID,       // 1
      TIPO,     // 2
      VALOR,    // 3
      /* OPB,        // 4 */
      AND,
      OR,
      SINAL,  // 5
      FATOR_OP,
      NEG,        // 6
      SE,         // 7
      SENAO,      // 8
      ENQUANTO,   // 9
      FACA,       // 10
      ACABOU,     // 11
      ATRIB,      // 12
      PNTVIRG,    // 13
      ABREPRNT,   // 14
      FECHAPRNT,  // 15
      FIMARQ,     // 16
      // não terminais
      $,
      S,
      PROGRAMA,
      BLOCO,
      COMANDO,
      STAT,
      NT_ENQUANTO,
      SEBLOCO,
      NT_SE,
      NT_SENAO,
      DECL,
      EXP,
      EXPL,
      TERMOEXP,
      TERMOEXP2,
      FATOREXP,
      FATOREXP2,
      TERMOEXPL,
      TERMOEXPL2,
      FATOREXPL,
      FATOREXPL2,
   };
   TipoToken id;
   std::string lexema;
   Token(const TipoToken &tk = TipoToken::INITIAl, const std::string &s = "");
   inline operator TipoToken() const { return id; }
};

std::ostream &operator<<(std::ostream &, const Token &);
