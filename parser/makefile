CC=gcc
CFLAGS= -fPIC -Wall -std=c11 -g
LDFLAGS= -L.
SRC = ./src/
BIN = ./bin/

all: parser

parser: (BIN)GEDCOMparser.o (BIN)GEDCOMutilities.o (BIN)LinkedListAPI.o
	gcc -shared -o ../GEDCOMlib.so $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o $(BIN)LinkedListAPI.o

(BIN)GEDCOMparser.o: $(SRC)GEDCOMparser.c include/GEDCOMparser.h include/GEDCOMutilities.h
	$(CC) $(CFLAGS) -c $(SRC)GEDCOMparser.c -Iinclude -o $(BIN)GEDCOMparser.o

(BIN)GEDCOMutilities.o: $(SRC)GEDCOMutilities.c include/GEDCOMutilities.h
	$(CC) $(CFLAGS) -c $(SRC)GEDCOMutilities.c -Iinclude -o $(BIN)GEDCOMutilities.o

(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c include/LinkedListAPI.h
	$(CC) $(CFLAGS) -c $(SRC)LinkedListAPI.c -Iinclude -o $(BIN)LinkedListAPI.o

clean:
	rm -rf bin/*.o ../*.so
