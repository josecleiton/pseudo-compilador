/*
 * =====================================================================================
 *
 *       Filename:  error.hpp
 *
 *    Description: Objeto Erro com algumas perfumarias
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

#include "lex.hpp"

class Erro {
  public:
   enum class TipoErro { Lexico, Sintatico, Semantico };
   Erro(Lex* const lex, const TipoErro& tipo, std::string& lexema,
        const char* const esperado);

  private:
   std::size_t primeiroCaracterNaLinha(std::ifstream& file) const;
   inline int unget(std::ifstream& file, int n) const {
      long pos;
      while (n--) {
         file.unget();
         if ((pos = file.tellg()) < 1) {
            pos = 0;
            break;
         }
      }
      return pos;
   }
   inline int limpaLexema(std::string& lexema) const {
      if (lexema.size() && isspace(lexema.back()) && lexema.back() != ' ') {
         lexema.pop_back();
         lexema += ' ';
         return limpaLexema(lexema) + 1;
      }
      return 0;
   }
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
