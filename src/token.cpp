/*
 * =====================================================================================
 *
 *       Filename:  token.cpp
 *
 *    Description:
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

Token::Token(const Token::TipoToken& tk, const std::string& s)
    : id(tk), lexema(s) {}

bool Token::substituiDelSeValor(std::string& lexema) {
   std::size_t i = 0;
   while (i < lexema.size() and lexema[i] >= '0' and lexema[i] <= '9') {
      i++;
   }
   if (lexema[i] == '.') {
      lexema[i] = ',';
      return true;
   }
   return false;
}

std::ostream& printToken(std::ostream& o, const Token& tk) {
   o << "{" << static_cast<int>(tk.id) << ", " << tk.lexema << "}";
   return o;
}

std::ostream& operator<<(std::ostream& out, const Token& tk) {
   if (tk == Token::TipoToken::VALOR) {
      Token novoTk = tk;
      Token::substituiDelSeValor(novoTk.lexema);
      return printToken(out, novoTk);
   }
   return printToken(out, tk);
}
