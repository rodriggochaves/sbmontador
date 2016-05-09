/*
 * Montador.cpp
 *
 *  Created on: Apr 27, 2016
 *      Author: guido
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>

using namespace std;

struct SymbolTable{
	string symbol;
	int address;
	bool external;
	struct SymbolTable *next;
};
struct TabelaDiretivas{
	string symbol;

};
struct Table{
	string symbol;
	int address;
	struct Table *next;
};

class Montador{

public:
	Montador();
	void montar(string sourceName, string objectName);
	ifstream fstream;

private:
	struct SymbolTable *symbolTable;
	struct Table *definitionTable;
	struct Table *usageTable;
	bool errorFlag;

	void primeiraPassagem(string sourceName);
	void segundaPassagem(string sourceName, string outName);
	void getToken();
	bool isLabel(string token);
	void addSymbol(string token, int position);
	void addToTable(string symbol,int position,struct Table*& table);
	int getSymbolAddress(string token);
	bool isExternalSymbol(string token);
	int getOPCode(string token);
	int OPSize(string token);
	bool isDirective(string token);
	int callDirective(string token);
	void generateObject(int position);
	int writeInstruction (int OPCode,istringstream& lstream,ostringstream& sout,int pc);
	bool writeSymbol (string arg,ostringstream& sout,int pc);

};

Montador::Montador(){
	symbolTable = NULL;
	definitionTable = NULL;
	usageTable = NULL;
	errorFlag = false;
}


void Montador::primeiraPassagem(string sourceName){
	sourceName = sourceName + ".asm";
	ifstream fstream;
	fstream.open(sourceName.c_str(),ifstream::in);
	if (fstream.is_open() == false){
		cout << "Erro: arquivo nao encontrado.\n";
		errorFlag = true;
	}
	string line;
	int programCounter = 0;
	int lineCounter = 0;
	while (fstream.eof() == false){

		getline(fstream,line);/* le uma linha */
		lineCounter++;
		while ((line == "")||(line == "\r")){
			getline(fstream,line);
			lineCounter++;
			if (fstream.eof()==true){
				break;
			}
		}

		if(fstream.eof() == true){/* erro de leitura */
			break;
		}

		unsigned int i;
		for (i=0;i<line.size();i++){/* transforma em letras minusculas */
			line.at(i) = tolower(line.at(i));
		}
		istringstream lstream(line);/* cria buffer para ler a linha de token em token */
		string token;

		lstream >> token;

		if (token.at(0)==';'){
			continue;
		}

		if (isLabel(token)){/* se é label,adiciona à tabela de simbolos */
			token = token.substr(0,token.size() - 1); /* remove ':' */
			if (getSymbolAddress(token) >= 0){
				cout << "Erro semantico: simbolo redefinido na linha " << lineCounter << ".\n";
				errorFlag = true;
				continue;
			}
			addSymbol(token,programCounter);
			lstream >> token;
		}

		if (getOPCode(token) >= 0){
			programCounter += OPSize(token);
		}
		else if (isDirective (token)){
			if (token == "space"){
				int token2 = -1;
				lstream >> token2;
				if (token2 == -1){
					programCounter += 1;
				}else {
					programCounter += token2;
				}
			}
			else if (token == "const"){
				programCounter += 1;
			}
			else if (token == "extern"){
				int i = 0;
				struct SymbolTable *aux = symbolTable;
				while (aux->next != NULL){
					aux = aux->next;
				}
				aux->external = true;
				aux->address = 0;
			}
			else if (token == "public"){
				lstream >> token;
				if (token == ""){
					cout << "Erro sintatico: faltam argumentos na linha " << lineCounter << ".\n";
					errorFlag = true;
				}
				else{
					addToTable (token, programCounter, definitionTable);
				}
			}
		}
		else {
			cout << "Erro sintatico: comando invalido na linha " << lineCounter << ".\n";
			errorFlag = true;
			continue;
		}
	}
	/* completa tabela de definicoes */
	struct Table *def = definitionTable;
	struct SymbolTable *sym = symbolTable;

	while (def != NULL){
		sym = symbolTable;
		while ((sym != NULL) && (sym->symbol != def->symbol)){
			sym = sym->next;
		}
		if (sym == NULL){
			cout << "Erro semantico: simbolo indefinido em " << def->address << ".\n";
			errorFlag = true;
		}
		else{
			def->address = sym->address;
		}
		def = def->next;
	}

	fstream.close();
}

void Montador::segundaPassagem(string sourceName, string outName){
	sourceName = sourceName + ".asm";
	ifstream fstream;
	fstream.open(sourceName.c_str(),ifstream::in);

	ostringstream codeString;
	ostringstream relative;

	string line;
	int programCounter = 0;
	int lineCounter = 0;


	while (fstream.eof() == false){

		getline(fstream,line);/* le uma linha */
		lineCounter++;
		while (line == ""||(line == "\r")){
			getline(fstream,line);
			lineCounter++;
			if (fstream.eof()==true){
				break;
			}
		}

		if(fstream.eof() == true){/* erro de leitura */
			break;
		}

		unsigned int i;
		for (i=0;i<line.size();i++){/* transforma em letras minusculas */
			line.at(i) = tolower(line.at(i));
		}
		istringstream lstream(line);/* cria buffer para ler a linha de token em token */
		string token;

		lstream >> token;
		if (token.at(0)==';'){
			continue;
		}

		if (isdigit(token.at(0))){
			cout << "Erro lexico: gramatica incorreta na linha " << lineCounter << ".\n";
			errorFlag = true;
		}
		if (isLabel(token)){/* se é label, verifica existencia na tabela de simbolos */
			lstream >> token;
		}

		int OPCode = getOPCode(token);
		if (OPCode >= 0){
			if (OPCode != 14){
				relative << programCounter+1 << " ";
				if (OPCode == 9){
					relative << programCounter+2 << " ";
				}
			}
			int aux = writeInstruction (OPCode,lstream,codeString,programCounter);
			if (aux < 0){
				cout << "Erro semantico: construcao incorreta na linha " << lineCounter << ".\n";
				errorFlag = true;
			}
			else{
				programCounter = aux;
			}
		}

		else if (isDirective (token)){
			if (token == "space"){
				int token2 = -1;
				lstream >> token2;
				if (token2 == -1){
					codeString << "0 ";
					programCounter += 1;
				}else {
					int i;
					for (i=0;i<token2;i++){
						codeString << "0 ";
					}
					programCounter += token2;
				}
			}
			else if (token == "const"){
				string token2;
				lstream >> token2;
				string aux = token2;

				if (aux.size()>1 && aux.at(0) == '-'){
					aux = aux.substr(1,aux.size() - 1);
				}
				bool isNumber = (aux.find_first_not_of("0123456789") == string::npos);
				if (isNumber==false||token2==""||token2.at(0)==';'){
					cout << "Erro sintatico: construcao incorreta na linha " << lineCounter << ".\n";
					errorFlag = true;
				}
				codeString << token2 << " ";
				programCounter += 1;
			}
			else if (token == "section"){
				string token2;
				lstream >> token2;
			}
			else {
				programCounter += 0;
			}
		}
		else {
			cout << "Erro sintatico: comando invalido na linha " << lineCounter << ".\n";
			errorFlag = true;
			return;
		}
	}
	/* escreve no arquivo de saida */

	if (errorFlag == true){
		return;
	}

	outName = outName + ".txt";
	ofstream out(outName.c_str(),ifstream::trunc);

	out << "TABLE USE\n";
	struct Table *aux = usageTable;
	while (aux != NULL){
		out << aux->symbol << " " << aux->address << "\n";
		aux = aux->next;
	}
	out << "\nTABLE DEFINITION\n";
	aux = definitionTable;
	while (aux != NULL){
		out << aux->symbol << " " << aux->address << "\n";
		aux = aux->next;
	}

	out << "\nRELATIVE\n";
	out << relative.str();

	out << "\nCODE\n";
	out << codeString.str();
}

bool Montador::isLabel(string token){
	if (token.at(token.size()-1) == ':'){
		return true;
	}
	return false;
}

int Montador::getSymbolAddress(string token){

	if (symbolTable == NULL){
		return -1;
	}
	struct SymbolTable *aux = symbolTable;
	while (aux->next != NULL){
		if (token == aux->symbol){
			return aux->address;
		}
		aux = aux->next;
	}
	if (token == aux->symbol){
		return aux->address;
	}
	return -1;
}

bool Montador::isExternalSymbol(string token){

	if (symbolTable == NULL){
		return false;
	}
	struct SymbolTable *aux = symbolTable;
	while (aux->next != NULL){
		if (token == aux->symbol){
			return aux->external;
		}
		aux = aux->next;
	}
	return false;
}

int Montador::getOPCode(string token){

	if (token == "add")
		return 1;
	if (token == "sub")
		return 2;
	if (token == "mult")
		return 3;
	if (token == "div")
		return 4;
	if (token == "jmp")
		return 5;
	if (token == "jmpz")
		return 6;
	if (token == "jmpp")
		return 7;
	if (token == "jmpz")
		return 8;
	if (token == "copy")
		return 9;
	if (token == "load")
		return 10;
	if (token == "store")
		return 11;
	if (token == "input")
		return 12;
	if (token == "output")
		return 13;
	if (token == "stop")
		return 14;
	return -1;
}

int Montador::OPSize(string token){
	if (token == "copy"){
		return 3;
	}
	if (token == "stop"){
		return 1;
	}
	return 2;
}

bool Montador::isDirective(string token){

	if (token == "section"){
		return true;
	}
	if (token == "space"){
		return true;
	}
	if (token == "const"){
		return true;
	}
	if (token == "public"){
		return true;
	}
	if (token == "equ"){
		return true;;
	}
	if (token == "if"){
		return true;
	}
	if (token == "extern"){
		return true;
	}
	if (token == "begin"){
		return true;
	}
	if (token == "end"){
		return true;
	}
	return false;
}

int Montador::callDirective(string token){

	if (token == "section"){
		// escreve section no arquivo objeto
		return 0;
	}else if (token == "space"){
		// aloca espaco
		return 1;
	}else if (token == "const"){
		// aloca 16 bits
		return 1;
	}else if (token == "public"){
		// tabela de definicoes
		return 0;
	}else if (token == "equ"){
		// define simbolo
		return 0;
	}else if (token == "if"){
		// escreve if
		return 0;
	}else if (token == "extern"){
		// tabela de uso
		return 0;
	}else if (token == "begin"){
		// inicia modulo
		return 0;
	}else if (token == "end"){
		// termina modulo
		return 0;
	}
	return -1;
}

void Montador::addToTable(string symbol,int position,struct Table*& table){

	if (symbol.at(symbol.size()-1) == ':'){
		symbol = symbol.substr(0,symbol.size() - 1); /* remove ':' */
	}

	if (table == NULL){
		table = new struct Table;
		table->address = position;
		table->symbol = symbol;
		table->next = NULL;
		return;
	}

	struct Table *aux = table;
	while (aux->next != NULL){
		aux = aux->next;
	}
	aux->next = new struct Table;
	aux = aux->next;
	aux->address = position;
	aux->symbol = symbol;
	aux->next = NULL;
}

void Montador::addSymbol(string symbol, int position){

	if (symbol.at(symbol.size()-1) == ':'){
		symbol = symbol.substr(0,symbol.size() - 1); /* remove ':' */
	}

	if (symbolTable == NULL){
		symbolTable = new struct SymbolTable;
		symbolTable->address = position;
		symbolTable->symbol = symbol;
		symbolTable->external = false;
		symbolTable->next = NULL;
		return;
	}
	struct SymbolTable *aux = symbolTable;
	while (aux->next != NULL){
		aux = aux->next;
	}
	aux->next = new struct SymbolTable;
	aux = aux->next;
	aux->address = position;
	aux->symbol = symbol;
	aux->external = false;
	aux->next = NULL;
}

int Montador::writeInstruction(int OPCode, istringstream& lstream, ostringstream& sout, int pc){
	sout << OPCode << " ";
	pc++;
	if (OPCode == 14){ /* stop */
		string arg1;
		lstream >> arg1;
		if (arg1 == "" || arg1.at(0) == ';'){ /* fim da linha ou comentario */
			return pc;
		}
		return -1;
	}

	if (OPCode == 9){ /* copy */
		string arg1, arg2, arg3;
		/* trata 1o argumento */
		lstream >> arg1;
		if (arg1 == "" || arg1.at(0) == ';' || arg1.at(arg1.size()-1) != ','){ /* fim da linha ou formato incorreto*/
			return -1;
		}
		arg1 = arg1.substr(0,arg1.size()-1); /* remove a virgula do operador */
		if (writeSymbol(arg1,sout,pc)==false){ /* escreve argumento */
			return -1;
		}
		pc++;
		/* trata segundo argumento */
		lstream >> arg2;
		if (arg2 == "" || arg2.at(0) == ';'){ /* fim da linha */
			return -1;
		}
		if (writeSymbol (arg2,sout,pc) == false){ /* escreve argumento */
			return -1;
		}
		pc++;
		/* verifica fim da linha */
		lstream >> arg3;
		if (arg3 == "" || arg2.at(0) == ';'){ /* fim da linha */
			return pc;
		}
		return -1;
	}
	/* outros */
	string arg1;
	lstream >> arg1;
	if (arg1 == "" || arg1.at(0) == ';'){ /* fim da linha ou comentario */
		return -1;
	}
	if (writeSymbol (arg1,sout,pc) == false){
		return -1;
	}
	pc++;
	string arg2; /* verifica que nao ha mais argumentos */
	lstream >> arg2;
	if (arg2 == "" || arg2.at(0) == ';'){ /* fim da linha ou comentario */
		return pc;
	}
	return -1;
}

bool Montador::writeSymbol (string arg, ostringstream& sout, int pc){

	if (arg.size()>=3){ /* pode ter operacao (a+b) */
		bool hasOp = false;
		unsigned int i;
		for (i=0;i<(arg.size()-1) && hasOp == false;i++){ /* checa se ha uma soma ou subtracao */
			if (arg.at(i) == '+' || arg.at(i) == '-'){
				hasOp = true;
			}
		}
		if (hasOp == true){ /* se ha operacao */
			i--;
			string aux = arg.substr(0,i); /* antes da operacao deve haver um simbolo valido */
			int address = getSymbolAddress(aux);
			if (address < 0){
				return false;
			}
			if (isExternalSymbol(aux) == true){
				addToTable(aux,pc,usageTable);
			}
			aux = arg.substr(i+1,arg.size()-i); /* depois um numero natural */

			bool isNumber = (aux.find_first_not_of("0123456789") == string::npos);
			if (isNumber == false){
				return false;
			}
			sout << (address + atoi (aux.c_str())) << " "; /* output simbolo + offset */
			return true;
		}
	}

	int address = getSymbolAddress(arg); /* se nao ha operacao, o argumento deve ser um simbolo valido */
	if (address < 0){ /* simbolo invalido */
		return false;
	}
	if (isExternalSymbol(arg) == true){
		addToTable(arg,pc,usageTable);
	}
	sout << address << " ";
	return true;
}


void Montador::montar(string sourceName, string objectName){
	primeiraPassagem(sourceName);
	segundaPassagem(sourceName, objectName);
}


int main (int argc, char* argv[]){
	if (argc != 3){
		cout << "argumentos invalidos";
		return 0;
	}
	std::string inFile = argv[1];
	std::string outFile = argv[2];
	Montador montador;
	montador.montar(inFile, outFile);
	return 0;
}
