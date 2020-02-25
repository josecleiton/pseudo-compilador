# Pseudo Compilador

Programa que faz a análise léxica, sintática e semântica de uma linguagem.
Trabalho proposto na disciplina de Compiladores 2019.2 na [Universidade do Estado da Bahia](https://uneb-si.github.io/).

# Build and Run

[![Run on Repl.it](https://repl.it/badge/github/josecleiton/pseudo-compilador)](https://repl.it/github/josecleiton/pseudo-compilador)

## Usando o terminal

Dependências:
- Compilador C++17

Se você tiver `make` basta:

- `cd src`
- `make`

Senão:

- `cd src`
- `g++ *.cpp -std=c++17 -Wall -Wcast-align -o pcc` (troque `g++` por qualquer compilador C++17)

E rodar:
- `pcc <nome>.c20192`

## Usando o QTCreator

Importe o `.pro` e run
