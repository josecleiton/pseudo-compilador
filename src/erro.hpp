/*
 * =====================================================================================
 *
 *       Filename:  error.hpp
 *
 *    Description: Formatador de Erros (perfumarias)
 *
 *        Version:  1.0
 *        Created:  02/15/2020 05:16:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#pragma once
#include <cctype>
#include <exception>

#include "lex.hpp"

/**
 * Classe para formatar erros do compilador
 */
class Erro : public std::exception {
  public:
   enum class TipoErro { Lexico, Sintatico, Semantico };

  private:
   TipoErro mTipo;

  public:
   /*
    * Erro lexico
    */
   Erro(Lex* const lex, std::string& lexema, const char* const esperado);
   /* Erro(Syn* const syn; const Token& tk); */

   const char* what() const throw() {
      switch (mTipo) {
         case TipoErro::Lexico:
            return "Erro Léxico.";
         case TipoErro::Sintatico:
            return "Erro Sintático.";
         case TipoErro::Semantico:
            return "Erro Semântico.";
         default:
            return "Erro desconhecido.";
      }
   }

  private:
   /*
    * deixa o buffer no inicio da linha/arquivo
    */
   std::size_t primeiroCaracterNaLinha(std::ifstream& file) const;
   /*
    * volta o buffer n vezes ou até o início
    * (função auxiliar a acima)
    */
   inline int unget(std::ifstream& file, int n) const {
      long pos{};
      while (n--) {
         file.unget();
         if ((pos = file.tellg()) < 1) {
            pos = 0;
            break;
         }
      }
      return pos;
   }
   /*
    * Substitui \r ou \n por espaço em branco
    */
   inline std::size_t limpaLexema(std::string& lexema) const {
      std::size_t ac{};
      for (auto& c : lexema) {
         if (isspace(c) && c != ' ') {
            c = ' ';
            ac++;
         }
      }
      return ac;
   }

   /*
    * Aponta para o caracter onde está o erro
    */
   inline std::string getSeta(const std::string& s, const unsigned col) const {
      std::string res = s;
      std::size_t t{};
      for (std::size_t i = 0; i < res.size(); i++) {
         if (!isspace(res[i])) {
            res[i] = ' ';
         } else if (res[i] == '\t') {
            t = i;
         }
      }
      res[t + col + 1] = '^';
      return res;
   }
};
