/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description: BIRL
 *
 *        Version:  1.0
 *        Created:  02/13/2020 10:31:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "lex.hpp"
#include <iostream>

void usage(void) {
  std::cerr << "Caminho do arquivo é requirido.\n";
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
  }
  Lex lex(argv[1]);
  Token tk{};
  while ((tk = lex.getToken()).id != Token::TipoToken::FIMARQ) {
    std::cout << tk << '\n';
  }
}
