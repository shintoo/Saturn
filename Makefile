CC=gcc
CFLAGS= -std=c11 -g
SRC= src/main.c src/parse.c src/instructions.c src/util.c
HDR= src/instructions.h src/parse.h src/util.h
INSTALLDIR=/usr/bin

all: Saturn

install: saturn
	cp saturn $(INSTALLDIR)

Saturn: $(SRC) $(HDR)
	$(CC) -o saturn $(CFLAGS) $(SRC)
