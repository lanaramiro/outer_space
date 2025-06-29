CC=gcc
CFLAGS=-Wall -pthread -lncurses
DEPS=inc/game.h inc/orders.h inc/kitchen.h inc/crew.h
OBJ=src/main.o src/game.o src/orders.o src/kitchen.o src/crew.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

outer_space: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o outer_space
