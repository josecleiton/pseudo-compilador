/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/23/2020 08:07:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include <list>
#include <iostream>

#include "ast.hpp"
using namespace std;
using namespace AnaliseSintatica;
using namespace AnaliseSemantica;

int main() {
   list<unique_ptr<AST::Node>> l;
   l.emplace_back(make_unique<AST::NodeBloco>(Token(), AST::Tipo::BLOCO, nullptr));
   l.emplace_back(make_unique<AST::Node>(Token(), AST::Tipo::REGULAR, l.front().get()));
   auto t = dynamic_cast<AST::NodeBloco*>(l.front().get());
   cout << t << '\n';
   return 0;
}

