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

inline void falhaAoAbrirArquivo(const std::string& path) {
   std::cerr << "Falha na abertura do arquivo: '" << path
             << "'. Verique se o caminho está "
                "correto.\n";
   throw std::runtime_error("Arquivo falhou ao abrir.");
}

Lex::Lex(const std::string& inPath, const std::string& outPath)
    : mFilename(inPath), mInputFile(mFilename), mOutputFile(outPath) {
   const std::string sufixo = ".c20192";
   const int pos = inPath.size() - sufixo.size() - 1;
   if (pos <= 0 || inPath.find(sufixo, pos) == std::string::npos) {
      std::cerr << "Arquivos de entrada devem terminar com '" << sufixo
                << "'\n";
      throw std::invalid_argument("Falta extensão '" + sufixo +
                                  "' em arquivo.");
   }
   if (!mInputFile.is_open()) {
      falhaAoAbrirArquivo(inPath);
   }
   if (!mOutputFile.is_open()) {
      falhaAoAbrirArquivo(outPath);
   }
   mOutputFile << "Lista de pares {COD_TOKEN, LEXEMA}\nO COD_TOKEN pode ser "
                  "checado no enum 'Token::TipoToken' em 'token.hpp'\nPS: "
                  "Comentários começam com '#' e vão até o final da linha\n\n";
}

Lex::~Lex() {
#ifdef DEBUG
   std::clog << "[DEBUG] Lex - Linhas lidas: " << mLinha << '\n';
#endif
   mInputFile.close();
   mOutputFile.close();
}

Token Lex::proxToken(void) {
   int estado = 0;
   std::string lexema;
   while (estado != EOF) {
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
                        mCol = 0;
                        mLinha++;
                     }
                     lexema.pop_back();
                     break;
                  } else {
                     throw Erro(this, lexema, "Σ");
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
            return Token(reservadaOuID(lexema), lexema);
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
               throw Erro(this, lexema, "[0-9]");
            }
            break;
         case 5:
            ungetChar(lexema);
            return Token(Token::TipoToken::VALOR, lexema);
         case 6:
            c = getChar(lexema);
            if (!isdigit(c)) {
               estado = 5;
            }
            break;
         case 7:
            return Token(Token::TipoToken::ATRIB, lexema);
         case 8:
            return Token(Token::TipoToken::SINAL, lexema);
         case 9:
            return Token(Token::TipoToken::FATOR_OP, lexema);
         case 12:
            c = getChar(lexema);
            if (c == '&') {
               return Token(Token::TipoToken::AND, lexema);
            } else {
               throw Erro(this, lexema, "&");
            }
            // error
            break;
         case 14:
            c = getChar(lexema);
            if (c == '|') {
               return Token(Token::TipoToken::OR, lexema);
            } else {
               throw Erro(this, lexema, "|");
            }
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
               estado = c;
            }
            break;
      }
   }
   return Token(Token::TipoToken::FIMARQ, "\"EOF\"");
}
