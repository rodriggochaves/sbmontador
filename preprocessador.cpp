#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include "Node.hpp"

// Classe
class Preprocessador {
    std::ifstream file;
    std::fstream processed_file;
    std::vector<Node> equ_list;
  public:
    Preprocessador(std::string namefile);
    void process_file();
    std::string remove_comment(std::string line);
    std::string remove_multiple_spaces(std::string line);
};

// Construtor do preprocessador
Preprocessador::Preprocessador(std::string namefile) {
  namefile = namefile + ".asm";
  std::string processed_namefile = namefile + ".o";

  this->file.open(namefile);
  if (!this->file.good()) {
    std::cout << "Erro na Abertura do arquivo" << std::endl;
    std::exit(1);
  }
  this->processed_file.open(processed_namefile, std::fstream::out | 
     std::ifstream::app);
}

// lê o arquivo linha a linha e passa para as funções de tratamento
void Preprocessador::process_file() {
  std::string line;
  std::string newline;

  while(std::getline(this->file, line)) {
    line = this->remove_comment(line);
    newline = this->remove_multiple_spaces(line);
    std::cout << newline;
    this->processed_file << newline;
  }
}

// recebe uma linha e remove os comentário da mesma.
std::string Preprocessador::remove_comment(std::string line) {
  std::string newline;

  for (int i = 0; i < line.size(); ++i) {
    if (line[i] == ';') {
      break;
    }
    newline += line[i];
  }
  newline += '\n';

  return newline;
}

bool Both_are_spaces(char lhs, char rhs) {
  return (lhs == rhs) && (lhs == ' '); 
}

// recebe uma linha e remove multiplos espaços consecutivos
std::string Preprocessador::remove_multiple_spaces(std::string line) {
  std::string newline;

  std::string::iterator new_end = std::unique(line.begin(), line.end(), 
    Both_are_spaces);
  line.erase(new_end, line.end());
  newline = line;
  if (newline[0] == ' ') {
    newline.erase(0, 1);
  }

  return newline;
}

// main
int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cout << "Por favor, informe um nome de um arquivo\n";
    return 0;
  }

  Preprocessador preprocessador(argv[1]);
  preprocessador.process_file();

  return 0;
}
