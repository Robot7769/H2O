CC = gcc
CFLAGS = -g -std=gnu99 -pedantic -Wall -Wextra #-Werror #-O2 #-m32
LDLIBS = -pthread -lrt

.PHONY: all zip clean

all: proj2 #

# souboty .o (závislosti)
proj2.o: proj2.c
	$(CC) $(CFLAGS) -c proj2.c -o proj2.o

#spustitelné binárky
proj2: proj2.o 
	$(CC) proj2.o $(LDLIBS) -o proj2

# pomocné příkazy
zip:
	zip proj2.zip *.c *.h Makefile


clean: 
	rm -f *.o proj2