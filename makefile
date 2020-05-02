CC=gcc

all: 
	$(CC) -Wall main.c  -lcrypto
debug:
	$(CC) -g main.c -lcrypto
clean:
	rm -f *.out *.o