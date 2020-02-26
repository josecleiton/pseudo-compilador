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

#include "token.hpp"

namespace AnaliseSemantica {
/*
 * Tipos que as variáveis podem assumir
 */
enum class Tipo {
   NULO,
   INTEIRO,
   QUEBRADO,
   LOGICO,
};

/*
 * Função auxiliar que converte o lexema para o Tipo
 * Exemplo: "INTEIRO" -> Tipo::INTEIRO
 */
Tipo lexemaTipo(const std::string&);

/*
 * Informações sobre as variáveis cmo:
 * Tipo e Valor
 */
class Dado {
  public:
   Tipo tipo{};
   double valor{};
   Dado() = default;
   /*
    * Um monte de construtores simplesmente
    * para shorthand
    */
   Dado(const Tipo, const double = 0.0f);
   Dado(const Dado&);
   Dado(const double);
   static constexpr bool ehCompativelCom(const Tipo, const Tipo);
   /*
    * Função auxiliar que seta valor e o tipo
    */
   inline void setValor(const double v) {
      valor = v;
      preencheTipo();
   }
   /*
    * Copy assignment
    * Ele checa a compatibilidade entres os tipos do 'this' e 'other'
    * Se não forem compatíveis -> throw exception
    */
   Dado& operator=(const Dado& other);
   Dado& operator=(Dado&&) = default;
   /*
    * Conversão implícita evitando coisas como:
    * d.tipo == Tipo::INTEIRO; -> d == Tipo::INTEIRO;
    */
   inline operator Tipo() const { return tipo; }

  private:
   /*
    * A partir do valor atribui tipo
    */
   void preencheTipo(void);
};

class SymbolTable {
   /*
    * Hash table [lexema] -> Dado
    */
   std::unordered_map<std::string, Dado> mTable;

  public:
   /*
    * Busca na hash table por dado lexema
    */
   inline Dado* getDado(const std::string& lexema) {
      try {
         return &mTable.at(lexema);
      } catch (const std::out_of_range& e) {
         return nullptr;
      }
   }
   /*
    * Insere nova variável na hash table
    * Se o lexema já estiver na hash table -> throw exception
    */
   inline void inserirVariavel(const std::string& lexema, const Tipo t,
                               const double v = {}) {
      if (getDado(lexema)) {
         throw std::domain_error(
             "Sobescrever uma entrada na tabela de simbolos não é permitido");
      }
      mTable[lexema] = {t, v};
   }
};

}  // namespace AnaliseSemantica

namespace AnaliseSintatica {

/*
 * Abstract Syntax Tree
 * Criada e manipulada pelo Sintático
 * Visualizada pelo Semântico para verificar tipos e escopo.
 */
class AST {
  public:
   /*
    * Os nós podem ser de tipo BLOCO, EXP, REGULAR, EXPOP, ARTIB
    * BLOCO - Mantém junto a ele uma tabela de símbolos (escopo)
    * EXP - Utilizado para as expressões
    * EXPOP - Operadores e operandos de uma expressao
    * ATRIB - Representa a atribuiçao
    * REGULAR - Representa um Tipo ou uma DECL
    */
   enum class Tipo { REGULAR, BLOCO, EXP, EXPOP, ATRIB, DECL };

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
      Tipo tipo;
      std::list<std::unique_ptr<Node>> childs;
      Node* super{};

      Node(const Token&, Node* = nullptr, const Tipo = Tipo::REGULAR);
      NodeBloco* getBlocoAcima(void) const;
      /*
       * Busca variável nos escopos acima de Node*
       * Se encontrar então
       *   atribue
       *   se os tipos forem diferentes então
       *     erro
       * Senao
       *   erro
       */
      AnaliseSemantica::Dado* getDadoVar(const std::string& lexema) const;
      inline auto& getLexema(void) const { return tk.lexema; }

      inline operator Tipo() const { return tipo; }
      virtual void avaliar(void) { throw std::domain_error("Apenas base."); }
      virtual ~Node() {}

     private:
      NodeBloco* getBlocoAcima(const Node* const) const;
   };

   struct NodeDecl : public Node {
      NodeDecl(const Token&, Node* = nullptr, const Tipo = Tipo::REGULAR);

      /*
       * avaliar nó DECL que tem sempre 2 nós filhos
       * selecionará o bloco imediatamente acima do 'no'
       * e inserirá o Identificador na sua Tabela de Símbolos.
       * 1) Tipo
       * 2) Identificador
       */
      void avaliar(void) override;
      ~NodeDecl() {}
   };

   struct NodeAtrib : public Node {
      AnaliseSemantica::Dado* var{};
      std::unique_ptr<AnaliseSemantica::Dado> resultadoExp;

      NodeAtrib(const Token&, Node* = nullptr, const Tipo = Tipo::ATRIB);
      /*
       * avaliar nó ATRIB que tem pelo menos 2 filhos
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
      ~NodeAtrib() {}
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

      NodeBloco(const Token&, Node* = nullptr, const Tipo = Tipo::BLOCO);
      /*
       * avaliar nó do tipo BLOCO
       * NT_SE/NT_SENAO/NT_ENQUANTO tem EXP como primeiro nó
       * tal EXP é a condição então deve retornar tipo LOGICO
       */
      void avaliar(void) override;
      ~NodeBloco() {}
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

      NodeExp(const Token&, Node* = nullptr, const Tipo = Tipo::EXP);
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
      ~NodeExp() {}
   };

   class NodeExpOp : public Node {
      mutable std::array<NodeExpOp*, 2> childs{};

     public:
      enum Direcao { ESQUERDA, DIREITA } mDirecao{};

      NodeExpOp(const Token&, Node* = nullptr, const Tipo = Tipo::EXP);
      inline auto& getEsquerda(void) const { return childs[ESQUERDA]; }
      inline auto& getDireita(void) const { return childs[DIREITA]; }
      inline auto& getOp(void) const { return tk.lexema.front(); }
      inline auto size(void) const {
         return (getEsquerda() != nullptr) + (getDireita() != nullptr);
      }
      void avaliar(void) override;
      /*
       * Caminho na árvore de expressão em Pos-Ordem
       *              *
       *             / \
       *            3   5
       *  Resultado -> { tipo: INTEIRO, valor: 15 }
       */
      AnaliseSemantica::Dado avaliarExp(std::size_t&) const;
      Direcao adicionaChild(NodeExpOp* const);
      ~NodeExpOp() {}

     private:
      /*
       * Função auxiliar para permitir a recursão
       */
      AnaliseSemantica::Dado avaliarExp(const NodeExpOp* const,
                                        std::size_t&) const;
      /*
       * Aplica os operadores *, /, +, -, &&, || em Dados
       */
      AnaliseSemantica::Dado aplicaBinop(const AnaliseSemantica::Dado&,
                                         const AnaliseSemantica::Dado&) const;
      /*
       * Aplica os operadores +, !, - no Dado
       */
      AnaliseSemantica::Dado aplicaUnop(const AnaliseSemantica::Dado&) const;
      /*
       * Se for VALOR então
       *   retorna VALOR
       * Senão // é um identificador (variavel)
       *   Busca variável nos escopos acima de Node*
       *   Se encontrar então
       *    retorna valor da variável
       *   Senao
       *     erro
       */
      AnaliseSemantica::Dado getValorVariavel(void) const;
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
   Node* inserirNode(const Token&, const Tipo = Tipo::REGULAR);
   /*
    * Insere um child no Nó que está no topo da mPilha
    */
   Node* inserirFolha(const Token&, const Tipo = Tipo::REGULAR);
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
         if (mPilha.top()->tk == Token::TipoToken::EXP) {
            static_cast<NodeExp*>(mPilha.top())->fimExp();
            mPilha.pop();
            ++i;
            if (*mPilha.top() == Tipo::BLOCO) {
               continue;
            }
         }
         mPilha.pop();
      }
      return i;
   }
   /*
    * getBlocoAcima busca o próximo nó do tipo Bloco a partir de Node* atual
    */
   inline Node* atual(void) { return mPilha.top(); }
   inline auto& size(void) const { return mNodeCount; }

  private:
   /*
    * DFS recursivo
    */
   std::size_t DFS(Node* const atual, const std::function<bool(Node*)>& func);
};

}  // namespace AnaliseSintatica
