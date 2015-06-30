CC=gcc
CFLAGS= -std=c11 -g
SRC= src/main.c src/parse.c src/instructions.c src/util.c
HDR= src/instructions.h src/parse.h src/util.h
PATH=/usr/bin

all: Saturn

install: saturn
	cp saturn $(PATH)

Saturn: $(SRC) $(HDR)
	$(CC) -o saturn $(CFLAGS) $(SRC)
