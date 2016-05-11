#include <iostream>
#include <fstream>
#include <regex>
#include <list>
#include "node.hpp"

// EQU List Node
struct equNode {
  std::string symbol;
  std::string value;
} typedef EquNode;


class Preprocessador {
    std::ifstream file;
    std::ofstream processedFile;
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
    bool symbolAlreadyDefinied(std::vector<EquNode> symbolList, 
        std::string token);
};
