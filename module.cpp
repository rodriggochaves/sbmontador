#include "module.hpp"

void Module::set_use_table(std::list<Nodo>& roll) {
  this->use_table = roll;
}

std::list<Nodo>& Module::get_use_table() {
  return this->use_table;
}

std::list<Nodo>& Module::get_definition_table() {
  return this->definition_table;
}

std::list<int>& Module::get_relative() {
  return this->relative; 
}

std::list<int>& Module::get_code() {
  return this->code; 
}