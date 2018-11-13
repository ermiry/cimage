CC = gcc
CFLAGS = -I $(IDIR)

IDIR = ./include/
SRCDIR = ./src/

SOURCES = $(SRCDIR)*.c $(SRCDIR)utils/*.c

all: cimage #run #clean

cimage: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o ./bin/cimage

# run:
# 	./bin/cimage

run:
	@./bin/cimage ${argv}

clean:
	rm ./bin/cimage     