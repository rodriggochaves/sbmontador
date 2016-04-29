# compilador
CC = gcc

# bibliotecas
LIBS = -std=c++11 -Wall

all: execute

ligador:
	$(CC) $(LIBS) nodo_tdg.hpp ligador.cpp -o ligador

execute: ligador
	./ligador moduloa modulob

nodo_tdg:
	$(CC) $(LIBS) nodo_tdg.hpp

clean:
	rm ligador
	rm nodo_tdg