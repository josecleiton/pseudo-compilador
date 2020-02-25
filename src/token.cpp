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

Token::Token(const Token::TipoToken _tipo, const std::string& _lexema)
    : tipo(_tipo), lexema(_lexema) {}

bool Token::substituiDelSeValor(std::string& lexema) {
   std::size_t i = 0;
   while (i < lexema.size() and lexema[i] >= '0' and lexema[i] <= '9') {
      ++i;
   }
   if (lexema[i] == '.') {
      lexema[i] = ',';
      return true;
   }
   return false;
}

std::ostream& printToken(std::ostream& out, const Token& tk) {
   out << "{" << static_cast<int>(tk.tipo) << ", " << tk.lexema << "}";
   return out;
}

std::ostream& operator<<(std::ostream& out, const Token& tk) {
   if (tk == Token::TipoToken::VALOR) {
      Token novoTk = tk;
      Token::substituiDelSeValor(novoTk.lexema);
      return printToken(out, novoTk);
   }
   return printToken(out, tk);
}
