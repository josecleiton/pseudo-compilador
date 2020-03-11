/*
 * =====================================================================================
 *
 *       Filename:  lex.cpp
 *
 *    Description: Analisador léxico
 *
 *        Version:  1.0
 *        Created:  02/13/2020 03:13:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#include "lex.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

#include "erro.hpp"
#include "global.hpp"
#include "token.hpp"

[[noreturn]] void falhaAoAbrirArquivo(const fs::path& path) {
   std::cerr << "Falha na abertura do arquivo: '" << path
             << "'. Verique se o caminho está "
                "correto.\n";
   throw std::runtime_error("Arquivo falhou ao abrir.");
}

Lex::Lex(const fs::path& in, const fs::path& out)
    : mInputFile(in),
      mOutputFile(out),
      mPalavrasReservadas{{"SE", TipoToken::SE},
                          {"FACA", TipoToken::FACA},
                          {"ACABOU", TipoToken::ACABOU},
                          {"SENAO", TipoToken::SENAO},
                          {"ENQUANTO", TipoToken::ENQUANTO},
                          {"INTEIRO", TipoToken::TIPO},
                          {"QUEBRADO", TipoToken::TIPO},
                          {"LOGICO", TipoToken::TIPO}} {
   /*
    * Verifica se o arquivo de entrada tem o sufixo .c20192
    */
   const std::string filename = in.filename();
   const std::string sufixo = ".c20192";
   const auto pos = filename.size() - sufixo.size() - 1;
   if (pos <= 0 or filename.find(sufixo, pos) == std::string::npos) {
      std::cerr << "Arquivos de entrada devem terminar com '" << sufixo
                << "'\n";
      throw std::invalid_argument("Falta extensão '" + sufixo +
                                  "' em arquivo.");
   }
   if (!mInputFile.is_open()) {
      falhaAoAbrirArquivo(in);
   }
   if (!mOutputFile.is_open()) {
      falhaAoAbrirArquivo(out);
   }

   G_filepath = in;
   /*
    * Cabeçalho de ajuda no arquivo de saída
    */
   mOutputFile << "Lista de pares {COD_TOKEN, LEXEMA}\nO COD_TOKEN pode ser "
                  "checado no enum 'TipoToken' em 'enum/tipo_token.hpp'\nPS: "
                  "Comentários começam com '#' e vão até o final da linha\n\n";
}

Lex::~Lex() {
#ifdef DEBUG
   std::clog << "[DEBUG] Lex - Linhas lidas: " << mLinhaCount << std::endl;
#endif
}

Token Lex::getToken(void) {
   const auto tk = proxToken();
   mOutputFile << tk << '\n';
   return tk;
}

void Lex::analiseAteEOF(void) {
   for (auto tk = getToken(); tk != TipoToken::FIMARQ; tk = getToken()) {
      std::cout << tk << '\n';
   }
}

Token Lex::proxToken(void) {
   int estado = 0;      // representa o q0 (estado inicial do afd)
   char c;              // representa a cabeça da fita
   std::string lexema;  // string lida
   while (estado != EOF) {
      switch (estado) {
         case 0:
            c = getChar(lexema);
            switch (c) {
               case '=':
                  estado = 7;
                  break;
               case ',':
                  estado = 4;
                  break;
               case '-':
               case '+':
                  estado = 8;
                  break;
               case '*':
               case '/':
                  estado = 9;
                  break;
               case '&':
                  estado = 12;
                  break;
               case '|':
                  estado = 14;
                  break;
               case '!':
                  estado = 16;
                  break;
               case ';':
                  estado = 18;
                  break;
               case '(':
                  estado = 19;
                  break;
               case ')':
                  estado = 20;
                  break;
               case '#':
                  estado = 21;
                  break;
               case EOF:
                  estado = c;
                  break;

               default:
                  if (isalpha(c)) {
                     estado = 1;
                  } else if (isdigit(c)) {
                     estado = 3;
                  } else if (isspace(c)) {
                     if (c == '\n') {
                        proximaLinha();
                     }
                     lexema.pop_back();
                     break;
                  } else {
                     throw ErroLexico(*this, lexema, "Σ");
                  }
            }
            break;
         case 1:
            if (c = getChar(lexema); !isalnum(c)) {
               estado = 2;
            }
            break;
         case 2:
            ungetChar(lexema);
            return criaToken(reservadaOuID(lexema), lexema);
         case 3:
            if (c = getChar(lexema); c == ',') {
               estado = 4;
            } else if (!isdigit(c)) {
               estado = 5;
            }
            break;
         case 4:
            /*
             * Substitue a ',' por '.' para facilitar o casting para double no
             * semântico
             */
            lexema.back() = '.';
            if (c = getChar(lexema); isdigit(c)) {
               estado = 6;
            } else {
               throw ErroLexico(*this, lexema, "[0-9]");
            }
            break;
         case 5:
            ungetChar(lexema);
            return criaToken(TipoToken::VALOR, lexema);
         case 6:
            if (c = getChar(lexema); !isdigit(c)) {
               estado = 5;
            }
            break;
         case 7:
            return criaToken(TipoToken::ATRIB, lexema);
         case 8:
            return criaToken(TipoToken::SINAL, lexema);
         case 9:
            return criaToken(TipoToken::BINOP, lexema);
         case 12:
            if (c = getChar(lexema); c == '&') {
               return criaToken(TipoToken::BINOP, lexema);
            }
            throw ErroLexico(*this, lexema, "&");
            // error
            break;
         case 14:
            if (c = getChar(lexema); c == '|') {
               return criaToken(TipoToken::BINOP, lexema);
            }
            throw ErroLexico(*this, lexema, "|");
            break;
         case 16:
            return criaToken(TipoToken::NEG, lexema);
         case 18:
            return criaToken(TipoToken::PNTVIRG, lexema);
         case 19:
            return criaToken(TipoToken::ABREPRNT, lexema);
         case 20:
            return criaToken(TipoToken::FECHAPRNT, lexema);
         case 21:
            /* Estado de Comentário na linguagem */
            if (c = getChar(lexema); isspace(c) and c == '\n') {
#ifdef DEBUG
               lexema.pop_back();
               std::clog << "[DEBUG - Lex] {Comentário: " << lexema << '}'
                         << std::endl;
#endif
               lexema.clear();
               estado = 0;
               proximaLinha();
            } else if (c == EOF) {
               estado = c;
            }
            break;
      }
   }
   return criaToken(TipoToken::FIMARQ, "\"EOF\"");
}

TipoToken Lex::reservadaOuID(const std::string& lexema) const {
   try {
      return mPalavrasReservadas.at(lexema);
   } catch (const std::out_of_range&) {
      return TipoToken::ID;
   }
}

Token Lex::criaToken(const TipoToken tipo, const std::string& lexema) const {
   return Token(tipo, lexema, mLinhaCount, mColCount);
}
