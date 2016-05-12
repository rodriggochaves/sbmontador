
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include "montador.hpp"

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


Montador::Montador(){
  symbolTable = NULL;
  definitionTable = NULL;
  usageTable = NULL;
  errorFlag = false;
  dataPos = -1;
}


void Montador::primeiraPassagem(string sourceName){
  sourceName = sourceName + ".asm";
  ifstream inStream;
  inStream.open(sourceName.c_str(),ifstream::in);
  if (inStream.is_open() == false){
    cout << "Erro: arquivo nao encontrado.\n";
    errorFlag = true;
    return;
  }
  string line;
  int programCounter = 0;
  int lineCounter = 0;
  int hasSection = 0;
  while (inStream.eof() == false){

    getline(inStream,line);/* le uma linha */
    lineCounter++;
    while ((line == "")||(line == "\r")){
      getline(inStream,line);
      lineCounter++;
      if (inStream.eof()==true){
        break;
      }
    }

    if(inStream.eof() == true){/* erro de leitura */
      break;
    }

    unsigned int i;
    for (i=0;i<line.size();i++){/* transforma em letras minusculas */
      line.at(i) = tolower(line.at(i));
    }
    istringstream lstream(line);/* cria buffer para ler a linha de token em token */
    string token = "";

    if (!(lstream >> token)){
    continue;
    }

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
      
      else if (token == "section"){
        string token2;
        lstream >> token2;
        if (hasSection == 0){
          if (token2 != "text"){
            cout << "Erro semantico: Secao inadequada na linha " << lineCounter << ".\n";
            errorFlag = true; 
          }
        }
        if (hasSection == 1){
          if (token2 != "data"){
            cout << "Erro semantico: Secao inadequada na linha " << lineCounter << ".\n";
            errorFlag = true;
          }
          else {
          	dataPos = programCounter;
          }
        }
        if (hasSection >= 2){
          cout << "Erro semantico: Secao inadequada na linha " << lineCounter << ".\n";
          errorFlag = true;
        }
        hasSection++;
      }
      
      else if (token == "extern"){
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
  
  if (hasSection < 2){
  	cout << "Erro semantico: nao ha secao text ou data.\n";
  	errorFlag = true;
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

  inStream.close();
}

void Montador::segundaPassagem(string sourceName, string outName){
  sourceName = sourceName + ".asm";
  ifstream inStream;
  inStream.open(sourceName.c_str(),ifstream::in);
  if (inStream.is_open() == false){
    return;
  }
  
  ostringstream codeString;
  ostringstream relative;

  string line;
  int programCounter = 0;
  int lineCounter = 0;
  int isModule = 0;
  bool hasStop = false;

  while (inStream.eof() == false){

    getline(inStream,line);/* le uma linha */
    lineCounter++;
    while (line == ""||(line == "\r")){
      getline(inStream,line);
      lineCounter++;
      if (inStream.eof()==true){
        break;
      }
    }

    if(inStream.eof() == true){/* erro de leitura */
      break;
    }

    unsigned int i;
    for (i=0;i<line.size();i++){/* transforma em letras minusculas */
      line.at(i) = tolower(line.at(i));
    }
    istringstream lstream(line);/* cria buffer para ler a linha de token em token */
    string token = "";
	
	if (!(lstream >> token)){
    continue;
    }
    
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
      else { // opcode STOP
      	hasStop = true;
      }
      if (programCounter >= dataPos){ // instrucao na section data
      	cout << "Erro semantico: instrucao na secao de dados na linha " << lineCounter << ".\n";
      	errorFlag = true;
      }
      else{
		  int aux = writeInstruction (OPCode,lstream,codeString,programCounter);
		  if (aux < 0){
		    cout << /*"Erro sintatico: argumento invalido ou erro semantico: operacao com enderecos inadequados na linha " <<*/lineCounter << ".\n";
		    errorFlag = true;
		  }
		  else{
		    programCounter = aux;
		  }
		}
	}

    else if (isDirective (token)){
      if (token == "space"){
      	if (programCounter < dataPos){
      		cout << "Erro semantico: diretiva em secao inadequada na linha " << lineCounter << ".\n";
      		errorFlag = true;
      	}
        string token2 = "";
        lstream >> token2;
        if (token2 == ""){
          codeString << "0 ";
          programCounter += 1;
        }else {
          bool isNumber = (token2.find_first_not_of("0123456789") == string::npos);
          if (isNumber==false){
            cout << "Erro sintatico: argumento invalido na linha " << lineCounter << ".\n";
            errorFlag = true;
          }
          int i;
          
          int aux = stoi(token2,nullptr,10);
          for (i=0;i<aux;i++){
            codeString << "0 ";
          }
          programCounter += aux;
          
          string token3;
          if ((lstream >> token3) && (token3!="") && (token3.at(0)!=';')){
          	cout << "Erro sintatico: construcao incorreta na linha " << lineCounter << ".\n";
          	errorFlag = true;
          }
        }
      }
      else if (token == "const"){
      	if (programCounter < dataPos){
      		cout << "Erro semantico: diretiva em secao inadequada na linha " << lineCounter << ".\n";
      		errorFlag = true;
      	}
        string token2;
        lstream >> token2;
        string aux = token2;

        if (aux.size()>1 && aux.at(0) == '-'){
          aux = aux.substr(1,aux.size() - 1);
        }
        bool isNumber = (aux.find_first_not_of("0123456789") == string::npos);
        if (isNumber==false||token2==""||token2.at(0)==';'){
          cout << "Erro sintatico: argumento invalido na linha " << lineCounter << ".\n";
          errorFlag = true;
        }
        codeString << token2 << " ";
        programCounter += 1;
        
        string token3;
        if ((lstream>>token3) && (token3!="") && (token3.at(0)!=';')){
          cout << "Erro sintatico: construcao incorreta na linha " << lineCounter << ".\n";
          errorFlag = true;
        }
      }
      else if (token == "begin"){
        if (isModule != 0){
          cout << "Erro semantico: inicio de modulo incorreto na linha " << lineCounter << ".\n";
          errorFlag = true;
        }
        isModule++;
      }
      else if (token == "end"){
        if (isModule != 1){
          cout << "Erro semantico: fim de modulo incorreto na linha " << lineCounter << ".\n";
          errorFlag = true;
        }
        isModule++;
      }
    }
    else {
      errorFlag = true;
    }
  }
  
  if (isModule == 1){
  	cout << "Erro semantico: modulo declarado incorretamente.\n";
  	errorFlag = true;
  }
  if (isModule == 0 && hasStop == false){
  	cout << "Erro semantico: programa sem funcao STOP.\n";
  	errorFlag = true;
  }
  
  /* escreve no arquivo de saida */
  
  if (errorFlag == true){
    return;
  }

  outName = outName + ".txt";
  ofstream out(outName.c_str(),fstream::trunc);

  if (isModule == 2){
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
    out << relative.str() << endl;

    out << "\nCODE\n";
  }
  
  out << codeString.str();
  out.close();
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
  
  bool isJump = false;
  if (OPCode > 4 && OPCode < 9){
  	isJump = true;
  }
  
  sout << OPCode << " ";
  pc++;
  if (OPCode == 14){ /* stop */
    string arg1;
    if(arg1 == "" || arg1.at(0) == ';'){ /* fim da linha ou comentario */
      return pc;
    }
    cout << "Erro sintatico: construcao inadequada na linha ";
    return -1;
  }

  if (OPCode == 9){ /* copy */
    string arg1, arg2, arg3;
    /* trata 1o argumento */
    lstream >> arg1;
    if (isdigit(arg1.at(0))){
      cout << "Erro lexico: gramatica incorreta na linha ";
      return -1;
    }
    if (arg1 == "" || arg1.at(0) == ';' || arg1.at(arg1.size()-1) != ','){ /* fim da linha ou formato incorreto*/
      cout << "Erro sintatico: construcao inadequada na linha ";
      return -1;
    }
    arg1 = arg1.substr(0,arg1.size()-1); /* remove a virgula do operador */
    if (writeSymbol(arg1,sout,pc,isJump)==false){ /* escreve argumento */
      return -1;
    }
    pc++;
    /* trata segundo argumento */
    lstream >> arg2;
    if (isdigit(arg2.at(0))){
      cout << "Erro lexico: gramatica incorreta na linha ";
      return -1;
    }
    if (arg2 == "" || arg2.at(0) == ';'){ /* fim da linha */
      cout << "Erro sintatico: construcao inadequada na linha ";
      return -1;
    }
    if (writeSymbol (arg2,sout,pc,isJump) == false){ /* escreve argumento */
      return -1;
    }
    pc++;
    /* verifica fim da linha */
    lstream >> arg3;
    if (arg3 == "" || arg2.at(0) == ';'){ /* fim da linha */
      return pc;
    }
    cout << "Erro sintatico: construcao inadequada na linha ";
    return -1;
  }
  
  /* outros */
  
  string arg1;
  lstream >> arg1;
  if (arg1 == "" || arg1.at(0) == ';'){ /* fim da linha ou comentario */
  	cout << "Erro sintatico: construcao inadequada na linha ";
    return -1;
  }
  if (isdigit(arg1.at(0))){
      cout << "Erro lexico: gramatica incorreta na linha ";
      return -1;
  }
  if (writeSymbol (arg1,sout,pc,isJump) == false){
    return -1;
  }
  pc++;
  string arg2; /* verifica que nao ha mais argumentos */
  lstream >> arg2;
  if (arg2 == "" || arg2.at(0) == ';'){ /* fim da linha ou comentario */
    return pc;
  }
  cout << "Erro sintatico: construcao inadequada na linha ";
  return -1;
}

bool Montador::writeSymbol (string arg, ostringstream& sout, int pc, bool isJump){

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
      	cout << "Erro sintatico: simbolo indefinido na linha ";
        return false;
      }
      if (isExternalSymbol(aux) == true){
        addToTable(aux,pc,usageTable);
      }
      else{
      	if (((isJump==true)&&(address>=dataPos))||((isJump==false)&&(address<dataPos))){
      		cout << "Erro semantico: acesso a endereco inadequado na linha ";
      		return false; // pulo para fora de TEXT ou operacao com label fora de DATA
      	}
      }
      aux = arg.substr(i+1,arg.size()-i); /* depois um numero natural */

      bool isNumber = (aux.find_first_not_of("0123456789") == string::npos);
      if (isNumber == false){
      	cout << "Erro sintatico: construcao inadequada na linha ";
        return false;
      }
      sout << (address + atoi (aux.c_str())) << " "; /* output simbolo + offset */
      return true;
    }
  }

  int address = getSymbolAddress(arg); /* se nao ha operacao, o argumento deve ser um simbolo valido */
  if (address < 0){ /* simbolo invalido */
  	cout << "Erro sintatico: simbolo indefinido na linha ";
    return false;
  }
  if (isExternalSymbol(arg) == true){
    addToTable(arg,pc,usageTable);
  }
  else{
    if (((isJump==true)&&(address>=dataPos))||((isJump==false)&&(address<dataPos))){
      cout << "Erro semantico: acesso a endereco inadequado na linha ";
      return false; // pulo para fora de TEXT ou operacao com label fora de DATA
    }
  }
  sout << address << " ";
  return true;
}


bool Montador::montar(string sourceName, string objectName){
  primeiraPassagem(sourceName);
  if (errorFlag == false){
  	segundaPassagem(sourceName, objectName);
  }
  return errorFlag;
}
