# compilador
CC = g++

# bibliotecas
LIBS = -std=c++11 -Wall

preprocessador: clean
	$(CC) $(LIBS) -c node.cpp
	$(CC) $(LIBS) -c preprocessador.cpp
	$(CC) $(LIBS) -o preprocessador node.o preprocessador.o 

execute_preprocessador:
	./preprocessador meuprograma2

ligador:
	$(CC) $(LIBS) -c node.cpp
	$(CC) $(LIBS) -c module.cpp
	$(CC) $(LIBS) -c ligador.cpp
	$(CC) $(LIBS) -o ligador node.o module.o ligador.o

execute_ligador: ligador
	./ligador moduloa modulob saida

main: clean
	$(CC) $(LIBS) -c node.cpp
	$(CC) $(LIBS) -c module.cpp
	$(CC) $(LIBS) -c preprocessador.cpp
	$(CC) $(LIBS) -c ligador.cpp
	$(CC) $(LIBS) -c montador.cpp
	$(CC) $(LIBS) -c mainModule.cpp
	$(CC) $(LIBS) -o main node.o mainModule.o montador.o ligador.o \
		preprocessador.o module.o

nodo:
	$(CC) $(LIBS) -c nodo.cpp

module:
	$(CC) $(LIBS) -c module.cpp

clean:
	rm -f ligador
	rm -f ligador.o
	rm -f module.o
	rm -f mainModule.o
	rm -f node.o
	rm -f montador.o
	rm -f meuprograma.asm.o
	rm -f meuprograma2.asm.o
	rm -f preprocessador
	rm -f preprocessador.o
	rm -f main
	rm -f saida.e
