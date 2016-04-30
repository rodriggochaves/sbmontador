#include <iostream>
#include <list>
#include "nodo.hpp"

class Module
{
  std::list<Nodo> use_table;
  std::list<Nodo> definition_table;
  std::list<int> relative;
  std::list<int> code;
public:
  std::list<Nodo> get_use_table();
  std::list<Nodo> get_definition_table();
  std::list<int> get_relative();
  std::list<int> get_code();
};