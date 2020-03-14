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
#include <sstream>

#include "token.hpp"
enum class TipoDado;

class Lex;

/**
 * Classe para formatar erros do compilador
 */
struct Erro : public std::exception {
  protected:
   mutable Pos mPos;
   mutable std::string mMsg, mLexema;
   std::string mEsperado;

  public:
   /*
    * Erro lexico
    */
   Erro(const Pos &, std::string lexema, const std::string_view esperado);
   // Erro(Token &tk, const std::string_view tipoErro,
   //      const std::string_view esperado = "");
   /* Erro(Syn* const syn; const Token& tk); */

   const char *what() const throw() override {
      if (mMsg.empty()) {
         formataErro();
      }
      return mMsg.c_str();
   }
   virtual ~Erro() = default;

  protected:
   void formataErro(void) const;

   /*
    * getLinha - pega a linha n  coloca na str
    */
   std::string_view getLinha(std::ifstream &file, std::string &str) const;
   /*
    * Substitui \r ou \n ou \t por espaço em branco
    */
   std::size_t limpaLexema(void) const {
      std::size_t count{};
      for (auto &c : mLexema) {
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
   std::string getSeta(const std::string &s) const;

  protected:
   virtual void formataStringStream(std::ostringstream &,
                                    const std::string_view,
                                    const std::string_view,
                                    const std::string_view) const {}
};

struct ErroLexico : public Erro {
   ErroLexico(Lex &lex, std::string &lexema, const std::string_view esperado);
   ~ErroLexico() = default;

  protected:
   void formataStringStream(std::ostringstream &,
                            const std::string_view linhaErro,
                            const std::string_view linhaFormatada,
                            const std::string_view seta) const override;
};

namespace AnaliseSintatica {
struct ErroSintatico : public Erro {
   ErroSintatico(const Token &tk, const std::string_view esperado);
   virtual ~ErroSintatico() = default;

  protected:
   virtual void formataStringStream(std::ostringstream &,
                                    const std::string_view linhaErro,
                                    const std::string_view linhaFormatada,
                                    const std::string_view seta) const override;
};
}  // namespace AnaliseSintatica

namespace AnaliseSemantica {
struct ErroSemantico : public AnaliseSintatica::ErroSintatico {
   ErroSemantico(const Token &tk, const std::string_view esperado);
   ErroSemantico(const Token &tk, const TipoDado, const TipoDado);
   ~ErroSemantico() = default;

  protected:
   void formataStringStream(std::ostringstream &,
                            const std::string_view linhaErro,
                            const std::string_view linhaFormatada,
                            const std::string_view seta) const override;
};

std::string formataEsperado(const TipoDado, const TipoDado);
}  // namespace AnaliseSemantica
