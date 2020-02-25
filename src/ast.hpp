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
   inline void insereDado(const std::string& lexema, const Tipo t,
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
    * Os nós podem ser de tipo BLOCO, EXP, REGULAR
    * BLOCO - Mantém junto a ele uma tabela de símbolos (escopo)
    * EXP - Utilizado para as expressões
    * ATRIB
    * DECL
    */
   enum class Tipo { REGULAR, BLOCO, EXP, EXPOP, ATRIB };

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
    *              { tk: ID, tipo: REGULAR, childs: [] }
    *            ]
    *  }
    *  PS: childs é uma lista e se estiver vazia representa uma folha
    */
   struct Node {
      Token tk;
      Tipo tipo;
      std::list<std::unique_ptr<Node>> childs;
      Node* super{};

      Node(const Token&, Node* = nullptr, const Tipo = Tipo::REGULAR);
      virtual ~Node() {}
   };

   /*
    * Um nó bloco da AST, também chamado de nó de escopo, exemplo conceitual:
    *       NT_SE
    *      /     \
    *    EXP     ATRIB
    *    /      /     \
    * VALOR    ID    VALOR
    *
    *  Exemplo mais físico:
    *  {
    *    tk: NT_SE,
    *    tipo: BLOCO,
    *    st: { { ID: Dado } }
    *    childs: [ { tk: EXP, tipo: EXP,
    *                childs: [ { tk: VALOR, tipo: EXP, childs: [] } ] },
    *              { tk: ATRIB, tipo: REGULAR, childs: [ ID, VALOR ] }
    *            ]
    *  }
    *  PS: childs é uma lista e se estiver vazia representa uma folha
    */
   struct NodeBloco : public Node {
      AnaliseSemantica::SymbolTable st;

      NodeBloco(const Token&, Node* = nullptr, const Tipo = Tipo::BLOCO);
      ~NodeBloco() {}
   };

   /*
    *  Um nós que representa parte de uma expressão
    *  Exemplo mais físico:
    *  {
    *   tk: EXP,
    *   tipo: EXP,
    *   dado: { valor: 0, tipo: NULO },
    *   childs: [ { tk: { lexema: "trinta" }, tipo: EXP },
    *             { tk: { lexema: "*" }, tipo: EXP },
    *             { tk: { lexema: "32.5" }, tipo: EXP } },
    *             { tk: { lexema: "-" }, tipo: EXP },
    *             { tk: { lexema: "(" }, tipo: EXP },
    *             { tk: { lexema: "k" }, tipo: EXP },
    *             { tk: { lexema: "+" }, tipo: EXP },
    *             { tk: { lexema: "5" }, tipo: EXP },
    *             { tk: { lexema: ")" }, tipo: EXP }
    *          ]
    *   }
    *   Acima está o nó EXP gerado para a seguinte expressão:
    *   trinta * 32.5 - ( k + 5 ) Por ser uma expressão infixa,
    *   é resolvida utilizando pilhas
    */
   struct NodeExpOp;
   struct NodeExp : public Node {
     private:
      std::stack<NodeExpOp*> mPilha;

     public:
      AnaliseSemantica::Dado dado;
      NodeExpOp* expRoot{};

      NodeExp(const Token&, Node* = nullptr, const Tipo = Tipo::EXP);
      void insereOp(NodeExpOp* const);
      void fimExp(void);
      ~NodeExp() {}
   };

   struct NodeExpOp : public Node {
      enum Direcao { ESQUERDA = 0, DIREITA } mDirecao{};
      std::array<NodeExpOp*, 2> childs{};

      NodeExpOp(const Token&, Node* = nullptr, const Tipo = Tipo::EXP);
      inline auto& getEsquerda(void) { return childs[ESQUERDA]; }
      inline auto& getDireita(void) { return childs[DIREITA]; }
      inline auto& getOp(void) const { return tk.lexema.front(); }
      inline std::size_t size(void) const {
         return (childs[ESQUERDA] != nullptr) + (childs[DIREITA] != nullptr);
      }
      Direcao adicionaChild(NodeExpOp*);
      ~NodeExpOp() {}
   };

   struct NodeAtrib : public Node {
      AnaliseSemantica::Dado* var{};
      std::unique_ptr<AnaliseSemantica::Dado> resultadoExp;
      NodeAtrib(const Token&, Node* = nullptr, const Tipo = Tipo::ATRIB);
      ~NodeAtrib() {}
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
    * A função func é utilizada para o algoritmo saber se desce ou não mais um
    * nível. Por exemplo, se estivermos analisando um Nó do tipo Bloco então
    * func -> true.
    * Já se tivermos analisando um Nó ATRIB não precisamos descer
    * mais um nível, visto que conseguimos todos os parâmetros de um ATRIB no
    * seu próprio nível, apenas acessando a lista childs
    */
   std::size_t DFS(const std::function<bool(Node*)>& func);
   /*
    * subirNivel retira n nós da mPilha
    */
   inline auto subirNivel(const std::size_t n) {
      std::size_t i = 0;
      for (; mPilha.size() and i < n; ++i) {
         if (auto exp = dynamic_cast<NodeExp*>(mPilha.top()); exp) {
            exp->fimExp();
            mPilha.pop();
            ++i;
            if (mPilha.top()->tipo == Tipo::BLOCO) {
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
   inline NodeBloco* getBlocoAcima(const Node* const atual) const {
      Node* result = atual->super;
      while (result and result->tipo != Tipo::BLOCO) {
         result = result->super;
      }
      return result ? dynamic_cast<NodeBloco*>(result) : nullptr;
   }
   inline Node* atual(void) { return mPilha.top(); }
   inline auto size(void) const { return mNodeCount; }

  private:
   std::size_t DFS(Node* atual, const std::function<bool(Node*)>& func);
};

}  // namespace AnaliseSintatica
