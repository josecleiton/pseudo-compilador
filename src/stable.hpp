/*
 * =====================================================================================
 *
 *       Filename:  st.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/26/2020 11:28:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  José Cleiton Borges Correia (JCBC), jcleitonbc@gmail.com
 *   Organization:  UNEB
 *
 * =====================================================================================
 */

#pragma once
#include <unordered_map>

#include "enum/tipo_dado.hpp"
#include "erro.hpp"

namespace AnaliseSemantica {
/*
 * Função auxiliar que converte o lexema para o Tipo
 * Exemplo: "INTEIRO" -> Tipo::INTEIRO
 */
TipoDado lexemaTipo(const std::string&);
constexpr bool tipoSaoCompativeis(const TipoDado t1, const TipoDado t2) {
   switch (t1) {
      case TipoDado::INTEIRO:
         return t2 != TipoDado::QUEBRADO;
      case TipoDado::LOGICO:
         return t2 == TipoDado::LOGICO;
      default:
         return true;
   }
}

/*
 * Informações sobre as variáveis cmo:
 * Tipo e Valor
 */
class Dado {
  public:
   TipoDado tipo{};
   double valor{};
   Dado() = default;
   /*
    * Um monte de construtores simplesmente
    * para shorthand
    */
   Dado(const TipoDado, const double = 0.0f);
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
   inline operator TipoDado() const { return tipo; }

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
    * retorna um pair { iterator, bool }
    * verdadeiro se encontrado, senão falso
    */
   inline auto getDado(const std::string& lexema) {
      auto it = mTable.find(lexema);
      return std::make_pair(it, it != mTable.end());
   }
   /*
    * Insere nova variável na hash table
    * Se o lexema já estiver na hash table -> throw exception
    */
   inline auto inserirVariavel(const std::string& lexema, const TipoDado t = {},
                               const double v = {}) {
      if (getDado(lexema).second) {
         throw ErroSemantico("Sobescrever uma entrada na tabela de simbolos não é permitido");
      }
      /* emplace retorna um par: { map iterator, bool } */
      return mTable.emplace(lexema, Dado(t, v)).first;
   }
};

}  // namespace AnaliseSemantica