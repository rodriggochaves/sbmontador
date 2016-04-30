# compilador
CC = g++

# bibliotecas
LIBS = -std=c++11 -Wall

all: execute

ligador:
	$(CC) $(LIBS) -c node.cpp
	$(CC) $(LIBS) -c module.cpp
	$(CC) $(LIBS) -c ligador.cpp
	$(CC) $(LIBS) -o ligador node.o module.o ligador.o

execute: ligador
	./ligador moduloa modulob saida

nodo:
	$(CC) $(LIBS) -c nodo.cpp

module:
	$(CC) $(LIBS) -c module.cpp

clean:
	rm ligador