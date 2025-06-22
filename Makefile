CC = gcc
CFLAGS = -g

main.o:
	gcc -I./lib -c main.c -o main.o
ducks.o:
	gcc -I./lib -c ./flocking/ducks.c -o flocking/ducks.o
all: main.o ducks.o
	gcc -lraylib main.o flocking/ducks.o -o duckGame
clean:
	rm main.o duckGame flocking/ducks.o
