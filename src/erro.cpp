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

std::ifstream Erro::sFile;

Erro::Erro(Lex &lex, std::string &lexema, const std::string_view esperado) {
   abreArq();
   const long long linha = lex.getLinha(), col = lex.getCol();
   formataErro("lexico", {linha, col}, lexema, esperado);
}

Erro::Erro(Token &tk, const std::string_view tipoErro,
           const std::string_view esperado) {
   abreArq();
   const Pos pos = tk.getPos();
#ifdef DEBUG
   std::clog << "[DEBUG - erro] token pos: " << pos << '\n';
#endif
   formataErro(tipoErro, pos, tk.lexema, esperado);
}

void Erro::formataErro(const std::string_view tipoErro, const Pos &pos,
                       std::string &lexema, const std::string_view esperado) {
   abreArq();
   const auto linha = pos.linha;
   auto col = pos.col;
   std::string textoLinha;
   getLinha(textoLinha, pos.linha);
   std::ostringstream ss1, ss2;
   // primeiroCaracterNaLinha();
   const auto limpos = limpaLexema(lexema);
   col -= limpos + 1;
   Token::substituiDelSeValor(lexema);
  //  std::clog << "texto: " << textoLinha << '\n';
   /* std::clog << "LINHA:" << textoLinha << ". COL:" << col */
   /*           << ". PCL:" << posPrimeiroC << " " << textoLinha.size() << "\n";
    */
   ss1 << '\t' << linha << " |\t" << textoLinha;
   /* std::clog << ss.str().length() << " " << ss.tellp() << "\n"; */
   std::string sstr = ss1.str();
   const auto seta = getSeta(sstr, col);
   ss2 << G_filepath.filename() << ':' << linha << ':' << col << ": erro " << tipoErro
       << ": esperado '" << esperado << "' recebeu '" << textoLinha[col]
       << "'\n"
       << sstr << '\n'
       << seta << '\n';
   msg = ss2.str();
}

std::string_view Erro::getLinha(std::string &str, unsigned long long linha) {
   while (linha--) {
      std::getline(sFile, str);
   }
   return str;
}

void Erro::abreArq(void) {
   if (!sFile.is_open()) {
      sFile.open(G_filepath);
      if (!sFile.is_open()) {
         throw std::runtime_error(
             "Arquivo não foi aberto para a formatação do Erro.");
      }
   }
}

namespace AnaliseSemantica {
ErroSemantico::ErroSemantico(const std::string &_m) : msg(_m) {}
}  // namespace AnaliseSemantica
