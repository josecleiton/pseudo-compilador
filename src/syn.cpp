/*
 * =====================================================================================
 *
 *       Filename:  syn.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/18/2020 05:29:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia
 *   Organization: UNEB
 *
 * =====================================================================================
 */

#include "syn.hpp"

#ifdef DEBUG
#include <iostream>
#endif

#include <stdexcept>

#include "lex.hpp"

namespace AnaliseSintatica {
enum { NULA = 99 };

Syn::Syn(Lex& l) : mLex(l) {
   /*
    * Inicializa a pilha com
    * S = START
    * $ = EOF
    */
   mPilha.push(TipoToken::FIMARQ);
   mPilha.push(TipoToken::S);
   /*
    * Inicializa a parser table com as devidas produções
    */
   mLL[TipoToken::S][TipoToken::TIPO] = mLL[TipoToken::S][TipoToken::ID] =
       mLL[TipoToken::S][TipoToken::SE] =
           mLL[TipoToken::S][TipoToken::ENQUANTO] = 1;
   mLL[TipoToken::PROGRAMA][TipoToken::TIPO] =
       mLL[TipoToken::PROGRAMA][TipoToken::ID] =
           mLL[TipoToken::PROGRAMA][TipoToken::SE] =
               mLL[TipoToken::PROGRAMA][TipoToken::ENQUANTO] = 1;
   mLL[TipoToken::PROGRAMA][TipoToken::FIMARQ] = NULA;
   mLL[TipoToken::BLOCO][TipoToken::TIPO] =
       mLL[TipoToken::BLOCO][TipoToken::ID] =
           mLL[TipoToken::BLOCO][TipoToken::SE] =
               mLL[TipoToken::BLOCO][TipoToken::ENQUANTO] = 2;
   mLL[TipoToken::BLOCO][TipoToken::ACABOU] =
       mLL[TipoToken::BLOCO][TipoToken::FACA] = NULA;
   mLL[TipoToken::COMANDO][TipoToken::SE] = 3;
   mLL[TipoToken::COMANDO][TipoToken::ENQUANTO] = 4;
   mLL[TipoToken::COMANDO][TipoToken::TIPO] =
       mLL[TipoToken::COMANDO][TipoToken::ID] = 5;
   mLL[TipoToken::STAT][TipoToken::TIPO] = 6;
   mLL[TipoToken::STAT][TipoToken::ID] = 7;
   mLL[TipoToken::SEBLOCO][TipoToken::SE] = 8;
   mLL[TipoToken::NT_SE][TipoToken::SE] = 9;
   mLL[TipoToken::NT_SENAO][TipoToken::ACABOU] = 10;
   mLL[TipoToken::NT_SENAO][TipoToken::SENAO] = 11;
   mLL[TipoToken::NT_ENQUANTO][TipoToken::ENQUANTO] = 12;
   mLL[TipoToken::DECL][TipoToken::TIPO] = 13;
   mLL[TipoToken::ATRIB][TipoToken::ID] = 14;
   mLL[TipoToken::EXP][TipoToken::ID] =
       mLL[TipoToken::EXP][TipoToken::ABREPRNT] =
           mLL[TipoToken::EXP][TipoToken::VALOR] = 15;
   mLL[TipoToken::EXP][TipoToken::SINAL] = mLL[TipoToken::EXP][TipoToken::NEG] =
       16;
   mLL[TipoToken::TERMO][TipoToken::SINAL] =
       mLL[TipoToken::TERMO][TipoToken::BINOP] = 17;
   mLL[TipoToken::TERMO][TipoToken::PNTVIRG] =
       mLL[TipoToken::TERMO][TipoToken::FACA] =
           mLL[TipoToken::TERMO][TipoToken::FECHAPRNT] = NULA;
   mLL[TipoToken::FATOR][TipoToken::ID] = 18;
   mLL[TipoToken::FATOR][TipoToken::ABREPRNT] = 19;
   mLL[TipoToken::FATOR][TipoToken::VALOR] = 20;
   mLL[TipoToken::OP][TipoToken::SINAL] = 21;
   mLL[TipoToken::OP][TipoToken::BINOP] = 22;
   mLL[TipoToken::UNOP][TipoToken::TIPO] =
       mLL[TipoToken::UNOP][TipoToken::ABREPRNT] = NULA;
   mLL[TipoToken::UNOP][TipoToken::SINAL] = 21;
   mLL[TipoToken::UNOP][TipoToken::NEG] = 23;
   mLL[TipoToken::COND][TipoToken::ID] =
       mLL[TipoToken::COND][TipoToken::ABREPRNT] =
           mLL[TipoToken::COND][TipoToken::VALOR] =
               mLL[TipoToken::COND][TipoToken::SINAL] =
                   mLL[TipoToken::COND][TipoToken::NEG] = 24;
}

AST& Syn::parse(void) {
   /*
    * Solicita ao Léxico prox token
    */
   Token tk = proximoToken();
   while (mPilha.size()) {
      const auto topo = mPilha.top();
      try {
         if (tk != topo) {
            /*
             * Verifica REGRA mLL[topo][tk],
             * caso não exista a regra, o map dispara uma exceção
             */
            const auto producao = mLL.at(topo).at(tk);
            mPilha.pop();
            switch (producao) {
               case 0:  // S -> programa
                  mPilha.push(TipoToken::PROGRAMA);
                  break;
               case 1:  // programa -> comando
                  mPilha.push(TipoToken::PROGRAMA);
                  mPilha.push(TipoToken::COMANDO);
                  // mAST.subirNivel(mBloco);
                  break;
               case 2:  // bloco -> comando
                  mPilha.push(TipoToken::BLOCO);
                  mPilha.push(TipoToken::COMANDO);
                  // mAST.subirNivel(mBloco);
                  break;
               case 3:  // comando -> sebloco
                  mPilha.push(TipoToken::SEBLOCO);
                  // cria node SEBLOCO
                  break;
               case 4:  // comando -> enquanto
                  mPilha.push(TipoToken::NT_ENQUANTO);
                  // cria node NT_ENQUANTO
                  break;
               case 5:  // comando -> stat;
                  mPilha.push(TipoToken::PNTVIRG);
                  mPilha.push(TipoToken::STAT);
                  break;
               case 6:  // stat -> decl
                  mPilha.push(TipoToken::DECL);
                  // cria node DECL
                  break;
               case 7:  // stat -> atrib
                  mPilha.push(TipoToken::ATRIB);
                  // cria node ATRIB
                  break;
               case 8:  // sebloco -> se senao
                  mPilha.push(TipoToken::NT_SENAO);
                  mPilha.push(TipoToken::NT_SE);
                  break;
               case 9:  // se -> SE exp FACA bloco
                  mPilha.push(TipoToken::BLOCO);
                  mPilha.push(TipoToken::FACA);
                  mPilha.push(TipoToken::COND);
                  mPilha.push(TipoToken::SE);
                  /*
                   * Ao ler SE cria-se um novo escopo na AST
                   */
                  mAST.inserirNode(TipoToken::NT_SE, AST::Tipo::BLOCO);
                  break;
               case 10:  // senao -> ACABOU
                  mPilha.push(TipoToken::ACABOU);
                  break;
               case 11:  // senao -> SENAO bloco ACABOU
                  mPilha.push(TipoToken::ACABOU);
                  mPilha.push(TipoToken::BLOCO);
                  mPilha.push(TipoToken::SENAO);
                  /*
                   * Ao ler SENAO cria-se um novo escopo na AST
                   */
                  mAST.inserirNode(TipoToken::NT_SENAO, AST::Tipo::BLOCO);
                  // cria node senao
                  break;
               case 12:  // enquanto -> ENQUANTO exp FACA bloco ACABOU
                  mPilha.push(TipoToken::ACABOU);
                  mPilha.push(TipoToken::BLOCO);
                  mPilha.push(TipoToken::FACA);
                  mPilha.push(TipoToken::COND);
                  mPilha.push(TipoToken::ENQUANTO);
                  /*
                   * Ao ler ENQUANTO cria-se um novo escopo na AST
                   */
                  mAST.inserirNode(TipoToken::NT_ENQUANTO, AST::Tipo::BLOCO);
                  break;
               case 13:  // decl -> tipo id
                  mPilha.push(TipoToken::ID);
                  mPilha.push(TipoToken::TIPO);
                  mAST.inserirNode(TipoToken::DECL);
                  break;
               case 14:  // atrib -> id = exp
                  mPilha.push(TipoToken::EXP);
                  mPilha.push(TipoToken::ATRIB);
                  mPilha.push(TipoToken::ID);
                  mAST.inserirNode(TipoToken::ATRIB, AST::Tipo::ATRIB);
                  break;
               case 15:  // exp -> termo fator
                  mPilha.push(TipoToken::TERMO);
                  mPilha.push(TipoToken::FATOR);
                  mAST.inserirNode(TipoToken::EXP, AST::Tipo::EXP);
                  break;
               case 16:  // exp -> unop exp
                  mPilha.push(TipoToken::EXP);
                  mPilha.push(TipoToken::UNOP);
                  mAST.inserirNode(TipoToken::EXP, AST::Tipo::EXP);
                  break;
               case 17:  // termo -> op exp
                  mPilha.push(TipoToken::EXP);
                  mPilha.push(TipoToken::OP);
                  break;
               case 18:  // fator -> id
                  mPilha.push(TipoToken::ID);
                  break;
               case 19:  // fator -> (exp)
                  mPilha.push(TipoToken::FECHAPRNT);
                  mPilha.push(TipoToken::EXP);
                  mPilha.push(TipoToken::ABREPRNT);
                  break;
               case 20:  // fator -> valor
                  mPilha.push(TipoToken::VALOR);
                  break;
               case 21:  // op -> sinal || unop -> sinal
                  mPilha.push(TipoToken::SINAL);
                  break;
               case 22:  // op -> binop
                  mPilha.push(TipoToken::BINOP);
                  break;
               case 23:  // unop -> !
                  mPilha.push(TipoToken::NEG);
                  break;
               case 24:  // cond -> exp
                  mPilha.push(TipoToken::EXP);
                  break;

               case NULA:  // producao -> ε
                  break;

               default:
                  throw std::runtime_error(
                      "Erro sintatico - Produção não registrada");
            }
         } else {
            // terminal retornado pelo lexico bate com o token no topo da pilha
            mPilha.pop();
            switch (tk) {
               /*
                * Tokens que fazem parte de uma expressão
                * criar-se nós folha que serão analisados e resolvidos no
                * semântico
                */
               case TipoToken::ID:
               case TipoToken::SINAL:
               case TipoToken::NEG:
               case TipoToken::BINOP:
               case TipoToken::ABREPRNT:
               case TipoToken::FECHAPRNT:
               case TipoToken::VALOR:
                  mAST.inserirFolha(tk, AST::Tipo::EXPOP);
                  break;
               /*
                * Token que faz parte de um Node DECL
                */
               case TipoToken::TIPO:
                  mAST.inserirFolha(tk);
                  break;
               /*
                * Tokens que representam final do Node
                * após ele subimos n níveis da árvore
                * Caso especial:
                *    Se o nó atual for EXP e o nó pai for ATRIB
                *       subirNivel subirá +1 nível
                */
               case TipoToken::ACABOU:
               case TipoToken::PNTVIRG:
               case TipoToken::FACA:
                  mAST.subirNivel(1);
                  break;
               /*
                * Alguns Tokens terminais não devem entrar na AST
                * Exemplo: ;, FACA, SE, SENAO, etc
                */
               default:
                  break;
            }
#ifdef DEBUG
            std::clog << "[DEBUG - parser] " << tk << std::endl;
#endif
            if (tk != TipoToken::FIMARQ) {
               tk = proximoToken();
            }
         }
      } catch (const std::exception& e) {
#ifdef DEBUG
         std::clog
             << "[DEBUG - parser] Transição não encontrada na parse table."
             << std::endl;
#endif
         throw e;
      }
   }
   return mAST;
}
}  // namespace AnaliseSintatica

