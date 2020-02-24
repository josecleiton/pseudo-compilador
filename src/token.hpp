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
#include <ostream>

struct Token {
   enum class TipoToken {
      INITIAl,
      ID,         // 1
      TIPO,       // 2
      VALOR,      // 3
      SINAL,      // 4
      NEG,        // 5
      SE,         // 6
      SENAO,      // 7
      ENQUANTO,   // 8
      FACA,       // 9
      ACABOU,     // 10
      ATRIB,      // 11
      PNTVIRG,    // 12
      ABREPRNT,   // 13
      FECHAPRNT,  // 14
      FIMARQ,     // 15
      BINOP,      // 16
      // não terminais
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
      COND,
      EXP,
      TERMO,
      FATOR,
      OP,
      UNOP,
   };
   TipoToken id;
   std::string lexema;
   /*
    * Substitue a virgula por ponto em tokens do tipo VALOR
    */
   static bool substituiDelSeValor(std::string &lexema);
   Token(const TipoToken tk = TipoToken::INITIAl, const std::string &s = "");
   /*
    * Todo token é implicitamente um TipoToken
    * apenas um shorthand para evitar tk.tipo == TipoToken::VALOR
    * visto que assim permite apenas: tk == TipoToken::VALOR
    */
   inline operator TipoToken() const { return id; }
};

/*
 * Função para printar o Token
 */
std::ostream &operator<<(std::ostream &, const Token &);
