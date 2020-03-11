/*
 * =====================================================================================
 *
 *       Filename:  token.cpp
 *
 *    Description: Tokens
 *
 *        Version:  1.0
 *        Created:  02/13/2020 09:55:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "token.hpp"

Token::Token(const TipoToken _tipo, const std::string& _lexema,
             const int _linha, const int _col)
    : mPos({_linha, _col}), tipo(_tipo), lexema(_lexema) {}

bool Token::substituiDelSeValor(std::string& lexema) {
   std::size_t i = 0;
   while (i < lexema.size() and lexema[i] >= '0' and lexema[i] <= '9') {
      ++i;
   }
   if (lexema[i] != '.') {
      return false;
   }
   lexema[i] = ',';
   return true;
}

std::ostream& operator<<(std::ostream& out, const Token& tk) {
   if (tk == TipoToken::VALOR) {
      Token novoTk = tk;
      Token::substituiDelSeValor(novoTk.lexema);
      return novoTk.print(out);
   }
   return tk.print(out);
}

Pos::Pos(const long long _linha, const long long _col): linha(_linha), col(_col) {}

std::ostream& operator<<(std::ostream& out, const Pos& pos) {
   out << '(' << pos.linha << ", " << pos.col << ')';
   return out;
}
