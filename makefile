CC = gcc # Compiler

main: main.o virtual_Memory.o
	$(CC) -o main main.o virtual_Memory.o

main.o: main.c
	$(CC) -c main.c

virtual_Memory.o: virtual_Memory.c
	$(CC) -c virtual_Memory.c

clean:
	rm -r main main.o virtual_Memory.o
