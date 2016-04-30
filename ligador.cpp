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
  void load_list(std::fstream& file, std::list<int>& roll, 
    std::string header);
  void carrega_tabela(std::fstream& file, std::list<Nodo>& roll,
    std::string header);
  void carrega_modulos();
};

// Abre o ponteiro para os dois arquivos modulos.
Ligador::Ligador(std::string fileA, std::string fileB, std::string out) {
  this->fileA.open(fileA + ".o");
  this->fileB.open(fileB + ".o");
  this->out.open(out + ".e");
}

void Ligador::load_list(std::fstream& file, std::list<int>& roll, 
  std::string header) {
   
    std::string line;
    std::string word;

    while(std::getline(file, line)) {
      if (line.empty()) break;
      if (line == header) continue;
      std::stringstream ss(line);
      while (ss >> word) {
        roll.push_back(std::stoi(word));
      }
    }  
}

void Ligador::carrega_tabela(std::fstream& file, std::list<Nodo>& roll,
  std::string header) {

  std::string line;
  std::string symbol;
  std::string value;

  while(std::getline(file, line)) {
    if (line.empty()) break;
    if (line == header) continue;
    std::stringstream ss(line);
    ss >> symbol;
    ss >> value;
    Nodo nodo(symbol, std::stoi(value));
    roll.push_back(nodo);
  }
}

void Ligador::carrega_modulos() {
  this->carrega_tabela(this->fileA, this->moduleA.get_use_table(), "TABLE USE");
  this->carrega_tabela(this->fileA, this->moduleA.get_definition_table(), 
    "TABLE DEFINITION");
  this->load_list(this->fileA, this->moduleA.get_relative(), "RELATIVE");
  this->load_list(this->fileA, this->moduleA.get_code(), "CODE");
  this->carrega_tabela(this->fileB, this->moduleA.get_use_table(), "TABLE USE");
  this->carrega_tabela(this->fileB, this->moduleA.get_definition_table(), 
    "TABLE DEFINITION");
  this->load_list(this->fileB, this->moduleB.get_relative(), "RELATIVE");
  this->load_list(this->fileB, this->moduleB.get_code(), "CODE");

}

int main(int argc, char const *argv[])
{
  if (argc < 4) {
    std::cout << "Por favor, informe nome de dois arquivos objetos." 
      << std::endl;
    return 0;
  }

  Ligador ligador(argv[1], argv[2], argv[3]);
  ligador.carrega_modulos();

  return 0;
}