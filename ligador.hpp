#pragma once

#include <iostream>
#include <fstream>
#include <regex>
#include <list>
#include "module.hpp"

class Ligador {
  Module moduleA;
  Module moduleB;
  std::vector<Node> global_definition_table;
  std::vector<int> global_code;
  std::fstream fileA;
  std::fstream fileB;
  std::fstream out;
public:
  Ligador(std::string fileA, std::string fileB, std::string out);
  void load_list(std::fstream& file, std::list<int>& roll, std::string header);
  void carrega_tabela(std::fstream& file, std::list<Node>& roll, 
    std::string header);
  void carrega_modulos();
  void create_global_defition_table();
  void create_global_code();
  void resolve_cross_references();
  void resolve_relative_address();
  void print_code();
  Node find_symbol(std::string wanted);
  void liga();
};
