CC := mpicc
CFLAGS := -Wall

all: hiperc

hiperc: hiperc.o
	$(CC) $(CFLAGS) -o $@ hiperc.c

clean:
	rm -f hiperc *.o

