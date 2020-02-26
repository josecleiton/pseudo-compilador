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

[[noreturn]] void usage(void) {
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
      std::cout << "[SINTATICO] - O seu programa foi aceito sintatica e "
                   "lexicamente!\n";
      auto nodeCounter = sem.analisaArvore();
#ifdef DEBUG
      std::cout << "[SINTATICO] - O total de nós da AST é: "
                << syn.getAST().size() << "\n[SEMANTICO] - Foram visitados "
                << nodeCounter << " nós (excluindo nós de expressão num)\n";
#endif
      std::cout << "[SEMANTICO] - O seu programa foi aceito!\n";
      std::cout << "[COMPILADOR] - O código fonte passou em todas as fases de "
                   "análise!\n";
      return EXIT_SUCCESS;
   } catch (const std::exception &e) {
#ifdef DEBUG
      std::cerr << "[DEBUG] Exception - " << e.what() << '\n';
#endif
      return EXIT_FAILURE;
   }
}
