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

#include "sem.hpp"
#include "syn.hpp"

using namespace AnaliseSintatica;
using namespace AnaliseSemantica;

void usage(void) {
   std::cerr << "Caminho do arquivo é requerido.\n";
   throw std::invalid_argument("Falta argv.");
}

int main(int argc, char *argv[]) {
   try {
      if (argc < 2) {
         usage();
      }
      Lex lex(argv[1]);
      Syn syn(lex);
      Sem sem(syn.parse());
      return EXIT_SUCCESS;
   } catch (const std::exception &e) {
#ifdef DEBUG
      std::cerr << "[DEBUG] Exception - " << e.what() << '\n';
#endif
      return EXIT_FAILURE;
   }
}
