#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Preprocessador {
    std::ifstream file;
  public:
    Preprocessador(std::string namefile);
    void readfile();
    void metodo();
};

Preprocessador::Preprocessador(std::string namefile) {
  std::cout << "Construindo preprocessador\n";
  this->file.open(namefile);
}

void Preprocessador::readfile() {
  std::string line;

  while(std::getline(file, line)) {
    std::istringstream iss(line);
    std::cout << line << '\n';
  }
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cout << "Por favor, informe um nome de um arquivo\n";
    return 0;
  }

  Preprocessador preprocessador(argv[1]);
  preprocessador.readfile();

  return 0;
}
