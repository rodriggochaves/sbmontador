#include <iostream>
#include <list>
#include "node.hpp"

class Module
{
  std::list<Node> use_table;
  std::list<Node> definition_table;
  std::list<int> relative;
  std::list<int> code;
  int correction_factor;
public:
  void set_use_table(std::list<Node>& roll);
  std::list<Node>& get_use_table();
  std::list<Node>& get_definition_table();
  std::list<int>& get_relative();
  std::list<int>& get_code();
  void set_correction_factor(int value);
  int get_correction_factor();
};