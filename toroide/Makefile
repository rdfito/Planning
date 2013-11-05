CC := mpicc
CFLAGS := -Wall

all: toroide

toroide: toroide.o
	$(CC) $(CFLAGS) -o $@ toroide.c

clean:
	rm -f toroide *.o

