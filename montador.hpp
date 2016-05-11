#include <iostream>

class Montador{

public:
	Montador();
	bool montar(std::string sourceName, std::string objectName);

private:
	struct SymbolTable *symbolTable;
	struct Table *definitionTable;
	struct Table *usageTable;
	bool errorFlag;

	int dataPos;

	void primeiraPassagem(std::string sourceName);
	void segundaPassagem(std::string sourceName, std::string outName);
	void getToken();
	bool isLabel(std::string token);
	void addSymbol(std::string token, int position);
	void addToTable(std::string symbol,int position,struct Table*& table);
	int getSymbolAddress(std::string token);
	bool isExternalSymbol(std::string token);
	int getOPCode(std::string token);
	int OPSize(std::string token);
	bool isDirective(std::string token);
	int callDirective(std::string token);
	void generateObject(int position);
	int writeInstruction (int OPCode,std::istringstream& lstream,std::ostringstream& sout,int pc);
	bool writeSymbol (std::string arg,std::ostringstream& sout,int pc);
};
