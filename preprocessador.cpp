#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <list>
#include "node.hpp"

// EQU List Node
struct equNode {
  std::string symbol;
  std::string value;
} typedef EquNode;

// Classe
class Preprocessador {
    std::ifstream file;
    std::fstream processedFile;
    std::vector<EquNode> equList;
  public:
    Preprocessador(std::string namefile);
    void processFile();
    bool getDirectiveIF(std::string line, bool& block);
    std::string getDirectiveEQU(std::string line);
    std::string translateEQU(std::string line);
    std::string removeComment(std::string line);
    std::string removeMultipleSpaces(std::string line);
    std::string lowercaseString(std::string word);
    std::string removeChar(std::string word, char c);
};

// Construtor do preprocessador
Preprocessador::Preprocessador(std::string namefile) {
  std::string processed_namefile = namefile + "Processado.asm";
  namefile = namefile + ".asm";

  this->file.open(namefile);
  if (!this->file.good()) {
    std::cout << "Erro na Abertura do arquivo" << std::endl;
    std::exit(1);
  }
  this->processedFile.open(processed_namefile, std::fstream::out | 
     std::ifstream::app);
}

// lê o arquivo linha a linha e passa para as funções de tratamento
void Preprocessador::processFile() {
  std::string line;
  std::string newLine;
  bool isIF = false;
  bool blocked = false;

  while(std::getline(this->file, line)) {
    if( !blocked ) {
      // deixa a linha mais bonita (sem comentátio e espaços)
      line = this->removeComment( line );
      line = this->removeMultipleSpaces( line );

      // diretiva EQU
      line = this->getDirectiveEQU( line );

      // diretiva IF
      // essa função tem um efeito colateral de alterar o arquivo de saída
      // adicionando um espaço em branco no lugar do IF se encontrado.
      isIF = this->getDirectiveIF( line, blocked );

      // Remove EQU
      newLine = this->translateEQU( line );

      if ( !isIF ) {
        // output
        // std::cout << newLine;
        this->processedFile << newLine;
      }
    } else {
      blocked = false;
    }
  }
}

// recebe uma linha e remove os comentário da mesma.
std::string Preprocessador::removeComment(std::string line) {
  std::string newLine;

  for (int i = 0; i < line.size(); ++i) {
    if (line[i] == ';') {
      newLine += '\n';
      break;
    }
    newLine += line[i];
  }
  newLine += '\n';

  return newLine;
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
std::string Preprocessador::removeMultipleSpaces(std::string line) {
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

std::string Preprocessador::getDirectiveEQU(std::string line) {
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

    EquNode newNode;
    newNode.symbol = tokenLine[0];
    newNode.value = tokenLine[2];

    this->equList.push_back(newNode);

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
    for (auto node : this->equList) {
      if (token == node.symbol) {
        position = newLine.find(node.symbol);
        newLine = line.replace(position, node.symbol.size(), node.value);
      }
    }
  }
  return newLine;
}

bool Preprocessador::getDirectiveIF( std::string line, bool& block ) {
  std::string token;
  std::stringstream ss(line);
  bool hasIF = false;

  // verifica se na linha tem a diretiva IF.
  while( ss >> token ) {
    if ( this->lowercaseString(token) == "if" ) {
      hasIF = true;
    }
  }

  // se a linha tem IF, então pecorre a lista de EQU procurando se esse símbolo
  // já foi definido.
  if ( hasIF ) {
    // defini-se true para caso a tabela de EQU esteja vazia, o que vai ocorrer
    // que não temos nenhum simbolo definido ainda.
    this->processedFile << std::endl;
    block = true;
    for ( auto node : this->equList ) {
      if ( node.symbol == token) {
        block = false;
      } else {
        block = true;
      }
    }
  }

  return hasIF;
}

// main
int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cout << "Por favor, informe um nome de um arquivo\n";
    return 0;
  }

  Preprocessador preprocessador(argv[1]);
  preprocessador.processFile();

  return 0;
}
