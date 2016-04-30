#include "node.hpp"

Node::Node(std::string symbol, int value) {
  this->symbol = symbol;
  this->value = value;
}

Node::Node() {
  
}

int Node::get_value() {
  return this->value;
}

std::string Node::get_symbol() {
  return this->symbol;
}

void Node::set_value(int value) {
  this->value = value;
}

void Node::set_symbol(std::string symbol) {
  this->symbol = symbol;
}