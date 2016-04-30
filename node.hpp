#include <iostream>
#include <string>

// Aqui é representado um nodo da tabela de definicação global e uso.

class Node
{
  std::string symbol;
  int value;
public:
  Node(std::string symbol, int value);
  int get_value();
  void set_value(int value);
  std::string get_symbol();
  void set_symbol(std::string symbol);
};