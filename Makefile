CC = gcc
FRAMEWORKS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
CFLAGS = -I/usr/local/include -Wall
LDFLAGS = -L/usr/local/lib -lraylib $(FRAMEWORKS)

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

