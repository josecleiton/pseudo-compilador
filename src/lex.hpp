/*
 * =====================================================================================
 *
 *       Filename:  lex.h
 *
 *    Description: Analisador léxico
 *
 *        Version:  1.0
 *        Created:  02/13/2020 02:34:26 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */
#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "pos.hpp"

#if __GNUC__ > 7
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

class Token;
enum class TipoToken;

class Lex {
  private:
   std::ifstream mInputFile;
   std::ofstream mOutputFile;
   /*
    * Posição de leitura no arquivo de entrada
    */
   Pos mPos;
   /*
    * Palavras reservadas
    */
   const std::unordered_map<std::string, TipoToken> mPalavrasReservadas;

  public:
   /*
    * Lex inicializado com:
    * o caminho do arquivo de entrada (vem do argv)
    * o caminho do arquivo de saida, que contera todos os lexemas lidos
    */
   Lex(const fs::path& in, const fs::path& out = "lexemas.txt");
   ~Lex();
   /*
    * Pega token vindo do arquivo de entrada
    * Grava no arquivo "lexemas.txt"
    * Retorna o token
    */
   Token getToken(void);
   auto& getPos(void) const { return mPos; }
   auto& getLinha(void) const { return mPos.linha; }
   auto& getCol(void) const { return mPos.col; }
   auto& getFile(void) { return mInputFile; }
   /*
    * Função para testes, lê todos os tokens e printa na tela
    */
   void analiseAteEOF(void);

  private:
   /*
    * Implementação do AFD da linguagem
    */
   Token proxToken(void);
   /*
    * Se o lexema for uma palavra reservada, retorne o seu respectivo token,
    * senão retorne token id
    */
   TipoToken reservadaOuID(const std::string& lexema) const;
   /*
    * Função auxiliar para criar token com atributo linha e coluna
    */
   inline Token criaToken(const TipoToken, const std::string& lexema) const;
   /*
    * Função auxiliar para pegar o próx char do buffer
    */
   char getChar(std::string& lexema) {
      ++mPos.col;
      lexema.push_back(mInputFile.get());
      return lexema.back();
   }
   /*
    * Função auxiliar para colocar char no buffer
    */
   void ungetChar(std::string& lexema) {
      --mPos.col;
      mInputFile.unget();
      lexema.pop_back();
   }
   /*
    * Função auxiliar para ajustar os contadores de linha e col
    * na leitura de um '\n'
    */
   void proximaLinha(void) {
      mPos.col = 0;
      ++mPos.linha;
   }
};
