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

Lex::Lex(const std::string& file)
    : mFile(file),
      mPalavrasReservadas({{"SE", Token::TipoToken::SE},
                           {"FACA", Token::TipoToken::FACA},
                           {"ACABOU", Token::TipoToken::ACABOU},
                           {"SENAO", Token::TipoToken::SENAO},
                           {"ENQUANTO", Token::TipoToken::ENQUANTO}}) {}

Token Lex::getToken(void) {
   int estado = 0;
   std::string lexema;
   enum { ID = 46 };
   Token::TipoToken tipoTk;
   while (!mFile.eof()) {
      char c;
      std::cout << lexema << '\n';
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
               case -1:
                  estado = 99;
                  break;

               default:
                  if (isalpha(c)) {
                     estado = 1;
                  } else if (isdigit(c)) {
                     estado = 3;
                  } else {
                     exit(EXIT_FAILURE);
                  }
            }
            break;
         case 1:
            c = getChar(lexema);
            std::cout << c << ", ";
            if (!(isalpha(c) or isdigit(c))) {
               estado = 2;
            }
            break;
         case 2:
            ungetChar(lexema);
            tipoTk = reservada(lexema);
            if (tipoTk != Token::TipoToken::INITIAl) {
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
            } else {
               estado = 5;
            }
            break;
         case 4:
            c = getChar(lexema);
            if (isdigit(c)) {
               estado = 6;
            } else {
               exit(EXIT_FAILURE);
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
            }
            exit(EXIT_FAILURE);
            // error
            break;
         case 13:
            return Token(Token::TipoToken::OPB, lexema);
         case 14:
            c = getChar(lexema);
            if (c == '|') {
               estado = 15;
            }
            exit(EXIT_FAILURE);
            // error
            break;
         case 15:
            return Token(Token::TipoToken::OPB, lexema);
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
         case 99:
            ungetChar(lexema);
            return Token(Token::TipoToken::FIMARQ, lexema);
      }
   }
}

