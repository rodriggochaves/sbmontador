#include "nodo.hpp"

Nodo::Nodo(std::string simbolo, int valor) {
  this->simbolo = simbolo;
  this->valor = valor;
}

int Nodo::getValor() {
  return this->valor;
}

std::string Nodo::getSimbolo() {
  return this->simbolo;
}