CC = gcc
CFLAGS = -g -std=gnu99 -pedantic -Wall -Wextra #-Werror #-O2 #-m32
LDLIBS = -pthread -lrt -lpthread 

.PHONY: all zip clean

all: proj2 #

# souboty .o (závislosti)
error.o: error.c error.h
	$(CC) $(CFLAGS) -c $< -o $@

proj2.o: proj2.c error.h proj2.h funkce.c
	$(CC) $(CFLAGS) -c $< -o $@

#spustitelné binárky
proj2: proj2.o error.o
	$(CC) $^ $(LDLIBS) -o $@

# pomocné příkazy
zip:
	zip proj2.zip *.c *.h Makefile


clean: 
	rm -f *.o proj2