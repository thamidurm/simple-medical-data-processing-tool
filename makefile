CC=gcc
all: 
	$(CC) main.c 
debug:
	$(CC) -g main.c
clean:
	rm -f *.out
