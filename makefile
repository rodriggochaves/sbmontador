# compilador
CC = g++

# bibliotecas
LIBS = -std=c++11 -Wall

preprocessador: clean
	$(CC) $(LIBS) -o preprocessador preprocessador.cpp

execute_preprocessador:
	./preprocessador meuprograma

ligador:
	$(CC) $(LIBS) -c node.cpp
	$(CC) $(LIBS) -c module.cpp
	$(CC) $(LIBS) -c ligador.cpp
	$(CC) $(LIBS) -o ligador node.o module.o ligador.o

execute_ligador: ligador
	./ligador moduloa modulob saida

nodo:
	$(CC) $(LIBS) -c nodo.cpp

module:
	$(CC) $(LIBS) -c module.cpp

clean:
	rm -f ligador
	rm -f ligador.o
	rm -f module.o
	rm -f node.o
	rm -f preprocessador