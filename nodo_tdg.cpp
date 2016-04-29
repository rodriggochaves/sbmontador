#include "nodo_tdg.hpp"

Nodo_TDG::Nodo_TDG(std::string simbolo, int valor) {
  std::cout << "Construindo um novo nodo" << std::endl;
  this->simbolo = simbolo;
  this->valor = valor;
}