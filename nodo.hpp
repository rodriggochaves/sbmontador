#include <iostream>
#include <string>

// Aqui é representado um nodo da tabela de definicação global

class Nodo
{
  std::string simbolo;
  int valor;
public:
  Nodo(std::string simbolo, int valor);
  int getValor();
  std::string getSimbolo();
};