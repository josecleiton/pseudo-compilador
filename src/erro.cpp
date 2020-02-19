/*
 * =====================================================================================
 *
 *       Filename:  erro.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/15/2020 05:26:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "erro.hpp"

#include <iostream>
#include <sstream>

Erro::Erro(Lex* const lex, std::string& lexema, const char* const esperado)
    : mTipo(Erro::TipoErro::Lexico) {
   const auto linha = lex->getLinha();
   auto col = lex->getCol();
   const auto filename = lex->getFilename();
   auto& file = lex->getFile();
   std::string textoLinha{};
   std::stringstream ss;
   primeiroCaracterNaLinha(file);
   const auto limpos = limpaLexema(lexema);
   col -= limpos + 1;
   std::getline(file, textoLinha);
   // std::clog << "CURSOR: " << cursor << " / texto: " << textoLinha << '\n';
   /* std::clog << "LINHA:" << textoLinha << ". COL:" << col */
   /*           << ". PCL:" << posPrimeiroC << " " << textoLinha.size() << "\n";
    */
   ss << '\t' << linha << " |\t" << textoLinha;
   /* std::clog << ss.str().length() << " " << ss.tellp() << "\n"; */
   std::string sstr = ss.str();
   const auto seta = getSeta(sstr, col);

   std::clog << filename << ':' << linha << ':' << col
             << ": erro lexico: esperado '" << esperado << "' recebeu '"
             << textoLinha[col] << "'\n"
             << sstr << '\n'
             << seta << '\n';
}

std::size_t Erro::primeiroCaracterNaLinha(std::ifstream& file) const {
   std::size_t count = 1;
   char c;
   auto pos = unget(file, 2);
   while ((c = file.get()) != EOF && pos && c != '\n' && c != '\r') {
      count++;
      pos = unget(file, 2);
   }
   if (!pos) {
      file.unget();
      count--;
   }
   return count;
}
