CC = gcc
FRAMEWORKS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
SRC = /opt/homebrew
CFLAGS = -I/$(SRC)/include -Wall
LDFLAGS = -L/$(SRC)/lib -lraylib $(FRAMEWORKS)

start: main
	./main

main: main.o horse.o cecil.o
	$(CC) -o $@ main.o horse.o cecil.o $(LDFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

horse.o: horse.c horse.h
	$(CC) -c horse.c $(CFLAGS)

cecil.o: cecil.c cecil.h
	$(CC) -c cecil.c $(CFLAGS)

clean:
	rm *.o

