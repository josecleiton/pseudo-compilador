/*
 * =====================================================================================
 *
 *       Filename:  ast.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/21/2020 11:26:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "ast.hpp"

typedef Token::TipoToken TipoToken;

namespace AnaliseSemantica {}
namespace AnaliseSintatica {
AST::AST(void)
    : root(std::make_unique<Node>(Token(TipoToken::PROGRAMA),
                                  AST::Tipo::BLOCO)) {}
AST::Node::Node(const Token& _tk, const Tipo& _t) : tk(_tk), tipo(_t) {}
}  // namespace AnaliseSintatica

