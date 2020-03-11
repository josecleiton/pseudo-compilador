/*
 * =====================================================================================
 *
 *       Filename:  ast.hpp
 *
 *    Description: Abstract Syntax Tree, criada pelo Sintatico
 *    mas utilizada pelo Semantico para suas analises
 *
 *
 *        Version:  1.0
 *        Created:  02/21/2020 09:54:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */
#pragma once

#include <functional>
#include <list>
#include <memory>
#include <stack>
#include <stdexcept>
#include <unordered_map>

#include "enum/tipo_ast.hpp"
#include "stable.hpp"
#include "token.hpp"

namespace AnaliseSintatica {

/*
 * Abstract Syntax Tree
 * Criada e manipulada pelo Sintático
 * Visualizada pelo Semântico para verificar tipos e escopo.
 */
class AST {
  public:
   /*
    * Um nó regular da AST, exemplo conceitual:
    *       DECL
    *      /    \
    *    TIPO    ID
    *
    *  Exemplo mais físico:
    *  {
    *    tk: DECL,
    *    tipo: REGULAR,
    *    childs: [ { tk: TIPO, tipo: REGULAR, childs: [] } ,
    *              { tk: ID, tipo: EXPOP, childs: [] }
    *            ]
    *  }
    *  PS: childs é uma lista e se estiver vazia representa uma folha
    */
   struct NodeBloco;
   struct Node {
      Token tk;
      TipoAST tipo;
      std::list<std::unique_ptr<Node>> childs;
      Node* super{};

      Node(const Token&, Node* = nullptr, const TipoAST = TipoAST::REGULAR);
      inline auto& getLexema(void) const { return tk.lexema; }

      inline operator TipoAST() const { return tipo; }
      /*
       * função abstrata que tem que ser sobreescrita por todos os filhos
       */
      virtual void avaliar(void) {}
      virtual ~Node() = default;
   };

   struct NodeDecl : public Node {
      NodeDecl(const Token&, Node* = nullptr, const TipoAST = TipoAST::REGULAR);

      /*
       * avaliar nó DECL que tem sempre 2 nós filhos
       * 1) Tipo
       * 2) Identificador
       *
       * selecionará o bloco imediatamente acima do 'no'
       * e inserirá o Identificador na sua Tabela de Símbolos.
       */
      void avaliar(void) override;
      virtual ~NodeDecl() override = default;
   };

   struct NodeAtrib : public Node {
      AnaliseSemantica::Dado* var{};
      std::unique_ptr<AnaliseSemantica::Dado> resultadoExp;

      NodeAtrib(const Token&, Node* = nullptr, const TipoAST = TipoAST::ATRIB);
      /*
       * avaliar nó ATRIB que tem sempre 2 filhos
       * 1) Identificador
       * 2) EXP
       *
       * A analise parte da "resolução" da expressão
       * A função abaixo verificará se o tipo resultante da expressao e o
       * tipo do identificador são compatíveis
       *
       * o valor dessa expressão não pode ser usado para otimizações se:
       *    algum dos nós folha da expressão for um Identificador
       */
      void avaliar(void) override;
      virtual ~NodeAtrib() override = default;
   };

   /*
    * Um nó bloco da AST, também chamado de nó de escopo, exemplo conceitual:
    *       NT_SE
    *      /     \
    *    EXP     ATRIB
    *    /      /     \
    * VALOR    ID     EXP
    *
    *  Exemplo mais físico:
    *  {
    *    tk: NT_SE,
    *    tipo: BLOCO,
    *    st: { { ID: Dado } }
    *    childs: [ { tk: EXP, tipo: EXP,
    *                childs: [ { tk: VALOR, tipo: EXP, childs: [] } ] },
    *              { tk: ATRIB, tipo: ATRIB, childs: [ ID, EXP ] }
    *            ]
    *  }
    *  PS: childs é uma lista e se estiver vazia representa uma folha
    */
   struct NodeBloco : public Node {
      AnaliseSemantica::SymbolTable st;

      NodeBloco(const Token&, Node* = nullptr, const TipoAST = TipoAST::BLOCO);
      /*
       * avaliar nó do tipo BLOCO
       * NT_SE/NT_SENAO/NT_ENQUANTO tem EXP como primeiro nó
       * tal EXP é a condição então deve retornar tipo LOGICO
       */
      void avaliar(void) override;
      virtual ~NodeBloco() override = default;
   };

   /*
    *  Um nó que representa parte de uma expressão
    *  Exemplo mais físico:
    *  {
    *   tk: EXP,
    *   tipo: EXP,
    *   dado: { valor: 0, tipo: NULO },
    *   childs: [ { tk: { lexema: "trinta" }, tipo: EXPOP },
    *             { tk: { lexema: "*" }, tipo: EXPOP },
    *             { tk: { lexema: "32.5" }, tipo: EXPOP } },
    *             { tk: { lexema: "-" }, tipo: EXPOP },
    *             { tk: { lexema: "(" }, tipo: EXPOP },
    *             { tk: { lexema: "k" }, tipo: EXPOP },
    *             { tk: { lexema: "+" }, tipo: EXPOP },
    *             { tk: { lexema: "5" }, tipo: EXPOP },
    *             { tk: { lexema: ")" }, tipo: EXPOP }
    *          ]
    *   }
    *   Acima está o nó EXP gerado para a seguinte expressão:
    *   trinta * 32.5 - ( k + 5 )
    *
    *   Por ser uma expressão infixa,  é resolvida utilizando pilha
    *   para ver o algoritmo, veja a classe NodeExpOp
    */
   class NodeExpOp;
   /*
    * Usa uma variação do algoritmo criado por Dijkstra conhecido como:
    *   SHUNTING YARD
    *
    * ENQUANTO existem tokens FAÇA:
    *   leia o token
    *   SE a pilha está vazia FAÇA:
    *     coloque token na pilha
    *     CONTINUE
    *   ESCOLHA token:
    *   caso FECHA_PARENTESE:
    *       ENQUANTO topo da pilha não for ABRE_PARENTESE:
    *         retire o topo
    *       retire ABRE_PARENTESE do topo
    *
    *     caso ABRE_PARENTESE:
    *       token se torna filho do topo
    *       coloque token na pilha
    *     caso ID: // operadores sempre são folhas
    *     caso VALOR:
    *       token se torna filho do topo
    *     caso SINAL:
    *     caso OPERADOR_BINARIO:
    *     caso NEGACAO:
    *       SE topo == ABRE_PARENTESE FAÇA:
    *         topo = token
    *       SENAO:
    *         SE topo tem 2 filhos FAÇA:
    *           SE precedencia(token) > precedencia(topo) FAÇA:
    *             o filho da direita do topo vira filho de token
    *             o filho da direita do topo vira token
    *             coloque token na pilha
    *             CONTINUE
    *         retire o topo
    *         topo se torna filho de token
    *         o filho da direita do topo se torna token
    *         coloque token na pilha
    */
   struct NodeExp : public Node {
     private:
      /*
       * Pilha auxiliar para o algoritmo de Dijkstra
       */
      std::stack<NodeExpOp*> mPilha;
      std::size_t mNodeCount{};

     public:
      AnaliseSemantica::Dado resultadoExp;
      NodeExpOp* raizExp{};

      NodeExp(const Token&, Node* = nullptr, const TipoAST = TipoAST::EXP);
      inline auto& size(void) const { return mNodeCount; }
      /*
       * Realiza o algoritmo descrito acima
       */
      void insereOp(NodeExpOp* const);
      /*
       * Desempilha todos os operadores/operandos analisados
       * O último da pilha é a raiz da árvore de expressão
       */
      NodeExpOp* fimExp(void);

      void avaliar(void) override;
      virtual ~NodeExp() override = default;
   };

   class NodeExpOp : public Node {
     public:
      enum Direcao { ESQUERDA, DIREITA };

     private:
      std::array<NodeExpOp*, 2> childs{};
      Direcao mDirecao{};
      bool mAbreParentese;

     public:
      NodeExpOp(const Token&, Node* = nullptr, const TipoAST = TipoAST::EXP);
      inline auto& getEsquerda(void) const { return childs[ESQUERDA]; }
      inline auto& getDireita(void) const { return childs[DIREITA]; }
      inline auto& getOp(void) const { return tk.lexema.front(); }
      inline auto size(void) const {
         return (getEsquerda() != nullptr) + (getDireita() != nullptr);
      }
      inline auto& getEsquerda(void) { return childs[ESQUERDA]; }
      inline auto& getDireita(void) { return childs[DIREITA]; }
      inline auto abreParentese(void) { return mAbreParentese; }
      void avaliar(void) override;
      /*
       * Caminho na árvore de expressão em Pos-Ordem
       *              *
       *             / \
       *            3   5
       *  Resultado -> { tipo: INTEIRO, valor: 15 }
       */
      virtual AnaliseSemantica::Dado avaliarExp(void);
      Direcao adicionaChild(NodeExpOp* const);
      virtual ~NodeExpOp() = default;

     private:
      /*
       * Aplica os operadores *, /, +, -, &&, || em Dados
       */
      AnaliseSemantica::Dado aplicaBinop(const AnaliseSemantica::Dado&,
                                         const AnaliseSemantica::Dado&) const;
      /*
       * Aplica os operadores +, !, - no Dado
       */
      AnaliseSemantica::Dado aplicaUnop(const AnaliseSemantica::Dado&) const;
   };

   class NodeExpID : public NodeExpOp {
     private:
      std::unordered_map<std::string, AnaliseSemantica::Dado>::iterator mSTDado;
      bool mIteratorInicializado{};

     public:
      NodeExpID(const Token&, Node* = nullptr, const TipoAST = TipoAST::EXP);
      AnaliseSemantica::Dado avaliarExp(void) override;
      inline void setTipo(const TipoDado tipo) { mSTDado->second.tipo = tipo; }
      ~NodeExpID() override = default;

     private:
      /*
       * getBlocoAcima busca o próximo nó do tipo Bloco a partir de Node* atual
       */
      NodeBloco* getBlocoAcima(const Node* const) const;
      /*
       * Busca variável nos escopos acima de Node*
       * Se encontrar então
       *   atribue
       *   se os tipos forem diferentes então
       *     erro
       * Senao
       *   erro
       */
      void getDadoVar(void);
      void declaraVar(void);
   };

   class NodeExpValor : public NodeExpOp {
     public:
      AnaliseSemantica::Dado val;
      NodeExpValor(const Token&, Node* = nullptr, const TipoAST = TipoAST::EXP);
      inline AnaliseSemantica::Dado avaliarExp(void) override {
         return val;
      }
      virtual ~NodeExpValor() override = default;
   };

  private:
   std::unique_ptr<Node> root;
   /*
    * Pilha utilizada pelo Sintatico para a inserção de nós
    */
   std::stack<Node*> mPilha;
   std::size_t mNodeCount{1};

  public:
   AST(void);
   /*
    * inserirNode Insere um child no Nó que está no topo da mPilha
    * o novo child se torna o novo topo da mPilha
    */
   Node* inserirNode(const Token&, const TipoAST = TipoAST::REGULAR);
   /*
    * Insere um child no Nó que está no topo da mPilha
    */
   Node* inserirFolha(const Token&, const TipoAST = TipoAST::REGULAR);
   /*
    * DFS faz um busca em profundidade na AST.
    *
    * A função func é utilizada para auxiliar na descida.
    * Por exemplo, se estivermos analisando um Nó BLOCO então
    * func -> true. (ou seja, desceremos mais um nível)
    *
    * Já se tivermos analisando um Nó ATRIB não precisamos descer
    * mais um nível, visto que conseguimos todos os parâmetros de um ATRIB no
    * seu próprio nível, apenas acessando a lista de filhos,
    * sendo assim func -> false
    */
   std::size_t DFS(const std::function<bool(Node*)>& func);
   /*
    * subirNivel retira n nós da mPilha
    */
   inline auto subirNivel(const std::size_t n) {
      std::size_t i = 0;
      for (; mPilha.size() and i < n; ++i) {
         /*
          * Se o no atual for uma EXP, devemos finaliza-la, ou seja,
          * retornar todos os niveis da AST de Expressao ate a raiz.
          */
         if (mPilha.top()->tk == TipoToken::EXP) {
            static_cast<NodeExp*>(mPilha.top())->fimExp();
            mPilha.pop();
            ++i;
            if (*mPilha.top() == TipoAST::BLOCO) {
               continue;
            }
         }
         mPilha.pop();
      }
      return i;
   }
   inline Node* atual(void) { return mPilha.top(); }
   inline auto& size(void) const { return mNodeCount; }
   void trocaToken(const Token& tk);

  private:
   /*
    * DFS recursivo
    */
   std::size_t DFS(Node* const atual, const std::function<bool(Node*)>& func);
};

}  // namespace AnaliseSintatica
