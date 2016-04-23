# compilador
CC = gcc

# bibliotecas
LIBS = -lstdc++ -Wall

all: execute

preprocessador:
	$(CC) $(LIBS) preprocessador.cpp -o preprocessador

execute: preprocessador
	./preprocessador meuprograma.asm

clean:
	rm -rf *.o
	rm -rf preprocessador