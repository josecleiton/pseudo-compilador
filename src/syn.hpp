/*
 * =====================================================================================
 *
 *       Filename:  syn.hpp
 *
 *    Description: Parser
 *
 *        Version:  1.0
 *        Created:  02/18/2020 04:06:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia
 *   Organization: UNEB
 *
 * =====================================================================================
 */

#pragma once

#include <list>
#include <stack>
#include <unordered_map>

#include "lex.hpp"
namespace AnaliseSintatica {
typedef Token::TipoToken TipoToken;

class SymbolTable {
  public:
   enum class Tipo {
      INTEIRO,
      QUEBRADO,
      LOGICO,
   };
   const unsigned GLOBAL{1};
   unsigned mContextoCounter{1};

  private:
   struct ID {
      Tipo tipo;
   };
   struct Contexto {
      unsigned super;
      std::unordered_map<std::string, ID> mIDs;
   };
   std::unordered_map<unsigned, Contexto> mContextos{{GLOBAL, {}}};
   std::stack<unsigned> mPilha;

  public:
   SymbolTable(void);
   const ID* buscaID(const unsigned codContexto,
                     const std::string& lexema) const;
   inline unsigned entrarContexto() {
      mContextos[++mContextoCounter].super = getContexto();
      mPilha.push(mContextoCounter);
      return mContextoCounter;
   }
   inline unsigned getContexto(void) const { return mPilha.top(); }
   inline void sairContexto(void) { mPilha.pop(); }
   bool inserirID(const unsigned, const Token&, const Tipo&);
   bool inserirID(const Token&, const Tipo&);
   inline Tipo getTipoByLexema(const std::string& lexema) const {
      if(lexema == "INTEIRO") {
         return Tipo::INTEIRO;
      } else if(lexema == "QUEBRADO") {
         return Tipo::QUEBRADO;
      }
      return Tipo::LOGICO;
   }
};

class Syn {
   std::stack<TipoToken> mPilha;
   std::list<Token> mTokens;
   // abaixo está a inicialização da parse table (LL(1))
   const std::unordered_map<TipoToken, std::unordered_map<TipoToken, int>> mLL{
       {TipoToken::S,
        {{TipoToken::TIPO, 1},
         {TipoToken::ID, 1},
         {TipoToken::SE, 1},
         {TipoToken::ENQUANTO, 1}}},
       {TipoToken::PROGRAMA,
        {{TipoToken::TIPO, 1},
         {TipoToken::ID, 1},
         {TipoToken::SE, 1},
         {TipoToken::ENQUANTO, 1},
         {TipoToken::$, 2}}},
       {TipoToken::BLOCO,
        {{TipoToken::TIPO, 2},
         {TipoToken::ID, 2},
         {TipoToken::SE, 2},
         {TipoToken::ENQUANTO, 2}}},
       {TipoToken::COMANDO,
        {{TipoToken::SE, 3},
         {TipoToken::ENQUANTO, 4},
         {TipoToken::TIPO, 5},
         {TipoToken::ID, 5}}},
       {TipoToken::STAT, {{TipoToken::TIPO, 6}, {TipoToken::ID, 7}}},
       {TipoToken::SEBLOCO, {{TipoToken::SE, 8}}},
       {TipoToken::NT_SE, {{TipoToken::SE, 9}}},
       {TipoToken::NT_SENAO, {{TipoToken::ACABOU, 10}, {TipoToken::SENAO, 11}}},
       {TipoToken::NT_ENQUANTO, {{TipoToken::ENQUANTO, 12}}},
       {TipoToken::DECL, {{TipoToken::TIPO, 13}}},
       {TipoToken::ATRIB, {{TipoToken::ID, 13}}},
   };
   SymbolTable mST;
   // referencia do objeto Lex (responsável pela análise léxica)
   Lex& mLex;

  public:
   Syn(Lex&);
   unsigned parse(void);
   void handleContexto(const Token&);
   inline const auto& getTokens(void) const { return mTokens; }
   inline const auto& getSymbols(void) const { return mST; }
};
}  // namespace AnaliseSintatica
