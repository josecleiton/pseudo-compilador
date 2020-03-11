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
#include "stable.hpp"
#include "token.hpp"

std::ifstream Erro::sFile;

Erro::Erro(const Pos &_pos, std::string _lexema,
           const std::string_view _esperado)
    : mPos(_pos), mLexema(_lexema), mEsperado(_esperado) {
   abreArq();
}

// Erro::Erro(Token &tk, const std::string_view tipoErro,
//            const std::string_view esperado) {
//    abreArq();
//    const Pos pos = tk.getPos();
// #ifdef DEBUG
//    std::clog << "[DEBUG - erro] token pos: " << pos << '\n';
// #endif
//    formataErro(tipoErro, pos, tk.lexema, esperado);
// }

void Erro::formataErro(void) const {
   abreArq();
   std::string textoLinha;
   getLinha(textoLinha);
   std::ostringstream ss1, ss2;
   // primeiroCaracterNaLinha();
   const auto limpos = limpaLexema();
   mPos.col -= limpos + 1;
   Token::substituiDelSeValor(mLexema);
   //  std::clog << "texto: " << textoLinha << '\n';
   /* std::clog << "LINHA:" << textoLinha << ". COL:" << col */
   /*           << ". PCL:" << posPrimeiroC << " " << textoLinha.size() << "\n";
    */
   ss1 << '\t' << mPos.linha << " |\t" << textoLinha;
   /* std::clog << ss.str().length() << " " << ss.tellp() << "\n"; */
   std::string sstr = ss1.str();
   const auto seta = getSeta(sstr);
   ss2 << G_filepath.filename() << ':' << mPos.linha << ':' << mPos.col;
   // formataStringStream(ss2, sstr, seta);
   formataStringStream(ss2, textoLinha, sstr, seta);
   mMsg = ss2.str();
}

std::string_view Erro::getLinha(std::string &str) const {
   for (auto linha = mPos.linha; linha; linha--) {
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

ErroLexico::ErroLexico(Lex &lex, std::string &lexema,
                       const std::string_view esperado)
    : Erro(Pos(lex.getLinha(), lex.getCol()), lexema, esperado) {}

void ErroLexico::formataStringStream(std::ostringstream &ss,
                                     const std::string_view linhaErro,
                                     const std::string_view linhaFormatada,
                                     const std::string_view seta) const {
   ss << ". lexico: esperado '" << mEsperado << "' recebeu '"
      << linhaErro[mPos.col] << "'\n"
      << linhaFormatada << '\n'
      << seta;
}

namespace AnaliseSintatica {
ErroSintatico::ErroSintatico(const Token &tk, const std::string_view esperado)
    : Erro(tk.getPos(), tk.lexema, esperado) {}
void ErroSintatico::formataStringStream(std::ostringstream &ss,
                                        const std::string_view,
                                        const std::string_view linhaFormatada,
                                        const std::string_view seta) const {
   ss << ". sintatico: " << mEsperado
      << "\nRevise as regras gramaticais da linguagem.\n"
      << linhaFormatada << '\n'
      << seta;
}
}  // namespace AnaliseSintatica

namespace AnaliseSemantica {

ErroSemantico::ErroSemantico(const Token &tk, const std::string_view esperado)
    : ErroSintatico(tk, esperado) {}
ErroSemantico::ErroSemantico(const Token &tk, const Dado d1, const Dado d2)
    : ErroSintatico(tk, formataEsperado(d1, d2)) {}
void ErroSemantico::formataStringStream(std::ostringstream &ss,
                                        const std::string_view,
                                        const std::string_view linhaFormatada,
                                        const std::string_view seta) const {
   ss << ". semantico: " << mEsperado
      << "\nRevise as regras semanticas da linguagem.\n"
      << linhaFormatada << '\n'
      << seta;
}
std::string formataEsperado(const Dado d1, const Dado d2) {
   return std::string("Tipo '") + tipoLexema(d1) + std::string("' não é compatível a tipo '") +
          tipoLexema(d2) + std::string("'.");
}
}  // namespace AnaliseSemantica
