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
  std::list<Node> global_definition_table;
  std::list<int> global_code;
  std::fstream fileA;
  std::fstream fileB;
  std::fstream out;
public:
  Ligador(std::string fileA, std::string fileB, std::string out);
  void load_list(std::fstream& file, std::list<int>& roll, 
    std::string header);
  void carrega_tabela(std::fstream& file, std::list<Node>& roll,
    std::string header);
  void carrega_modulos();
  void create_global_defition_table();
  void create_global_code();
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

void Ligador::carrega_tabela(std::fstream& file, std::list<Node>& roll,
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
    Node node(symbol, std::stoi(value));
    roll.push_back(node);
  }
}

void Ligador::create_global_code() {
  
  // Consegue o fator de correção do módulo A vendo o tamanho do código geral.
  this->moduleA.set_correction_factor(this->global_code.size());

  // Coloca os códigos do módulo A dentro da lista geral de código.
  for (auto code : this->moduleA.get_code()) {
    this->global_code.push_back(code);
  }

  // Consegue o fator de correção do módulo B vendo o tamanho do código geral.
  this->moduleB.set_correction_factor(this->global_code.size());

  // Coloca os códigos do módulo B dentro da lista geral de código.
  for (auto code : this->moduleB.get_code()) {
    this->global_code.push_back(code);
  }
}

void Ligador::create_global_defition_table() {
  for (auto node : this->moduleA.get_definition_table()) {
    // Aplica fator de correção para tabela de definição global
    node.set_value(node.get_value() + this->moduleA.get_correction_factor());

    // Salva nodo na tabela de definição global
    this->global_definition_table.push_back(node);
  }

  for (auto node : this->moduleB.get_definition_table()) {
    // Aplica fator de correção para tabela de definição global
    node.set_value(node.get_value() + this->moduleB.get_correction_factor());

    // Salva nodo na tabela de definição global 
    this->global_definition_table.push_back(node);
  }

  for ( auto node : this->global_definition_table ) {
    std::cout << node.get_symbol() << " " << node.get_value() << std::endl;
  }
}

void Ligador::carrega_modulos() {
  this->carrega_tabela(this->fileA, this->moduleA.get_use_table(), "TABLE USE");
  this->carrega_tabela(this->fileA, this->moduleA.get_definition_table(), 
    "TABLE DEFINITION");
  this->load_list(this->fileA, this->moduleA.get_relative(), "RELATIVE");
  this->load_list(this->fileA, this->moduleA.get_code(), "CODE");
  this->carrega_tabela(this->fileB, this->moduleB.get_use_table(), "TABLE USE");
  this->carrega_tabela(this->fileB, this->moduleB.get_definition_table(), 
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
  ligador.create_global_code();
  ligador.create_global_defition_table();

  return 0;
}