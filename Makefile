PROG=saturn
CC=gcc
CFLAGS= -c -std=c11 -g
SRC=src/main.c src/Saturn.c
OBJ=main.o Saturn.o

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) -o $(PROG) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC)

clean:
	rm $(OBJ)
