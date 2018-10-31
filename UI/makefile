CC = gcc
CFLAGS = -I $(IDIR) `sdl2-config --cflags --libs` -lm -lSDL2main -lSDL2 -lSDL2_image 

IDIR = ./include/
SRCDIR = ./src/

SOURCES = $(SRCDIR)*.c 

all: UIexec  #run #clean

UIexec: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o ./UIexec

run:
	./UIexec

clean:
	rm ./UIexec
