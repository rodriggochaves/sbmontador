#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <list>
#include <sstream>
#include "ligador.hpp"


// Abre o ponteiro para os dois arquivos modulos.
Ligador::Ligador(std::string fileA, std::string fileB, std::string out) {
  this->fileA.open(fileA + ".txt");
  this->fileB.open(fileB + ".txt");
  this->out.open(out + ".txt", std::fstream::out);
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
}

Node Ligador::find_symbol(std::string wanted) {
  Node null_node("", 0);

  for (auto node : this->global_definition_table) {
    if (wanted == node.get_symbol()) return node;
  }
  return null_node;
}

void Ligador::resolve_cross_references() {
  std::string symbol;
  int address;
  int value;
  Node definition_node;

  for (auto use_node : this->moduleA.get_use_table()) {
    // vê o simbolo na tabela de uso
    symbol = use_node.get_symbol();

    // recupera o endereço do simbolo no codigo
    address = use_node.get_value() + this->moduleA.get_correction_factor();

    // encontra simbolo na tabela de definicao
    definition_node = this->find_symbol(symbol);

    // ve o valor do simbolo na tabela de simbolos
    value = definition_node.get_value();

    // soma o valor do símbolo ao valor relativo no código
    this->global_code[address] = this->global_code[address] + value;
  }

  for (auto use_node : this->moduleB.get_use_table()) {
    // vê o simbolo na tabela de uso
    symbol = use_node.get_symbol();

    // recupera o endereço do simbolo no codigo
    address = use_node.get_value() + this->moduleB.get_correction_factor();

    // encontra simbolo na tabela de definicao
    definition_node = this->find_symbol(symbol);

    // ve o valor do simbolo na tabela de simbolos
    value = definition_node.get_value();

    // soma o valor do símbolo ao valor relativo no código
    this->global_code[address] = this->global_code[address] + value;
  }
}

void Ligador::resolve_relative_address() {

  for( auto address : this->moduleA.get_relative() ) {
    address = address + this->moduleA.get_correction_factor();

    this->global_code[address] = this->global_code[address] 
      + this->moduleA.get_correction_factor();
  }

  for( auto address : this->moduleB.get_relative() ) {
    address = address + this->moduleB.get_correction_factor();

    this->global_code[address] = this->global_code[address] 
      + this->moduleB.get_correction_factor();
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

void Ligador::print_code() {
  for ( auto code : this->global_code ) {
    this->out << code << " ";
  }
}

void Ligador::liga(){
  carrega_modulos();
  create_global_code();
  create_global_defition_table();
  resolve_cross_references();
  resolve_relative_address();
  print_code();
}

