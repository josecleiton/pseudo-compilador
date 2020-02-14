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

std::ostream& operator<<(std::ostream& o, const Token& tk) {
   o << "{" << static_cast<int>(tk.id) << ", " << tk.lexema << "}";
   return o;
}
