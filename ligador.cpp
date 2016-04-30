#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <list>
#include <sstream>
#include "module.hpp"

class Ligador {
  Module moduleA;
  Module moduleB;
  std::fstream fileA;
  std::fstream fileB;
  std::fstream out;
public:
  Ligador(std::string fileA, std::string fileB, std::string out);
  void carrega_tabelas();
};

// Abre o ponteiro para os dois arquivos modulos.
Ligador::Ligador(std::string fileA, std::string fileB, std::string out) {
  this->fileA.open(fileA + ".o");
  this->fileB.open(fileB + ".o");
  this->out.open(out + ".e");
}

// TODO: refatorar esse metodo
void Ligador::carrega_tabelas() {
  std::string linha;
  std::string palavra;
  std::string simbolo;
  std::string valor;

  // carrega tabela de uso da modulo A
  while(std::getline(this->fileA, linha)) {
    if (linha.empty()) break;
    if (linha == "TABLE USE") continue;
    std::stringstream ss(linha);
    ss >> simbolo;
    ss >> valor;
    Nodo nodo(simbolo, std::stoi(valor));
    this->moduleA.get_use_table().push_back(nodo);
  }

  // carrega tabela de uso do módulo B
  while(std::getline(this->fileB, linha)) {
    if (linha.empty()) break;
    if (linha == "TABLE USE") continue;
    std::stringstream ss(linha);
    ss >> simbolo;
    ss >> valor;
    Nodo nodo(simbolo, std::stoi(valor));
    this->moduleB.get_use_table().push_back(nodo);
  }

  // carrega tabela de definicao do modulo A
  while(std::getline(this->fileA, linha)) {
    if (linha.empty()) break;
    if (linha == "TABLE DEFINITION") continue;
    std::stringstream ss(linha);
    ss >> simbolo;
    ss >> valor;
    Nodo nodo(simbolo, std::stoi(valor));
    this->moduleA.get_definition_table().push_back(nodo);
  }

  // carrega tabela de definicao do modulo B
  while(std::getline(this->fileB, linha)) {
    if (linha.empty()) break;
    if (linha == "TABLE DEFINITION") continue;
    std::stringstream ss(linha);
    ss >> simbolo;
    ss >> valor;
    Nodo nodo(simbolo, std::stoi(valor));
    this->moduleB.get_definition_table().push_back(nodo);
  }

  // carrega tabela de uso do modulo A
  while(std::getline(this->fileA, linha)) {
    if (linha.empty()) break;
    if (linha == "RELATIVE") continue;
    std::stringstream ss(linha);
    while (ss >> palavra) {
      this->moduleA.get_relative().push_back(std::stoi(palavra));
    }
  }

  // carrega tabela de uso do modulo B
  while(std::getline(this->fileB, linha)) {
    if (linha.empty()) break;
    if (linha == "RELATIVE") continue;
    std::stringstream ss(linha);
    while (ss >> palavra) {
      this->moduleB.get_relative().push_back(std::stoi(palavra));
    }
  }

  // carrega o codigo do modulo de A
  while(std::getline(this->fileA, linha)) {
    if (linha.empty()) break;
    if (linha == "CODE") continue;
    std::stringstream ss(linha);
    while (ss >> palavra) {
      this->moduleA.get_code().push_back(std::stoi(palavra));
    }
  }

  // carrega o codigo do modulo de B
  while(std::getline(this->fileB, linha)) {
    if (linha.empty()) break;
    if (linha == "CODE") continue;
    std::stringstream ss(linha);
    while (ss >> palavra) {
      this->moduleB.get_code().push_back(std::stoi(palavra));
    }
  }
}

int main(int argc, char const *argv[])
{
  if (argc < 4) {
    std::cout << "Por favor, informe nome de dois arquivos objetos." 
      << std::endl;
    return 0;
  }

  Ligador ligador(argv[1], argv[2], argv[3]);
  ligador.carrega_tabelas();

  return 0;
}