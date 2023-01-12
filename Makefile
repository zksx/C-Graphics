DEBUG = -g
CFLAGS = -Wall -std=c99 -pedantic -c $(DEBUG)
LFLAGS = -Wall -std=c99 -pedantic $(DEBUG)
LIBS = -lm

all: file_reader.o objects.o v3math.o raycast.o ppmrw.o
	$(CC) $(LFLAGS) file_reader.o objects.o v3math.o raycast.o ppmrw.o -o raycast $(LIBS)

file_reader.o: file_reader.c file_reader.h
	$(CC) $(CFLAGS) file_reader.c

objects.o: objects.c objects.h
	$(CC) $(CFLAGS) objects.c

ppmrw.o: ppmrw.c ppmrw.h
	$(CC) $(CFLAGS) ppmrw.c

raycast.o: raycast.c raycast.h
	$(CC) $(CFLAGS) raycast.c

v3math.o: v3math.c v3math.h
	$(CC) $(CFLAGS) v3math.c

clean:
	rm -rf *.o raycast
