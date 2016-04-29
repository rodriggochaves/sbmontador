#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

class Ligador {
  std::fstream moduleA;
  std::fstream moduleB;
  std::fstream out;
public:
  Ligador(std::string moduleA, std::string moduleB);
};

// Abre o ponteiro para os dois arquivos modulos.
Ligador::Ligador(std::string moduleA, std::string moduleB, std::string out) {
  this->moduleA.open(moduleA + ".o");
  this->moduleB.open(moduleB + ".o");
  this->out.open(out + ".e");
}

int main(int argc, char const *argv[])
{
  if (argc < 4) {
    std::cout << "Por favor, informe nome de dois arquivos objetos." 
      << std::endl;
    return 0;
  }

  Ligador ligador(argv[1], argv[2], argv[3]);

  return 0;
}