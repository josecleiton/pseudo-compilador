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

#include <iostream>

#include "lex.hpp"

inline void usage(void) { std::cerr << "Caminho do arquivo é requerido.\n"; }

int main(int argc, char *argv[]) {
   if (argc < 2) {
      usage();
      return EXIT_FAILURE;
   }
   try {
      Lex lex(argv[1]);
      Token tk;
      while ((tk = lex.getToken()).id != Token::TipoToken::FIMARQ) {
         std::cout << tk << '\n';
      }
      return EXIT_SUCCESS;
   } catch (const std::exception &e) {
#ifdef DEBUG
      std::cerr << "[DEBUG] Exception - " << e.what() << '\n';
#endif
      return EXIT_FAILURE;
   }
}