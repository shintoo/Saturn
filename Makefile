CC=gcc
CFLAGS= -std=c11
SRC= src/main.c src/instructions.c src/parse.c
HDR= src/instructions.h src/parse.h
PATH=/usr/bin

all: Saturn

install: saturn
	cp saturn $(PATH)

Saturn: $(SRC) $(HDR)
	$(CC) -o saturn $(CFLAGS) $(SRC)
