/*
 * =====================================================================================
 *
 *       Filename:  pos.hpp
 *
 *    Description: Representa uma posição (x, y)
 *
 *        Version:  1.0
 *        Created:  03/16/2020 06:19:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */
#pragma once
struct Pos {
   long long linha;
   long long col;

   constexpr Pos(const long long _linha = 0, const long long _col = 0)
       : linha(_linha), col(_col) {}
};


