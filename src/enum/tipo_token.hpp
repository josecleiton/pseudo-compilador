/*
 * =====================================================================================
 *
 *       Filename:  tipotoken.hpp
 *
 *    Description:  Tipo Token enum
 *
 *        Version:  1.0
 *        Created:  02/26/2020 11:18:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#pragma once

enum class TipoToken {
   INITIAl,
   ID,         // 1
   TIPO,       // 2
   VALOR,      // 3
   SINAL,      // 4
   NEG,        // 5
   SE,         // 6
   SENAO,      // 7
   ENQUANTO,   // 8
   FACA,       // 9
   ACABOU,     // 10
   ATRIB,      // 11
   PNTVIRG,    // 12
   ABREPRNT,   // 13
   FECHAPRNT,  // 14
   FIMARQ,     // 15
   BINOP,      // 16
   // não terminais
   S,
   PROGRAMA,
   BLOCO,
   COMANDO,
   STAT,
   NT_ENQUANTO,
   SEBLOCO,
   NT_SE,
   NT_SENAO,
   DECL,
   COND,
   EXP,
   TERMO,
   FATOR,
   OP,
   UNOP,
};
