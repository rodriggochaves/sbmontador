#include "module.hpp"

void Module::set_use_table(std::list<Node>& roll) {
  this->use_table = roll;
}

std::list<Node>& Module::get_use_table() {
  return this->use_table;
}

std::list<Node>& Module::get_definition_table() {
  return this->definition_table;
}

std::list<int>& Module::get_relative() {
  return this->relative; 
}

std::list<int>& Module::get_code() {
  return this->code; 
}

void Module::set_correction_factor(int value) {
  this->correction_factor = value;
}

int Module::get_correction_factor() {
  return this->correction_factor;
}