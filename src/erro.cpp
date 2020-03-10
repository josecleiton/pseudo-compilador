/*
 * =====================================================================================
 *
 *       Filename:  erro.cpp
 *
 *    Description: Erro
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

#include "global.hpp"
#include "lex.hpp"
#include "token.hpp"

Erro::Erro(Lex& lex, std::string& lexema, const char* const esperado) {
   const long long linha = lex.getLinha(), col = lex.getCol();
   formataErro("lexico", {linha, col}, lexema, esperado);
   /* const auto linha = lex->getLinha(); */
   /* auto col = lex->getCol(); */
   /* const auto& filename = lex->getFilename(); */
   /* auto& file = lex->getFile(); */
   /* std::string textoLinha{}; */
   /* std::stringstream ss; */
   /* primeiroCaracterNaLinha(file); */
   /* const auto limpos = limpaLexema(lexema); */
   /* col -= limpos + 1; */
   /* Token::substituiDelSeValor(lexema); */
   /* std::getline(file, textoLinha); */
   /* ss << '\t' << linha << " |\t" << textoLinha; */
   /* std::string sstr = ss.str(); */
   /* ss.clear(); */
   /* const auto seta = getSeta(sstr, col); */

   /* ss << filename << ':' << linha << ':' << col << ": erro lexico: esperado
    * '" */
   /*    << esperado << "' recebeu '" << textoLinha[col] << "'\n" */
   /*    << sstr << '\n' */
   /*    << seta << '\n'; */
   /* msg = ss.str(); */
}

Erro::Erro(Token& tk, const std::string& tipoErro, const char* const esperado) {
   formataErro(tipoErro, tk.getPos(), tk.lexema, esperado);
}

const std::string& Erro::formataErro(const std::string& tipoErro, const Pos pos,
                                     std::string& lexema,
                                     const char* const esperado) {
   auto [linha, col] = pos;
   std::string textoLinha{};
   std::stringstream ss1, ss2;
   primeiroCaracterNaLinha();
   const auto limpos = limpaLexema(lexema);
   col -= limpos + 1;
   Token::substituiDelSeValor(lexema);
   std::getline(*G_file, textoLinha);
   // std::clog << "CURSOR: " << cursor << " / texto: " << textoLinha << '\n';
   /* std::clog << "LINHA:" << textoLinha << ". COL:" << col */
   /*           << ". PCL:" << posPrimeiroC << " " << textoLinha.size() << "\n";
    */
   ss1 << '\t' << linha << " |\t" << textoLinha;
   /* std::clog << ss.str().length() << " " << ss.tellp() << "\n"; */
   std::string sstr = ss1.str();
   const auto seta = getSeta(sstr, col);
   ss2 << G_filename << ':' << linha << ':' << col
       << ": erro " + tipoErro + ": esperado '" << esperado << "' recebeu '"
       << textoLinha[col] << "'\n"
       << sstr << '\n'
       << seta << '\n';
   return msg = ss2.str();
}

std::size_t Erro::primeiroCaracterNaLinha(void) const {
   std::size_t count = 1;
   char c;
   auto pos = unget(*G_file, 2);
   while ((c = G_file->get()) != EOF and pos and c != '\n' and c != '\r') {
      ++count;
      pos = unget(*G_file, 2);
   }
   if (!pos) {
      G_file->unget();
      count--;
   }
   return count;
}
namespace AnaliseSemantica {
ErroSemantico::ErroSemantico(const std::string& _m) : msg(_m) {}
}  // namespace AnaliseSemantica
