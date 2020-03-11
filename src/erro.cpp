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

Erro::Erro(const Pos &_pos, std::string _lexema,
           const std::string_view _esperado)
    : mPos(_pos), mLexema(_lexema), mEsperado(_esperado) {}

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
   /*
    * Reabre o arquivo de entrada (fornecido pelo usuario)
    * para pegar a linha onde ocorreu o erro e mostrar adequadamente no terminal
    */
   std::ifstream file(G_filepath);
   if (!file.is_open()) {
      throw std::runtime_error("Arquivo para formatação do Erro não abriu.");
   }
   std::string textoLinha;
   getLinha(file, textoLinha);
   std::ostringstream ss1, ss2;
   // primeiroCaracterNaLinha();
   const auto limpos = limpaLexema();
   mPos.col -= limpos + 1;
   Token::substituiDelSeValor(mLexema);
   ss1 << '\t' << mPos.linha << " |\t" << textoLinha;
   std::string sstr = ss1.str();
   const auto seta = getSeta(sstr);
   ss2 << G_filepath.filename() << ':' << mPos.linha << ':' << mPos.col;
   /*
    * Funçao polimorfica, formata a mensagem de jeitos diferentes, dependendo de
    * onde ocorreu.
    */
   formataStringStream(ss2, textoLinha, sstr, seta);
   mMsg = ss2.str();
}

std::string_view Erro::getLinha(std::ifstream &file, std::string &str) const {
   for (auto linha = mPos.linha; linha; linha--) {
      std::getline(file, str);
   }
   return str;
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
ErroSemantico::ErroSemantico(const Token &tk, const TipoDado t1,
                             const TipoDado t2)
    : ErroSintatico(tk, formataEsperado(t1, t2)) {}
void ErroSemantico::formataStringStream(std::ostringstream &ss,
                                        const std::string_view,
                                        const std::string_view linhaFormatada,
                                        const std::string_view seta) const {
   ss << ". semantico: " << mEsperado
      << "\nRevise as regras semanticas da linguagem.\n"
      << linhaFormatada << '\n'
      << seta;
}
std::string formataEsperado(const TipoDado t1, const TipoDado t2) {
   return std::string("Tipo '") + tipoLexema(t1) +
          std::string("' não é compatível a tipo '") + tipoLexema(t2) +
          std::string("'.");
}
}  // namespace AnaliseSemantica
