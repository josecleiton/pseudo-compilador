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

Lex::Lex(std::ifstream& file) {
   int estado = 0;
   char c;
   std::string lexema;
   bool aceitacao = false;
   enum { ID = 46 };
   while (!file.eof()) {
      try {
         c = file.get();
         if (lexema.empty() and ignore(c)) {
            continue;
         }
         if (aceitacao and ignore(c)) {
         } else {
            aceitacao = false;
         }
         switch (estado) {
            case 0:
               switch (c) {
                  case 'S':
                     estado = 1;
                     break;
                  case 'E':
                     estado = 6;
                     break;
                  case 'I':
                     estado = 15;
                     break;
                  case 'Q':
                     estado = 24;
                     break;
                  case 'L':
                     estado = 34;
                     break;
                  default:
                     if (isdigit(c)) {
                        estado = 42;
                     } else if (isalpha(c)) {
                        estado = ID;
                     }
                     break;
               }
               break;
            case 1:
               switch (c) {
                  case 'E':
                     estado = 2;
                     aceitacao = true;
                     break;
                  default:
                     estado = ID;
                     break;
               }
            case 2:
               switch (c) {
                  case 'N':
                     estado = 3;
                     break;
                  default:
                     estado = ID;
               }
            case 3:
               switch (c) {
                  case 'A':
                     estado = 4;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 4:
               switch (c) {
                  case 'O':
                     estado = 5;
                     aceitacao = true;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 6:
               switch (c) {
                  case 'N':
                     estado = 7;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 7:
               switch (c) {
                  case 'Q':
                     estado = 8;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 8:
               switch (c) {
                  case 'U':
                     estado = 9;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 9:
               switch (c) {
                  case 'A':
                     estado = 10;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 10:
               switch (c) {
                  case 'N':
                     estado = 11;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 11:
               switch (c) {
                  case 'T':
                     estado = 12;
                     break;
                  default:
                     estado = ID;
               }
               break;
            case 12:
               switch (c) {
                  case 'O':
                     estado = 13;
                     aceitacao = true;
                     break;
                  default:
                     estado = ID;
               }
               break;
         }
      } catch (const std::exception& e) {
         std::cout << "Transição não reconhecida na linha " << mLine
                   << ".\nEstado atual: " << static_cast<int>(estado)
                   << "Caracter lido: " << c << "\n";
      }
   }
}

