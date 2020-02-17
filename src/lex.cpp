/*
 * =====================================================================================
 *
 *       Filename:  lex.cpp
 *
 *    Description:
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

Lex::Lex(const std::string &filepath) : mFilename(filepath), mFile(mFilename) {
   const std::string sufixo = ".pl";
   const int pos = filepath.size() - sufixo.size() - 1;
   if (pos <= 0 || filepath.find(sufixo, pos) == std::string::npos) {
      std::cerr << "Arquivos de entrada devem terminar com '" << sufixo
                << "'\n";
      throw std::invalid_argument("Falta extensão '.pl' em arquivo.");
   }
   if (!mFile.is_open()) {
      std::cerr << "Falha na abertura do arquivo: '" << filepath
                << "'. Verique se o caminho está "
                   "correto.\n";
      throw std::runtime_error("Arquivo falhou ao abrir.");
   }
}

Lex::~Lex() {
#ifdef DEBUG
   std::clog << "[DEBUG] Lex - Linhas lidas: " << mLinha << '\n';
#endif
   mFile.close();
}

Token Lex::getToken(void) {
   int estado = 0;
   std::string lexema;
   Token::TipoToken tipoTk;
   const auto tipoErro = Erro::TipoErro::Lexico;
   while (true) {
      char c;
      /* std::cout << lexema << ' ' << estado << '\n'; */
      /* std::cout << "inCol:" << mCol << '\n'; */
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
               case '+':
                  estado = 8;
                  break;
               case '*':
                  estado = 9;
                  break;
               case '/':
                  estado = 10;
                  break;
               case '-':
                  estado = 11;
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
                  estado = 99;
                  break;

               default:
                  if (isalpha(c)) {
                     estado = 1;
                  } else if (isdigit(c)) {
                     estado = 3;
                  } else if (isspace(c)) {
                     if (c == '\n') {
                        mCol = 0;
                        mLinha++;
                     }
                     lexema.pop_back();
                     break;
                  } else {
                     throw Erro(this, tipoErro, lexema, "Σ");
                  }
            }
            break;
         case 1:
            c = getChar(lexema);
            if (!isalnum(c)) {
               estado = 2;
            }
            break;
         case 2:
            // std::cout << "Final do estado 2: " << lexema.back() << "\n";
            ungetChar(lexema);
            // std::cout << "Final do estado 2 (lexema): " << lexema << ".\n";
            if ((tipoTk = reservada(lexema)) != Token::TipoToken::INITIAl) {
               return Token(tipoTk, lexema);
               /* mTk = Token(tipoTk, lexema); */
            } else {
               return Token(Token::TipoToken::ID, lexema);
            }
            break;
         case 3:
            c = getChar(lexema);
            if (c == ',') {
               estado = 4;
            } else if (!isdigit(c)) {
               estado = 5;
            }
            break;
         case 4:
            c = getChar(lexema);
            if (isdigit(c)) {
               estado = 6;
            } else {
               throw Erro(this, tipoErro, lexema, "[0-9]");
            }
            break;
         case 5:
            ungetChar(lexema);
            return Token(Token::TipoToken::VALOR, lexema);
            break;

         case 6:
            c = getChar(lexema);
            if (!isdigit(c)) {
               estado = 5;
            }
            break;
         case 7:
            return Token(Token::TipoToken::ATRIB, lexema);
         case 8:
         case 9:
         case 10:
         case 11:
            return Token(Token::TipoToken::OPA, lexema);
         case 12:
            c = getChar(lexema);
            if (c == '&') {
               estado = 13;
            } else {
               throw Erro(this, tipoErro, lexema, "&");
            }
            // error
            break;
         case 13:
            return Token(Token::TipoToken::OPB, lexema);
         case 14:
            c = getChar(lexema);
            if (c == '|') {
               estado = 13;
            } else {
               throw Erro(this, tipoErro, lexema, "|");
            }
            // error
            break;
         case 16:
            c = getChar(lexema);
            if (c != '!') {
               estado = 17;
               break;
            }
            break;
         case 17:
            ungetChar(lexema);
            return Token(Token::TipoToken::NEG, lexema);
         case 18:
            return Token(Token::TipoToken::PNTVIRG, lexema);
         case 19:
            return Token(Token::TipoToken::ABREPRNT, lexema);
         case 20:
            return Token(Token::TipoToken::FECHAPRNT, lexema);
         case 21:
            c = getChar(lexema);
            if (isspace(c) && c != ' ') {
#ifdef DEBUG
               lexema.pop_back();
               std::clog << "{Comentário: " << lexema << "}\n";
#endif
               lexema.clear();
               estado = 0;
            } else if (c == EOF) {
               estado = 99;
            }
            break;
         case 99:
            ungetChar(lexema);
            return Token(Token::TipoToken::FIMARQ, lexema);
      }
   }
   return Token(Token::TipoToken::FIMARQ, lexema);
}
