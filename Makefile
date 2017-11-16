# MAKEFILE

EXE = 1 

all: 1

1: 1.c 1.h 
	gcc 1.c -o 1 -Wall -O2 -lm -g
clean:
	rm *.o *~ $(EXE)
