#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <list>
#include "Node.hpp"

// EQU List Node
struct equ_node {
  std::string symbol;
  std::string value;
} typedef Equ_node;

// Classe
class Preprocessador {
    std::ifstream file;
    std::fstream processed_file;
    std::vector<Equ_node> equ_list;
  public:
    Preprocessador(std::string namefile);
    void process_file();
    std::string get_directive_EQU(std::string line);
    std::string translateEQU(std::string line);
    std::string remove_comment(std::string line);
    std::string remove_multiple_spaces(std::string line);
    std::string lowercaseString(std::string word);
    std::string removeChar(std::string word, char c);
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
  std::string newLine;

  while(std::getline(this->file, line)) {
    // deixa a linha mais bonita (sem comentátio e espaços)
    line = this->remove_comment(line);
    line = this->remove_multiple_spaces(line);

    // diretiva EQU
    newLine = this->get_directive_EQU(line);
    newLine = this->translateEQU(newLine);

    // output
    std::cout << newLine;
    this->processed_file << newLine;
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

std::string Preprocessador::removeChar(std::string word, char c) {
  int position = word.find(c);

  if (position != -1) {
    word.erase(position, position + 1);
  }

  return word;
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
  
  // caso a linha comece com um espaço, esse espaço é removido.
  if (newline[0] == ' ') {
    newline.erase(0, 1);
  }

  return newline;
}

std::string Preprocessador::lowercaseString(std::string word) {
  for(int i = 0; i < word.size(); i++) {
    word[i] = std::tolower(word[i]);
  }
  return word;
}

std::string Preprocessador::get_directive_EQU(std::string line) {
  std::string token;
  std::vector<std::string> tokenLine;
  bool hasEQU = false;

  std::stringstream ss(line);
  while(ss >> token) {
    if(this->lowercaseString(token) == "equ") {
      hasEQU = true;
    }
  }

  if (hasEQU) {
    std::stringstream tokenizer(line);
    while( tokenizer >> token ) {
      tokenLine.push_back(token);
    }

    tokenLine[0] = this->removeChar(tokenLine[0], ':');

    Equ_node newNode;
    newNode.symbol = tokenLine[0];
    newNode.value = tokenLine[2];

    this->equ_list.push_back(newNode);

    return "";
  } else {
    return line;
  }
}

std::string Preprocessador::translateEQU(std::string line) {
  std::stringstream ss(line);
  std::string token;
  std::string newLine = line;
  int position;

  while (ss >> token) {
    for (auto node : this->equ_list) {
      if (token == node.symbol) {
        position = newLine.find(node.symbol);
        newLine = line.replace(position, node.symbol.size(), node.value);
      }
    }
  }
  return newLine;
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
