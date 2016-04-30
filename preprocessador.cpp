#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

// Classe
class Preprocessador {
    std::ifstream file;
    std::fstream processed_file;
  public:
    Preprocessador(std::string namefile);
    void process_file();
    std::string remove_comment(std::string line);
    std::string remove_multiple_spaces(std::string line);
};

// Construtor do preprocessador
Preprocessador::Preprocessador(std::string namefile) {
  std::string processed_namefile = namefile + "o";

  this->file.open(namefile);
  this->processed_file.open(processed_namefile, std::fstream::out | 
     std::ifstream::app);
}

// lê o arquivo linha a linha
void Preprocessador::process_file() {
  std::string line;
  std::string newline;

  while(std::getline(this->file, line)) {
    newline = this->remove_comment(line);
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

// recebe uma linha e remove multiplos espaços consecutivos
std::string Preprocessador::remove_multiple_spaces(std::string line) {
  std::string newline;


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
