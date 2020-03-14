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

#include "erro.hpp"
#include "token.hpp"

enum class TipoDado;

namespace AnaliseSemantica {
/*
 * Função auxiliar que converte o lexema para o Tipo
 * Exemplo: "INTEIRO" -> Tipo::INTEIRO
 */
TipoDado lexemaTipo(const std::string_view);
std::string tipoLexema(const TipoDado);

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
   Dado(const Dado&, const Dado&);
   /*
    * Função auxiliar que seta valor e o tipo
    */
   void setValor(const double v) {
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
   operator TipoDado() const { return tipo; }

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
   auto getDado(const Token& tk) {
      auto it = mTable.find(tk.lexema);
      return std::make_pair(it, it != mTable.end());
   }
   /*
    * Insere nova variável na hash table
    * Se o lexema já estiver na hash table -> throw exception
    */
   auto inserirVariavel(const Token& tk, const TipoDado t = {},
                        const double v = {}) {
      if (getDado(tk).second) {
         throw ErroSemantico(
             tk,
             "Sobescrever uma entrada na tabela de simbolos não é permitido");
      }
      /* emplace retorna um par: { map iterator, bool } */
      return mTable.emplace(tk.lexema, Dado(t, v)).first;
   }
};

}  // namespace AnaliseSemantica
