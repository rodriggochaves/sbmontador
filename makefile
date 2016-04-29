# compilador
CC = gcc

# bibliotecas
LIBS = -lstdc++ -Wall

all: execute

ligador:
	$(CC) $(LIBS) ligador.cpp -o ligador

execute: ligador
	./ligador moduloa modulob

clean:
	rm ligador