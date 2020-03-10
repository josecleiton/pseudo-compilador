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
#include <fstream>

class Lex;
class Token;
struct Pos;

/**
 * Classe para formatar erros do compilador
 */
struct Erro : public std::exception {
  private:
   std::string msg;

  public:
   /*
    * Erro lexico
    */
   Erro(Lex& lex, std::string& lexema, const char* const esperado);
   Erro(Token& tk, const std::string& tipoErro, const char* const esperado);
   /* Erro(Syn* const syn; const Token& tk); */

   virtual inline const char* what() const throw() { return msg.c_str(); }

  private:
   const std::string& formataErro(const std::string&, const Pos, std::string&,
                                  const char* const);
   /*
    * deixa o buffer no inicio da linha/arquivo
    */
   std::size_t primeiroCaracterNaLinha() const;
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
      std::size_t count{};
      for (auto& c : lexema) {
         if (isspace(c) and c != ' ') {
            c = ' ';
            ++count;
         }
      }
      return count;
   }

   /*
    * Aponta para o caracter onde está o erro
    */
   inline std::string getSeta(const std::string& s,
                              const std::size_t col) const {
      std::string res = s;
      std::size_t t{};
      for (std::size_t i = 0; i < res.size(); ++i) {
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

namespace AnaliseSemantica {
struct ErroSemantico : public std::exception {
   mutable std::string msg;
   ErroSemantico(const std::string&);
   const char* what() const throw() override {
      msg = "erro semantico: " + msg;
      return msg.c_str();
   }
};
}  // namespace AnaliseSemantica
