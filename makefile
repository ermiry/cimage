CC = gcc
CFLAGS = -I $(IDIR)

IDIR = ./include/
SRCDIR = ./src/

SOURCES = $(SRCDIR)*.c 

all: cimage #run #clean

cimage: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o ./bin/cimage

run:
	./bin/cimage

clean:
	rm ./bin/cimage      