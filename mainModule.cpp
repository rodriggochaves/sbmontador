#include <iostream>

#include "module.hpp"
#include "montador.hpp"
#include "preprocessador.hpp"
#include "ligador.hpp"

int main (int argc, char* argv[]){
	if ((argc < 3) || (argc > 4)){
		std::cout << "argumentos invalidos";
		return 0;
	}
	
	std::string inFile = argv[1];
	std::string inFile2;
	std::string outFile;
	
	
	if (argc == 3){
		outFile = argv[2];
	}
	if (argc == 4){
		inFile2 = argv[2];
		outFile = argv[3];
	}
	
	Preprocessador preprocessador(inFile);
	preprocessador.processFile();
	inFile = inFile + "Processado";
	
	Montador montador;
	
	if (argc == 3){
		montador.montar(inFile, outFile);
	}
	
	if (argc == 4){
		Preprocessador preprocessador2(inFile2);
		preprocessador2.processFile();
		inFile2 = inFile2 + "Processado";
		std::string outFile1 = inFile + "OTemp";
		std::string outFile2 = inFile2 + "OTemp";
		bool error;
		error = montador.montar(inFile, outFile1);
		Montador montador2;
		error = error | montador2.montar(inFile2,outFile2);
		if (error == true){
			return 0;
		}
		Ligador ligador(outFile1, outFile2, outFile);
		ligador.liga ();
	}
	
	return 0;
}
