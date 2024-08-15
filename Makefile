CC = gcc
CFLAGS = -Werror -Wall -std=gnu11

all: bmp

bmp: bmp.o transformations.o main.o
	$(CC) $(CFLAGS) -o bmp bmp.o transformations.o main.o -lm

bmp.o: bmp.c bmp.h
	$(CC) $(CFLAGS) -c bmp.c -lm

transformations.o: transformations.c transformations.h
	$(CC) $(CFLAGS) -c transformations.c -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -lm

clean:
	rm -f *.o bmp