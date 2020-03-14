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
#include <string>

#ifndef DEBUG
#define DEBUG 1
#include <iostream>
#endif

#include "enum/tipo_token.hpp"

struct Pos {
   long long linha;
   long long col;
   constexpr Pos(const long long _linha = 0, const long long _col = 0)
       : linha(_linha), col(_col) {}
};

class Token {
  private:
   Pos mPos;

  public:
   TipoToken tipo;
   std::string lexema;
   /*
    * Substitue a virgula por ponto em tokens do tipo VALOR
    */
   static bool substituiDelSeValor(std::string &lexema);
   Token(const TipoToken tk = TipoToken::INITIAl, const std::string & = "",
         const int linha = -1, const int col = -1);
   const auto &getPos(void) const { return mPos; }
   std::ostream &print(std::ostream &out) const {
      out << '{' << static_cast<int>(tipo) << ", " << lexema << '}';
      return out;
   }
   /*
    * Todo token é implicitamente um TipoToken
    * apenas um shorthand para evitar tk.tipo == TipoToken::VALOR
    * visto que assim permite apenas: tk == TipoToken::VALOR
    */
   operator TipoToken() const { return tipo; }
};

/*
 * Função para printar o Token
 */
std::ostream &operator<<(std::ostream &, const Token &);
std::ostream &operator<<(std::ostream &, const Pos &);
